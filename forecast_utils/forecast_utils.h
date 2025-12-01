#ifndef TRAFFIC_FORECAST_UTILS_H
#define TRAFFIC_FORECAST_UTILS_H

#include <ctime>
#include <string>
#include <vector>
#include <ostream>

using namespace std;

/**
 * Преобразует строку формата "MM/DD/YYYY" в time_t.
 * Если парсинг не удаётся, возвращается time_t(0).
 * @param s дата в виде строки "MM/DD/YYYY" (например, "12/31/2020").
 * @return значение time_t, соответствующее полуночи указанного дня в локальном часовом поясе, либо 0 при ошибке.
 */
time_t parseDateString(const string &s);

/**
 * Преобразует строку с числом, где разделителем тысяч может быть запятая, в int.
 * Пример: "1,234" -> 1234. При ошибке возвращает 0.
 * @param s строковое представление числа (возможно с запятыми).
 * @return целое значение, полученное после удаления запятых и приведения к int.
 */
int parseNumberString(const string &s);

/**
 * Выводит std::vector<T> в поток в формате [elem1, elem2, ...].
 * Требуется, чтобы для типа T была определена операция вывода в поток (operator<<).
 * @tparam T тип элементов вектора.
 * @param os выходной поток.
 * @param v вектор.
 * @param sep разделитель между элементами (по умолчанию ", ").
 * @return ссылка на тот же поток os.
 */
template<typename T>
std::ostream& printVector(std::ostream& os, const std::vector<T>& v, const std::string& sep = ", ");

/**
 * Перегрузка operator<< для std::vector<T> в глобальной области видимости.
 */
template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v);

// TODO: add documentation
string nextDayString(string currentDay);

// TODO: add documentation
time_t nextDayTimeT(time_t currentDate);

#endif