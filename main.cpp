#include <fstream>
#include <iomanip>
#include <iostream>
#include "Dataset.h"
#include "forecast.h"
#include "forecast_utils.h"
using namespace std;

int main(const int argc, char** argv) {
    if (argc<2){
        cerr<<"Использование: "<<argv[0]<<" <csv_path> [H=30] [season_m=7]\n";
        return 1;
    }
    const string path = argv[1];
    const int H = (argc>=3? stoi(argv[2]) : 30);
    const int m = (argc>=4? stoi(argv[3]) : 7);

    cout << "Загрузка датасета из CSV..." << endl;
    Dataset dataset;
    dataset.fromCSV(path);
    cout << "Датасет загружен, строк: " << dataset.size() << endl;

    if (dataset.size() < static_cast<size_t>(m * 2)) {
        cerr << "Датасет слишком маленький, минимум строк для выбранного season_m = " << m * 2 << endl;
        return 1;
    }

    vector<int> pageLoadsData;
    vector<int> uniqueVisitorsData;
    vector<int> firstTimeVisitsData;
    vector<int> returningVisitsData;
    for (size_t i = 0; i < dataset.size(); ++i) {
        auto row = dataset.getRow(i);
        pageLoadsData.push_back(row.getPageLoads());
        uniqueVisitorsData.push_back(row.getUniqueVisitors());
        firstTimeVisitsData.push_back(row.getFirstTimeVisitors());
        returningVisitsData.push_back(row.getReturningVisitors());
    }

    cout << pageLoadsData << endl; // TODO: delete debug
    auto pageLoadsOdds = betterCoefficient(pageLoadsData, m);
    auto uniqueVisitorsOdds = betterCoefficient(uniqueVisitorsData, m);
    auto firstTimeVisitsOdds = betterCoefficient(firstTimeVisitsData, m);
    auto returningVisitsOdds = betterCoefficient(returningVisitsData, m);

    vector<int> pageLoadsForecast = exponentialSmoothing(
        pageLoadsData,
        pageLoadsOdds.alpha,
        pageLoadsOdds.beta,
        pageLoadsOdds.gamma,
        m,
        H
    );

    cout << pageLoadsForecast << endl; //TODO: delete debug

    vector<int> uniqueVisitorsForecast = exponentialSmoothing(
        uniqueVisitorsData,
        uniqueVisitorsOdds.alpha,
        uniqueVisitorsOdds.beta,
        uniqueVisitorsOdds.gamma,
        m,
        H
    );
    vector<int> firstTimeVisitsForecast = exponentialSmoothing(
        firstTimeVisitsData,
        firstTimeVisitsOdds.alpha,
        firstTimeVisitsOdds.beta,
        firstTimeVisitsOdds.gamma,
        m,
        H
    );
    vector<int> returningVisitsForecast = exponentialSmoothing(
        returningVisitsData,
        returningVisitsOdds.alpha,
        returningVisitsOdds.beta,
        returningVisitsOdds.gamma,
        m,
        H
    );

    struct ForecastEntry {
        string day;
        time_t date;
        int pageLoads;
        int uniqueVisitors;
        int firstTimeVisitors;
        int returningVisitors;
    };

    vector<ForecastEntry> forecast;
    forecast.push_back(ForecastEntry{
        nextDayString(dataset.getRow(dataset.size() - 1).getDay()),
        nextDayTimeT(dataset.getRow(dataset.size() - 1).getDate()),
        pageLoadsForecast[0],
        uniqueVisitorsForecast[0],
        firstTimeVisitsForecast[0],
        returningVisitsForecast[0]
    });

    for (int i = 1; i < H; ++i) {
        forecast.push_back(ForecastEntry{
            nextDayString(forecast[i - 1].day),
            nextDayTimeT(forecast[i - 1].date),
            pageLoadsForecast[i],
            uniqueVisitorsForecast[i],
            firstTimeVisitsForecast[i],
            returningVisitsForecast[i]
        });
    }

    std::ofstream outFile("forecast.csv");
    outFile << "Day,Date,Page Loads,Unique Visitors,First Time Visitors, Returning Visitors\n";
    for (const auto& entry : forecast) {
        outFile << entry.day << ','
                << std::put_time(std::localtime(&entry.date), "%m/%d/%Y") << ','
                << entry.pageLoads << ','
                << entry.uniqueVisitors << ','
                << entry.firstTimeVisitors << ','
                << entry.returningVisitors << '\n';
    }

    cout << "Прогноз сохранён в forecast.csv" << endl;

    cout << "----------" << endl;
    cout << "Сезоны m: " << m << endl;
    cout << "Количество прогнозируемых точек H: " << H << endl;
    cout << "Page Loads коэффициенты: alpha=" << pageLoadsOdds.alpha
         << ", beta=" << pageLoadsOdds.beta
         << ", gamma=" << pageLoadsOdds.gamma << endl;
    cout << "Unique Visitors коэффициенты: alpha=" << uniqueVisitorsOdds.alpha
         << ", beta=" << uniqueVisitorsOdds.beta
        << ", gamma=" << uniqueVisitorsOdds.gamma << endl;
    cout << "First Time Visitors коэффициенты: alpha=" << firstTimeVisitsOdds.alpha
         << ", beta=" << firstTimeVisitsOdds.beta
         << ", gamma=" << firstTimeVisitsOdds.gamma << endl;
    cout << "Returning Visitors коэффициенты: alpha=" << returningVisitsOdds.alpha
         << ", beta=" << returningVisitsOdds.beta
         << ", gamma=" << returningVisitsOdds.gamma << endl;

    return 0;
}