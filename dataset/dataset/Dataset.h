#ifndef TRAFFIC_FORECAST_DATASET_H
#define TRAFFIC_FORECAST_DATASET_H

#include <vector>
#include <string>

#include "DatasetValue.h"

using namespace std;

class Dataset {
    vector<DatasetValue> rows;

public:
    void addRow(DatasetValue d);
    void clearRows();

    [[nodiscard]] const vector<DatasetValue>& getRows() const;
    void setRows(vector<DatasetValue> rows);

    void fromCSV(const string &filename);
};

std::ostream& operator<<(std::ostream& os, const Dataset& dv);

#endif