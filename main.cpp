#include <fstream>
#include <iostream>
#include "Dataset.h"
using namespace std;

int main(const int argc, char** argv) {
    if (argc<2){
        cerr<<"Usage: "<<argv[0]<<" <csv_path> [H=30] [season_m=7]\n";
        return 1;
    }
    const string path = argv[1];
    int H = (argc>=3? stoi(argv[2]) : 30);
    int m = (argc>=4? stoi(argv[3]) : 7);

    cout << "Loading dataset from CSV..." << endl;
    Dataset dataset;
    dataset.fromCSV(path);
    cout << dataset;

    return 0;
}