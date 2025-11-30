#include "Dataset.h"
#include "DatasetValue.h"
#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include <cstdio>

// Тест добавления строки и доступа к rows
TEST(DatasetTest, AddRowAndGetRows) {
    Dataset ds;
    DatasetValue dv("Wed", 3, time_t(0), 1, 2, 3, 4);
    ds.addRow(dv);

    const auto &rows = ds.getRows();
    ASSERT_EQ(rows.size(), 1u);
    EXPECT_EQ(rows[0].getDay(), "Wed");
    EXPECT_EQ(rows[0].getDayOfWeek(), 3);
    EXPECT_EQ(rows[0].getPageLoads(), 1);
}

// Тест чтения из CSV (создаём временный файл)
TEST(DatasetTest, FromCSV) {
    const char *fname = "tmp_dataset_test.csv";
    std::ofstream ofs(fname);
    // header (пропускается в fromCSV)
    ofs << "row,day,dayOfWeek,date,pageLoads,uniqueVisitors,firstTimeVisitors,returningVisitors\n";
    // запишем одну строку
    ofs << "1,Mon,1,12/31/2020,100,200,10,20\n";
    ofs.close();

    Dataset ds;
    ds.fromCSV(fname);

    const auto &rows = ds.getRows();
    ASSERT_EQ(rows.size(), 1u);
    const auto &r = rows[0];
    EXPECT_EQ(r.getDay(), "Mon");
    EXPECT_EQ(r.getDayOfWeek(), 1);
    EXPECT_EQ(r.getPageLoads(), 100);
    EXPECT_EQ(r.getUniqueVisitors(), 200);
    EXPECT_EQ(r.getFirstTimeVisitors(), 10);
    EXPECT_EQ(r.getReturningVisitors(), 20);

    // проверим дату
    time_t t = r.getDate();
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

    // cleanup
    std::remove(fname);
}

// Тест очистки строк
TEST(DatasetTest, ClearRows) {
    Dataset ds;
    ds.addRow(DatasetValue("Tue", 2, time_t(0), 1, 1, 1, 1));
    ds.addRow(DatasetValue("Wed", 3, time_t(0), 2, 2, 2, 2));
    ASSERT_GT(ds.getRows().size(), 0u);
    ds.clearRows();
    EXPECT_EQ(ds.getRows().size(), 0u);
}

// Тест оператора вывода
TEST(DatasetTest, OutputOperator) {
    Dataset ds;
    ds.addRow(DatasetValue("Sun", 0, std::string("01/02/2003"), 10, 20, 30, 40));
    std::ostringstream oss;
    oss << ds;
    std::string s = oss.str();
    EXPECT_NE(s.find("Sun"), std::string::npos);
    EXPECT_NE(s.find("2003-01-02"), std::string::npos);
    EXPECT_NE(s.find("pageLoads=10"), std::string::npos);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

