#include "MinMaxHeap.h"
#include <algorithm>
#include <functional>
#include <stdexcept>

namespace MinMaxHeap_Trait {
    bool isMinNode(size_t id) 
    {
        // 轉成 1-indexed
        // 因為在1-indexed的情況下，可以透過「小於等於id」的「最大2的冪」來判斷所在的層數
        ++id;
        assert(id != 0);

        // 「小於等於id」的「最大2的冪」，其實就是「id最高位的1」
        size_t highest_one = 1;
        // 最上層是 min node
        bool is_min_node = true;

        // 當「id」減去「id最高位的1」後，得到的值會小於「id最高位的1」
        // 如果 id - highest_one 「沒有」小於 highest_one，代表 highest_one 存的不是「id最高位的1」
        while (id - highest_one >= highest_one) {
            highest_one <<= 1;
            // min node 和 max node 會交替出現
            is_min_node = !is_min_node;
        }

        return is_min_node;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

MinMaxHeap::value_type MinMaxHeap::popMin()
{
    if (size() == 0) throw std::out_of_range("MinMaxHeap::popMin - no element");

    value_type ret = std::move(m_data.front());

    m_data.front() = std::move(m_data.back());
    m_data.pop_back();
    pushDown(0);

    return ret;
}

MinMaxHeap::value_type MinMaxHeap::popMax()
{
    switch (size())
    {
    case 0:
        throw std::out_of_range("MinMaxHeap::popMax - no element");

    case 1: case 2: {
        value_type ret = std::move(m_data.back());
        m_data.pop_back();
        return ret;
    }

    default: {
        size_t max_node = m_data[1] > m_data[2] ? 1 : 2;
        value_type ret = std::move(m_data[max_node]);

        m_data[max_node] = std::move(m_data.back());
        m_data.pop_back();
        pushDown(max_node);

        return ret;
    }
    }
}

void MinMaxHeap::push(value_type value)
{
    using namespace MinMaxHeap_Trait;

    m_data.push_back(value);

    // 新插入的節點放哪
    size_t id = size() - 1;
    const size_t parentId = parent(id);

    // 只有一個節點 或 和父節點的值一樣
    // 不論父節點是min或max node，放一樣的值在底下都不會違反父節點的性質
    // 對於更上層的節點，因為沒有更大或更小的值出現，所以一樣不會違反性質
    if (m_data[id] == m_data[parentId])
        return;
    
    // 新節點 < 到root的路徑上所有的max node
    // 目標：將新節點插入路徑上的min node序列內，使min node由上至下遞增
    if (m_data[id] < m_data[parentId]) {
        // 比id上層的min node
        size_t prevMin = isMinNode(id) ? parent(parentId) : parentId;

        while (id != 0) {
            if (value < m_data[prevMin]) {
                m_data[id] = std::move(m_data[prevMin]);
                id = prevMin;
                prevMin = parent(parent(prevMin));
            }
            else
                break;
        }
        
        m_data[id] = value;
    }
    // 新節點 > 到root的路徑上所有的min node
    // 目標：將新節點插入路徑上的max node序列內，使max node由上至下遞減
    else {
        // 比id上層的max node
        size_t prevMax = isMinNode(id) ? parentId : parent(parentId);

        while (id != 1 && id != 2) {
            if (value > m_data[prevMax]) {
                m_data[id] = std::move(m_data[prevMax]);
                id = prevMax;
                prevMax = parent(parent(prevMax));
            }
            else
                break;
        }

        m_data[id] = value;
    }
}

void MinMaxHeap::pushDown(size_t root)
{
    using namespace MinMaxHeap_Trait;

    // min node 和 max node 的處理方式是對稱的，只不過一個是用「小於」、一個是用「大於」
    std::function<bool(const value_type&, const value_type&)> _less;

    if (isMinNode(root)) _less = std::less<value_type>();
    else                 _less = std::greater<value_type>();

    // 在下面的註解中，我假設 root 是「min node」，而 _less 是「小於」
    // root 是「max node」的情形，請自行將「」內的字替換成反義詞
    while (exist(root)) {
        // root 的兩個子節點及四個孫子
        const size_t children[] = {
            leftChild(root),                                 rightChild(root),
            leftChild(children[0]), rightChild(children[0]), leftChild(children[1]), rightChild(children[1])
        };

        // 找子樹中最「小」的節點
        // 搜尋時只要找兩層，因為再往下不會有更「小」的（Note: 孫子那層是「min node」，所以孫子「<=」更下層的節點）
        size_t M = root;
        for (auto id : children) {
            if (exist(id) && _less(m_data[id],  m_data[M]))
                M = id;
        }

        // 已經滿足特性
        if (M == root)
            return;
        else {
            // root變最「小」的值，而M變「大」
            std::swap(m_data[root], m_data[M]);

            size_t parentM = parent(M);
            
            // 若M是「max node」，他的值變「大」不會影響子樹的性質
            if (parentM == root) return;
            
            // 否則，M是「min node」，值不能變得比parent「大」
            if (_less(m_data[parentM], m_data[M]))
                std::swap(m_data[parentM], m_data[M]);

            // M的值可能變得比子樹「大」，所以繼續pushDown
            root = M;
        }
    }
}
