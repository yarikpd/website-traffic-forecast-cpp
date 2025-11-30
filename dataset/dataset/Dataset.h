#ifndef TRAFFIC_FORECAST_DATASET_H
#define TRAFFIC_FORECAST_DATASET_H

#include <vector>
#include <string>

#include "DatasetValue.h"

using namespace std;

/**
 * @brief Представляет коллекцию записей набора данных.
 *
 * Класс Dataset хранит несколько объектов DatasetValue и предоставляет
 * вспомогательные методы для управления коллекцией и загрузки из CSV-файлов.
 */
class Dataset {
    vector<DatasetValue> rows;

public:
    /**
     * @brief Добавить запись в набор данных.
     *
     * Переданный объект DatasetValue перемещается во внутренний контейнер.
     *
     * @param d Добавляемый DatasetValue (перемещается).
     */
    void addRow(DatasetValue d);

    /**
     * @brief Удалить все записи из набора данных.
     */
    void clearRows();

    /**
     * @brief Вернуть константную ссылку на хранимые записи.
     *
     * @return Константная ссылка на вектор объектов DatasetValue.
     */
    [[nodiscard]] const vector<DatasetValue>& getRows() const;

    /**
     * @brief Заменить хранимые записи на заданный вектор.
     *
     * Параметр перемещается во внутреннее хранилище.
     *
     * @param rows Вектор объектов DatasetValue, передаваемый в собственность.
     */
    void setRows(vector<DatasetValue> rows);

    /**
     * @brief Загрузить записи набора данных из CSV-файла.
     *
     * Функция ожидает, что первая строка — заголовок и пропускает её.
     * Каждая последующая строка должна содержать следующие поля через запятую:
     * row,day,dayOfWeek,date,pageLoads,uniqueVisitors,firstTimeVisitors,returningVisitors
     *
     * Числовые поля могут передаваться как строки и будут распарсены
     * соответствующими конструкторами DatasetValue.
     *
     * @param filename Путь к CSV-файлу для чтения.
     */
    void fromCSV(const string &filename);
};

/**
 * @brief Оператор вывода для Dataset.
 *
 * Записывает каждую сохранённую запись DatasetValue, по одной на строку.
 *
 * @param os Поток вывода.
 * @param dv Набор данных для сериализации.
 * @return Ссылка на поток вывода.
 */
std::ostream& operator<<(std::ostream& os, const Dataset& dv);

#endif