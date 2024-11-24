#include <gtest/gtest.h>
#include <random>
#include "sat.h"

TEST(PageAllocTest, CreateSetsPropertiesCorrectly) {
    /*PageAlloc page_alloc = default_page_alloc();

    EXPECT_EQ(page_alloc_page_count_get(&page_alloc), PAGE_COUNT);
    EXPECT_EQ(page_alloc_page_size_get(&page_alloc), PAGE_SIZE);
    EXPECT_EQ(page_alloc_free_block_count_get(&page_alloc), PAGE_COUNT);
    EXPECT_EQ(page_alloc_allocated_count_get(&page_alloc), 0);*/
}

TEST(PageAllocDeathTest, CreateBadPageSize) {
    /*ASSERT_DEATH(page_alloc_create(0, PAGE_COUNT), MATCHER_ANY);*/
}