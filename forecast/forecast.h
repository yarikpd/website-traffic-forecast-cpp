#ifndef TRAFFIC_FORECAST_FORECAST_H
#define TRAFFIC_FORECAST_FORECAST_H
#include <vector>
using namespace std;

// TODO: add documentation
struct SmoothingComponents {
    int y;
    double level;
    double trend;
    double season;
};

// TODO: add documentation
struct SmoothingOdds {
    double alpha;
    double beta;
    double gamma;
};

// TODO: add documentation
vector<int> exponentialSmoothing(
    vector<int> y,
    double alpha,
    double beta,
    double gamma,
    int seasonLength,
    int forecastLength
);

// TODO: add documentation
SmoothingOdds betterCoefficient(
    vector<int> y,
    int seasonLength
);

#endif