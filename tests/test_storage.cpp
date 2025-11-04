#include "../src/storage.h"
#include <gtest/gtest.h>

TEST(StorageTest, FlushesDataToDisk)
{
    Storage s(3);
    s.insert("vector1", {1.0f, 2.0f, 3.0f});

    std::string filepath = "test_storage.vdb";
    ErrorCode ec = s.flush_to_disk(filepath);
    EXPECT_EQ(ec, ErrorCode::None);

    Storage s2(3);
    s2.load_from_disk(filepath);

    auto vec = s2.retrieve("vector1");
    EXPECT_TRUE(vec.has_value());
    EXPECT_EQ(vec.value(), std::vector<float>({1.0f, 2.0f, 3.0f}));
}