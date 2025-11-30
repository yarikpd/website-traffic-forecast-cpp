#ifndef TRAFFIC_FORECAST_DATASET_VALUE_H
#define TRAFFIC_FORECAST_DATASET_VALUE_H

#include <string>
#include <ostream>
using namespace std;

/**
 * Представляет запись набора данных по трафику за один день.
 */
class DatasetValue {
    string day;
    int dayOfWeek;
    time_t date;
    int pageLoads;
    int uniqueVisitors;
    int firstTimeVisitors;
    int returningVisitors;

    /**
     * Преобразует строку формата "MM/DD/YYYY" в time_t.
     * Если парсинг не удаётся, возвращается time_t(0).
     * @param s дата в виде строки "MM/DD/YYYY" (например, "12/31/2020").
     * @return значение time_t, соответствующее полуночи указанного дня в локальном часовом поясе, либо 0 при ошибке.
     */
    static time_t parseDateString(const string &s);

    /**
     * Преобразует строку с числом, где разделителем тысяч может быть запятая, в int.
     * Пример: "1,234" -> 1234. При ошибке возвращает 0.
     * @param s строковое представление числа (возможно с запятыми).
     * @return целое значение, полученное после удаления запятых и приведения к int.
     */
    static int parseNumberString(const string &s);

public:
    /**
     * Полный конструктор: дата и числовые поля как значения.
     * @param _day название дня (строка, например "Mon")
     * @param _dayOfWeek номер дня недели (0..6 или по вашей принятой конвенции)
     * @param _date значение времени (time_t) — дата
     * @param _pageLoads количество загрузок страниц
     * @param _uniqueVisitors количество уникальных посетителей
     * @param _firstTimeVisitors количество посетителей в первый раз
     * @param _returningVisitors количество возвращающихся посетителей
     */
    DatasetValue(string _day, int _dayOfWeek, time_t _date, int _pageLoads, int _uniqueVisitors, int _firstTimeVisitors, int _returningVisitors);

    /**
     * Конструктор, где дата передаётся строкой формата "MM/DD/YYYY", числовые поля — как int.
     * @param _day название дня
     * @param _dayOfWeek номер дня недели
     * @param _dateStr дата в виде строки "MM/DD/YYYY"
     * @param _pageLoads количество загрузок страниц
     * @param _uniqueVisitors количество уникальных посетителей
     * @param _firstTimeVisitors количество посетителей в первый раз
     * @param _returningVisitors количество возвращающихся посетителей
     */
    DatasetValue(const string &_day, int _dayOfWeek, const string &_dateStr, int _pageLoads, int _uniqueVisitors, int _firstTimeVisitors, int _returningVisitors);

    /**
     * Конструктор, где числовые поля передаются как строки с запятыми (например "1,234").
     * Дата передаётся как time_t.
     * @param _day название дня
     * @param _dayOfWeek номер дня недели
     * @param _date значение time_t
     * @param _pageLoadsStr строковое представление количества загрузок
     * @param _uniqueVisitorsStr строковое представление уникальных посетителей
     * @param _firstTimeVisitorsStr строковое представление посетителей в первый раз
     * @param _returningVisitorsStr строковое представление возвращающихся посетителей
     */
    DatasetValue(const string &_day, int _dayOfWeek, time_t _date, const string &_pageLoadsStr, const string &_uniqueVisitorsStr, const string &_firstTimeVisitorsStr, const string &_returningVisitorsStr);

    /**
     * Конструктор, где и дата, и числовые поля передаются в виде строк.
     * Дата — "MM/DD/YYYY", числа — возможно с запятыми.
     */
    DatasetValue(const string &_day, int _dayOfWeek, const string &_dateStr, const string &_pageLoadsStr, const string &_uniqueVisitorsStr, const string &_firstTimeVisitorsStr, const string &_returningVisitorsStr);

    // Геттеры
    /** @return название дня (ссылка на внутреннюю строку) */
    [[nodiscard]] const string& getDay() const;
    /** @return номер дня недели */
    [[nodiscard]] int getDayOfWeek() const;
    /** @return дата в виде time_t */
    [[nodiscard]] time_t getDate() const;
    /** @return количество загрузок страниц */
    [[nodiscard]] int getPageLoads() const;
    /** @return количество уникальных посетителей */
    [[nodiscard]] int getUniqueVisitors() const;
    /** @return количество посетителей в первый раз */
    [[nodiscard]] int getFirstTimeVisitors() const;
    /** @return количество возвращающихся посетителей */
    [[nodiscard]] int getReturningVisitors() const;

    // Сеттеры
    /** Устанавливает название дня. */
    void setDay(const string &d);
    /** Устанавливает номер дня недели. */
    void setDayOfWeek(int d);
    /** Устанавливает номер дня недели из строки (например "1"), поддерживает удаление запятых. */
    void setDayOfWeek(const string &d);
    /** Устанавливает дату (time_t). */
    void setDate(time_t d);
    /** Устанавливает количество загрузок страниц. */
    void setPageLoads(int v);
    /** Устанавливает количество загрузок страниц из строки (например "1,234"). */
    void setPageLoads(const string &v);
    /** Устанавливает количество уникальных посетителей. */
    void setUniqueVisitors(int v);
    /** Устанавливает количество уникальных посетителей из строки (например "1,234"). */
    void setUniqueVisitors(const string &v);
    /** Устанавливает количество посетителей в первый раз. */
    void setFirstTimeVisitors(int v);
    /** Устанавливает количество посетителей в первый раз из строки (например "1,234"). */
    void setFirstTimeVisitors(const string &v);
    /** Устанавливает количество возвращающихся посетителей. */
    void setReturningVisitors(int v);
    /** Устанавливает количество возвращающихся посетителей из строки (например "1,234"). */
    void setReturningVisitors(const string &v);
};

// Оператор вывода в поток для удобного логирования/отладки
std::ostream& operator<<(std::ostream& os, const DatasetValue& dv);

#endif
