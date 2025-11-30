#include "Dataset.h"
#include <fstream>
#include <sstream>

/**
 * @brief Добавить запись в набор данных.
 *
 * Переданный объект DatasetValue перемещается во внутренний контейнер.
 *
 * @param d Добавляемый DatasetValue (перемещается).
 */
void Dataset::addRow(DatasetValue d) {
    rows.push_back(std::move(d));
}

/**
 * @brief Удалить все записи из набора данных.
 */
void Dataset::clearRows() {
    rows.clear();
}

/**
 * @brief Вернуть константную ссылку на хранимые записи.
 *
 * @return Константная ссылка на вектор объектов DatasetValue.
 */
const vector<DatasetValue>& Dataset::getRows() const {
    return rows;
}

/**
 * @brief Заменить хранимые записи на заданный вектор.
 *
 * Параметр перемещается во внутреннее хранилище.
 *
 * @param r Вектор объектов DatasetValue, передаваемый в собственность.
 */
void Dataset::setRows(vector<DatasetValue> r) {
    rows = std::move(r);
}

/**
 * @brief Загрузить записи набора данных из CSV-файла.
 *
 * Функция ожидает, что первая строка — заголовок и пропускает её.
 * Каждая последующая строка должна содержать следующие поля через запятую:
 * row,day,dayOfWeek,date,pageLoads,uniqueVisitors,firstTimeVisitors,returningVisitors
 *
 * Числовые поля могут передаваться как строки и будут распарсены
 * соответствующими конструкторами DatasetValue. Строки с неправильным
 * количеством полей будут проигнорированы.
 *
 * @param filename Путь к CSV-файлу для чтения. Если файл не может быть
 * открыт, набор данных останется пустым.
 */
void Dataset::fromCSV(const string &filename) {
    rows.clear();
    std::ifstream file(filename);
    string line;

    std::getline(file, line);
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        string day, dateStr, pageLoadsStr, uniqueVisitorsStr, firstTimeVisitorsStr, returningVisitorsStr, row, dayOfWeek;

        if (std::getline(iss, row, ',') &&
            std::getline(iss, day, ',') &&
            std::getline(iss, dayOfWeek, ',') &&
            std::getline(iss, dateStr, ',') &&
            std::getline(iss, pageLoadsStr, ',') &&
            std::getline(iss, uniqueVisitorsStr, ',') &&
            std::getline(iss, firstTimeVisitorsStr, ',') &&
            std::getline(iss, returningVisitorsStr, ',')) {

            DatasetValue dv(day, stoi(dayOfWeek), dateStr, pageLoadsStr, uniqueVisitorsStr, firstTimeVisitorsStr,
                            returningVisitorsStr);
            rows.push_back(std::move(dv));
        }
    }

    file.close();
}

/**
 * @brief Оператор вывода для Dataset.
 *
 * Записывает каждую сохранённую запись DatasetValue, по одной на строку.
 *
 * @param os Поток вывода.
 * @param dv Набор данных для сериализации.
 * @return Ссылка на поток вывода.
 */
std::ostream& operator<<(std::ostream& os, const Dataset& dv) {
    for (const auto& row : dv.getRows()) {
        os << row << std::endl;
    }
    return os;
}