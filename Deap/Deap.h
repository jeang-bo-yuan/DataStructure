/**
 * @file Deap.h
 * @brief Double-Ended Heap
 */
#ifndef DEAP_H
#define DEAP_H

#include <assert.h>
#include <vector>
#include <initializer_list>

/**
 * @brief Deap 中節點 index 的計算函數，index 為 0-indexed
 * @details
 * Deap中的root node是不存資料的，所以實際上在儲存時，我也不打算存root。
 * Index的分佈如下所示。其中0、2、3是在min heap；1、4、5是在max heap。
 * ```
 *     <root>
 *    /     \
 *   0       1
 *  / \     / \
 * 2   3   4   5
 * ```
 */
namespace Deap_Trait {
    /// 父節點
    inline constexpr size_t parent     (size_t id) { return id < 2 ? id : (id - 2) / 2; }
    /// 左子節點
    inline constexpr size_t leftChild  (size_t id) { return id * 2 + 2; }
    /// 右子節點
    inline constexpr size_t rightChild (size_t id) { return id * 2 + 3; }

    /// @brief 回傳id在二階制表示法中最高位的1
    /// @param id - 傳入的數字
    /// @return 最高位的1
    inline size_t highestOne (size_t id) {
        assert(id != 0);

        size_t res = size_t(1) << (sizeof(size_t) * 8 - 1);
        while ((res & id) == 0) res >>= 1;
        return res;
    }

    /// @brief 確認某個節點是不是在min heap內
    /// @param id - 節點的index
    /// @return `true`：在min heap內。`false`：在max heap內。
    inline bool inMinHeap (size_t id) {
        id += 2;          // 先將index做轉換，root為1，min heap的根為2，max heap的根為3
        assert(id >= 2);

        // 對於每一層的節點，它的二進制型如「1xxx...」
        // 並將其分成「10xx...」（Min Heap）和「11xx...」（Max Heap）兩塊
        const size_t high = highestOne(id);
        return ! static_cast<bool>((high >> 1) & id);
    }

    /// @brief 取得id在另一個heap中對應的節點（假設該節點存在）
    /// @param id - 節點的index
    /// @return 對應的節點，該點和id在各自的heap中有同樣的相對位置
    inline size_t correspond(size_t id) {
        id += 2;
        assert(id >= 2);

        const size_t high = highestOne(id);
        return (id ^ (high >> 1)) - 2; // toggle 最高位的1的左邊的bit，然後減2（還原）
    }
}

/**
 * @brief 可以同時存取最大／最小值的heap
 * @details
 * Deap儲存時
 * 1. 左子樹是Min Heap
 * 2. 右子樹是Max Heap
 * 3. 對於每個節點和其對應的節點，min heap中對應的節點 <= max heap中對應的節點
 * 
 * 事實上，如果條件1、2滿足的話，我們只需要對葉子節點檢查是否滿足條件3。
 * 
 * 因為min heap的節點由上而下遞增，而max heap中的節點由下而上遞增。
 * 如果我們從min heap取一條從「根節點」到「葉節點」的path出來：
 * > m1, m2, ..., mi
 * 然後假設mi對應到max heap中的Mj，並取出「根節點」到「Mj」的path：
 * > M1, M2, ..., Mj
 * 只要mi <= Mj，那麼：
 * > m1 <= m2 <= ... <= mi <= Mj <= ... <= M2 <= M1
 * 顯然的，當mi <= Mj時，path上的其他節點必定會滿足條件3。
 */
class Deap {
public:
    typedef int value_type;

private:
    std::vector<value_type> m_data;

public:
    /// @brief 建立空的Deap
    Deap() = default;

    /// @brief 將[first, last)內的元素插入Deap
    /// @tparam InputIt - Input Iterator型別
    /// @param first - 開始（含）
    /// @param last - 結尾（不含）
    template<typename InputIt>
    Deap(InputIt first, InputIt last) : m_data(first, last) { buildDeap(); }

    /// @brief 將list中的所有內容插入Deap內
    /// @param list - 初始化串列
    Deap(std::initializer_list<value_type> list) : m_data(list) { buildDeap(); }

    /// @brief 插入新的值
    /// @param v - 新的值
    void push(const value_type& v) { m_data.push_back(v); insert(m_data.size() - 1); }

    /// @brief 移除最小值並返回
    /// @throw std::out_of_range - 如果Deap為空
    value_type popMin();

    /// @brief 移除最大值並返回
    /// @throw std::out_of_range - 如果Deap為空
    value_type popMax();

    size_t size() const { return m_data.size(); }

private:
    /// 是否存在
    bool exist(size_t id) const { return id < m_data.size(); }
    /// 是否是葉子節點
    bool isLeaf(size_t id) const { return exist(id) && !exist(Deap_Trait::leftChild(id)); }

    /// @brief 先計算correspond，如果它不存在，則取它的父節點
    /// @param id - 節點的index
    /// @return 實際上對應的節點
    size_t safeCorrespond(size_t id) const {
        assert(exist(id));
        size_t corr = Deap_Trait::correspond(id);
        return exist(corr) ? corr : Deap_Trait::parent(corr);
    }

    /// 初始化時呼叫，將m_data的內容轉成Deap
    void buildDeap();

    /// @brief 當有新的值插入原本符合規範的Deap
    /// @param id - 葉子節點的index
    void insert(size_t id);

    /// @brief 將節點 id 向上拉。如果是min heap中的節點，將較小的值向上拉；否則是max heap中的節點，將較大的值向上拉。
    /// @param id - 節點的index
    void pullUp(size_t id);

    /// @brief 將節點 id 向下推。如果是min heap中的節點，將較大的值向下推；否則是max heap中的節點，將較小的值向下推。
    /// @param id - 節點的index
    void pushDown(size_t id);   

#ifndef NDEBUG
public:
    /// @brief 檢查Deap的內容是否符合規範
    /// @return `true`，有；`false`，沒有。
    bool verify() const;

    /// @brief 印出m_data
    void printData() const;
#endif
};

#endif // DEAP_H
