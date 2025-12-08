/**
 * @file crypt.h
 * @brief Заголовочный файл для реализации алгоритма шифрования SEED
 *
 * Содержит классы SeedKey и SeedCryptor для работы с симметричным
 * блочным шифрованием по алгоритму SEED (128-битный ключ, 128-битные блоки)
 */

#ifndef TRAFFIC_FORECAST_CRYPT_H
#define TRAFFIC_FORECAST_CRYPT_H

#include <string>
#include <vector>
#include <array>
#include <fstream>

/**
 * @class SeedKey
 * @brief Класс для управления 128-битным ключом шифрования SEED
 *
 * Предоставляет функциональность для генерации случайного ключа,
 * сохранения ключа в файл и чтения ключа из файла
 */
class SeedKey {
public:
    /// Размер ключа SEED в байтах (128 бит)
    static constexpr size_t KEY_SIZE = 16;

    /**
     * @brief Конструктор по умолчанию
     *
     * Создает объект с нулевым ключом
     */
    SeedKey();

    /**
     * @brief Конструктор с явным указанием ключа
     * @param keyData Массив байтов ключа размером KEY_SIZE
     */
    explicit SeedKey(const std::array<unsigned char, KEY_SIZE>& keyData);

    /**
     * @brief Конструктор, читающий ключ из файла
     * @param filePath Путь к файлу с ключом
     * @throws std::runtime_error если файл не может быть прочитан
     */
    explicit SeedKey(const std::string& filePath);

    /**
     * @brief Генерирует случайный 128-битный ключ
     * @return Новый объект SeedKey со случайным ключом
     */
    static SeedKey generateRandom();

    /**
     * @brief Сохраняет ключ в файл
     * @param filePath Путь к файлу для сохранения
     * @return true при успешном сохранении, false при ошибке
     */
    [[nodiscard]] bool saveToFile(const std::string& filePath) const;

    /**
     * @brief Читает ключ из файла
     * @param filePath Путь к файлу с ключом
     * @return true при успешном чтении, false при ошибке
     */
    [[nodiscard]] bool loadFromFile(const std::string& filePath);

    /**
     * @brief Возвращает данные ключа
     * @return Константная ссылка на массив байтов ключа
     */
    [[nodiscard]] const std::array<unsigned char, KEY_SIZE>& getData() const;

    /**
     * @brief Устанавливает данные ключа
     * @param keyData Массив байтов ключа размером KEY_SIZE
     */
    void setData(const std::array<unsigned char, KEY_SIZE>& keyData);

    /**
     * @brief Проверяет, инициализирован ли ключ (не нулевой)
     * @return true если ключ не нулевой, false если все байты равны нулю
     */
    [[nodiscard]] bool isValid() const;

    /**
     * @brief Сравнение двух ключей на равенство
     * @param other Другой ключ для сравнения
     * @return true если ключи равны
     */
    bool operator==(const SeedKey& other) const;

    /**
     * @brief Сравнение двух ключей на неравенство
     * @param other Другой ключ для сравнения
     * @return true если ключи не равны
     */
    bool operator!=(const SeedKey& other) const;

private:
    std::array<unsigned char, KEY_SIZE> _keyData; ///< Данные ключа (128 бит)
};

/**
 * @class SeedCryptor
 * @brief Класс для шифрования и расшифрования данных по алгоритму SEED
 *
 * Реализует симметричное блочное шифрование SEED с режимом CBC
 * (Cipher Block Chaining) и паддингом PKCS7
 */
class SeedCryptor {
public:
    /// Размер блока SEED в байтах (128 бит)
    static constexpr size_t BLOCK_SIZE = 16;

    /**
     * @brief Конструктор с ключом
     * @param key Объект SeedKey с ключом шифрования
     */
    explicit SeedCryptor(const SeedKey& key);

    /**
     * @brief Конструктор, читающий ключ из файла
     * @param keyFilePath Путь к файлу с ключом
     * @throws std::runtime_error если файл не может быть прочитан
     */
    explicit SeedCryptor(const std::string& keyFilePath);

    /**
     * @brief Шифрует строку
     * @param plaintext Исходная строка для шифрования
     * @return Строка с зашифрованными данными (IV + шифротекст)
     */
    std::string encrypt(const std::string& plaintext);

    /**
     * @brief Шифрует вектор байтов
     * @param plainData Исходные данные для шифрования
     * @return Строка с зашифрованными данными (IV + шифротекст)
     */
    std::string encrypt(const std::vector<unsigned char>& plainData);

    /**
     * @brief Расшифровывает данные из вектора байтов в строку
     * @param cipherData Зашифрованные данные (IV + шифротекст)
     * @return Расшифрованная строка
     * @throws std::runtime_error при ошибке расшифрования
     */
    std::string decrypt(const std::vector<unsigned char>& cipherData);

    /**
     * @brief Расшифровывает данные из строки в строку
     * @param cipherData Зашифрованная строка (IV + шифротекст)
     * @return Расшифрованная строка
     * @throws std::runtime_error при ошибке расшифрования
     */
    std::string decrypt(const std::string& cipherData);

    /**
     * @brief Расшифровывает данные из вектора байтов в вектор байтов
     * @param cipherData Зашифрованные данные (IV + шифротекст)
     * @return Вектор расшифрованных байтов
     * @throws std::runtime_error при ошибке расшифрования
     */
    std::vector<unsigned char> decryptToBytes(const std::vector<unsigned char>& cipherData);

    /**
     * @brief Расшифровывает данные из строки в вектор байтов
     * @param cipherData Зашифрованная строка (IV + шифротекст)
     * @return Вектор расшифрованных байтов
     * @throws std::runtime_error при ошибке расшифрования
     */
    std::vector<unsigned char> decryptToBytes(const std::string& cipherData);

    /**
     * @brief Возвращает текущий ключ
     * @return Константная ссылка на объект SeedKey
     */
    [[nodiscard]] const SeedKey& getKey() const;

    /**
     * @brief Устанавливает новый ключ
     * @param key Новый ключ шифрования
     */
    void setKey(const SeedKey& key);

private:
    SeedKey _key; ///< Ключ шифрования
    std::array<unsigned int, 32> _roundKeys; ///< Раундовые ключи

    /**
     * @brief Генерирует раундовые ключи из основного ключа
     */
    void generateRoundKeys();

    /**
     * @brief Шифрует один 128-битный блок
     * @param block Массив из 16 байт для шифрования (модифицируется на месте)
     */
    void encryptBlock(std::array<unsigned char, BLOCK_SIZE>& block);

    /**
     * @brief Расшифровывает один 128-битный блок
     * @param block Массив из 16 байт для расшифрования (модифицируется на месте)
     */
    void decryptBlock(std::array<unsigned char, BLOCK_SIZE>& block);

    /**
     * @brief Добавляет PKCS7 паддинг к данным
     * @param data Данные для дополнения
     */
    void addPadding(std::vector<unsigned char>& data);

    /**
     * @brief Удаляет PKCS7 паддинг из данных
     * @param data Данные с паддингом
     * @throws std::runtime_error при неверном паддинге
     */
    void removePadding(std::vector<unsigned char>& data);

    /**
     * @brief Генерирует случайный вектор инициализации (IV)
     * @return Массив из BLOCK_SIZE случайных байт
     */
    std::array<unsigned char, BLOCK_SIZE> generateIV();

    // Вспомогательные функции алгоритма SEED
    static unsigned int G(unsigned int x);
    static unsigned int F(unsigned int Ki0, unsigned int Ki1, unsigned int R0, unsigned int R1);
};

#endif