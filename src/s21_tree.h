#ifndef S21_CONTAINER_SRC_S21_TREE_H_
#define S21_CONTAINER_SRC_S21_TREE_H_

#include <memory>
#include <utility>

namespace s21 {

enum class RBNodeColor : int8_t { red, black, doubleBlack};

struct RBTreeNodeBase {
  using BasePtr = RBTreeNodeBase*;

  RBTreeNodeBase() = default;
  explicit RBTreeNodeBase(RBNodeColor color) : m_color(color) {}

  RBNodeColor m_color{};
  BasePtr m_parent{};
  BasePtr m_left{};
  BasePtr m_right{};
};

bool operator==(RBTreeNodeBase *lhs, RBNodeColor rhs) {
  return rhs == (lhs == nullptr ? RBNodeColor::black : lhs->m_color);
}

bool operator!=(RBTreeNodeBase *lhs, RBNodeColor rhs) {
  return !(lhs == rhs);
}

template<typename Tp>
struct RBTreeNode : public RBTreeNodeBase {
  Tp m_data;
  explicit RBTreeNode(const Tp &data, RBNodeColor color = RBNodeColor::red) noexcept
      : m_data(data), RBTreeNodeBase(color) {}
};

template<typename NodePtrType>
NodePtrType RBTreeIncrement(NodePtrType node) {
  if (node->m_right) {
    node = node->m_right;
    while (node->m_left) {
      node = node->m_left;
    }
  } else {
    NodePtrType tmp = node->m_parent;
    while (tmp && node == tmp->m_right) {
      node = tmp;
      tmp = tmp->m_parent;
    }
    node = tmp;
  }
  return node;
}

template<typename NodePtrType>
NodePtrType RBTreeDecrement(NodePtrType node) {
  if (node == RBNodeColor::red &&
      node->m_parent->m_parent == node) {
    node = node->m_right;
  } else if (node->m_left) {
    NodePtrType tmp = node->m_left;
    while (tmp->m_right) {
      tmp = tmp->m_right;
    }
    node = tmp;
  } else {
    NodePtrType tmp = node->m_parent;
    while (tmp && node == tmp->m_left) {
      node = tmp;
      tmp = tmp->m_parent;
    }
    node = tmp;
  }
  return node;
}

template<typename Tp>
struct RBTreeIterator {
  using value_type = Tp;
  using reference = Tp&;
  using pointer = Tp*;
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = ptrdiff_t;
  using Base_ptr = RBTreeNodeBase::BasePtr;
  using NodePtr = RBTreeNode<Tp>*;
  using Self = RBTreeIterator<Tp>;

  explicit RBTreeIterator(Base_ptr node = nullptr) noexcept: m_current(node) {}

  Self &operator++() noexcept {
    m_current = RBTreeIncrement(m_current);
    return *this;
  }

  Self operator++(int) noexcept {
    Self tmp = *this;
    m_current = RBTreeIncrement(m_current);
    return tmp;
  }

  Self &operator--() noexcept {
    m_current = RBTreeDecrement(m_current);
    return *this;
  }

  Self operator--(int) noexcept {
    Self tmp = *this;
    m_current = RBTreeDecrement(m_current);
    return tmp;
  }

  reference operator*() noexcept {
    return static_cast<NodePtr>(m_current)->m_data;
  }

  pointer operator->() noexcept {
    return &static_cast<NodePtr>(m_current)->m_data;
  }

  friend bool operator==(const Self &lhs, const Self &rhs) {
    return lhs.m_current == rhs.m_current;
  }

  friend bool operator!=(const Self &lhs, const Self &rhs) {
    return lhs.m_current != rhs.m_current;
  }

 private:
  Base_ptr m_current;
};

template<typename Tp>
struct RBTreeConstIterator {
  using value_type = Tp;
  using reference = const Tp&;
  using pointer = const Tp*;
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = ptrdiff_t;
  using ConstBasePtr = const RBTreeNodeBase*;
  using BasePtr = typename RBTreeNodeBase::BasePtr;
  using ConstNodePtr = const RBTreeNode<Tp>*;
  using Self = RBTreeConstIterator<Tp>;
  using iterator = RBTreeIterator<Tp>;

  explicit RBTreeConstIterator(ConstBasePtr node = nullptr) noexcept
      : m_current(node) {}

  iterator const_cast_() {
    return iterator(const_cast<BasePtr>(m_current));
  }

  Self &operator++() noexcept {
    m_current = RBTreeIncrement(m_current);
    return *this;
  }

  Self operator++(int) noexcept {
    Self tmp = *this;
    m_current = RBTreeIncrement(m_current);
    return tmp;
  }

  Self &operator--() noexcept {
    m_current = RBTreeDecrement(m_current);
    return *this;
  }

  Self operator--(int) noexcept {
    Self tmp = *this;
    m_current = RBTreeDecrement(m_current);
    return tmp;
  }

  reference operator*() noexcept {
    return static_cast<ConstNodePtr>(m_current)->m_data;
  }

  pointer operator->() noexcept {
    return &static_cast<ConstNodePtr>(m_current)->m_data;
  }

  friend bool operator==(const Self &lhs, const Self &rhs) {
    return lhs.m_current == rhs.m_current;
  }

  friend bool operator!=(const Self &lhs, const Self &rhs) {
    return lhs.m_current != rhs.m_current;
  }

 private:
  ConstBasePtr m_current;
};

template<typename Tp, typename Compare = std::less<Tp>>
class RBTree {
 public:
  using Color = RBNodeColor;
  using BasePtr = typename RBTreeNodeBase::BasePtr;
  using Node = RBTreeNode<Tp>;
  using NodePtr = Node*;
  using value_type = Tp;
  using pointer = Tp*;
  using reference = Tp&;
  using const_reference = const Tp&;
  using iterator = RBTreeIterator<Tp>;
  using const_iterator = RBTreeConstIterator<Tp>;
  using size_type = size_t;

  const_iterator cbegin() const noexcept { return const_iterator(GetMinNode(m_root)); }

  const_iterator cend() const noexcept { return const_iterator(); }

  iterator begin() noexcept { return iterator(GetMinNode(m_root)); }

  iterator end() noexcept { return iterator(); }

  const_iterator begin() const noexcept { return const_iterator(GetMinNode(m_root)); }

  const_iterator end() const noexcept { return const_iterator(); }

  RBTree() = default;

  RBTree(const RBTree& other)
    : m_size(other.m_size) {
    m_root = CopyTree(other.m_root);
  }

  RBTree(RBTree &&other) noexcept
    : m_root(other.m_root), m_size(other.m_size) {
    other.m_root = nullptr;
  }

  ~RBTree() { DeleteTree(m_root); }

  RBTree& operator=(const RBTree& other) {
    RBTree tmp(other);
    swap(tmp);
    return *this;
  }

  RBTree& operator=(RBTree&& other) noexcept {
    RBTree tmp(std::move);
    swap(tmp);
    return *this;
  }

  iterator find(const value_type& key) noexcept {
    return iterator(FindBST(m_root,key));
  }

  const_iterator find(const value_type& key) const noexcept {
    return const_iterator(FindBST(m_root,key));
  }

  size_type size() const noexcept {
    return m_size;
  }

  size_type max_size() const noexcept {
    return std::allocator<Node>().max_size();
  }

  void swap(RBTree& other) noexcept {
    BasePtr tmp = other.m_root;
    other.m_root = m_root;
    m_root = tmp;
  }

  void clear() {
    DeleteTree(m_root);
  }

  bool empty() const noexcept {
    return (m_root == nullptr);
  }

  void erase(value_type data) {
    BasePtr nodeToDelete = GetNodeToDelete(m_root, data);
    DeleteFixup(nodeToDelete);
  }

  std::pair<iterator, bool> insert_unique(const_reference data) {
      BasePtr current_node = m_root;
      BasePtr parent = nullptr;
      while(current_node) {
        parent = current_node;
        value_type curr_data = static_cast<NodePtr>(current_node)->m_data;
        if(m_compare(data,curr_data)) {
          current_node = current_node->m_left;
        } else if (m_compare(curr_data, data)) {
          current_node = current_node->m_right;
        } else {
          return std::make_pair(iterator(current_node), false);
        }
      }
      return m_insert(data, parent);
  }

  std::pair<iterator, bool> insert_equal(const_reference data) {
    BasePtr current_node = m_root;
    BasePtr parent = nullptr;
    while(current_node) {
      parent = current_node;
      value_type curr_data = static_cast<NodePtr>(current_node)->m_data;
      if(m_compare(data,curr_data)) {
        current_node = current_node->m_left;
      } else {
        current_node = current_node->m_right;
      }
    }
    return m_insert(data, parent);
  }

  size_type count(const_reference key) const noexcept {
    size_type count_keys = 0;
    for(auto &i : *this) {
      if (equal(i, key)) {
        ++count_keys;
      }
    }
    return count_keys;
  }

  std::pair<iterator,iterator> equal_range(const_reference key) {
    for (auto first = begin(); first != end();++first) {
      if (equal(*first,key)) {
        auto second = first;
        for(;equal(*second,key); ++second)
          ;
        return std::pair<iterator,iterator>(first, second);
      }
    }
    return std::make_pair<iterator, iterator>(iterator(end()), iterator(end()));
  }

  iterator lower_bound(const_reference key) {
    for(auto it = begin(); it != end(); ++it) {
      if (!m_compare(*it, key) || equal(*it,key)) {
        return it;
      }
    }
    return end();
  }

  iterator upper_bound(const_reference key) {
    for(auto it = begin(); it != end(); ++it) {
      if (!m_compare(*it, key) && !equal(*it,key)) {
        return it;
      }
    }
    return end();
  }

 protected:
  bool equal(const_reference lhs, const_reference rhs) const noexcept {
    return !m_compare(lhs, rhs) && !(m_compare(rhs,lhs));
  }

  static BasePtr GetMinNode(BasePtr node) {
    while (node->m_left) {
      node = node->m_left;
    }
    return node;
  }

  BasePtr GetNodeToDelete(BasePtr root, value_type data) {
    if (root == nullptr) {
      return root;
    }
    reference root_data = static_cast<NodePtr>(root)->m_data;
    if (m_compare(data,root_data)) {
      return GetNodeToDelete(root->m_left, data);
    }
    if (m_compare(root_data, data)) {
      return GetNodeToDelete(root->m_right, data);
    }
    if (!root->m_left || !root->m_right) {
      return root;
    }
    BasePtr temp = GetMinNode(root->m_right);
    root_data = static_cast<NodePtr>(temp)->m_data;
    return GetNodeToDelete(root->m_right, root_data);
  }

  void RotateRight(BasePtr node) {
    BasePtr left_child = node->m_left;
    node->m_left = left_child->m_right;
    if (node->m_left) {
      node->m_left->m_parent = node;
    }
    left_child->m_parent = node->m_parent;
    if (!node->m_parent) {
      m_root = left_child;
    } else if (node == node->m_parent->m_right) {
      node->m_parent->m_right = left_child;
    } else {
      node->m_parent->m_left = left_child;
    }
    left_child->m_right = node;
    node->m_parent = left_child;
  }

  void RotateLeft(BasePtr node) {
    BasePtr right_child = node->m_right;
    node->m_right = right_child->m_left;
    if (node->m_right) {
      node->m_right->m_parent = node;
    }
    right_child->m_parent = node->m_parent;
    if (!node->m_parent) {
      m_root = right_child;
    } else if (node == node->m_parent->m_left) {
      node->m_parent->m_left = right_child;
    } else {
      node->m_parent->m_right = right_child;
    }
    right_child->m_left = node;
    node->m_parent = right_child;
  }

  std::pair<iterator, bool> m_insert(const_reference data, BasePtr parent) {
    auto new_node = new Node(data);
    new_node->m_parent = parent;
    if(!parent) {
      m_root = new_node;
    } else if(m_compare(data, static_cast<NodePtr>(parent)->m_data)) {
      parent->m_left = new_node;
    } else {
      parent->m_right = new_node;
    }
    InsertFixup(new_node);
    ++m_size;
    return std::make_pair(iterator(new_node), true);
  }

  void InsertFixup(BasePtr new_node){
    BasePtr uncle;
    while(new_node != m_root && new_node->m_parent == Color::red) {
      if(new_node->m_parent == new_node->m_parent->m_parent->m_left) {
        uncle = new_node->m_parent->m_parent->m_right;
        if(uncle == Color::red) {
          new_node->m_parent->m_color = Color::black;
          uncle->m_color = Color::black;
          new_node->m_parent->m_parent->m_color = Color::red;
          new_node = new_node->m_parent->m_parent;
        } else {
          if(new_node == new_node->m_parent->m_right) {
            new_node = new_node->m_parent;
            RotateLeft(new_node);
          }
          new_node->m_parent->m_color = Color::black;
          new_node->m_parent->m_parent->m_color = Color::red;
          RotateRight(new_node->m_parent->m_parent);
        }
      } else {
        uncle = new_node->m_parent->m_parent->m_left;
        if(uncle == Color::red) {
          new_node->m_parent->m_color = Color::black;
          uncle->m_color = Color::black;
          new_node->m_parent->m_parent->m_color = Color::red;
          new_node = new_node->m_parent->m_parent;
        } else {
          if(new_node == new_node->m_parent->m_left) {
            new_node = new_node->m_parent;
            RotateRight(new_node);
          }
          new_node->m_parent->m_color = Color::black;
          new_node->m_parent->m_parent->m_color = Color::red;
          RotateLeft(new_node->m_parent->m_parent);
        }
      }
    }
    m_root->m_color = Color::black;
  }

  void DeleteFixup(BasePtr node) {
    if (node == nullptr) {
      return;
    }
    if (node == m_root) {
      m_root = nullptr;
      return;
    }

    if (node == Color::red || node->m_left == Color::red || node->m_right == Color::red) {
      BasePtr child = node->m_left ? node->m_left : node->m_right;
      if (node == node->m_parent->m_left) {
        node->m_parent->m_left = child;
      } else {
        node->m_parent->m_right = child;
      }
      if (child) {
        child->m_parent = node->m_parent;
        child->m_color = Color::black;
      }
      delete node;
    } else {
      BasePtr sibling, parent, ptr = node;
      ptr->m_color = Color::doubleBlack;
      while (ptr != m_root && ptr == Color::doubleBlack) {
        parent = ptr->m_parent;
        if (ptr == parent->m_left) {
          sibling = parent->m_right;
          if (sibling == Color::red) {
            sibling->m_color = Color::black;
            parent->m_color = Color::red;
            RotateLeft(parent);
          } else {
            if (sibling->m_left == Color::black && sibling->m_right == Color::black) {
              sibling->m_color = Color::red;
              if (parent == Color::red) {
                parent->m_color = Color::black;
              } else {
                parent->m_color = Color::doubleBlack;
              }
              ptr = parent;
            } else {
              if (sibling->m_right == Color::black) {
                sibling->m_left->m_color = Color::black;
                sibling->m_color = Color::red;
                RotateRight(sibling);
                sibling = parent->m_right;
              }
              sibling->m_color = parent->m_color;
              parent->m_color = Color::black;
              sibling->m_right->m_color = Color::black;
              RotateLeft(parent);
              break;
            }
          }
        } else {
          sibling = parent->m_left;
          if (sibling == Color::red) {
            sibling->m_color = Color::black;
            parent->m_color = Color::red;
            RotateRight(parent);
          } else {
            if (sibling->m_left == Color::black && sibling->m_right == Color::black) {
              sibling->m_color = Color::red;;
              if (parent == Color::red) {
                parent->m_color = Color::black;
              } else {
                parent->m_color = Color::doubleBlack;
              }
              ptr = parent;
            } else {
              if (sibling->m_left == Color::black) {
                sibling->m_right->m_color = Color::black;
                sibling->m_color = Color::red;
                RotateLeft(sibling);
                sibling = parent->m_left;
              }
              sibling->m_color = parent->m_color;
              parent->m_color = Color::black;
              sibling->m_left->m_color = Color::black;
              RotateRight(parent);
              break;
            }
          }
        }
      }
      if (node == node->m_parent->m_left) {
        node->m_parent->m_left = nullptr;
      } else {
        node->m_parent->m_right = nullptr;
      }
      delete node;
      m_root->m_color = Color::black;
    }
  }

  void DeleteTree(BasePtr node) noexcept {
    if (!node) {
      return;
    }
    DeleteTree(node->m_left);
    DeleteTree(node->m_right);
    delete node;
  }

  BasePtr FindBST(BasePtr root, value_type key) const noexcept {
    if (!root) {
      return nullptr;
    }
    value_type val = static_cast<NodePtr>(root)->m_data;
    if (m_compare(val, key)) {
      return FindBST(root->m_right, key);
    } else if(m_compare(key,val)) {
      return FindBST(root->m_left, key);
    }
    return root;
  }

  static BasePtr CopyTree(BasePtr root) noexcept {
    if (!root) {
      return nullptr;
    }
    BasePtr new_root = new Node(static_cast<NodePtr>(root)->m_data, root->m_color);
    new_root->m_left = CopyTree(root->m_left);
    new_root->m_right = CopyTree(root->m_right);
    if (new_root->m_left) {
      new_root->m_left->m_parent = new_root;
    }
    if (new_root->m_right) {
      new_root->m_right->m_parent = new_root;
    }
    return new_root;
  }

 private:
  BasePtr m_root{};
  Compare m_compare;
  size_type m_size{};
};

} // namespace s21

#endif // S21_CONTAINER_SRC_S21_TREE_H_