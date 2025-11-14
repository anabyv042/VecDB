#include <gtest/gtest.h>
#include "../../src/error.h"
#include "../../src/storage/page.h"

TEST(PageTest, Insert)
{
    Page p = Page(1, 1);
    uint64_t id = 0;
    std::vector<float> vector = {1.0f};
    p.insert_vector(id, vector);
    auto vec = p.get_vector(id);
    EXPECT_EQ(vec, vector);
}

TEST(PageTest, InsertInvalidDimensionality)
{
    Page p = Page(1, 2);
    uint64_t id = 0;
    std::vector<float> vector = {1.0f};
    EXPECT_THROW(p.insert_vector(id, vector), error::DimensionalityMismatch);
}

TEST(PageTest, InsertNotEnoughSpace)
{
    Page p = Page(1, 512); // each vector takes 2KB + metadata, so only one can fit in a 4KB page
    uint64_t id1 = 0;
    std::vector<float> vector1(512, 1.0f);
    p.insert_vector(id1, vector1);

    uint64_t id2 = 1;
    std::vector<float> vector2(512, 2.0f);
    EXPECT_THROW(p.insert_vector(id2, vector2), error::NotEnoughSpace);
}

TEST(PageTest, GetVectorNotFound)
{
    Page p = Page(1, 1);
    EXPECT_THROW(p.get_vector(999), error::NotFound);
}

TEST(PageTest, HasSpace)
{
    Page p = Page(1, 512); // each vector takes 2KB + metadata, so only one can fit in a 4KB page
    EXPECT_TRUE(p.has_space());
    uint64_t id = 0;
    std::vector<float> vector(512, 1.0f);
    p.insert_vector(id, vector);
    EXPECT_FALSE(p.has_space());
}

TEST(PageTest, GetPageId)
{
    Page p = Page(42, 1);
    EXPECT_EQ(p.get_page_id(), 42);
}

TEST(PageTest, FlushToDiskAndLoad)
{
    uint32_t pageId = 1;
    Page p = Page(pageId, 1);
    uint64_t id = 0;
    std::vector<float> vector = {1.0f};
    p.insert_vector(id, vector);

    uint64_t id2 = 1;
    std::vector<float> vector2 = {2.0f};
    p.insert_vector(id2, vector2);

    std::string filepath = "temp_page.dat";
    p.flush_to_disk(filepath);

    Page p2 = Page(filepath);
    EXPECT_EQ(p2.get_page_id(), pageId);

    auto vec = p2.get_vector(id);
    EXPECT_EQ(vec, vector);

    auto vec2 = p2.get_vector(id2);
    EXPECT_EQ(vec2, vector2);
}