/**
 * @file MinMaxHeap.h
 * @brief Min-Max Heap 是能同時取出最大值及最小值的 Heap 結構。
 */
#ifndef MINMAXHEAP_H
#define MINMAXHEAP_H

#include <vector>
#include <initializer_list>
#include <stdint.h>
#include <assert.h>
#include <limits>

/// MinMaxHeap 中節點 index 的計算函數，傳入的參數都假設是0-indexed
namespace MinMaxHeap_Trait {
    /// 父節點
    inline size_t parent     (size_t id) { return id == 0 ? 0 : (id - 1) / 2; }
    /// 左子節點
    inline size_t leftChild  (size_t id) { return (id * 2) + 1; }
    /// 右子節點
    inline size_t rightChild (size_t id) { return (id * 2) + 2; }

    /// @brief 確認是不是 min node
    /// @param id - 節點在 m_data 中的 index
    /// @return `true`，是 min node；`fasle`，是 max node
    bool isMinNode(size_t id);
}

/**
 * @brief Min-Max Heap 是能同時取出最大值及最小值的 Heap 結構。
 * @details
 * 儲存時會將節點分成 min node 和 max node。
 * min node「小於等於」子樹中的其他節點；max node 則是「大於等於」子樹中的其他節點。
 * 區分的方式是基於節點所在的層數。
 * root node 是 min node；下一層的兩個節點為 max node；再下一層的四個節點為 min node；如此交錯出現……
 */
class MinMaxHeap {
public:
    typedef int value_type;

private:
    std::vector<value_type> m_data;

public:
    /// 建立空的 Min-Max Heap
    MinMaxHeap() = default;

    /// @brief  從 [first, last) 建立Min-Max Heap
    /// @tparam InputIt - 滿足 input iterator
    /// @param first - 範圍的起點（包含）
    /// @param last - 範圍的終點（不包含）
    template<typename InputIt>
    MinMaxHeap(InputIt first, InputIt last) : m_data(first, last) {
        if (m_data.empty()) return;
        
        size_t i = MinMaxHeap_Trait::parent(m_data.size() - 1);

        // i 從最後一項的父節點 到 第0項
        while (i != std::numeric_limits<size_t>::max()) {
            pushDown(i);
            --i;
        }
    }

    /// @brief 從初始化串列建立Min-Max Heap
    /// @param list - 初始化串列
    MinMaxHeap(std::initializer_list<value_type> list) : MinMaxHeap(list.begin(), list.end()) {}

    /// @brief 移除最小值並回傳
    /// @return 被移除的最小值
    value_type popMin();

    /// @brief 移除最大值並回傳
    /// @return 被移除的最大值
    value_type popMax();

    /// @brief 將value插入Min-Max Heap
    /// @param value 插入的值
    void push(value_type value);

    /// 有幾個元素
    size_t size() const { return m_data.size(); }

private:
    /// 確認節點存在
    bool exist(size_t id) { return id < m_data.size(); }

    /// @brief 使以 root 為根的子樹滿足 Min-Max Heap 的特性（min node「小於等於」子樹的其他節點，max node「大於等於」子樹的其他節點）
    /// @param root - 子樹的根
    /// @pre root 的左右子樹都滿足 Min-Max Heap 的特性
    void pushDown(size_t root);
};

#endif // MINMAXHEAP_H