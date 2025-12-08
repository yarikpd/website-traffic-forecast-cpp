/**
 * @file test_crypt.cpp
 * @brief Модульные тесты для классов SeedKey и SeedCryptor
 *
 * Содержит тесты для проверки корректности генерации ключей,
 * сохранения/загрузки из файлов, шифрования и расшифрования
 */

#include "crypt.h"
#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <sstream>

// ============================================================================
// Тесты для класса SeedKey
// ============================================================================

/**
 * @brief Тест конструктора по умолчанию
 *
 * Проверяет, что ключ инициализируется нулями
 */
TEST(SeedKeyTest, DefaultConstructor) {
    SeedKey key;
    const auto& data = key.getData();

    for (size_t i = 0; i < SeedKey::KEY_SIZE; ++i) {
        EXPECT_EQ(data[i], 0) << "Байт " << i << " должен быть нулевым";
    }

    EXPECT_FALSE(key.isValid()) << "Нулевой ключ должен быть невалидным";
}

/**
 * @brief Тест конструктора с массивом данных
 *
 * Проверяет, что ключ корректно инициализируется переданными данными
 */
TEST(SeedKeyTest, ArrayConstructor) {
    std::array<unsigned char, SeedKey::KEY_SIZE> testData = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
    };

    SeedKey key(testData);
    EXPECT_EQ(key.getData(), testData);
    EXPECT_TRUE(key.isValid());
}

/**
 * @brief Тест генерации случайного ключа
 *
 * Проверяет, что сгенерированный ключ валиден и уникален
 */
TEST(SeedKeyTest, GenerateRandom) {
    SeedKey key1 = SeedKey::generateRandom();
    SeedKey key2 = SeedKey::generateRandom();

    EXPECT_TRUE(key1.isValid()) << "Сгенерированный ключ должен быть валидным";
    EXPECT_TRUE(key2.isValid()) << "Сгенерированный ключ должен быть валидным";

    // С высокой вероятностью два случайных ключа будут разными
    EXPECT_NE(key1, key2) << "Два случайных ключа должны быть разными";
}

/**
 * @brief Тест сохранения и загрузки ключа из файла
 *
 * Проверяет корректность сохранения и последующей загрузки ключа
 */
TEST(SeedKeyTest, SaveAndLoadFromFile) {
    const std::string testFilePath = "test_key.bin";

    // Создаем и сохраняем ключ
    SeedKey originalKey = SeedKey::generateRandom();
    ASSERT_TRUE(originalKey.saveToFile(testFilePath)) << "Не удалось сохранить ключ";

    // Загружаем ключ
    SeedKey loadedKey;
    ASSERT_TRUE(loadedKey.loadFromFile(testFilePath)) << "Не удалось загрузить ключ";

    EXPECT_EQ(originalKey, loadedKey) << "Загруженный ключ должен совпадать с оригинальным";

    // Удаляем тестовый файл
    std::filesystem::remove(testFilePath);
}

/**
 * @brief Тест конструктора с путём к файлу
 *
 * Проверяет, что ключ корректно загружается из файла через конструктор
 */
TEST(SeedKeyTest, FilePathConstructor) {
    const std::string testFilePath = "test_key_constructor.bin";

    // Создаем и сохраняем ключ
    SeedKey originalKey = SeedKey::generateRandom();
    ASSERT_TRUE(originalKey.saveToFile(testFilePath));

    // Загружаем через конструктор
    SeedKey loadedKey(testFilePath);

    EXPECT_EQ(originalKey, loadedKey);

    // Удаляем тестовый файл
    std::filesystem::remove(testFilePath);
}

/**
 * @brief Тест исключения при загрузке несуществующего файла
 */
TEST(SeedKeyTest, FilePathConstructorThrowsOnMissingFile) {
    EXPECT_THROW(SeedKey("nonexistent_file.bin"), std::runtime_error);
}

/**
 * @brief Тест сеттера данных ключа
 */
TEST(SeedKeyTest, SetData) {
    SeedKey key;
    std::array<unsigned char, SeedKey::KEY_SIZE> testData = {
        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11, 0x22,
        0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0x00
    };

    key.setData(testData);
    EXPECT_EQ(key.getData(), testData);
}

/**
 * @brief Тест операторов сравнения
 */
TEST(SeedKeyTest, ComparisonOperators) {
    std::array<unsigned char, SeedKey::KEY_SIZE> data1 = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
    };
    std::array<unsigned char, SeedKey::KEY_SIZE> data2 = {
        0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80,
        0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0, 0x00
    };

    SeedKey key1(data1);
    SeedKey key2(data1);
    SeedKey key3(data2);

    EXPECT_TRUE(key1 == key2);
    EXPECT_FALSE(key1 != key2);
    EXPECT_TRUE(key1 != key3);
    EXPECT_FALSE(key1 == key3);
}

/**
 * @brief Тест загрузки из несуществующего файла
 */
TEST(SeedKeyTest, LoadFromNonExistentFile) {
    SeedKey key;
    EXPECT_FALSE(key.loadFromFile("this_file_does_not_exist.bin"));
}

// ============================================================================
// Тесты для класса SeedCryptor
// ============================================================================

/**
 * @brief Тест шифрования и расшифрования строки
 *
 * Проверяет, что расшифрованная строка совпадает с оригинальной
 */
TEST(SeedCryptorTest, EncryptDecryptString) {
    SeedKey key = SeedKey::generateRandom();
    SeedCryptor cryptor(key);

    std::string originalText = "Привет, мир! Hello, World! 12345";

    auto encrypted = cryptor.encrypt(originalText);
    std::string decrypted = cryptor.decrypt(encrypted);

    EXPECT_EQ(originalText, decrypted);
}

/**
 * @brief Тест шифрования и расшифрования бинарных данных
 */
TEST(SeedCryptorTest, EncryptDecryptBytes) {
    SeedKey key = SeedKey::generateRandom();
    SeedCryptor cryptor(key);

    std::vector<unsigned char> originalData = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                                          0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

    auto encrypted = cryptor.encrypt(originalData);
    auto decrypted = cryptor.decryptToBytes(encrypted);

    EXPECT_EQ(originalData, decrypted);
}

/**
 * @brief Тест шифрования пустой строки
 */
TEST(SeedCryptorTest, EncryptDecryptEmptyString) {
    SeedKey key = SeedKey::generateRandom();
    SeedCryptor cryptor(key);

    std::string originalText = "";

    auto encrypted = cryptor.encrypt(originalText);
    std::string decrypted = cryptor.decrypt(encrypted);

    EXPECT_EQ(originalText, decrypted);
}

/**
 * @brief Тест шифрования строки размером ровно в один блок
 */
TEST(SeedCryptorTest, EncryptDecryptOneBlock) {
    SeedKey key = SeedKey::generateRandom();
    SeedCryptor cryptor(key);

    // Строка ровно 16 байт (один блок)
    std::string originalText = "0123456789ABCDEF";

    auto encrypted = cryptor.encrypt(originalText);
    std::string decrypted = cryptor.decrypt(encrypted);

    EXPECT_EQ(originalText, decrypted);
}

/**
 * @brief Тест шифрования длинной строки (несколько блоков)
 */
TEST(SeedCryptorTest, EncryptDecryptLongString) {
    SeedKey key = SeedKey::generateRandom();
    SeedCryptor cryptor(key);

    std::string originalText;
    for (int i = 0; i < 1000; ++i) {
        originalText += "Test message " + std::to_string(i) + "\n";
    }

    auto encrypted = cryptor.encrypt(originalText);
    std::string decrypted = cryptor.decrypt(encrypted);

    EXPECT_EQ(originalText, decrypted);
}

/**
 * @brief Тест: разные ключи дают разные результаты
 */
TEST(SeedCryptorTest, DifferentKeysProduceDifferentCiphertext) {
    SeedKey key1 = SeedKey::generateRandom();
    SeedKey key2 = SeedKey::generateRandom();

    // Убедимся, что ключи разные
    ASSERT_NE(key1, key2);

    SeedCryptor cryptor1(key1);
    SeedCryptor cryptor2(key2);

    std::string text = "Secret message";

    auto encrypted1 = cryptor1.encrypt(text);
    auto encrypted2 = cryptor2.encrypt(text);

    // Зашифрованные данные должны отличаться (с высокой вероятностью)
    // Сравниваем только шифротекст (без IV, который первые 16 байт)
    std::vector<unsigned char> cipher1(encrypted1.begin() + 16, encrypted1.end());
    std::vector<unsigned char> cipher2(encrypted2.begin() + 16, encrypted2.end());

    EXPECT_NE(cipher1, cipher2);
}

/**
 * @brief Тест: расшифровка с неправильным ключом не работает
 */
TEST(SeedCryptorTest, DecryptWithWrongKeyFails) {
    SeedKey key1 = SeedKey::generateRandom();
    SeedKey key2 = SeedKey::generateRandom();

    ASSERT_NE(key1, key2);

    SeedCryptor cryptor1(key1);
    SeedCryptor cryptor2(key2);

    std::string originalText = "Secret message";
    auto encrypted = cryptor1.encrypt(originalText);

    // Попытка расшифровать другим ключом
    // Может выбросить исключение из-за неверного паддинга или вернуть мусор
    try {
        std::string decrypted = cryptor2.decrypt(encrypted);
        // Если не выбросило исключение, проверяем что результат неверный
        EXPECT_NE(originalText, decrypted);
    } catch (const std::runtime_error&) {
        // Ожидаемое поведение - ошибка паддинга
        SUCCEED();
    }
}

/**
 * @brief Тест конструктора с путём к файлу ключа
 */
TEST(SeedCryptorTest, FilePathConstructor) {
    const std::string testFilePath = "test_cryptor_key.bin";

    SeedKey key = SeedKey::generateRandom();
    ASSERT_TRUE(key.saveToFile(testFilePath));

    SeedCryptor cryptor(testFilePath);

    EXPECT_EQ(cryptor.getKey(), key);

    std::filesystem::remove(testFilePath);
}

/**
 * @brief Тест геттера и сеттера ключа
 */
TEST(SeedCryptorTest, GetSetKey) {
    SeedKey key1 = SeedKey::generateRandom();
    SeedKey key2 = SeedKey::generateRandom();

    ASSERT_NE(key1, key2);

    SeedCryptor cryptor(key1);
    EXPECT_EQ(cryptor.getKey(), key1);

    cryptor.setKey(key2);
    EXPECT_EQ(cryptor.getKey(), key2);

    // Проверяем, что шифрование работает с новым ключом
    std::string text = "Test message";
    auto encrypted = cryptor.encrypt(text);
    std::string decrypted = cryptor.decrypt(encrypted);
    EXPECT_EQ(text, decrypted);
}

/**
 * @brief Тест: каждое шифрование одного и того же текста даёт разный результат (из-за IV)
 */
TEST(SeedCryptorTest, SameTextDifferentCiphertext) {
    SeedKey key = SeedKey::generateRandom();
    SeedCryptor cryptor(key);

    std::string text = "Same message";

    auto encrypted1 = cryptor.encrypt(text);
    auto encrypted2 = cryptor.encrypt(text);

    // Зашифрованные данные должны отличаться из-за случайного IV
    EXPECT_NE(encrypted1, encrypted2);

    // Но оба должны корректно расшифровываться
    EXPECT_EQ(text, cryptor.decrypt(encrypted1));
    EXPECT_EQ(text, cryptor.decrypt(encrypted2));
}

/**
 * @brief Тест расшифровки слишком коротких данных
 */
TEST(SeedCryptorTest, DecryptTooShortDataThrows) {
    SeedKey key = SeedKey::generateRandom();
    SeedCryptor cryptor(key);

    // Данные короче, чем IV + один блок
    std::vector<unsigned char> shortData = {0x01, 0x02, 0x03};

    EXPECT_THROW(cryptor.decrypt(shortData), std::runtime_error);
}

/**
 * @brief Тест расшифровки данных неправильного размера
 */
TEST(SeedCryptorTest, DecryptInvalidSizeThrows) {
    SeedKey key = SeedKey::generateRandom();
    SeedCryptor cryptor(key);

    // Данные не кратные размеру блока (после IV)
    std::vector<unsigned char> invalidData(35); // 16 (IV) + 19 (не кратно 16)

    EXPECT_THROW(cryptor.decrypt(invalidData), std::runtime_error);
}

/**
 * @brief Тест шифрования строки с Unicode символами
 */
TEST(SeedCryptorTest, EncryptDecryptUnicode) {
    SeedKey key = SeedKey::generateRandom();
    SeedCryptor cryptor(key);

    std::string originalText = "Привет мир! 你好世界! مرحبا بالعالم! 🌍🌎🌏";

    auto encrypted = cryptor.encrypt(originalText);
    std::string decrypted = cryptor.decrypt(encrypted);

    EXPECT_EQ(originalText, decrypted);
}

/**
 * @brief Тест шифрования данных с нулевыми байтами
 */
TEST(SeedCryptorTest, EncryptDecryptWithNullBytes) {
    SeedKey key = SeedKey::generateRandom();
    SeedCryptor cryptor(key);

    std::vector<unsigned char> originalData = {0x00, 0x00, 0x00, 0xFF, 0x00, 0x00};

    auto encrypted = cryptor.encrypt(originalData);
    auto decrypted = cryptor.decryptToBytes(encrypted);

    EXPECT_EQ(originalData, decrypted);
}

/**
 * @brief Тест: размер зашифрованных данных корректен
 */
TEST(SeedCryptorTest, EncryptedDataSizeIsCorrect) {
    SeedKey key = SeedKey::generateRandom();
    SeedCryptor cryptor(key);

    // Для строки длиной N байт, зашифрованные данные должны быть:
    // IV (16 байт) + ceiling((N + padding) / 16) * 16 байт

    // Тест 1: 10 байт -> паддинг 6 -> итого 16 байт шифротекста
    std::string text10 = "1234567890";
    auto encrypted10 = cryptor.encrypt(text10);
    EXPECT_EQ(encrypted10.size(), 16 + 16); // IV + 1 блок

    // Тест 2: 16 байт -> паддинг 16 (полный блок) -> итого 32 байта шифротекста
    std::string text16 = "1234567890123456";
    auto encrypted16 = cryptor.encrypt(text16);
    EXPECT_EQ(encrypted16.size(), 16 + 32); // IV + 2 блока

    // Тест 3: 17 байт -> паддинг 15 -> итого 32 байта шифротекста
    std::string text17 = "12345678901234567";
    auto encrypted17 = cryptor.encrypt(text17);
    EXPECT_EQ(encrypted17.size(), 16 + 32); // IV + 2 блока
}

