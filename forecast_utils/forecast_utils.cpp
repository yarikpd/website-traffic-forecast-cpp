#include "forecast_utils.h"

#include <sstream>
#include <iostream>
using namespace std;

/**
 * Преобразует строку формата "MM/DD/YYYY" в time_t.
 * Если парсинг не удаётся, возвращается time_t(0).
 * @param s дата в виде строки "MM/DD/YYYY" (например, "12/31/2020").
 * @return значение time_t, соответствующее полуночи указанного дня в локальном часовом поясе, либо 0 при ошибке.
 */
time_t parseDateString(const string &s) {
    std::tm tm{};
    std::istringstream iss(s);
    int month = 0, day_ = 0, year = 0;
    char sep1, sep2;
    if (!(iss >> month >> sep1 >> day_ >> sep2 >> year)) {
        return time_t(0);
    }
    if (month < 1) month = 1;
    if (month > 12) month = 12;
    if (day_ < 1) day_ = 1;
    if (year < 1900) year = 1900;

    tm.tm_mon = month - 1;
    tm.tm_mday = day_;
    tm.tm_year = year - 1900;
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;
    return mktime(&tm);
}

/**
 * Убирает запятые из строки и конвертирует результат в int.
 * Если преобразование не удаётся, возвращает 0.
 * @param s строка с числом, возможно с разделителями тысяч (запятая).
 */
int parseNumberString(const string &s) {
    string t;
    t.reserve(s.size());
    for (const char c: s) {
        if (c != ',' && c != '"') t.push_back(c);
    }
    try {
        return stoi(t);
    } catch (...) {
        return 0;
    }
}

// TODO: add documentation
template<typename T>
std::ostream& printVector(std::ostream& os, const std::vector<T>& v, const std::string& sep) {
    os << '[';
    for (size_t i = 0; i < v.size(); ++i) {
        if (i) os << sep;
        os << v[i];
    }
    os << ']';
    return os;
}

// TODO: add documentation
template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    return printVector(os, v);
}

template std::ostream& printVector<int>(std::ostream& os, const std::vector<int>& v, const std::string& sep);
template std::ostream& operator<< <int>(std::ostream& os, const std::vector<int>& v);

template std::ostream& printVector<double>(std::ostream& os, const std::vector<double>& v, const std::string& sep);
template std::ostream& operator<< <double>(std::ostream& os, const std::vector<double>& v);

template std::ostream& printVector<std::string>(std::ostream& os, const std::vector<std::string>& v, const std::string& sep);
template std::ostream& operator<< <std::string>(std::ostream& os, const std::vector<std::string>& v);

// TODO: add documentation
string nextDayString(const string currentDay) {
    if (currentDay == "Monday") return "Tuesday";
    if (currentDay == "Tuesday") return "Wednesday";
    if (currentDay == "Wednesday") return "Thursday";
    if (currentDay == "Thursday") return "Friday";
    if (currentDay == "Friday") return "Saturday";
    if (currentDay == "Saturday") return "Sunday";
    if (currentDay == "Sunday") return "Monday";
    return "Monday";
}

// TODO: add documentation
time_t nextDayTimeT(const time_t currentDate) {
    return currentDate + 24 * 60 * 60;
}