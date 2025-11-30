#include <fstream>
#include <iostream>
#include "Dataset.h"
using namespace std;

int main() {
    cout << "Loading dataset from CSV..." << endl;
    Dataset dataset;
    dataset.fromCSV("../dataset.csv");
    cout << dataset;

    return 0;
}