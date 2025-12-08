#include "forecast_utils.h"

#include <ctime>
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

/**
 * @brief Выводит содержимое вектора в формате [a, b, c].
 *
 * Функция использует operator<< для каждого элемента. Поддерживает любой тип T,
 * для которого определён вывод в поток.
 */
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

/**
 * @brief Перегрузка operator<< для std::vector<T> через printVector.
 */
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

/**
 * @brief Возвращает название следующего дня недели (англ.).
 *
 * Если вход неизвестен, возвращает "Monday" по умолчанию.
 */
string nextDayString(const string& currentDay) {
    if (currentDay == "Monday") return "Tuesday";
    if (currentDay == "Tuesday") return "Wednesday";
    if (currentDay == "Wednesday") return "Thursday";
    if (currentDay == "Thursday") return "Friday";
    if (currentDay == "Friday") return "Saturday";
    if (currentDay == "Saturday") return "Sunday";
    if (currentDay == "Sunday") return "Monday";
    return "Monday";
}

/**
 * @brief Возвращает time_t, увеличенный на один день (24 часа).
 */
time_t nextDayTimeT(const time_t currentDate) {
    return currentDate + 24 * 60 * 60;
}

// TODO: add docs
Args parseArgs(const int argc, char** argv) {
    SeedKey seedKey;
    if (argc<2){
        cerr << "Использование: " << argv[0] << " <csv_path>\n";
        cerr << "Для справки используйте: " << argv[0] << " --help\n";
        return Args{"", "", 0, 0, false, false, {}, true, SeedCryptor(seedKey)};
    }
    const string path = argv[1];
    string outputPath = "forecast.csv";
    int H = 30;
    int m = 7;
    bool crypt = false;

    for (int i=1; i<argc; ++i){
        string arg = argv[i];
        if (arg == "--help" || arg == "-h"){
            return Args{"", "", 0, 0, false, true, {}, false, SeedCryptor(seedKey)};
        }
        if (arg == "--output"){
            if (i + 1 >= argc) {
                cerr << "Ошибка: отсутствует значение для параметра --output\n";
                return Args{"", "", 0, 0, false, false, {}, true, SeedCryptor(seedKey)};
            }

            outputPath = argv[++i];
        } else if (arg == "--H"){
            if (i + 1 >= argc) {
                cerr << "Ошибка: отсутствует значение для параметра --H\n";
                return Args{"", "", 0, 0, false, false, {}, true, SeedCryptor(seedKey)};
            }

            H = stoi(argv[++i]);
        } else if (arg == "--season_m"){
            if (i + 1 >= argc) {
                cerr << "Ошибка: отсутствует значение для параметра --season_m\n";
                return Args{"", "", 0, 0, false, false, {}, true, SeedCryptor(seedKey)};
            }

            m = stoi(argv[++i]);
        } else if (arg == "--crypt"){
            if (i + 1 >= argc) {
                cerr << "Ошибка: отсутствует значение для параметра --crypt\n";
                return Args{"", "", 0, 0, false, false, {}, true, SeedCryptor(seedKey)};
            }

            string keyStr = argv[++i];
            seedKey = SeedKey(keyStr);
            crypt = true;
        } else if (arg == "--newCryptKey") {
            if (i + 1 >= argc) {
                cerr << "Ошибка: отсутствует значение для параметра --newCryptKey\n";
                return Args{"", "", 0, 0, false, false, {}, true, SeedCryptor(seedKey)};
            }

            string keyFilePath = argv[++i];
            seedKey = SeedKey::generateRandom();
            crypt = true;
            if (seedKey.saveToFile(keyFilePath)) {
                cout << "Новый ключ шифрования сохранён в " << keyFilePath << endl;
            } else {
                cerr << "Ошибка при сохранении ключа в " << keyFilePath << endl;
                return Args{"", "", 0, 0, false, false, {}, true, SeedCryptor(seedKey)};
            }
        }
    }

    return Args{
        path,
        outputPath,
        H,
        m,
        crypt,
        false,
        seedKey,
        false,
        SeedCryptor(seedKey)
    };
}
