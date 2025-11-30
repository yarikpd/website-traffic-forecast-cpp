#include "DatasetValue.h"
#include <gtest/gtest.h>
#include <sstream>

// Проверка парсинга чисел со строчек с запятыми через конструктор
TEST(DatasetValueTest, ParseNumberStrings) {
    DatasetValue dv("Mon", 1, time_t(0), std::string("1,234"), std::string("2,345"), std::string("100"), std::string("200"));
    EXPECT_EQ(dv.getPageLoads(), 1234);
    EXPECT_EQ(dv.getUniqueVisitors(), 2345);
    EXPECT_EQ(dv.getFirstTimeVisitors(), 100);
    EXPECT_EQ(dv.getReturningVisitors(), 200);
}

// Проверка парсинга даты из строки
TEST(DatasetValueTest, ParseDateString) {
    // 2020-12-31
    DatasetValue dv("Thu", 4, std::string("12/31/2020"), 10, 20, 5, 5);
    time_t t = dv.getDate();
    ASSERT_NE(t, time_t(0));
    std::tm tm{};
#ifdef _POSIX_VERSION
    localtime_r(&t, &tm);
#else
    std::tm *ptm = std::localtime(&t);
    ASSERT_NE(ptm, nullptr);
    tm = *ptm;
#endif
    EXPECT_EQ(tm.tm_year + 1900, 2020);
    EXPECT_EQ(tm.tm_mon + 1, 12);
    EXPECT_EQ(tm.tm_mday, 31);
}

// Проверка геттеров/сеттеров
TEST(DatasetValueTest, GettersSetters) {
    DatasetValue dv("Wed", 3, time_t(0), 1, 2, 3, 4);
    dv.setDay("Fri");
    dv.setDayOfWeek(5);
    dv.setPageLoads(555);
    dv.setUniqueVisitors(666);
    dv.setFirstTimeVisitors(77);
    dv.setReturningVisitors(88);

    EXPECT_EQ(dv.getDay(), "Fri");
    EXPECT_EQ(dv.getDayOfWeek(), 5);
    EXPECT_EQ(dv.getPageLoads(), 555);
    EXPECT_EQ(dv.getUniqueVisitors(), 666);
    EXPECT_EQ(dv.getFirstTimeVisitors(), 77);
    EXPECT_EQ(dv.getReturningVisitors(), 88);
}

// Проверка оператора вывода
TEST(DatasetValueTest, OutputOperator) {
    DatasetValue dv("Sun", 0, std::string("01/02/2003"), 10, 20, 30, 40);
    std::ostringstream oss;
    oss << dv;
    std::string s = oss.str();
    // содержит дату в формате YYYY-MM-DD и имя дня и ключи
    EXPECT_NE(s.find("2003-01-02"), std::string::npos);
    EXPECT_NE(s.find("Sun"), std::string::npos);
    EXPECT_NE(s.find("pageLoads=10"), std::string::npos);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

