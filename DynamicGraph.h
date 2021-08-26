#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <functional>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <random>
#include <string>
#include <vector>

std::mt19937 rng(228);

template <typename Node>
struct BottomupTreap {
    typedef Node* Ref;
    static uint32_t Size(Ref tree) { return !tree ? 0 : tree->size; }

private:
    inline bool ChoiceRandomlyBySizes(Ref left, Ref right)
    {
        return rng() % (left->size + right->size) < left->size;
    }

public:
    Ref Merge(Ref left, Ref right)
    {
        if (!left)
            return right;
        if (!right)
            return left;

        Ref tree = nullptr;
        uint64_t dirs = 0;
        int deep;
        for (deep = 0;; ++deep) {
            if (deep >= static_cast<int32_t>(sizeof(dirs) * 8 - 2)) {
                tree = Merge(left->right, right->left);
                dirs = dirs << 2 | 1;
                deep++;
                break;
            }
            dirs <<= 1;
            if (ChoiceRandomlyBySizes(left, right)) {
                Ref last_vertex = left->right;
                if (!last_vertex) {
                    tree = right;
                    right = right->parent;
                    break;
                }
                left = last_vertex;
            } else {
                dirs |= 1;
                Ref last_vertex = right->left;
                if (!last_vertex) {
                    tree = left;
                    left = left->parent;
                    break;
                }
                right = last_vertex;
            }
        }
        for (; deep >= 0; --deep) {
            if (!(dirs & 1)) {
                Ref parent = left->parent;
                tree = left->LinkRight(tree);
                left = parent;
            } else {
                Ref parent = right->parent;
                tree = right->LinkLeft(tree);
                right = parent;
            }
            dirs >>= 1;
        }
        return tree;
    }

    typedef std::pair<Ref, Ref> RefPair;

    RefPair Split2(Ref tree)
    {
        Ref left = tree->left, right = tree;
        Node::Cut(left);
        tree->LinkLeft(nullptr);
        Ref parent = tree->parent;
        while (parent) {
            tree->parent = nullptr;
            if (parent->left == tree)
                right = parent->LinkLeft(right);
            else
                left = parent->LinkRight(left);
            tree = parent;
            parent = tree->parent;
        }
        return RefPair(left, right);
    }
    RefPair Split3(Ref tree)
    {
        Ref left = tree->left, right = tree->right;
        Node::Cut(left), Node::Cut(right);
        tree->LinkLeftRight(nullptr, nullptr);
        Ref parent = tree->parent;
        while (parent) {
            tree->parent = nullptr;
            if (parent->left == tree)
                right = parent->LinkLeft(right);
            else
                left = parent->LinkRight(left);
            tree = parent;
            parent = tree->parent;
        }
        return RefPair(left, right);
    }
    Ref Cons(Ref vertex, Ref tree)
    {
        assert(Size(vertex) == 1);
        if (!tree)
            return vertex;
        Ref last_vertex = nullptr;

        bool is_leaf = false;
        do {
            if (ChoiceRandomlyBySizes(vertex, tree)) {
                Ref parent = tree->parent;
                last_vertex = vertex->LinkRight(tree);
                tree = parent;
                break;
            }
            Ref left = tree->left;
            if (!left) {
                last_vertex = vertex;
                break;
            }
            tree = left;
        } while (!is_leaf);

        while (tree) {
            last_vertex = tree->LinkLeft(last_vertex);
            tree = tree->parent;
        }
        return last_vertex;
    }
};

class EulerTourTreeWithMarks {
    struct Node {
        typedef BottomupTreap<Node> Treap;

        Node *left, *right, *parent;
        uint32_t size;
        char marks, markUnions;

        Node()
                : left(nullptr)
                , right(nullptr)
                , parent(nullptr)
                , size(1)
                , marks(0)
                , markUnions(0)
        {
        }

        inline Node* Update()
        {
            int size_tree = 1, mark_unions_t = marks;
            if (left) {
                size_tree += left->size;
                mark_unions_t |= left->markUnions;
            }
            if (right) {
                size_tree += right->size;
                mark_unions_t |= right->markUnions;
            }
            size = size_tree, markUnions = mark_unions_t;
            return this;
        }

        inline Node* LinkLeft(Node* vertex)
        {
            left = vertex;
            if (left)
                vertex->parent = this;
            return Update();
        }

        inline Node* LinkRight(Node* vertex)
        {
            right = vertex;
            if (right)
                vertex->parent = this;
            return Update();
        }

        inline Node* LinkLeftRight(Node* new_left, Node* new_right)
        {
            left = new_left;
            if (left)
                new_left->parent = this;
            right = new_right;
            if (right)
                new_right->parent = this;
            return Update();
        }

        inline static Node* Cut(Node* vertex)
        {
            if (vertex)
                vertex->parent = nullptr;
            return vertex;
        }

        inline static const Node* FindRoot(const Node* vertex)
        {
            while (vertex->parent)
                vertex = vertex->parent;
            return vertex;
        }

        inline static const Node* FindHead(const Node* vertex)
        {
            while (vertex->left)
                vertex = vertex->left;
            return vertex;
        }
        static void UpdatePath(Node* vertex)
        {
            while (vertex) {
                vertex->Update();
                vertex = vertex->parent;
            }
        }
    };

    typedef Node::Treap Treap;
    Treap treap_;

    std::vector<Node> nodes_;
    std::vector<int> first_pos_;
    std::vector<bool> edge_mark_, vertex_mark_;

    inline int GetEdgeId(const Node* pos) const { return pos - &nodes_[0]; }
    inline int GetFirstPos(int edge_id) const { return edge_id; }
    inline int GetSecondPos(int edge_id) const { return edge_id + (NumVertices() - 1); }

public:
    inline int NumVertices() const { return first_pos_.size(); }
    inline int NumEdges() const { return NumVertices() - 1; }

    inline bool GetEdgeMark(int pos) const
    {
        return pos < NumEdges() && edge_mark_[pos];
    }
    inline bool GetVertexMark(int vertex) const
    {
        return vertex_mark_[vertex];
    }

private:
    inline void UpdateMarks(int pos, int vertex)
    {
        Node* tree = &nodes_[pos];
        tree->marks = GetEdgeMark(pos) << 0 | GetVertexMark(vertex) << 1;
        Node::UpdatePath(tree);
    }

    inline void FirstPosChanged(int vertex, int pos_first, int pos_second)
    {
        if (pos_first != -1)
            UpdateMarks(pos_first, vertex);
        if (pos_second != -1)
            UpdateMarks(pos_second, vertex);
    }

public:
    class TreeRef {
        friend class EulerTourTreeWithMarks;
        const Node* ref_;

    public:
        explicit TreeRef(const Node* ref = nullptr)
                : ref_(ref)
        {
        }
        inline bool operator==(const TreeRef& that) const { return ref_ == that.ref_; }
        inline bool operator!=(const TreeRef& that) const { return ref_ != that.ref_; }
        inline bool IsIsolatedVertex() const { return ref_ == nullptr; }
    };

    inline void Init(int vertex_count)
    {
        int edges_count = vertex_count - 1;
        first_pos_.assign(vertex_count, -1);
        nodes_.assign(edges_count * 2, Node());
        edge_mark_.assign(edges_count, false);
        vertex_mark_.assign(vertex_count, false);
    }

    inline TreeRef GetTreeRef(int vertex) const
    {
        int pos = first_pos_[vertex];
        return TreeRef(pos == -1 ? nullptr : Node::FindRoot(&nodes_[pos]));
    }

    inline bool IsConnected(int first, int second) const
    {
        if (first == second)
            return true;
        int first_pos = first_pos_[first], second_pos = first_pos_[second];
        if (first_pos == -1 || second_pos == -1)
            return false;
        return Node::FindRoot(&nodes_[first_pos]) == Node::FindRoot(&nodes_[second_pos]);
    }

    inline static int GetSize(TreeRef tree)
    {
        if (tree.IsIsolatedVertex())
            return 1;
        else
            return tree.ref_->size / 2 + 1;
    }

    inline void Link(int edge_id, int first, int second)
    {
        int first_edge_pos = GetFirstPos(edge_id), second_edge_pos = GetSecondPos(edge_id);
        if (first > second)
            std::swap(first_edge_pos, second_edge_pos);

        int first_id = first_pos_[first], second_id = first_pos_[second];

        Node *left, *mid, *right;
        if (first_id != -1) {
            std::pair<Node*, Node*> parent = treap_.Split2(&nodes_[first_id]);
            mid = treap_.Merge(parent.second, parent.first);
        } else {
            mid = nullptr;
            first_pos_[first] = first_edge_pos;
            FirstPosChanged(first, -1, first_edge_pos);
        }
        if (second_id != -1) {
            std::pair<Node*, Node*> parent = treap_.Split2(&nodes_[second_id]);
            left = parent.first, right = parent.second;
        } else {
            left = right = nullptr;
            first_pos_[second] = second_edge_pos;
            FirstPosChanged(second, -1, second_edge_pos);
        }
        mid = treap_.Cons(&nodes_[second_edge_pos], mid);
        right = treap_.Cons(&nodes_[first_edge_pos], right);

        treap_.Merge(treap_.Merge(left, mid), right);
    }

    inline void Cut(int edge_id, int first, int second)
    {
        if (first > second)
            std::swap(first, second);

        int first_edge_pos = GetFirstPos(edge_id), second_edge_pos = GetSecondPos(edge_id);
        std::pair<Node*, Node*> first_splitted = treap_.Split3(&nodes_[first_edge_pos]);
        uint32_t previous_size = Treap::Size(first_splitted.second);
        std::pair<Node*, Node*> second_splitted = treap_.Split3(&nodes_[second_edge_pos]);
        Node *left, *mid, *right;
        if (first_splitted.second == &nodes_[second_edge_pos] ||
            Treap::Size(first_splitted.second) != previous_size) {
            left = first_splitted.first;
            mid = second_splitted.first;
            right = second_splitted.second;
        } else {
            std::swap(first, second);
            std::swap(first_edge_pos, second_edge_pos);
            left = second_splitted.first;
            mid = second_splitted.second,
                    right = first_splitted.second;
        }

        if (first_pos_[first] == first_edge_pos) {
            int new_pos;
            if (right != nullptr) {
                new_pos = GetEdgeId(Node::FindHead(right));
            } else {
                new_pos = !left ? -1 : GetEdgeId(Node::FindHead(left));
            }
            first_pos_[first] = new_pos;
            FirstPosChanged(first, first_edge_pos, new_pos);
        }
        if (first_pos_[second] == second_edge_pos) {
            int new_pos = !mid ? -1 : GetEdgeId(Node::FindHead(mid));
            first_pos_[second] = new_pos;
            FirstPosChanged(second, second_edge_pos, new_pos);
        }

        treap_.Merge(left, right);
    }

    inline void ChangeEdgeMark(int edge_id, bool mark)
    {
        edge_mark_[edge_id] = mark;
        Node* t = &nodes_[edge_id];
        t->marks = (mark << 0) | (t->marks & (1 << 1));
        Node::UpdatePath(t);
    }
    inline void ChangeVertexMark(int vertex, bool mark)
    {
        vertex_mark_[vertex] = mark;
        int pos = first_pos_[vertex];
        if (pos != -1) {
            Node* tree = &nodes_[pos];
            tree->marks = (tree->marks & (1 << 0)) | (mark << 1);
            Node::UpdatePath(tree);
        }
    }

    template <typename Callback>
    inline bool EnumMarkedEdges(TreeRef tree, Callback callback) const
    {
        return EnumMarks<0, Callback>(tree, callback);
    }
    template <typename Callback>
    inline bool EnumMarkedVertices(TreeRef tree, Callback callback) const
    {
        return EnumMarks<1, Callback>(tree, callback);
    }

private:
    template <int Mark, typename Callback>
    inline bool EnumMarks(TreeRef tree, Callback callback) const
    {
        if (tree.IsIsolatedVertex())
            return true;
        const Node* vertex = tree.ref_;
        if (vertex->markUnions >> Mark & 1)
            return EnumMarksRec<Mark, Callback>(vertex, callback);
        else
            return true;
    }

    template <int Mark, typename Callback>
    inline bool EnumMarksRec(const Node* tree, Callback callback) const
    {
        const Node *left = tree->left, *right = tree->right;
        if (left && (left->markUnions >> Mark & 1))
            if (!EnumMarksRec<Mark, Callback>(left, callback))
                return false;
        if (tree->marks >> Mark & 1)
            if (!callback(GetEdgeId(tree)))
                return false;
        if (right && (right->markUnions >> Mark & 1))
            if (!EnumMarksRec<Mark, Callback>(right, callback))
                return false;
        return true;
    }
};

class DynamicGraph {
    typedef EulerTourTreeWithMarks Forest;
    typedef Forest::TreeRef TreeRef;

    uint32_t num_vertices_m_;
    uint32_t num_samplings_;
    uint32_t components_number_;
    uint32_t count_added_edges_;

    std::vector<Forest> forests_;

    std::vector<char> edge_level_;
    std::vector<int> tree_edge_index_; // EdgeIndex -> TreeEdgeIndex
    std::vector<int> tree_edge_map_; // TreeEdgeIndex -> EdgeIndex
    std::vector<int> tree_edge_index_free_list_; // [TreeEdgeIndex]

    std::vector<int> pos_head_;

    std::vector<std::vector<int>> first_incident_pos_;
    std::vector<int> next_incident_pos_, prev_incident_pos_;

    std::vector<bool> edge_visited_;
    std::vector<int> visited_edges_; // [EdgeIndex | TreeEdgeIndex]

    std::multimap<std::pair<int, int>, int> all_edges_; // { v, u } -> edgesId

    inline int GetFirstPos(int edge_id) const { return 2 * edge_id; }
    inline int GetSecondPos(int edge_id) const { return 2 * edge_id + 1; }
    inline int GetEdgeId(int pos) const { return pos / 2; }

    bool Replace(uint32_t lvl, int first, int second)
    {
        Forest& forest = forests_[lvl];

        TreeRef v_root = forest.GetTreeRef(first), w_root = forest.GetTreeRef(second);

        int v_size = forest.GetSize(v_root), w_size = forest.GetSize(w_root);

        int smallest;
        TreeRef u_root;
        if (v_size <= w_size)
            smallest = first, u_root = v_root;
        else
            smallest = second, u_root = w_root;

        int replacement_edge = -1;
        EnumIncidentPos(forest, u_root, smallest, lvl,
                        FindReplacementEdgeObject(u_root, &replacement_edge));

        if (replacement_edge != -1 && visited_edges_.size() + 1 <= num_samplings_) {
            DeleteNontreeEdge(replacement_edge);
            AddTreeEdge(replacement_edge);
            for (size_t i = 0; i < visited_edges_.size(); ++i) {
                edge_visited_[visited_edges_[i]] = false;
            }
            visited_edges_.clear();
            return true;
        }

        bool level_up = lvl + 1 < forests_.size();
        for (size_t i = 0; i < visited_edges_.size(); ++i) {
            int edge_id = visited_edges_[i];
            edge_visited_[edge_id] = false;

            if (level_up) {
                DeleteNontreeEdge(edge_id);
                ++edge_level_[edge_id];
                InsertNontreeEdge(edge_id);
            }
        }
        visited_edges_.clear();

        forest.EnumMarkedEdges(u_root, EnumLevelTreeEdgesObject(this));
        for (int edge_id : visited_edges_) {
            int pos = tree_edge_map_[edge_id];
            int new_first = pos_head_[GetSecondPos(pos)];
            int new_second = pos_head_[GetFirstPos(pos)];
            int new_lvl = edge_level_[pos];

            if (level_up) {
                edge_level_[pos] = new_lvl + 1;

                forests_[new_lvl].ChangeEdgeMark(edge_id, false);
                forests_[new_lvl + 1].ChangeEdgeMark(edge_id, true);

                forests_[new_lvl + 1].Link(edge_id, new_first, new_second);
            }
        }
        visited_edges_.clear();

        if (replacement_edge != -1) {
            DeleteNontreeEdge(replacement_edge);
            AddTreeEdge(replacement_edge);
            return true;
        } else if (lvl > 0) {
            return Replace(lvl - 1, first, second);
        } else {
            return false;
        }
    }

    struct EnumLevelTreeEdgesObject {
        DynamicGraph* thisp;
        explicit EnumLevelTreeEdgesObject(DynamicGraph* thisp)
                : thisp(thisp)
        {
        }

        inline bool operator()(int id)
        {
            thisp->EnumLevelTreeEdges(id);
            return true;
        }
    };

    inline void EnumLevelTreeEdges(int edge_id)
    {
        visited_edges_.push_back(edge_id);
    }

    template <typename Callback>
    inline bool EnumIncidentPos(Forest& forest, TreeRef tree, int vertex, int lvl,
                                Callback callback)
    {
        if (tree.IsIsolatedVertex())
            return EnumIncidentPosWithVertex<Callback>(lvl, vertex, callback);
        else
            return forest.EnumMarkedVertices(tree,
                                             EnumIncidentPosObject<Callback>(this, lvl, callback));
    }

    template <typename Callback>
    struct EnumIncidentPosObject {
        DynamicGraph* thisp;
        int lvl;
        Callback callback;

        EnumIncidentPosObject(DynamicGraph* thisp, int lvl, Callback callback)
                : thisp(thisp)
                , lvl(lvl)
                , callback(callback)
        {
        }

        inline bool operator()(int pos) const
        {
            return thisp->EnumIncidentPosWithTreePos(pos, lvl, callback);
        }
    };

    template <typename Callback>
    inline bool EnumIncidentPosWithTreePos(int tii, int lvl, Callback callback)
    {
        int vertex = GetTreeEdgeIndexTailVertex(tii);
        return EnumIncidentPosWithVertex(lvl, vertex, callback);
    }

    inline int GetTreeEdgeIndexTailVertex(int vertex_id) const
    {
        bool dir = vertex_id >= NumVertices() - 1;
        int pos = dir ? vertex_id - (NumVertices() - 1) : vertex_id;
        int edge_id = tree_edge_map_[pos];
        int first = pos_head_[GetSecondPos(edge_id)], w = pos_head_[GetFirstPos(edge_id)];
        int second = dir == (first > w) ? first : w;
        return second;
    }

    template <typename Callback>
    inline bool EnumIncidentPosWithVertex(int lvl, int vertex, Callback callback)
    {
        int pos = first_incident_pos_[lvl][vertex];
        while (pos != -1) {
            if (!callback(this, pos))
                return false;
            pos = next_incident_pos_[pos];
        }
        return true;
    }

    struct FindReplacementEdgeObject {
        TreeRef uRoot;
        int* replacementEdge;
        FindReplacementEdgeObject(TreeRef u_root, int* replacement_edge)
                : uRoot(u_root)
                , replacementEdge(replacement_edge)
        {
        }

        inline bool operator()(DynamicGraph* thisp, int pos) const
        {
            return thisp->FindReplacementEdge(pos, uRoot, replacementEdge);
        }
    };

    inline bool FindReplacementEdge(int pos, TreeRef u_root, int* replacement_edge)
    {
        int edge_id = GetEdgeId(pos);
        if (edge_visited_[edge_id])
            return true;

        int lvl = edge_level_[edge_id];
        TreeRef h_root = forests_[lvl].GetTreeRef(pos_head_[pos]);

        if (h_root.IsIsolatedVertex() || h_root != u_root) {
            *replacement_edge = edge_id;
            return false;
        }
        edge_visited_[edge_id] = true;
        visited_edges_.push_back(edge_id);
        return true;
    }

    inline void AddTreeEdge(int edge_id)
    {
        int first = pos_head_[GetSecondPos(edge_id)], second = pos_head_[GetFirstPos(edge_id)];
        int lvl = edge_level_[edge_id];

        int pos = tree_edge_index_free_list_.back();
        tree_edge_index_free_list_.pop_back();
        tree_edge_index_[edge_id] = pos;
        tree_edge_map_[pos] = edge_id;

        forests_[lvl].ChangeEdgeMark(pos, true);

        for (int i = 0; i <= lvl; ++i) {
            forests_[i].Link(pos, first, second);
        }
    }

    inline void InsertIncidentPos(int pos, int vertex)
    {
        int edge_id = GetEdgeId(pos);
        int lvl = edge_level_[edge_id];

        int next = first_incident_pos_[lvl][vertex];
        first_incident_pos_[lvl][vertex] = pos;
        next_incident_pos_[pos] = next;
        prev_incident_pos_[pos] = -1;
        if (next != -1)
            prev_incident_pos_[next] = pos;

        if (next == -1)
            forests_[lvl].ChangeVertexMark(vertex, true);
    }

    inline void DeleteIncidentPos(int pos, int vertex)
    {
        int edge_id = GetEdgeId(pos);
        int lvl = edge_level_[edge_id];

        int next = next_incident_pos_[pos], prev = prev_incident_pos_[pos];
        next_incident_pos_[pos] = prev_incident_pos_[pos] = -2;

        if (next != -1)
            prev_incident_pos_[next] = prev;
        if (prev != -1)
            next_incident_pos_[prev] = next;
        else
            first_incident_pos_[lvl][vertex] = next;

        if (next == -1 && prev == -1)
            forests_[lvl].ChangeVertexMark(vertex, false);
    }

    inline void InsertNontreeEdge(int edge_id)
    {
        int pos_first = GetFirstPos(edge_id), pos_second = GetSecondPos(edge_id);
        InsertIncidentPos(pos_first, pos_head_[pos_second]);
        InsertIncidentPos(pos_second, pos_head_[pos_first]);
    }

    inline void DeleteNontreeEdge(int edge_id)
    {
        int pos_first = GetFirstPos(edge_id), pos_second = GetSecondPos(edge_id);
        DeleteIncidentPos(pos_first, pos_head_[pos_second]);
        DeleteIncidentPos(pos_second, pos_head_[pos_first]);
    }

public:
    explicit DynamicGraph(int vertex_count)
            : num_vertices_m_(vertex_count)
            , num_samplings_(std::min(static_cast<int>(log2(vertex_count)) + 1, 16))
            , components_number_(vertex_count)
            , count_added_edges_(0)
            , forests_(num_samplings_)
            , tree_edge_map_(vertex_count - 1, -1)
            , tree_edge_index_free_list_(vertex_count - 1)
            , first_incident_pos_(num_samplings_, std::vector<int>(vertex_count, -1))
    {
        for (size_t lvl = 0; lvl < num_samplings_; ++lvl) {
            forests_[lvl].Init(vertex_count);
        }
        std::iota(tree_edge_index_free_list_.begin(), tree_edge_index_free_list_.end(), 0);
    }

    inline int NumVertices() const { return num_vertices_m_; }

    inline void AddEdge(int first, int second)
    {
        if (second > first)
            std::swap(second, first);
        int edge_id = count_added_edges_++;
        all_edges_.insert({ { first, second }, edge_id });

        pos_head_.push_back(second);
        pos_head_.push_back(first);
        next_incident_pos_.push_back(-2);
        next_incident_pos_.push_back(-2);
        prev_incident_pos_.push_back(-2);
        prev_incident_pos_.push_back(-2);
        edge_visited_.push_back(false);
        tree_edge_index_.push_back(-1);
        edge_level_.push_back(0);

        bool tree_edge = !forests_[0].IsConnected(first, second);
        if (tree_edge) {
            AddTreeEdge(edge_id);
        } else {
            tree_edge_index_[edge_id] = -1;
            if (first != second)
                InsertNontreeEdge(edge_id);
        }

        components_number_ -= tree_edge;
    }

    inline void RemoveEdge(int first, int second)
    {
        if (second > first)
            std::swap(second, first);
        auto iter_edge = all_edges_.find({ first, second });
        int edge_id = iter_edge->second;
        all_edges_.erase(iter_edge);

        int pos_first = GetFirstPos(edge_id), pos_second = GetSecondPos(edge_id);
        first = pos_head_[pos_second], second = pos_head_[pos_first];

        int lvl = edge_level_[edge_id];
        int pos = tree_edge_index_[edge_id];

        bool splitted = false;
        if (pos != -1) {
            tree_edge_map_[pos] = -1;
            tree_edge_index_[edge_id] = -1;
            tree_edge_index_free_list_.push_back(pos);

            for (int i = 0; i <= lvl; ++i) {
                forests_[i].Cut(pos, first, second);
            }

            forests_[lvl].ChangeEdgeMark(pos, false);

            splitted = !Replace(lvl, first, second);
        } else {
            if (first != second)
                DeleteNontreeEdge(edge_id);
        }

        pos_head_[pos_first] = pos_head_[pos_second] = -1;
        edge_level_[edge_id] = -1;

        components_number_ += splitted;
    }

    inline int GetComponentsNumber() const
    {
        return components_number_;
    }

    inline bool IsConnected(int first, int second) const
    {
        return forests_[0].IsConnected(first, second);
    }
};
