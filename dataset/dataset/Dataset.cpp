#include "Dataset.h"
#include <fstream>
#include <sstream>

void Dataset::addRow(DatasetValue d) {
    rows.push_back(std::move(d));
}

void Dataset::clearRows() {
    rows.clear();
}

const vector<DatasetValue>& Dataset::getRows() const {
    return rows;
}

void Dataset::setRows(vector<DatasetValue> r) {
    rows = std::move(r);
}

void Dataset::fromCSV(const string &filename) {
    rows.clear();
    std::ifstream file(filename);
    string line;

    std::getline(file, line);
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        string day, dateStr, pageLoadsStr, uniqueVisitorsStr, firstTimeVisitorsStr, returningVisitorsStr, row, dayOfWeek;

        if (std::getline(iss, row, ',') &&
            std::getline(iss, day, ',') &&
            std::getline(iss, dayOfWeek, ',') &&
            std::getline(iss, dateStr, ',') &&
            std::getline(iss, pageLoadsStr, ',') &&
            std::getline(iss, uniqueVisitorsStr, ',') &&
            std::getline(iss, firstTimeVisitorsStr, ',') &&
            std::getline(iss, returningVisitorsStr, ',')) {

            DatasetValue dv(day, stoi(dayOfWeek), dateStr, pageLoadsStr, uniqueVisitorsStr, firstTimeVisitorsStr,
                            returningVisitorsStr);
            rows.push_back(std::move(dv));
        }
    }

    file.close();
}

std::ostream& operator<<(std::ostream& os, const Dataset& dv) {
    for (const auto& row : dv.getRows()) {
        os << row << std::endl;
    }
    return os;
}