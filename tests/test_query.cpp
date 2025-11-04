#include "../src/query.h"
#include <gtest/gtest.h>

TEST(QueryTest, Insert)
{
    Query q;
    std::string id = "test_id";
    std::vector<float> vector = {1.0f};
    q.insert(id, vector);
    auto vec = q.get_by_id(id);
    EXPECT_TRUE(vec.has_value());
    EXPECT_EQ(vec.value(), vector);
}

TEST(QueryTest, InsertTwice)
{
    Query q;
    std::string id = "test_id";
    std::vector<float> vector1 = {1.0f};
    std::vector<float> vector2 = {2.0f};
    q.insert(id, vector1);
    q.insert(id, vector2);
    auto vec = q.get_by_id(id);
    EXPECT_TRUE(vec.has_value());
    EXPECT_EQ(vec.value(), vector2);
}

TEST(QueryTest, GetByIdNotFound)
{
    Query q;
    auto vec = q.get_by_id("non_existent_id");
    EXPECT_FALSE(vec.has_value());
}