#include <fstream>
#include <iomanip>
#include <iostream>
#include "Dataset.h"
#include "forecast.h"
#include "forecast_utils.h"
#include "crypt.h"
using namespace std;

int main(const int argc, char** argv) {
    const auto args = parseArgs(argc, argv);
    if (args.has_error) {
        return 1;
    }
    if (args.help) {
        cout << "Использование: " << argv[0] << " <csv_path> [--output <output_path>] [--H <forecast_horizon>] [--season_m <season_length>] [--crypt <key>] [--newCryptKey <key_file>]\n";
        cout << "Параметры:\n";
        cout << "  <csv_path>            Путь к входному CSV файлу с данными.\n";
        cout << "  --output <output_path> Путь к выход ному CSV файлу для сохранения прогноза (по умолчанию forecast.csv).\n";
        cout << "  --H <forecast_horizon> Количество точек для прогноза (по умолчанию 30).\n";
        cout << "  --season_m <season_length> Длина сезона для экспоненциального сглаживания (по умолчанию 7).\n";
        cout << "  --crypt <key>         Ключ для расшифровки входного CSV файла.\n";
        cout << "  --newCryptKey <key_file> Генерирует новый ключ шифрования и сохраняет его в указанный файл.\n";
        return 0;
    }

    int H = args.H > 0 ? args.H : 30;
    int m = args.season_m > 0 ? args.season_m : 7;

    cout << "Загрузка датасета из CSV..." << endl;
    Dataset dataset;
    dataset.fromCSV(args.csv_path);
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

    SeedCryptor cryptor = args.cryptor;

    std::ofstream outFile(args.output_path);
    outFile << "Day,Date,Page Loads,Unique Visitors,First Time Visitors, Returning Visitors\n";
    for (const auto&[day, date, pageLoads, uniqueVisitors, firstTimeVisitors, returningVisitors] : forecast) {
        if (!args.crypt) {
            outFile << day << ','
                    << std::put_time(std::localtime(&date), "%m/%d/%Y") << ','
                    << pageLoads << ','
                    << uniqueVisitors << ','
                    << firstTimeVisitors << ','
                    << returningVisitors << '\n';
        } else {
            outFile << day << ','
                    << std::put_time(std::localtime(&date), "%m/%d/%Y") << ','
                    << cryptor.encrypt(to_string(pageLoads)) << ','
                    << cryptor.encrypt(to_string(uniqueVisitors)) << ','
                    << cryptor.encrypt(to_string(firstTimeVisitors)) << ','
                    << cryptor.encrypt(to_string(returningVisitors)) << '\n';
        }
    }

    cout << "Прогноз сохранён в forecast.csv" << endl;

    cout << "----------" << endl;
    cout << "Сезоны m: " << m << endl;
    cout << "Количество прогнозируемых точек H: " << H << endl;
    cout << "Page Loads коэффициенты: alpha=" << pageLoadsOdds.alpha
        << ", beta=" << pageLoadsOdds.beta
        << ", gamma=" << pageLoadsOdds.gamma
        << ", WAPETest=" << pageLoadsOdds.WAPETest << endl;
    cout << "Unique Visitors коэффициенты: alpha=" << uniqueVisitorsOdds.alpha
        << ", beta=" << uniqueVisitorsOdds.beta
        << ", gamma=" << uniqueVisitorsOdds.gamma
        << ", WAPETest=" << uniqueVisitorsOdds.WAPETest << endl;
    cout << "First Time Visitors коэффициенты: alpha=" << firstTimeVisitsOdds.alpha
        << ", beta=" << firstTimeVisitsOdds.beta
        << ", gamma=" << firstTimeVisitsOdds.gamma
        << ", WAPETest=" << firstTimeVisitsOdds.WAPETest << endl;
    cout << "Returning Visitors коэффициенты: alpha=" << returningVisitsOdds.alpha
        << ", beta=" << returningVisitsOdds.beta
        << ", gamma=" << returningVisitsOdds.gamma
        << ", WAPETest=" << returningVisitsOdds.WAPETest << endl;

    return 0;
}