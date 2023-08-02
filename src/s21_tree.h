#ifndef S21_CONTAINER_SRC_S21_TREE_H_
#define S21_CONTAINER_SRC_S21_TREE_H_

#include <limits>
#include <utility>
#include <iterator>

namespace s21 {

enum class RBTreeNodeColor : bool { kRed, kBlack };

struct RBTreeNodeBase {
  using ColorType = RBTreeNodeColor;
  using BasePtr = RBTreeNodeBase*;

  RBTreeNodeBase() = default;
  explicit RBTreeNodeBase(ColorType color) noexcept : m_color(color) {}

  ColorType m_color{};
  BasePtr m_parent{};
  BasePtr m_left{};
  BasePtr m_right{};
};

template<typename Tp>
struct RBTreeNode : public RBTreeNodeBase {
  Tp m_data;

  RBTreeNode() = default;
  RBTreeNode(const RBTreeNode& other) : RBTreeNodeBase(other.m_color), m_data(other.m_data) {}
  explicit RBTreeNode(const Tp& value) : m_data(value) {}
};

struct RBTreeIteratorBase {
  using BasePtr = typename RBTreeNodeBase::BasePtr;
  using ColorType = typename RBTreeNodeBase::ColorType;
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = ptrdiff_t;

  BasePtr node;

  explicit RBTreeIteratorBase(BasePtr other_node) noexcept : node(other_node) {}

  void Increment() noexcept {
    if (node->m_right) {
      node = node->m_right;
      while (node->m_left) {
        node = node->m_left;
      }
    } else {
      BasePtr tmp = node->m_parent;
      while (node == tmp->m_right) {
        node = tmp;
        tmp = tmp->m_parent;
      }
      if (node->m_right != tmp) {
        node = tmp;
      }
    }
  }

  void Decrement() noexcept {
    if (node->m_color == ColorType::kRed &&
        node->m_parent->m_parent == node) {
      node = node->m_right;
    } else if (node->m_left) {
      BasePtr tmp = node->m_left;
      while (tmp->m_right) {
        tmp = tmp->m_right;
      }
      node = tmp;
    } else {
      BasePtr tmp = node->m_parent;
      while (node == tmp->m_left) {
        node = tmp;
        tmp = tmp->m_parent;
      }
      node = tmp;
    }
  }
};

template<typename Val, typename Ref, typename Ptr>
struct RBTreeIterator : public RBTreeIteratorBase {
  using value_type = Val;
  using reference = Ref;
  using pointer = Ptr;
  using Self = RBTreeIterator<Val, Ref, Ptr>;
  using NodePtr = RBTreeNode<Val>*;

  explicit RBTreeIterator(BasePtr ptr = nullptr) noexcept : RBTreeIteratorBase(ptr) {}

  reference operator*() const noexcept { return static_cast<NodePtr>(node)->m_data; }

  pointer operator->() const noexcept { return &static_cast<NodePtr>(node)->m_data; }

  Self& operator++() noexcept {
    Increment();
    return *this;
  }

  Self operator++(int) noexcept {
    Self tmp = *this;
    Increment();
    return tmp;
  }

  Self& operator--() noexcept {
    Decrement();
    return *this;
  }

  Self operator--(int) noexcept {
    Self tmp = *this;
    Decrement();
    return tmp;
  }
};

inline bool operator==(const RBTreeIteratorBase &rhs,
                       const RBTreeIteratorBase &lhs) noexcept {
  return rhs.node == lhs.node;
}

inline bool operator!=(const RBTreeIteratorBase &rhs,
                       const RBTreeIteratorBase &lhs) noexcept {
  return rhs.node != lhs.node;
}

namespace KeyGetters {

template<class Tp>
struct identity {
  const Tp &operator()(const Tp &r) const noexcept { return r; }
};

template<class Pair>
struct select_first {
  const typename Pair::first_type &operator()(const Pair &p) const noexcept { return p.first; }
};

} // KeyGetters

template<typename Key, typename Value, typename KeyOfValue, typename Compare>
class RBTree {
 public:
  using key_type = Key;
  using value_type = Value;
  using pointer = Value*;
  using const_pointer = const Value*;
  using reference = Value&;
  using const_reference = const Value&;
  using Node = RBTreeNode<Value>;
  using NodePtr = Node*;
  using BasePtr = typename RBTreeNodeBase::BasePtr;
  using ColorType = typename RBTreeNodeBase::ColorType;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using iterator = RBTreeIterator<value_type, reference, pointer>;
  using const_iterator = RBTreeIterator<value_type, const_reference, const_pointer>;

  RBTree() : m_base(new Node()) {
    m_base->m_parent = nullptr;
    m_base->m_left = m_base->m_right = m_base;
  }

  RBTree(const RBTree& other)
      : m_node_count(other.m_node_count), m_key_compare(other.m_key_compare), m_base(new Node()) {
    m_base->m_color = ColorType::kRed;
    if (!other.m_base->m_parent) {
      m_base->m_parent = nullptr;
      m_base->m_left = m_base->m_right = m_base;
    } else {
      m_base->m_parent = CopyTree(other.m_base->m_parent, m_base);
      m_base->m_left = GetMinNode(m_base->m_parent);
      m_base->m_right = GetMaxNode(m_base->m_parent);
    }
  }

  RBTree(RBTree&& other) noexcept
      : m_node_count(other.m_node_count), m_key_compare(other.m_key_compare), m_base(other.m_base) {
    other.m_base = nullptr;
    other.m_node_count = 0;
  }

  ~RBTree() {
    clear();
    delete m_base;
  }

  RBTree& operator=(const RBTree& other) {
    if (this != &other) {
      RBTree tmp(other);
      swap(tmp);
    }
    return *this;
  }

  RBTree& operator=(RBTree&& other) noexcept {
    RBTree tmp(std::move(other));
    if (this != &other) {
      swap(tmp);
    }
    return *this;
  }

  void clear() {
    if (m_node_count != 0) {
      DeleteTree(m_base->m_parent);
      m_base->m_parent = nullptr;
      m_base->m_left = m_base->m_right = m_base;
      m_node_count = 0;
    }
  }

  void swap(RBTree& other) noexcept {
    std::swap(other.m_base, m_base);
    std::swap(other.m_node_count, m_node_count);
    std::swap(other.m_key_compare, m_key_compare);
  }

  iterator begin() noexcept { return iterator(m_base->m_left); }

  const_iterator begin() const noexcept { return const_iterator(m_base->m_left); }

  const_iterator cbegin() const noexcept { return const_iterator(m_base->m_left); }

  iterator end() noexcept { return iterator(m_base); }

  const_iterator end() const noexcept { return const_iterator(m_base); }

  const_iterator cend() const noexcept { return const_iterator(m_base); }

  bool empty() const noexcept { return m_node_count == 0; }

  size_type size() const noexcept { return m_node_count; }

  size_type max_size() const noexcept { return std::numeric_limits<size_type>::max()/2/sizeof(Node); }

  iterator find(const Key& key) noexcept {
    return iterator(m_find(key));
  }

  const_iterator find(const Key& key) const noexcept {
    return const_iterator(m_find(key));
  }

  std::pair<iterator, iterator> equal_range(const Key& key) noexcept {
    return std::make_pair(lower_bound(key), upper_bound(key));
  }

  std::pair<const_iterator, const_iterator> equal_range(const Key& key) const noexcept {
    return std::make_pair(lower_bound(key), upper_bound(key));
  }

  iterator lower_bound(const Key& key) noexcept {
    return iterator(m_low_bound(key));
  }

  const_iterator lower_bound(const Key& key) const noexcept {
    return const_iterator(m_low_bound(key));
  }

  iterator upper_bound(const Key& key) noexcept {
    return iterator(m_up_bound(key));
  }

  const_iterator upper_bound(const Key& key) const noexcept {
    return const_iterator(m_up_bound(key));
  }

  size_type count(const Key& key) const noexcept {
    std::pair<const_iterator, const_iterator> r_pair = equal_range(key);
    size_type n_nodes = 0;
    for (; r_pair.first != r_pair.second; ++r_pair.first, ++n_nodes);
    return n_nodes;
  }

  void merge_unique(RBTree& other) {
    int i = 0;
    iterator to_delete[other.size()];
    for(auto it = other.begin(); it != other.end(); ++it) {
      auto pair_found = insert_unique(*it);
      if (pair_found.second) {
        to_delete[i++] = it;
      }
    }
    while(i) {
      other.erase(to_delete[--i]);
    }
  }

  void merge_equal(RBTree& other) {
    for(auto& val : other) {
      insert_equal(val);
    }
    other.clear();
  }

  std::pair<iterator, bool> insert_unique(const Value& val) {
    BasePtr curr_node = m_base->m_parent, prev_node = m_base;
    bool cmp = true;
    while (curr_node) {
      prev_node = curr_node;
      cmp = m_key_compare(KeyOfValue()(val), GetKey(curr_node));
      curr_node = cmp ? curr_node->m_left : curr_node->m_right;
    }
    auto it = iterator(prev_node);
    if (cmp) {
      if (it == begin()) {
        return std::pair<iterator, bool>(m_insert(curr_node, prev_node, val), true);
      } else {
        --it;
      }
    }
    if (m_key_compare(GetKey(it.node), KeyOfValue()(val))) {
      return std::pair<iterator, bool>(m_insert(curr_node, prev_node, val), true);
    }
    return std::pair<iterator, bool>(it, false);
  }

  std::pair<iterator, bool> insert_equal(const Value& val) {
    BasePtr prev_node = m_base, curr_node = m_base->m_parent;
    while (curr_node) {
      prev_node = curr_node;
      curr_node = m_key_compare(KeyOfValue()(val), GetKey(curr_node)) ?
                  curr_node->m_left : curr_node->m_right;
    }
    return std::make_pair(m_insert(curr_node, prev_node, val), true);
  }

  void erase(iterator position) {
    auto node_to_delete = EraseRebalance(position.node);
    delete node_to_delete;
    --m_node_count;
  }

  void erase(const Key& key) {
    std::pair<iterator, iterator> r_pair = equal_range(key);
    if (r_pair.first == begin() && r_pair.second == end()) {
      clear();
    } else {
      while (r_pair.first != r_pair.second) {
        erase(r_pair.first++);
      }
    }
  }

 protected:
  static const Key& GetKey(BasePtr node) noexcept { return KeyOfValue()(static_cast<NodePtr>(node)->m_data); }

  static BasePtr GetMinNode(BasePtr root) noexcept {
    while (root->m_left) {
      root = root->m_left;
    }
    return root;
  }
  static BasePtr GetMaxNode(BasePtr root) noexcept {
    while (root->m_right) {
      root = root->m_right;
    }
    return root;
  }

  static void DeleteTree(BasePtr root) {
    while (root) {
      DeleteTree(root->m_right);
      BasePtr left = root->m_left;
      delete root;
      root = left;
    }
  }

  static NodePtr CopyTree(BasePtr root, BasePtr head) {
    auto top = new Node(*static_cast<NodePtr>(root));
    top->m_parent = head;
    if (root->m_right) {
      top->m_right = CopyTree(root->m_right, top);
    }
    head = top;
    root = root->m_left;
    while (root) {
      auto copy = new Node(*static_cast<NodePtr>(root));
      head->m_left = copy;
      copy->m_parent = head;
      if (root->m_right) {
        copy->m_right = CopyTree(root->m_right, copy);
      }
      head = copy;
      root = root->m_left;
    }
    return top;
  }

  BasePtr m_up_bound(const Key& key) const noexcept {
    BasePtr prev_node = m_base, curr_node = m_base->m_parent;
    while (curr_node) {
      if (m_key_compare(key, GetKey(curr_node))) {
        prev_node = curr_node;
        curr_node = curr_node->m_left;
      } else {
        curr_node = curr_node->m_right;
      }
    }
    return prev_node;
  }

  BasePtr m_low_bound(const Key& key) const noexcept {
    BasePtr prev_node = m_base, curr_node = m_base->m_parent;
    while (curr_node) {
      if (!m_key_compare(GetKey(curr_node), key)) {
        prev_node = curr_node;
        curr_node = curr_node->m_left;
      } else {
        curr_node = curr_node->m_right;
      }
    }
    return prev_node;
  }

  BasePtr m_find(const Key& key) const noexcept {
    BasePtr found_node = m_low_bound(key);
    if (found_node == m_base || m_key_compare(key, GetKey(found_node))) {
      return m_base;
    }
    return found_node;
  }

  iterator m_insert(BasePtr curr_node, BasePtr prev_node, const Value& val) {
    auto new_node = new Node(val);
    if (prev_node == m_base || curr_node ||
        m_key_compare(KeyOfValue()(val), GetKey(prev_node))) {
      prev_node->m_left = new_node;
      if (prev_node == m_base) {
        m_base->m_parent = new_node;
        m_base->m_right = new_node;
      } else if (prev_node == m_base->m_left) {
        m_base->m_left = new_node;
      }
    } else {
      prev_node->m_right = new_node;
      if (prev_node == m_base->m_right) {
        m_base->m_right = new_node;
      }
    }
    new_node->m_parent = prev_node;
    new_node->m_left = new_node->m_right = nullptr;
    InsertRebalance(new_node);
    ++m_node_count;
    return iterator(new_node);
  }

  void RotateLeft(BasePtr node) noexcept {
    BasePtr right_child = node->m_right;
    node->m_right = right_child->m_left;
    if (right_child->m_left) {
      right_child->m_left->m_parent = node;
    }
    right_child->m_parent = node->m_parent;
    if (node == m_base->m_parent) {
      m_base->m_parent = right_child;
    } else if (node == node->m_parent->m_left) {
      node->m_parent->m_left = right_child;
    } else {
      node->m_parent->m_right = right_child;
    }
    right_child->m_left = node;
    node->m_parent = right_child;
  }

  void RotateRight(BasePtr node) noexcept {
    BasePtr left_child = node->m_left;
    node->m_left = left_child->m_right;
    if (left_child->m_right) {
      left_child->m_right->m_parent = node;
    }
    left_child->m_parent = node->m_parent;
    if (node == m_base->m_parent) {
      m_base->m_parent = left_child;
    } else if (node == node->m_parent->m_right) {
      node->m_parent->m_right = left_child;
    } else {
      node->m_parent->m_left = left_child;
    }
    left_child->m_right = node;
    node->m_parent = left_child;
  }

  void InsertRebalance(BasePtr node) noexcept {
    node->m_color = ColorType::kRed;
    while (node != m_base->m_parent && node->m_parent->m_color == ColorType::kRed) {
      BasePtr uncle;
      if (node->m_parent == node->m_parent->m_parent->m_left) {
        uncle = node->m_parent->m_parent->m_right;
        if (uncle && uncle->m_color == ColorType::kRed) {
          node->m_parent->m_color = ColorType::kBlack;
          uncle->m_color = ColorType::kBlack;
          node->m_parent->m_parent->m_color = ColorType::kRed;
          node = node->m_parent->m_parent;
        } else {
          if (node == node->m_parent->m_right) {
            node = node->m_parent;
            RotateLeft(node);
          }
          node->m_parent->m_color = ColorType::kBlack;
          node->m_parent->m_parent->m_color = ColorType::kRed;
          RotateRight(node->m_parent->m_parent);
        }
      } else {
        uncle = node->m_parent->m_parent->m_left;
        if (uncle && uncle->m_color == ColorType::kRed) {
          node->m_parent->m_color = ColorType::kBlack;
          uncle->m_color = ColorType::kBlack;
          node->m_parent->m_parent->m_color = ColorType::kRed;
          node = node->m_parent->m_parent;
        } else {
          if (node == node->m_parent->m_left) {
            node = node->m_parent;
            RotateRight(node);
          }
          node->m_parent->m_color = ColorType::kBlack;
          node->m_parent->m_parent->m_color = ColorType::kRed;
          RotateLeft(node->m_parent->m_parent);
        }
      }
    }
    m_base->m_parent->m_color = ColorType::kBlack;
  }

  BasePtr EraseRebalance(BasePtr node) noexcept {
    BasePtr curr_node = node, prev_node, pv_parent;
    if (!curr_node->m_left) {
      prev_node = curr_node->m_right;
    } else {
      if (!curr_node->m_right) {
        prev_node = curr_node->m_left;
      } else {
        curr_node = GetMinNode(curr_node->m_right);
        prev_node = curr_node->m_right;
      }
    }
    if (curr_node != node) {
      node->m_left->m_parent = curr_node;
      curr_node->m_left = node->m_left;
      if (curr_node != node->m_right) {
        pv_parent = curr_node->m_parent;
        if (prev_node) {
          curr_node->m_parent = curr_node->m_parent;
        }
        curr_node->m_parent->m_left = prev_node;
        curr_node->m_right = node->m_right;
        node->m_right->m_parent = curr_node;
      } else {
        pv_parent = curr_node;
      }
      if (m_base->m_parent == node) {
        m_base->m_parent = curr_node;
      } else if (node->m_parent->m_left == node) {
        node->m_parent->m_left = curr_node;
      } else {
        node->m_parent->m_right = curr_node;
      }
      curr_node->m_parent = node->m_parent;
      std::swap(curr_node->m_color, node->m_color);
      curr_node = node;
    } else {
      pv_parent = curr_node->m_parent;
      if (prev_node) {
        prev_node->m_parent = curr_node->m_parent;
      }
      if (m_base->m_parent == node) {
        m_base->m_parent = prev_node;
      } else {
        if (node->m_parent->m_left == node) {
          node->m_parent->m_left = prev_node;
        } else {
          node->m_parent->m_right = prev_node;
        }
      }
      if (m_base->m_left == node) {
        if (!node->m_right) {
          m_base->m_left = node->m_parent;
        } else {
          m_base->m_left = GetMinNode(prev_node);
        }
      }
      if (m_base->m_right == node) {
        if (!node->m_left) {
          m_base->m_right = node->m_parent;
        } else {
          m_base->m_right = GetMaxNode(prev_node);
        }
      }
    }
    if (curr_node->m_color != ColorType::kRed) {
      while (prev_node != m_base->m_parent &&
          (!prev_node || prev_node->m_color == ColorType::kBlack)) {
        BasePtr uncle;
        if (prev_node == pv_parent->m_left) {
          uncle = pv_parent->m_right;
          if (uncle->m_color == ColorType::kRed) {
            uncle->m_color = ColorType::kBlack;
            pv_parent->m_color = ColorType::kRed;
            RotateLeft(pv_parent);
            uncle = pv_parent->m_right;
          }
          if ((!uncle->m_left || uncle->m_left->m_color == ColorType::kBlack) &&
              (!uncle->m_right || uncle->m_right->m_color == ColorType::kBlack)) {
            uncle->m_color = ColorType::kRed;
            prev_node = pv_parent;
            pv_parent = pv_parent->m_parent;
          } else {
            if (!uncle->m_right || uncle->m_right->m_color == ColorType::kBlack) {
              if (uncle->m_left) {
                uncle->m_left->m_color = ColorType::kBlack;
              }
              uncle->m_color = ColorType::kRed;
              RotateRight(uncle);
              uncle = pv_parent->m_right;
            }
            uncle->m_color = pv_parent->m_color;
            pv_parent->m_color = ColorType::kBlack;
            if (uncle->m_right) {
              uncle->m_right->m_color = ColorType::kBlack;
            }
            RotateLeft(pv_parent);
            break;
          }
        } else {
          uncle = pv_parent->m_left;
          if (uncle->m_color == ColorType::kRed) {
            uncle->m_color = ColorType::kBlack;
            pv_parent->m_color = ColorType::kRed;
            RotateRight(pv_parent);
            uncle = pv_parent->m_left;
          }
          if ((!uncle->m_right || uncle->m_right->m_color == ColorType::kBlack) &&
              (!uncle->m_left || uncle->m_left->m_color == ColorType::kBlack)) {
            uncle->m_color = ColorType::kRed;
            prev_node = pv_parent;
            pv_parent = pv_parent->m_parent;
          } else {
            if (!uncle->m_left || uncle->m_left->m_color == ColorType::kBlack) {
              if (uncle->m_right) {
                uncle->m_right->m_color = ColorType::kBlack;
              }
              uncle->m_color = ColorType::kRed;
              RotateLeft(uncle);
              uncle = pv_parent->m_left;
            }
            uncle->m_color = pv_parent->m_color;
            pv_parent->m_color = ColorType::kBlack;
            if (uncle->m_left) {
              uncle->m_left->m_color = ColorType::kBlack;
            }
            RotateRight(pv_parent);
            break;
          }
        }
      }
      if (prev_node) {
        prev_node->m_color = ColorType::kBlack;
      }
    }
    return curr_node;
  }

 private:
  size_type m_node_count{};
  Compare m_key_compare;
  BasePtr m_base;
};

} // namespace s21

#endif // S21_CONTAINER_SRC_S21_TREE_H_