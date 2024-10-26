#include "MinMaxHeap.h"
#include "gtest/gtest.h"

TEST(MinMaxHeap, ParentTest) {
    ASSERT_TRUE(MinMaxHeap_Trait::parent(0) == 0);
    ASSERT_TRUE(MinMaxHeap_Trait::parent(1) == 0);
    ASSERT_TRUE(MinMaxHeap_Trait::parent(2) == 0);

    ASSERT_TRUE(MinMaxHeap_Trait::parent(3) == 1);
    ASSERT_TRUE(MinMaxHeap_Trait::parent(4) == 1);

    ASSERT_TRUE(MinMaxHeap_Trait::parent(5) == 2);
    ASSERT_TRUE(MinMaxHeap_Trait::parent(6) == 2);
}

TEST(MinMaxHeap, ChildTest) {
    ASSERT_TRUE(MinMaxHeap_Trait::leftChild(0) == 1);
    ASSERT_TRUE(MinMaxHeap_Trait::rightChild(0) == 2);

    ASSERT_TRUE(MinMaxHeap_Trait::leftChild(1) == 3);
    ASSERT_TRUE(MinMaxHeap_Trait::rightChild(1) == 4);

    ASSERT_TRUE(MinMaxHeap_Trait::leftChild(2) == 5);
    ASSERT_TRUE(MinMaxHeap_Trait::rightChild(2) == 6);
}

TEST(MinMaxHeap, NodeTest) {
    for (auto i : {0, 3, 4, 5, 6})
        ASSERT_TRUE(MinMaxHeap_Trait::isMinNode(i));

    for (auto i : {1, 2, 7, 8, 9, 10, 11, 12, 13, 14})
        ASSERT_FALSE(MinMaxHeap_Trait::isMinNode(i));

    ASSERT_FALSE(MinMaxHeap_Trait::isMinNode(UINT64_MAX - 1));
}

TEST(MinMaxHeap, popTest) {
    // 1, 3, 3, 6, 8, 9
    MinMaxHeap mmheap {9, 1, 6, 3, 3, 8};

    ASSERT_TRUE(mmheap.popMin() == 1);
    ASSERT_TRUE(mmheap.popMin() == 3);
    ASSERT_TRUE(mmheap.popMin() == 3);
    ASSERT_TRUE(mmheap.popMax() == 9);
    ASSERT_TRUE(mmheap.popMax() == 8);
    ASSERT_TRUE(mmheap.popMax() == 6);

    ASSERT_THROW(mmheap.popMin(), std::out_of_range);
    ASSERT_THROW(mmheap.popMax(), std::out_of_range);
}

TEST(MinMaxHeap, popTest2) {
    std::vector<int> vec;

    unsigned seed = time(NULL);
    std::cerr << "Random seed = " << seed;
    srand(seed);

    for (int i = 0; i < 100; ++i) vec.push_back(rand() % 10);

    MinMaxHeap mmheap(vec.begin(), vec.end());
    ASSERT_TRUE(mmheap.size() == 100);

    int minV = INT_MIN, maxV = INT_MAX;

    for (int i = 0; i < 100; ++i) {
        if (i & 1) {
            int m = mmheap.popMin();
            ASSERT_TRUE(minV <= m && m <= maxV);
            minV = m;
        }
        else {
            int m = mmheap.popMax();
            ASSERT_TRUE(minV <= m && m <= maxV);
            maxV = m;
        }
    }
}

TEST(MinMaxHeap, pushTest) {
    MinMaxHeap mmheap;
    unsigned seed = rand();
    std::cerr << "Random seed = " << seed;
    srand(seed);
    for (int i = 0; i < 100; ++i)
        mmheap.push(rand() % 10);

    ASSERT_TRUE(mmheap.size() == 100);

    int minV = INT_MIN, maxV = INT_MAX;

    for (int i = 0; i < 100; ++i) {
        if (i & 1) {
            int m = mmheap.popMin();
            ASSERT_TRUE(minV <= m && m <= maxV);
            minV = m;
        }
        else {
            int m = mmheap.popMax();
            ASSERT_TRUE(minV <= m && m <= maxV);
            maxV = m;
        }
    }
}

