#include "DatasetValue.h"
#include <sstream>
#include <ctime>
#include "forecast_utils.h"

/**
 * Полный конструктор: дата и числовые поля как значения.
 */
DatasetValue::DatasetValue(string _day, int _dayOfWeek, time_t _date, int _pageLoads, int _uniqueVisitors, int _firstTimeVisitors, int _returningVisitors) {
    day = std::move(_day);
    dayOfWeek = _dayOfWeek;
    date = _date;
    pageLoads = _pageLoads;
    uniqueVisitors = _uniqueVisitors;
    firstTimeVisitors = _firstTimeVisitors;
    returningVisitors = _returningVisitors;
}

/**
 * Конструктор, где дата передаётся строкой формата "MM/DD/YYYY", числовые поля — как int.
 */
DatasetValue::DatasetValue(const string &_day, int _dayOfWeek, const string &_dateStr, int _pageLoads, int _uniqueVisitors, int _firstTimeVisitors, int _returningVisitors) {
    day = _day;
    dayOfWeek = _dayOfWeek;
    date = parseDateString(_dateStr);
    pageLoads = _pageLoads;
    uniqueVisitors = _uniqueVisitors;
    firstTimeVisitors = _firstTimeVisitors;
    returningVisitors = _returningVisitors;
}

/**
 * Конструктор, где числовые поля передаются как строки с запятыми (например "1,234").
 * Дата передаётся как time_t.
 */
DatasetValue::DatasetValue(const string &_day, int _dayOfWeek, time_t _date, const string &_pageLoadsStr, const string &_uniqueVisitorsStr, const string &_firstTimeVisitorsStr, const string &_returningVisitorsStr) {
    day = _day;
    dayOfWeek = _dayOfWeek;
    date = _date;
    pageLoads = parseNumberString(_pageLoadsStr);
    uniqueVisitors = parseNumberString(_uniqueVisitorsStr);
    firstTimeVisitors = parseNumberString(_firstTimeVisitorsStr);
    returningVisitors = parseNumberString(_returningVisitorsStr);
}

/**
 * Конструктор, где и дата, и числовые поля передаются в виде строк.
 * Дата — "MM/DD/YYYY", числа — возможно с запятыми.
 */
DatasetValue::DatasetValue(const string &_day, int _dayOfWeek, const string &_dateStr, const string &_pageLoadsStr, const string &_uniqueVisitorsStr, const string &_firstTimeVisitorsStr, const string &_returningVisitorsStr) {
    day = _day;
    dayOfWeek = _dayOfWeek;
    date = parseDateString(_dateStr);
    pageLoads = parseNumberString(_pageLoadsStr);
    uniqueVisitors = parseNumberString(_uniqueVisitorsStr);
    firstTimeVisitors = parseNumberString(_firstTimeVisitorsStr);
    returningVisitors = parseNumberString(_returningVisitorsStr);
}

/** @return название дня (ссылка на внутреннюю строку) */
const string& DatasetValue::getDay() const { return day; }
/** @return номер дня недели */
int DatasetValue::getDayOfWeek() const { return dayOfWeek; }
/** @return дата в виде time_t */
time_t DatasetValue::getDate() const { return date; }
/** @return количество загрузок страниц */
int DatasetValue::getPageLoads() const { return pageLoads; }
/** @return количество уникальных посетителей */
int DatasetValue::getUniqueVisitors() const { return uniqueVisitors; }
/** @return количество посетителей в первый раз */
int DatasetValue::getFirstTimeVisitors() const { return firstTimeVisitors; }
/** @return количество возвращающихся посетителей */
int DatasetValue::getReturningVisitors() const { return returningVisitors; }

/** Устанавливает название дня. */
void DatasetValue::setDay(const string &d) { day = d; }
/** Устанавливает номер дня недели. */
void DatasetValue::setDayOfWeek(const int d) { dayOfWeek = d; }
/** Устанавливает номер дня недели из строки (поддерживает запятые). */
void DatasetValue::setDayOfWeek(const string &d) { dayOfWeek = parseNumberString(d); }
/** Устанавливает дату (time_t). */
void DatasetValue::setDate(time_t d) { date = d; }
/** Устанавливает количество загрузок страниц. */
void DatasetValue::setPageLoads(const int v) { pageLoads = v; }
/** Устанавливает количество загрузок страниц из строки. */
void DatasetValue::setPageLoads(const string &v) { pageLoads = parseNumberString(v); }
/** Устанавливает количество уникальных посетителей. */
void DatasetValue::setUniqueVisitors(const int v) { uniqueVisitors = v; }
/** Устанавливает количество уникальных посетителей из строки. */
void DatasetValue::setUniqueVisitors(const string &v) { uniqueVisitors = parseNumberString(v); }
/** Устанавливает количество посетителей в первый раз. */
void DatasetValue::setFirstTimeVisitors(const int v) { firstTimeVisitors = v; }
/** Устанавливает количество посетителей в первый раз из строки. */
void DatasetValue::setFirstTimeVisitors(const string &v) { firstTimeVisitors = parseNumberString(v); }
/** Устанавливает количество возвращающихся посетителей. */
void DatasetValue::setReturningVisitors(const int v) { returningVisitors = v; }
/** Устанавливает количество возвращающихся посетителей из строки. */
void DatasetValue::setReturningVisitors(const string &v) { returningVisitors = parseNumberString(v); }

/** Оператор вывода для удобного логирования/отладки. Формат:
 * YYYY-MM-DD day dow=<n> pageLoads=<n> uniqueVisitors=<n> firstTimeVisitors=<n> returningVisitors=<n>
 */
std::ostream& operator<<(std::ostream& os, const DatasetValue& dv) {
    // format date as YYYY-MM-DD
    const time_t t = dv.getDate();
    if (t != 0) {
        std::tm tm{};
#ifdef _POSIX_VERSION
        localtime_r(&t, &tm);
#else
        // fallback (not thread-safe) for non-POSIX
        if (const std::tm *ptm = std::localtime(&t)) tm = *ptm;
#endif
        char buf[32];
        if (std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm)) {
            os << buf;
        } else {
            os << "0000-00-00";
        }
    } else {
        os << "0000-00-00";
    }

    os << " " << dv.getDay()
       << " dow=" << dv.getDayOfWeek()
       << " pageLoads=" << dv.getPageLoads()
       << " uniqueVisitors=" << dv.getUniqueVisitors()
       << " firstTimeVisitors=" << dv.getFirstTimeVisitors()
       << " returningVisitors=" << dv.getReturningVisitors();
    return os;
}
