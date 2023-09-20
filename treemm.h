/*
Author: Joseph Hu
*/

#ifndef TREEMULTIMAP_INCLUDED
#define TREEMULTIMAP_INCLUDED
#include <vector>

template <typename KeyType, typename ValueType>

class TreeMultimap
{
public:
    class Iterator
    {
    public:
        Iterator()
            : m_view_data(nullptr), m_index(-1) {}

        Iterator(std::vector<ValueType> &view_data)
            : m_view_data(&view_data)
        {
            if (m_view_data->size() == 0)
                m_index = -1;
            else
                m_index = 0;
        }

        ValueType &get_value() const
        {
            return (*m_view_data)[m_index];
        }

        bool is_valid() const
        {
            if (m_view_data == nullptr)
                return false;
            if (m_view_data->size() == 0)
                return false;
            if (m_index < 0 || m_index > m_view_data->size() - 1)
                return false;
            return true;
        }

        void advance()
        {
            m_index++;
        }

    private:
        std::vector<ValueType> *m_view_data;
        int m_index;
    };

    TreeMultimap()
        : m_root(nullptr) {}

    ~TreeMultimap()
    {
        deleteAll(m_root);
    }

    void insert(const KeyType &key, const ValueType &value)
    {
        insert_helper(m_root, key, value);
    }

    Iterator find(const KeyType &key) const
    {
        return find_helper(m_root, key);
    }

private:
    struct Node
    {
        KeyType m_key;
        Node *m_left;
        Node *m_right;
        std::vector<ValueType> m_data;
    };
    Node *m_root;

    void insert_helper(Node *&added_node, const KeyType &key, const ValueType &value)
    {
        if (added_node == nullptr)
        {
            added_node = new Node;
            added_node->m_key = key;
            added_node->m_left = nullptr;
            added_node->m_right = nullptr;
            added_node->m_data.push_back(value);
            return;
        }
        if (key == added_node->m_key)
            added_node->m_data.push_back(value);
        else if (key < added_node->m_key)
            insert_helper(added_node->m_left, key, value);
        else // if (key > added_node->m_key)
            insert_helper(added_node->m_right, key, value);
    }

    Iterator find_helper(Node *searched_node, const KeyType &key) const
    {
        if (searched_node == nullptr)
            return Iterator();
        else if (key == searched_node->m_key)
            return Iterator(searched_node->m_data);
        else if (key < searched_node->m_key)
            return find_helper(searched_node->m_left, key);
        else
            return find_helper(searched_node->m_right, key);
    }

    void deleteAll(Node *p)
    {
        if (p != nullptr)
        {
            deleteAll(p->m_left);
            deleteAll(p->m_right);
            delete p;
        }
    }
};

#endif // TREEMULTIMAP_INCLUDED
