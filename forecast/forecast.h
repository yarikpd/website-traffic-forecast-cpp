#ifndef TRAFFIC_FORECAST_FORECAST_H
#define TRAFFIC_FORECAST_FORECAST_H
#include <vector>
using namespace std;

/**
 * @brief Компоненты сглаживания для каждой точки времени.
 *
 * Хранит исходное значение y и рассчитанные компоненты: уровень (level),
 * тренд (trend) и сезонность (season). Используется во внутренних расчётах
 * алгоритма экспоненциального сглаживания.
 */
struct SmoothingComponents {
    int y;
    double level;
    double trend;
    double season;
};

/**
 * @brief Набор коэффициентов сглаживания.
 *
 * alpha — коэффициент уровня, beta — коэффициент тренда, gamma — коэффициент
 * сезонности.
 */
struct SmoothingOdds {
    double alpha;
    double beta;
    double gamma;
};

/**
 * @brief Выполняет прогноз методом экспоненциального сглаживания с учётом сезонности.
 *
 * Реализует вариацию трёхкомпонентного экспоненциального сглаживания (уровень,
 * тренд, сезонность). Функция принимает временной ряд наблюдений y и набор
 * коэффициентов (alpha, beta, gamma), длину сезонного периода и длину
 * горизонта прогноза.
 *
 * @param y Входной ряд наблюдаемых целых значений.
 * @param alpha Коэффициент адаптации уровня (0..1).
 * @param beta Коэффициент адаптации тренда (0..1).
 * @param gamma Коэффициент адаптации сезонности (0..1).
 * @param seasonLength Длина сезонного периода (количество точек в сезоне).
 * @param forecastLength Количество прогнозируемых шагов.
 * @return Вектор целых значений длиной forecastLength с прогнозом.
 */
vector<int> exponentialSmoothing(
    vector<int> y,
    double alpha,
    double beta,
    double gamma,
    int seasonLength,
    int forecastLength
);

/**
 * @brief Подбирает лучшие коэффициенты сглаживания по среднему абсолютному отклонению.
 *
 * Функция перебирает дискретную сетку значений alpha, beta и gamma и вычисляет
 * прогноз для последних seasonLength точек, затем сравнивает с фактическими
 * значениями и возвращает набор коэффициентов, дающий наименьшую среднюю
 * ошибку по модулю.
 *
 * @param y Входной ряд наблюдаемых значений.
 * @param seasonLength Длина сезонного периода.
 * @return Структура SmoothingOdds с подобранными alpha, beta, gamma.
 */
SmoothingOdds betterCoefficient(
    vector<int> y,
    int seasonLength
);

#endif