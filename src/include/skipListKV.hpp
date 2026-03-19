#pragma once

#include <vector>
#include <cstdlib>
#include <mutex>
#include <shared_mutex>
#include <fstream>
#include <string>

namespace SL
{
    template <typename K, typename V>
    struct NodeKV
    {
        K _key;
        V _value;
        size_t _hv;
        int _level;
        std::vector<NodeKV<K, V> *> next;
        NodeKV(K key, V value, int level) : _key(key), _value(value), _hv(std::hash<K>{}(key))
        {
            next.resize(level, nullptr);
        };
    };
    
    template <typename K, typename V>
    class SkipListKV
    {
    public:
        SkipListKV()
        {
            _dummyhead = new NodeKV<K, V>(K(), V());
            _dummyhead->_level = MAX_LEVEL;
            _dummyhead->_hv = 0;
            _dummyhead->next.resize(MAX_LEVEL, nullptr);
        }

        ~SkipListKV()
        {
            NodeKV<K, V> *current = _dummyhead;
            while (current != nullptr)
            {
                NodeKV<K, V> *next = current->next[0];
                delete current;
                current = next;
            }
        }

        void setMaxLevelKV(int level) { MAX_LEVEL = level; }

        void insert(K key, V value)
        {
            NodeKV<K, V> *newnode = new NodeKV<K, V>(key, value);
            newnode->_level = getRandomLevel();

            NodeKV<K, V> *current = _dummyhead;
            std::vector<NodeKV<K, V> *> update(MAX_LEVEL, nullptr);

            // 从最高层开始查找插入位置
            for (int i = MAX_LEVEL - 1; i >= 0; i--)
            {
                while (current->next[i] != nullptr && (sortBaseOnHash ? compareByHash(current->next[i], newnode->_key) < 0 : compareByData(current->next[i], newnode->_key) < 0))
                {
                    current = current->next[i];
                }
                update[i] = current;
            }

            // 检查是否已存在相同元素
            if (current->next[0] != nullptr && (sortBaseOnHash ? compareByHash(current->next[0], newnode->_key) == 0 : compareByData(current->next[0], newnode->_key) == 0))
            {
                delete newnode;
                return; // 元素已存在，不插入
            }

            // 在每一层插入新节点
            for (int i = 0; i < newnode->_level; i++)
            {
                newnode->next[i] = update[i]->next[i];
                update[i]->next[i] = newnode;
            }
            _size++;
        }

        void insertl(K key, V value)
        {
            std::lock_guard<std::shared_mutex> lock(_mutex);
            insert(key, value);
        }

        V search(K key)
        {
            NodeKV<K, V> *prenode = getPreNode(key);
            return prenode != nullptr ? prenode->_value : V();
        }

        V searchl(K key)
        {
            std::lock_guard<std::shared_mutex> lock(_mutex);
            return search(key);
        }

        bool remove(K key)
        {
            NodeKV<K, V> *current = _dummyhead;
            std::vector<NodeKV<K, V> *> update(MAX_LEVEL, nullptr);

            // 从最高层开始查找要删除的节点
            for (int i = MAX_LEVEL - 1; i >= 0; i--)
            {
                while (current->next[i] != nullptr && (sortBaseOnHash ? compareByHash(current->next[i], key) < 0 : compareByData(current->next[i], key) < 0))
                {
                    current = current->next[i];
                }
                update[i] = current;
            }

            // 检查下一个节点是否是目标节点
            current = current->next[0];
            if (current == nullptr)
                return false; // 元素不存在，不删除

            // 检查节点是否匹配目标
            bool matches;
            if (sortBaseOnHash)
            {
                matches = (compareByHash(current, key) == 0);
            }
            else
            {
                matches = (compareByData(current, key) == 0);
            }

            if (!matches)
                return false; // 不是目标节点，不删除

            // 在每一层删除节点
            for (int i = 0; i < current->_level; i++)
            {
                if (update[i]->next[i] != current)
                {
                    break; // 该层没有指向目标节点的指针
                }
                update[i]->next[i] = current->next[i];
            }

            delete current;
            _size--;
            return true; // 成功删除
        }

        bool removel(K key)
        {
            std::lock_guard<std::shared_mutex> lock(_mutex);
            return remove(key);
        }

        bool update(K oldkey, V newvalue)
        {
            // 基于哈希比较时的特殊处理
            if (sortBaseOnHash)
            {
                // 对于基于哈希的比较，需要先找到节点，然后直接修改数据
                NodeKV<K, V> *current = _dummyhead;
                std::vector<NodeKV<K, V> *> update(MAX_LEVEL, nullptr);

                // 从最高层开始查找要更新的节点
                for (int i = MAX_LEVEL - 1; i >= 0; i--)
                {
                    while (current->next[i] != nullptr && compareByHash(current->next[i], oldkey) < 0)
                    {
                        current = current->next[i];
                    }
                    update[i] = current;
                }

                // 检查下一个节点是否是目标节点
                current = current->next[0];
                if (current == nullptr)
                    return false; // 元素不存在，更新失败

                // 检查节点是否匹配目标
                if (compareByHash(current, oldkey) != 0)
                    return false; // 不是目标节点，更新失败

                // 直接修改节点数据
                current->_value = newvalue;
                current->_hv = std::hash<K>{}(oldkey);
                return true;
            }
            else
            {
                // 对于基于数据的比较，使用原来的先删除后插入方法
                if (remove(oldkey))
                {
                    insert(oldkey, newvalue);
                    return true;
                }
                return false; // 元素不存在，更新失败
            }
        }

        bool updatel(K oldkey, V newvalue)
        {
            std::lock_guard<std::shared_mutex> lock(_mutex);
            return update(oldkey, newvalue);
        }

        void setSortBaseOnHash(bool sort)
        {
            if (_size == 0)
                sortBaseOnHash = sort;
        }

        void setStorageFile(std::string file) { _storageFile = file; }

        void saveToFile()
        {
            if (_storageFile.empty())
                return;
            std::ofstream outfile(_storageFile, std::ios::binary);
            if (!outfile.is_open())
            {
                std::cerr << "Error: Could not open file " << _storageFile << " for writing." << std::endl;
                return;
            }

            NodeKV<K, V> *current = _dummyhead->next[0];
            while (current != nullptr)
            {
                outfile << current->_key << " " << current->_value << std::endl;
                current = current->next[0];
            }

            outfile.close();
        }

        void loadFromFile()
        {
            if (_storageFile.empty())
                return;
            std::ifstream infile(_storageFile, std::ios::binary);
            if (!infile.is_open())
            {
                std::cerr << "Error: Could not open file " << _storageFile << " for reading." << std::endl;
                return;
            }

            K key;
            V value;
            while (infile >> key >> value)
            {
                insert(key, value);
            }

            infile.close();
        }

    private:
        NodeKV<K, V> *_dummyhead;
        bool sortBaseOnHash = false; // no sort base on hash, so base on data default compare
        std::shared_mutex _mutex;
        size_t _size = 0;
        std::string _storageFile = "";

        int MAX_LEVEL = 18;

        int getRandomLevel()
        {
            int level = 1;
            while ((rand() % 2) && (level < MAX_LEVEL))
            {
                level++;
            }
            return level;
        }

        int compareByHash(const NodeKV<K, V> *lnode, const K rkey) const
        {
            const size_t rnodehv = std::hash<K>{}(rkey);
            if (lnode->_hv == rnodehv)
            {
                // 哈希值相同，必须比较实际数据来确认
                if (lnode->_key == rkey)
                    return 0;
                return lnode->_key < rkey ? -1 : 1;
            }
            if (lnode->_hv < rnodehv)
                return -1;
            return 1;
        }

        int compareByHashl(const NodeKV<K, V> *lnode, const K rkey) const
        {
            std::lock_guard<std::shared_mutex> lock(_mutex);
            return compareByHash(lnode, rkey);
        }

        int compareByData(const NodeKV<K, V> *lnode, K rkey) const
        {
            if (lnode->_key == rkey)
                return 0;
            if (lnode->_key < rkey)
                return -1;
            return 1;
        }

        int compareByDatal(const NodeKV<K, V> *lnode, K rkey) const
        {
            std::lock_guard<std::shared_mutex> lock(_mutex);
            return compareByData(lnode, rkey);
        }

        NodeKV<K, V> *getPreNode(K key)
        {
            NodeKV<K, V> *current = _dummyhead;

            // 从最高层开始查找
            for (int i = MAX_LEVEL - 1; i >= 0; i--)
            {
                // 在当前层向前移动，直到找到大于等于目标节点的位置
                while (current->next[i] != nullptr)
                {
                    // 安全地获取下一个节点
                    NodeKV<K, V> *next_node = current->next[i];

                    // 检查节点是否有效且小于目标数据/哈希值
                    if (sortBaseOnHash ? compareByHash(next_node, key) < 0 : compareByData(next_node, key) < 0)
                    {
                        current = next_node;
                    }
                    else
                    {
                        break; // 找到大于等于目标的位置
                    }
                }
            }

            // 现在current指向目标节点的前驱节点（如果存在）
            // 检查下一个节点是否是目标节点
            if (current->next[0] != nullptr && (sortBaseOnHash ? compareByHash(current->next[0], key) == 0 : compareByData(current->next[0], key) == 0))
            {
                return current;
            }

            return nullptr;
        }

        NodeKV<K, V> *getPreNodel(K key)
        {
            std::lock_guard<std::shared_mutex> lock(_mutex);
            return getPreNode(key);
        }
    };
}