#include "forecast.h"
#include <iostream>
#include "forecast_utils.h"

// TODO: add documentation
vector<int> exponentialSmoothing(
    vector<int> y,
    const double alpha,
    const double beta,
    const double gamma,
    const int seasonLength,
    const int forecastLength
) {
    vector<SmoothingComponents> components;

    double startingTrend = 0.0;
    double startingLevel = 0.0;
    for (int t = seasonLength * 2 - 1; t >= 0; t--) {
        if (t >= seasonLength) {
            startingTrend += static_cast<double>(y[t]);
        }
        else {
            startingTrend -= static_cast<double>(y[t]);
            startingLevel += static_cast<double>(y[t]);
        }
    }
    startingTrend /= static_cast<double>(seasonLength);
    startingLevel /= static_cast<double>(seasonLength);
    // startingTrend = min(1e-6, startingTrend);

    auto currentValue = static_cast<double>(y[0]);
    const double zeroLevel = alpha * currentValue + (1 - alpha) * (startingLevel + startingTrend);
    const double zeroTrend = beta * (zeroLevel - startingLevel) + (1 - beta) * startingTrend;
    const double zeroSeason = gamma * (currentValue / zeroLevel) + (1 - gamma);

    components.push_back(SmoothingComponents{
        y[0],
        zeroLevel,
        zeroTrend,
        zeroSeason
    });

    const size_t originalSize = y.size();

    for (size_t t = 1; t < originalSize + forecastLength; ++t) {
        if (t < originalSize)
            currentValue = static_cast<double>(y[t]);
        else
            currentValue = (components[t - 1].level + components[t - 1].trend) *
                           (t - seasonLength > 0 ? components[t - seasonLength] : components[0]).season;

        double level = alpha * (currentValue / (t >= seasonLength ? components[t - seasonLength]: components[0]).season) +
                       (1 - alpha) * (components[t - 1].level + components[t - 1].trend);
        // level = min(1e-6, level);

        double trend = beta * (level - components[t - 1].level) +
                       (1 - beta) * components[t - 1].trend;
        // trend = min(trend, 1e-6);

        double season = gamma * (currentValue / level) +
                        (1 - gamma) * (t >= seasonLength ? components[t - seasonLength] : components[0]).season;
        // season = min(1e-6, season);

        if (t >= originalSize) {
            const int forecastedValue = static_cast<int>(
                (level + trend) *
                (t - seasonLength + 1 > 0 ? components[t - seasonLength + 1] : components[0]).season
            );

            y.push_back(forecastedValue);
        }

        components.push_back(SmoothingComponents(
            y[t],
            level,
            trend,
            season
        ));
    }

    // TODO: fix possible negative forecasted values
    vector<int> forecastedValues;
    for (size_t t = originalSize; t < y.size(); ++t) {
        forecastedValues.push_back(y[t]);
    }
    return forecastedValues;
}

// TODO: add documentation
SmoothingOdds betterCoefficient(
    vector<int> y,
    const int seasonLength
) {
    double bestAlpha = 0.1, bestBeta = 0.1, bestGamma = 0.1;
    double minError = 1e9;

    vector<int> yData = y;
    if (yData.size() >= static_cast<size_t>(seasonLength)) {
        yData.erase(yData.end() - seasonLength, yData.end());
    } else {
        cerr << "Not enough elements to create yData\n";
    }

    vector<int> realForecast;
    if (y.size() >= static_cast<size_t>(seasonLength)) {
        realForecast.assign(y.end() - seasonLength, y.end());
    } else {
        cerr << "Not enough elements to create realForecast\n";
    }

    for (int intAlpha = 1; intAlpha < 10; intAlpha += 1) {
        double alpha = intAlpha / 10.0;
        for (int intBeta = 1; intBeta < 10; intBeta += 1) {
            double beta = intBeta / 10.0;
            for (int intGamma = 1; intGamma < 10; intGamma += 1) {
                double gamma = intGamma / 10.0;
                vector<int> forecast = exponentialSmoothing(
                    yData,
                    alpha,
                    beta,
                    gamma,
                    seasonLength,
                    seasonLength
                );

                if (forecast.size() != realForecast.size()) {
                    cerr << "Forecast size does not match real forecast size\n";
                    continue;
                }

                double error = 0.0;
                for (size_t i = 0; i < forecast.size(); ++i) {
                    error += abs(forecast[i] - realForecast[i]);
                }
                error /= static_cast<double>(forecast.size());

                if (error < minError) {
                    minError = error;
                    bestAlpha = alpha;
                    bestBeta = beta;
                    bestGamma = gamma;
                }
            }
        }
    }

    return SmoothingOdds{
        bestAlpha,
        bestBeta,
        bestGamma
    };
}