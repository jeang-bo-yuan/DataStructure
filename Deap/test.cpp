#include "Deap.h"
#include "gtest/gtest.h"

TEST(Deap, ParentTest) {
    using Deap_Trait::parent;
    ASSERT_TRUE(parent(0) == 0);
    ASSERT_TRUE(parent(1) == 1);

    ASSERT_TRUE(parent(2) == 0);
    ASSERT_TRUE(parent(3) == 0);

    ASSERT_TRUE(parent(4) == 1);
    ASSERT_TRUE(parent(5) == 1);

    ASSERT_TRUE(parent(6) == 2);
    ASSERT_TRUE(parent(7) == 2);

    ASSERT_TRUE(parent(12) == 5);
    ASSERT_TRUE(parent(13) == 5);
}

TEST(Deap, ChildTest) {
    using Deap_Trait::leftChild;
    using Deap_Trait::rightChild;
    ASSERT_TRUE(leftChild(0) == 2);
    ASSERT_TRUE(rightChild(0) == 3);

    ASSERT_TRUE(leftChild(1) == 4);
    ASSERT_TRUE(rightChild(1) == 5);

    ASSERT_TRUE(leftChild(2) == 6);
    ASSERT_TRUE(rightChild(2) == 7);

    ASSERT_TRUE(leftChild(3) == 8);
    ASSERT_TRUE(rightChild(3) == 9);

    ASSERT_TRUE(leftChild(4) == 10);
    ASSERT_TRUE(rightChild(4) == 11);

    ASSERT_TRUE(leftChild(5) == 12);
    ASSERT_TRUE(rightChild(5) == 13);
}

TEST(Deap, inMinHeap) {
    using namespace Deap_Trait;
    const size_t thresh = 100000;

    // 測試id以下的節點丟進inMinHeap後是否回傳expected
    std::function<void(size_t, bool)> test = [&test, thresh](size_t id, bool expected) {
        if (id > thresh) return;
        ASSERT_TRUE(inMinHeap(id) == expected);
        test(leftChild(id), expected);
        test(rightChild(id), expected);
    };

    test(0, true);
    test(1, false);
}

TEST(Deap, correspond) {
    using namespace Deap_Trait;
    const size_t thresh = 100000;

    // 測試一起向左或一起向右，應該要呼相對應
    std::function<void(size_t, size_t)> test = [&](size_t L, size_t R) {
        if (R > thresh) return;
        ASSERT_TRUE(correspond(L) == R);
        ASSERT_TRUE(L == correspond(R));

        test(leftChild(L), leftChild(R));
        test(rightChild(L), rightChild(R));
    };

    test(0, 1);
}

TEST(Deap, buildDeap) {
    std::vector<int> arr;

    const Deap empty(arr.begin(), arr.end());
    ASSERT_TRUE(empty.verify());

    unsigned seed = time(NULL);
    srand(seed);
    std::cerr << "Random seed = " << seed << '\n';

    for (size_t i = 0; i < 100; ++i) {
        arr.push_back(rand() % 10);
        const Deap tmp(arr.begin(), arr.end());
        bool res;
        EXPECT_TRUE(res = tmp.verify());

        if (res == false) {
            std::cerr << "arr = \n\t";
            for (int num : arr) std::cerr << num << ' ';
            std::cerr << "\n\n";

            tmp.printData();

            ASSERT_TRUE(false) << "Error in build deap";
        }

    }
}

static Deap randomDeap(size_t size) {
    Deap d;
    
    // failed case : seed = 1429, 19085
    unsigned seed = rand();
    srand(seed);
    std::cerr << "Random seed = " << seed << '\n';

    for (size_t i = 0; i < size; ++i) d.push(rand() % 10);
    return d;
}

TEST(Deap, push) {
    Deap d(randomDeap(100));
    ASSERT_TRUE(d.verify());
}

TEST(Deap, popMin) {
    Deap tmp = randomDeap(100);

    int min = INT_MIN;

    while (tmp.size()) {
        int x = tmp.popMin();
        ASSERT_TRUE(x >= min);
        ASSERT_TRUE(tmp.verify());
        min = x;
    }
}

TEST(Deap, popMax) {
    Deap tmp = randomDeap(100);

    int max = INT_MAX;

    while (size_t size = tmp.size()) {
        int x = tmp.popMax();
        ASSERT_TRUE(x <= max);
        
        if (!tmp.verify()) {
            tmp.verify();
            ASSERT_TRUE(false) << "tmp.verify() return false";
        }

        max = x;
    }
}

TEST(Deap, pop) {
    Deap tmp = randomDeap(100);

    int min = INT_MIN, max = INT_MAX;

    while (tmp.size()) {
        if (rand() & 1) {
            int x = tmp.popMin();
            ASSERT_TRUE(x >= min);
            ASSERT_TRUE(tmp.verify());
            min = x;
        }
        else {
            int x = tmp.popMax();
            ASSERT_TRUE(x <= max);
            ASSERT_TRUE(tmp.verify());
            max = x;
        }
    }
}