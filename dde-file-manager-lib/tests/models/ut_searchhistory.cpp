#include "models/searchhistory.h"

#include <gtest/gtest.h>

namespace {
class TestSearchHistory : public testing::Test
{
public:
    void SetUp() override
    {
        std::cout << "start TestSearchHistory";
        p = new SearchHistory;
    }

    void TearDown() override
    {
        std::cout << "end TestSearchHistory";
    }

public:
    SearchHistory *p;
};
} // namespace

TEST_F(TestSearchHistory, searchDateTime)
{
    QDateTime dt = QDateTime::currentDateTime();
    p->setDateTime(dt);
    EXPECT_STREQ(dt.toString().toStdString().c_str(), p->getDateTime().toString().toStdString().c_str());
}

TEST_F(TestSearchHistory, searchKeyword)
{
    QString key = "test";
    p->setKeyword(key);
    EXPECT_STREQ("test", p->getKeyword().toStdString().c_str());
}
