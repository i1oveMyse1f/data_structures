#pragma once

#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <utility>
#include <iterator>

template <typename ValueType>
struct NodeAvl {
    NodeAvl* parent = nullptr;
    NodeAvl* left = nullptr;
    NodeAvl* right = nullptr;
    int height = 1;
    const ValueType value;
    NodeAvl(const ValueType& value, NodeAvl* parent = nullptr)
        : parent(parent)
        , value(value)
    {
    }

    static void Clear(NodeAvl<ValueType>* vertex)
    {
        if (vertex == nullptr) {
            return;
        }
        Clear(vertex->left);
        Clear(vertex->right);
        delete vertex;
    }

    static NodeAvl<ValueType>* Find(NodeAvl<ValueType>* root, const ValueType& value)
    {
        while (root != nullptr) {
            if (!(root->value < value) && !(value < root->value)) {
                return root;
            } else if (value < root->value) {
                root = root->left;
            } else {
                root = root->right;
            }
        }
        return nullptr;
    }

    static NodeAvl<ValueType>* LowerBound(NodeAvl<ValueType>* root, const ValueType& value)
    {
        NodeAvl<ValueType>* current_upper_bound = nullptr;
        while (root != nullptr) {
            if (!(root->value < value) && !(value < root->value)) {
                return root;
            } else if (value < root->value) {
                current_upper_bound = root;
                root = root->left;
            } else {
                root = root->right;
            }
        }
        return current_upper_bound;
    }

    static NodeAvl<ValueType>* UpperBound(NodeAvl<ValueType>* root, const ValueType& value)
    {
        NodeAvl<ValueType>* current_upper_bound = nullptr;
        while (root != nullptr) {
            if (value < root->value) {
                current_upper_bound = root;
                root = root->left;
            } else {
                root = root->right;
            }
        }
        return current_upper_bound;
    }
     
    static int GetHeight(NodeAvl<ValueType>* vertex)
    {
        return vertex == nullptr ? 0 : vertex->height;
    }

    static void Update(NodeAvl<ValueType>* vertex)
    {
        vertex->height = std::max(GetHeight(vertex->left), GetHeight(vertex->right)) + 1;
    }

    static NodeAvl<ValueType>* LLRotate(NodeAvl<ValueType>* vertex)
    {
        auto a = vertex->left->left;
        auto b = vertex->left->right;
        auto c = vertex->right;
        auto root = vertex->left;

        root->parent = vertex->parent;

        root->left = a;
        a->parent = root;
        root->right = vertex;
        root->right->parent = root;

        root->right->left = b;
        if (b) {
            b->parent = root->right;
        }
        root->right->right = c;
        if (c) {
            c->parent = root->right;
        }

        Update(root->left);
        Update(root->right);
        Update(root);

        return root;
    }

    static NodeAvl<ValueType>* RRRotate(NodeAvl<ValueType>* vertex)
    {
        auto a = vertex->right->right;
        auto b = vertex->right->left;
        auto c = vertex->left;
        auto root = vertex->right;

        root->parent = vertex->parent;

        root->right = a;
        a->parent = root;
        root->left = vertex;
        root->left->parent = root;

        root->left->right = b;
        if (b) {
            b->parent = root->left;
        }
        root->left->left = c;
        if (c) {
            c->parent = root->left;
        }

        Update(root->left);
        Update(root->right);
        Update(root);

        return root;
    }

    static NodeAvl<ValueType>* LRRotate(NodeAvl<ValueType>* vertex)
    {
        auto a = vertex->left->left;
        auto b = vertex->left->right->left;
        auto c = vertex->left->right->right;
        auto d = vertex->right;
        auto root = vertex->left->right;

        root->parent = vertex->parent;

        root->left = vertex->left;
        root->left->parent = root;
        root->right = vertex;
        root->right->parent = root;

        root->left->left = a;
        if (a) {
            a->parent = root->left;
        }
        root->left->right = b;
        if (b) {
            b->parent = root->left;
        }
        root->right->left = c;
        if (c) {
            c->parent = root->right;
        }
        root->right->right = d;
        if (d) {
            d->parent = root->right;
        }

        Update(root->left);
        Update(root->right);
        Update(root);
        return root;
    }

    static NodeAvl<ValueType>* RLRotate(NodeAvl<ValueType>* vertex)
    {
        auto a = vertex->right->right;
        auto b = vertex->right->left->right;
        auto c = vertex->right->left->left;
        auto d = vertex->left;
        auto root = vertex->right->left;

        root->parent = vertex->parent;

        root->right = vertex->right;
        root->right->parent = root;
        root->left = vertex;
        root->left->parent = root;

        root->right->right = a;
        if (a) {
            a->parent = root->right;
        }
        root->right->left = b;
        if (b) {
            b->parent = root->right;
        }
        root->left->right = c;
        if (c) {
            c->parent = root->left;
        }
        root->left->left = d;
        if (d) {
            d->parent = root->left;
        }

        Update(root->left);
        Update(root->right);
        Update(root);
        return root;
    }

    static NodeAvl<ValueType>* Balance(NodeAvl<ValueType>* vertex)
    {
        if (std::abs(GetHeight(vertex->left) - GetHeight(vertex->right)) == 2) {
            if (GetHeight(vertex->left) > GetHeight(vertex->right)) {
                if (GetHeight(vertex->left->left) >= GetHeight(vertex->left->right)) {
                    vertex = LLRotate(vertex);
                } else {
                    vertex = LRRotate(vertex);
                }
            } else {
                if (GetHeight(vertex->right->left) > GetHeight(vertex->right->right)) {
                    vertex = RLRotate(vertex);
                } else {
                    vertex = RRRotate(vertex);
                }
            }
        }
        Update(vertex);
        return vertex;
    }

    static NodeAvl<ValueType>* Insert(NodeAvl<ValueType>* vertex, const ValueType& value)
    {
        if (vertex == nullptr) {
            return new NodeAvl(value);
        }

        if (!(vertex->value < value) && !(value < vertex->value)) {
            return vertex;
        } else if (value < vertex->value) {
            if (vertex->left == nullptr) {
                vertex->left = new NodeAvl<ValueType>(value, vertex);
            } else {
                vertex->left = Insert(vertex->left, value);
                Update(vertex->left);
            }
            return vertex = Balance(vertex);
        } else {
            if (vertex->right == nullptr) {
                vertex->right = new NodeAvl<ValueType>(value, vertex);
            } else {
                vertex->right = Insert(vertex->right, value);
                Update(vertex->right);
            }
            return vertex = Balance(vertex);
        }
    }

    static NodeAvl<ValueType>* FindMin(NodeAvl<ValueType>* vertex)
    {
        vertex = vertex->right;
        while (vertex->left) {
            vertex = vertex->left;
        }
        return vertex;
    }

    static NodeAvl<ValueType>* Erase(NodeAvl<ValueType>* vertex, const ValueType& value)
    {
        if (vertex == nullptr) {
            return nullptr;
        }

        if (vertex->value == value) {
            if (vertex->left == nullptr && vertex->right == nullptr) {
                delete vertex;
                return nullptr;
            } else if (vertex->left == nullptr) {
                vertex->right->parent = vertex->parent;
                auto cuted = vertex->right;
                delete vertex;
                return cuted = Balance(cuted);
            } else if (vertex->right == nullptr) {
                vertex->left->parent = vertex->parent;
                auto cuted = vertex->left;
                delete vertex;
                return cuted = Balance(cuted);
            } else {
                auto replacement = FindMin(vertex);

                if (replacement->parent->left == replacement) {
                    replacement->parent->left = replacement->right; // null or size = 1
                    if (replacement->right)
                        replacement->right->parent = replacement->parent;
                } else {
                    replacement->parent->right = replacement->right; // null or size = 1
                    if (replacement->right)
                        replacement->right->parent = replacement->parent;
                }

                auto go_up = replacement->parent; // balance way from vertex to repl
                while (go_up != nullptr && go_up != vertex) {
                    if (go_up->parent->left == go_up) {
                        go_up = Balance(go_up);
                        assert(go_up != nullptr);
                        go_up->parent->left = go_up;
                    } else {
                        go_up = Balance(go_up);
                        assert(go_up != nullptr);
                        go_up->parent->right = go_up;
                    }
                    go_up = go_up->parent;
                }

                replacement->parent = vertex->parent;

                replacement->left = vertex->left;
                if (replacement->left) {
                    replacement->left->parent = replacement;
                }
                replacement->right = vertex->right;
                if (replacement->right) {
                    replacement->right->parent = replacement;
                }

                replacement = Balance(replacement);

                delete vertex;
                return replacement;
            }
        } else if (value < vertex->value) {
            if (vertex->left == nullptr) {
                return nullptr;
            } else {
                vertex->left = Erase(vertex->left, value);
            }
            return vertex = Balance(vertex);
        } else {
            if (vertex->right == nullptr) {
                return nullptr;
            } else {
                vertex->right = Erase(vertex->right, value);
            }
            return vertex = Balance(vertex);
        }
    }
};


template <typename ValueType>
class Set {
public:
    using key_type = ValueType;
    using value_type = ValueType;
    using size_type = size_t;
    using node_type = NodeAvl<value_type>;

    class iterator {
    private:
        NodeAvl<ValueType>* node;
        const Set<ValueType>* set;

        NodeAvl<ValueType>* find_preend() const
        {
            auto vertex = set->root;
            if (vertex == nullptr) {
                return nullptr;
            }
            while (vertex->right != nullptr) {
                vertex = vertex->right;
            }
            return vertex;
        }

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = ValueType;
        using reference = std::add_lvalue_reference_t<std::add_const_t<value_type>>;
        using pointer = std::add_pointer_t<std::add_const_t<value_type>>;
        using difference_type = int64_t;

        iterator(NodeAvl<ValueType>* node, const Set<ValueType>* set)
            : node(node)
            , set(set)
        {
        }
        iterator()
            : node(nullptr)
            , set(nullptr)
        {
        }

        inline iterator& operator++()
        {
            if (node->right != nullptr) {
                node = node->right;
                while (node->left != nullptr) {
                    node = node->left;
                }
            } else {
                while (node != nullptr && node->parent != nullptr && node == node->parent->right) {
                    node = node->parent;
                }
                node = node->parent;
            }
            return *this;
        }

        inline iterator& operator--()
        {
            if (node == nullptr) {
                node = find_preend();
                return *this;
            }

            if (node->left != nullptr) {
                node = node->left;
                while (node->right != nullptr) {
                    node = node->right;
                }
            } else {
                while (node != nullptr && node->parent != nullptr && node == node->parent->left) {
                    node = node->parent;
                }
                node = node->parent;
            }
            return *this;
        }

        iterator operator++(int)
        {
            iterator old = *this;
            ++(*this);
            return old;
        }
        iterator operator--(int)
        {
            iterator old = *this;
            --(*this);
            return old;
        }

        const ValueType operator*() { return node->value; }
        const ValueType* operator->() { return &node->value; }

        bool operator==(const iterator& other) const { return node == other.node; }
        bool operator!=(const iterator& other) const { return node != other.node; }
    };

    Set()
        : begin_(nullptr, nullptr)
    {
    }

    template <typename InputIt>
    Set(InputIt first, InputIt last)
        : begin_(nullptr, this)
    {
        while (first != last) {
            insert(*first++);
        }
    }

    explicit Set(std::initializer_list<ValueType> list)
        : Set(list.begin(), list.end())
    {
    }
    // TODO
    Set(const Set<ValueType>& other) // O(n log n)
        : Set(other.begin(), other.end())
    {
    } 

    ~Set()
    {
        node_type::Clear(root);
        root = nullptr;
        begin_ = { nullptr, nullptr };
    }

    // TODO
    Set<ValueType>& operator=(const Set<ValueType>& other)  // O(n log n)
    {
        if (this == &other)
            return *this;

        node_type::Clear(root);
        root = nullptr;
        begin_ = { nullptr, this };
        size_ = 0;

        for (const auto& x : other) {
            insert(x);
        }
        
        return *this;
    }

    iterator begin() const { return begin_; }
    iterator end() const { return { nullptr, this }; }

    void insert(const ValueType& value)
    {
        if (find(value) == end()) {
            root = node_type::Insert(root, value);
            auto inserted = lower_bound(value);
            if (size_ == 0 || *inserted < *begin_) {
                begin_ = inserted;
            }
            ++size_;
        }
    }

    void erase(iterator iter)
    {
        if (iter == end()) {
            return;
        } else {
            erase(*iter);
        }
    }

    void erase(const ValueType& value)
    {
        if (find(value) != end()) {
            root = node_type::Erase(root, value);
            begin_ = { find_begin(), this };
            --size_;
        }
    }

    iterator find(const ValueType& value) const
    {
        return { node_type::Find(root, value), this };
    }
    iterator lower_bound(const ValueType& value) const
    {
        return { node_type::LowerBound(root, value), this };
    }
    iterator upper_bound(const ValueType& value) const
    {
        return { node_type::UpperBound(root, value), this };
    }

    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

private:
    node_type* root = nullptr;
    iterator begin_;
    size_t size_ = 0;

    node_type* find_begin() const
    {
        auto vertex = root;
        if (vertex == nullptr)
            return vertex;
        while (vertex->left != nullptr)
            vertex = vertex->left;
        return vertex;
    }
};
