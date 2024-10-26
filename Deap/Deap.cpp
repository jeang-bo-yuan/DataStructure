#include "Deap.h"
#include <functional>
#include <stdexcept>

using namespace Deap_Trait;

Deap::value_type Deap::popMin()
{
    if (m_data.size() == 0) throw std::out_of_range("Deap::popMin - No element");

    const value_type ret = m_data[0];

    /**
     * # 演算法
     * 最小值被移除後，要不斷地將較小的子節點往上移。
     */
    size_t emptyNode = 0;
    // 只要底下還有子節點
    while (!isLeaf(emptyNode)) {
        const size_t L = leftChild(emptyNode), R = rightChild(emptyNode);

        // 如果左子節點比較小
        if (!exist(R) || m_data[L] < m_data[R]) {
            m_data[emptyNode] = std::move(m_data[L]); // 將左子節點往上移
            emptyNode = L;
        }
        // 否則，移動右子節點
        else {
            m_data[emptyNode] = std::move(m_data[R]);
            emptyNode = R;
        }
    }

    if (emptyNode == m_data.size() - 1) {
        /**
         * @note Edge Case: 如果最後一個元素被往上提了，那可以保證性質3不會被違反。
         * 最後一個元素在min heap => 在max heap中對應的位置是空的 => 最後一個元素的「safeCorrespond」是對應位置的父節點。
         * 當最後一個元素被往上提時，「safeCorrespnd」不變。
         */
        m_data.pop_back();
    }
    else {
        /**
         * 往上移後形成的空位，拿最後一個元素補，然後呼叫 insert()
         */
        m_data[emptyNode] = std::move(m_data.back()); // 否則拿最後一個元素補
        m_data.pop_back();
        this->insert(emptyNode);
    }

    return ret;
}

Deap::value_type Deap::popMax()
{
    if (m_data.size() == 0) throw std::out_of_range("Deap::popMin - No element");
    
    if (m_data.size() == 1) {
        const value_type ret = m_data.front(); // 回傳第一個元素
        m_data.pop_back();
        return ret;
    }

    const value_type ret = m_data[1];

    /**
     * # 演算法
     * 最大元素被移除後，要不斷拿較大的子節點往上遞補。
     */
    size_t emptyNode = 1;
    while (!isLeaf(emptyNode)) {
        const size_t L = leftChild(emptyNode), R = rightChild(emptyNode);

        // 左子節點比較大
        if (!exist(R) || m_data[L] > m_data[R]) {
            m_data[emptyNode] = std::move(m_data[L]);
            emptyNode = L;
        }
        else {
            m_data[emptyNode] = std::move(m_data[R]);
            emptyNode = R;
        }
    }

    if (emptyNode == m_data.size() - 1) {
        /**
         * @note Edge Case: 最後一個元素被往上遞補
         * > 為討論方便，原本「在min heap中和最後一個元素對應」的節點被稱作P
         * >
         * > - Case 1: 最後一個元素在 parent 的右子樹 -> 不用管  
         * >   在最後一個元素往上移後，P的「safeCorrespond」仍是原本的最後一個元素。
         * >
         * > - Case 2: 最後一個元素在 parent 的左子樹 -> 往上移後對 parent 呼叫 insert  
         * >   原本只有P和最後一個元素互相對應，但往上移後，P和「P的兄弟節點」的「safeCorrespond」都會是原本的最後一個元素。
         * >   所以要呼叫 insert 來避免「P的兄弟節點」和最後一個元素衝突。
         */
        m_data.pop_back();
        if (isLeaf(parent(emptyNode)))
            this->insert(parent(emptyNode));
    }
    else {
        /**
         * 往上移後形成的空位，拿最後一個元素補，然後呼叫 insert()
         */
        m_data[emptyNode] = std::move(m_data.back());
        m_data.pop_back();
        this->insert(emptyNode);
    }

    return ret;
}

// Private Function /////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @details
 * # 演算法
 * 1. 首先做一般的heapify。對於min heap中的節點，將較大的值向下推；對於max heap中的節點，將較小的值向下推。  
 *    使得左子樹為min heap，右子樹為max heap。
 * 2. 對於每個葉子節點，每當min heap中對應的節點 > max heap中對應的節點，則交換兩節點的值，並分別對兩個節點pullUp。
 * 
 * 這演算法是自己想的，但經過unit test後，我覺得應該是對的。
 * 
 * # 步驟2是如何確保條件3成立的
 * 
 * 基本想法是從min heap和max heap中各取一條從「根節點」到「葉節點」的path：
 * > m1, m2, ..., mi
 * 和
 * > M1, M2, ..., Mj
 * 其中mi的對應節點為Mj，接下來就要將兩條path上的節點給排序，使得`m1 <= m2 <= ... <= mi <= Mj <= ... <= M2 <= M1`。
 * 因為 m1 ~ mi 和 Mj ~ M1 已經是遞增的，所以只要當 mi > Mj 時，將兩節點的值交換然後分別對兩條 path 排序（使用 pullUp）。
 * 重覆直到 mi <= Mj。
 */
void Deap::buildDeap()
{
    if (m_data.size() < 2) return;

    // 一般的heapify
    for (size_t i = parent(m_data.size() - 1); i != static_cast<size_t>(-1); --i) {
        pushDown(i);
    }

    // 對每個葉節點
    for (size_t i = m_data.size() - 1; isLeaf(i); --i) {
        size_t minHeapNode = i;
        size_t maxHeapNode = safeCorrespond(i);

        // 對調，使得 minHeapNode 在 min heap 內
        if (!inMinHeap(minHeapNode)) std::swap(minHeapNode, maxHeapNode);

        // 如果 min heap 中的節點較大
        while (m_data[minHeapNode] > m_data[maxHeapNode]) {
            std::swap(m_data[minHeapNode], m_data[maxHeapNode]);
            pullUp(minHeapNode);
            pullUp(maxHeapNode);
        }
    }
}

/**
 * @details 這操作在 push 和 pop 都會用到
 * # 演算法
 * 對於新插入的葉節點 id，將它和「對應節點」比較大小。
 * - 如果滿足性質3的大小要求，則直接對 id pullUp()。
 * - 否則，交換兩節點的值，然後對「對應節點」 pullUp()。
 */
void Deap::insert(const size_t id)
{
    assert(isLeaf(id));
    if (id == 0) return;

    // N -> node
    size_t minN = id, maxN = safeCorrespond(id);
    if (!inMinHeap(minN)) std::swap(minN, maxN);

    /**
    * @note
    * Edge Case: id 在 max heap，但它直接對應的節點不是 leaf。不過對應節點只有一個子節點，所以直接取它的子節點
    * > 可能發生的情境： popMax
    */
    if (!isLeaf(minN) && !exist(rightChild(minN))) minN = leftChild(minN);

    if (isLeaf(minN)) {
        if (m_data[minN] <= m_data[maxN]) { // 葉節點的大小滿足規定，只需對id所在的heap排序
            pullUp(id);                     // 若 id 的值被往上移，葉節點的性質3還是被保留
        }
        else {
            std::swap(m_data[minN], m_data[maxN]); // 交換使葉節點滿足規定
            pullUp(safeCorrespond(id));
        }
    }
    /**
     * @note
     * Edge Case: id在max heap，而且min heap中有兩個「葉節點」和其對應
     * > 可能發生情境：popMax.
     * > 
     * > 此時要拿id和兩個葉節點比較
     * > - 如果 id 節點的值較大，則 pullUp(id)。
     * > - 否則要拿較大的葉節點和id互換，然後對葉節點 pullUp。
     */
    else {
        assert(maxN == id);
        const size_t minLeaf1 = leftChild(minN), minLeaf2 = rightChild(minN);

        // 如果 id >= 另兩個對應的葉節點，只要將id向上拉
        if (m_data[minLeaf1] <= m_data[id] && m_data[minLeaf2] <= m_data[id]) {
            pullUp(id);
        }
        // 否則，從 minLeaf1 和 minLeaf2 取較大的值和 id 互換，然後排序min heap
        else if (m_data[minLeaf1] > m_data[minLeaf2]) {
            std::swap(m_data[minLeaf1], m_data[id]);
            pullUp(minLeaf1);
        }
        else {
            std::swap(m_data[minLeaf2], m_data[id]);
            pullUp(minLeaf2);
        }
    }
}

// 基礎操作 /////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @details
 */
void Deap::pullUp(size_t id)
{
    std::function<bool(size_t, size_t)> _less;
    if (inMinHeap(id)) _less = std::less<size_t>{};
    else               _less = std::greater<size_t>{};

    while (exist(id)) {
        // Note: id = 0, 1 時為 heap 的根，此時 p == id。然後因為沒有比父節點「小」，所以就break。
        size_t p = parent(id);

        // 如果比父節點「小」，則要往上移
        if (_less(m_data[id], m_data[p])) {
            std::swap(m_data[id], m_data[p]);
            // 繼續pullUp
            id = p;
        }
        else
            break;
    }
}

/**
 * @details 和一般的heapify一樣
 */
void Deap::pushDown(size_t id)
{
    std::function<bool(size_t, size_t)> _less;
    if (inMinHeap(id)) _less = std::less<size_t>{};
    else               _less = std::greater<size_t>{};

    while (exist(id)) {
        size_t _minNode = id;
        const size_t L = leftChild(id), R = rightChild(id);

        // 找到最「小」的節點
        if (exist(L) && _less(m_data[L], m_data[_minNode]))
            _minNode = L;
        if (exist(R) && _less(m_data[R], m_data[_minNode]))
            _minNode = R;

        // 不用再繼續
        if (_minNode == id) return;

        // 將最「小」的節點往上移
        std::swap(m_data[id], m_data[_minNode]);
        // 繼續pushDown
        id = _minNode;
    }
}

// Debug /////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NDEBUG
#include <iostream>

bool Deap::verify() const
{
    if (m_data.size() < 2) return true;
    
    // for each node
    for (size_t i = 0; i < m_data.size(); ++i) {
        const size_t L = leftChild(i), R = rightChild(i);

        if (inMinHeap(i)) {
            if (exist(L) && m_data[i] > m_data[L]) 
                return false;
            if (exist(R) && m_data[i] > m_data[R]) 
                return false;
            if (m_data[i] > m_data[safeCorrespond(i)])
                return false;
        }
        else {
            if (exist(L) && m_data[i] < m_data[L]) 
                return false;
            if (exist(R) && m_data[i] < m_data[R]) 
                return false;
            if (m_data[i] < m_data[safeCorrespond(i)]) 
                return false;
        }
    }

    return true;
}

void Deap::printData() const
{
    std::cerr << "Deap::m_data = \n\t";
    for (value_type num : m_data) {
        std::cerr << num << ' ';
    }
    std::cerr.put('\n');
}
#endif
