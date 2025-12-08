#ifndef TRAFFIC_FORECAST_UTILS_H
#define TRAFFIC_FORECAST_UTILS_H

#include <string>
#include <vector>
#include <ostream>

#include "crypt.h"

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

/**
 * @brief Возвращает название следующего дня недели на английском.
 *
 * Ожидается, что входная строка содержит одно из значений:
 * Monday, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday.
 * Если значение неизвестно, возвращается "Monday" по умолчанию.
 *
 * @param currentDay Название текущего дня недели на английском (передаётся по ссылке).
 * @return Название следующего дня недели на английском.
 */
string nextDayString(const string& currentDay);

/**
 * @brief Прибавляет к времени один календарный день (24 часа).
 *
 * @param currentDate Временная метка time_t.
 * @return time_t, соответствующий следующему дню (currentDate + 24*60*60).
 */
time_t nextDayTimeT(time_t currentDate);

/**
 * @brief Структура для хранения аргументов командной строки.
 *
 * Содержит все параметры, переданные программе через командную строку,
 * включая пути к файлам, параметры прогнозирования и настройки шифрования.
 */
struct Args {
    string csv_path;              ///< Путь к входному CSV файлу с данными
    string output_path;           ///< Путь к выходному файлу для сохранения прогноза
    int H;                        ///< Горизонт прогнозирования (количество точек)
    int season_m;                 ///< Длина сезона для экспоненциального сглаживания
    bool decrypt;                 ///< Флаг режима расшифровки файла
    string decrypt_output_path;   ///< Путь к выходному файлу при расшифровке
    bool encrypt_file;            ///< Флаг режима шифрования файла
    string encrypt_output_path;   ///< Путь к выходному файлу при шифровании
    bool help;                    ///< Флаг запроса справки
    SeedKey crypt_key;            ///< Ключ шифрования SEED
    bool has_error;               ///< Флаг ошибки при парсинге аргументов
    SeedCryptor cryptor;          ///< Объект криптора для шифрования/расшифровки
};

/**
 * @brief Разбирает аргументы командной строки.
 *
 * Обрабатывает следующие параметры:
 * - csv_path: путь к входному CSV файлу (обязательный)
 * - --output <path>: путь к выходному файлу (по умолчанию "forecast.csv")
 * - --H <n>: горизонт прогнозирования (по умолчанию 30)
 * - --season_m <n>: длина сезона (по умолчанию 7)
 * - --crypt <key_file>: путь к файлу с ключом шифрования
 * - --newCryptKey <key_file>: генерация нового ключа и сохранение в файл
 * - --decrypt <output_file>: расшифровка файла csv_path в output_file
 * - --encrypt <output_file>: шифрование файла csv_path в output_file
 * - --help, -h: вывод справки
 *
 * @param argc Количество аргументов командной строки
 * @param argv Массив строк аргументов командной строки
 * @return Структура Args с разобранными параметрами
 */
Args parseArgs(int argc, char** argv);

#endif