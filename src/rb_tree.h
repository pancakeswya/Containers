#ifndef RB_TREE_H
#define RB_TREE_H

#include <limits>
#include <iterator>

namespace s21 {

enum RBTreeColor { RED, BLACK, DOUBLE_BLACK };

struct RBTreeNodeBase {
  typedef RBTreeNodeBase*       Base_ptr;
  typedef const RBTreeNodeBase* ConstBase_ptr;

  explicit RBTreeNodeBase(const RBTreeColor &color) : color_(color), left_(), right_(), parent_() {}

  friend bool operator==(ConstBase_ptr lhs, const RBTreeColor &rhs) {
    return rhs == (lhs == nullptr ? BLACK : lhs->color_);
  }

  friend bool operator!=(ConstBase_ptr lhs, const RBTreeColor &rhs) {
    return !(lhs == rhs);
  }

  RBTreeColor color_;
  Base_ptr left_;
  Base_ptr right_;
  Base_ptr parent_;
};

template<typename Tp>
struct RBTreeNode : public RBTreeNodeBase {
  explicit RBTreeNode(const Tp& data) noexcept: data_(data), RBTreeNodeBase(RED) {}

  explicit RBTreeNode(const Tp& data, const RBTreeColor &color) noexcept: data_(data), RBTreeNodeBase(color) {}

  Tp data_;
};

static RBTreeNodeBase *RBTreeIncrement(RBTreeNodeBase *node) {
  if (node->right_) {
    node = node->right_;
    while (node->left_) {
      node = node->left_;
    }
  } else {
    RBTreeNodeBase * temp = node->parent_;
    while (temp && node == temp->right_) {
      node = temp;
      temp = temp->parent_;
    }
    node = temp;
  }
  return node;
}

static RBTreeNodeBase *RBTreeDecrement(RBTreeNodeBase *node) {
  if (node == RED &&
    node->parent_->parent_ == node) {
    node = node->right_;
  } else if (node->left_) {
    RBTreeNodeBase * temp = node->left_;
    while (temp->right_) {
      temp = temp->right_;
    }
    node = temp;
  } else {
    RBTreeNodeBase * temp = node->parent_;
    while (temp && node == temp->left_) {
      node = temp;
      temp = temp->parent_;
    }
    node = temp;
  }
  return node;
}

static const RBTreeNodeBase *RBTreeIncrement(const RBTreeNodeBase *node) {
  if (node->right_) {
    node = node->right_;
    while (node->left_) {
      node = node->left_;
    }
  } else {
    RBTreeNodeBase *temp = node->parent_;
    while (temp && node == temp->right_) {
      node = temp;
      temp = temp->parent_;
    }
    node = temp;
  }
  return node;
}

static const RBTreeNodeBase *RBTreeDecrement(const RBTreeNodeBase *node) {
  if (node == RED &&
    node->parent_->parent_ == node) {
    node = node->right_;
  } else if (node->left_) {
    RBTreeNodeBase * temp = node->left_;
    while (temp->right_) {
      temp = temp->right_;
    }
    node = temp;
  } else {
    RBTreeNodeBase * temp = node->parent_;
    while (temp && node == temp->left_) {
      node = temp;
      temp = temp->parent_;
    }
    node = temp;
  }
  return node;
}


template<typename Tp>
struct RBTreeIterator {
  typedef Tp                              value_type;
  typedef Tp&                             reference;
  typedef Tp*                             pointer;
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef ptrdiff_t                       difference_type;
  typedef RBTreeNodeBase::Base_ptr        Base_ptr;
  typedef RBTreeNode<Tp>                  Node;
  typedef RBTreeIterator<Tp>              Self;

  RBTreeIterator() noexcept : current_() {}

  explicit RBTreeIterator(Base_ptr node = nullptr) noexcept : current_(node) {}

  Self &operator++() noexcept {
    current_ = RBTreeIncrement(current_);
    return *this;
  }

  Self operator++(int) noexcept {
    Self tmp = *this;
    current_ = RBTreeIncrement(current_);
    return tmp;
  }

  Self &operator--() noexcept {
    current_ = RBTreeDecrement(current_);
    return *this;
  }

  Self operator--(int) noexcept {
    Self tmp = *this;
    current_ = RBTreeDecrement(current_);
    return tmp;
  }

  reference operator*() noexcept {
    return static_cast<Node *>(current_)->data_;
  }

  pointer operator->() noexcept {
    return &static_cast<Node *>(current_)->data_;
  }

  friend bool operator==(const Self& lhs, const Self& rhs) {
    return lhs.current_ == rhs.current_;
  }

  friend bool operator!=(const Self& lhs, const Self& rhs) {
    return lhs.current_ != rhs.current_;
  }

 private:
  Base_ptr current_;

};

template<typename Tp>
struct RBTreeConstIterator {
  typedef Tp                              value_type;
  typedef const Tp&                       reference;
  typedef const Tp*                       pointer;
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef ptrdiff_t                       difference_type;
  typedef RBTreeNodeBase::ConstBase_ptr   ConstBase_ptr;
  typedef RBTreeNodeBase::Base_ptr        Base_ptr;
  typedef const RBTreeNode<Tp>            Node;
  typedef RBTreeConstIterator<Tp>         Self;
  typedef RBTreeIterator<Tp>              iterator;

  RBTreeConstIterator() noexcept : current_() {}

  explicit RBTreeConstIterator(ConstBase_ptr node = nullptr) noexcept
      : current_(node) {}

  iterator const_cast_() {
    return iterator(const_cast<Base_ptr>(current_));
  }

  Self &operator++() noexcept {
    current_ = RBTreeIncrement(current_);
    return *this;
  }

  Self operator++(int) noexcept {
    Self tmp = *this;
    current_ = RBTreeIncrement(current_);
    return tmp;
  }

  Self &operator--() noexcept {
    current_ = RBTreeDecrement(current_);
    return *this;
  }

  Self operator--(int) noexcept {
    Self tmp = *this;
    current_ = RBTreeDecrement(current_);
    return tmp;
  }

  reference operator*() noexcept {
    return static_cast<Node*>(current_)->data_;
  }

  pointer operator->() noexcept {
    return &static_cast<Node*>(current_)->data_;
  }

  friend bool operator==(const Self& lhs, const Self& rhs) {
    return lhs.current_ == rhs.current_;
  }

  friend bool operator!=(const Self& lhs, const Self& rhs) {
    return lhs.current_ != rhs.current_;
  }

 private:
  ConstBase_ptr current_;
};

template<typename Tp, typename Compare>
class rb_tree {
 public:
  typedef Tp                         value_type;
  typedef Tp&                        reference;
  typedef Tp*                        pointer;
  typedef RBTreeNode<Tp>             Node;
  typedef RBTreeIterator<Tp>         iterator;
  typedef RBTreeConstIterator<Tp>    const_iterator;
  typedef RBTreeNodeBase*            Base_ptr;
  typedef const RBTreeNodeBase*      ConstBase_ptr;
  typedef size_t                     size_type;

  rb_tree() noexcept: root_() {}

  explicit rb_tree(const Compare& cmp) : root_(), key_compare(cmp) {}

  rb_tree(const rb_tree &other) noexcept : rb_tree() {
    root_ = copyTree(other.root_);
  }

  rb_tree(rb_tree &&other) noexcept : root_(other.root_) {
    other.root_ = nullptr;
  }

  ~rb_tree() noexcept { clear(); }

  rb_tree& operator=(const rb_tree& other) {
    rb_tree tmp(other);
    swap(tmp);
    return *this;
  }

  rb_tree& operator=(rb_tree&& other)  noexcept {
    rb_tree tmp(std::move);
    swap(tmp);
    return *this;
  }

  std::pair<iterator, bool> insert(value_type n) noexcept {
    Base_ptr node = new Node(n);
    return insertNode(node);
  }

  template<typename... Args>
  std::pair<iterator, bool> emplace(Args&&... args) {
    auto node = static_cast<Node*>(::operator new(sizeof(Node)));
    new (node)RBTreeNodeBase(RBTreeNodeBase(RED));
    new (&node->data_)value_type(std::forward<Args>(args)...);
    return insertNode(node);
  }

  iterator find(const value_type& key) noexcept {
    return iterator(findBST(root_,key));
  }

  const_iterator find(const value_type& key) const noexcept {
    return const_iterator(findBST(root_,key));
  }

  void erase(value_type data) noexcept {
    Base_ptr node = findBST(root_, data);
    if (node) {
      deleteNode(node);
    }
  }

   size_type size() const noexcept {
    size_type count = 0;
    for(auto it = begin(); it != end(); ++it) {
      count++;
    }
    return count;
  }

   size_type max_size() const noexcept {
    return std::numeric_limits<size_type>::max()/2/sizeof(Node);
  }

  void clear() {
    deleteTree(root_);
  }

   bool empty() const noexcept {
    return (root_ == nullptr);
  }

  void swap(rb_tree& other) noexcept {
    Base_ptr tmp = other.root_;
    other.root_ = *this;
    *this = tmp;
  }

  const_iterator cbegin() const noexcept { return const_iterator(getMinNode(root_)); }

  const_iterator cend() const noexcept { return const_iterator(root_->parent_); }

  iterator begin() noexcept { return iterator(getMinNode(root_)); }

  iterator end() noexcept { return iterator(root_->parent_); }

  const_iterator begin() const noexcept { return const_iterator(getMinNode(root_)); }

  const_iterator end() const noexcept { return const_iterator(root_->parent_); }

 protected:
  static Base_ptr getMinNode(Base_ptr node) {
    if (node) {
      while (node->left_) {
        node = node->left_;
      }
    }
    return node;
  }

  void deleteTree(Base_ptr node) noexcept {
    if (!node) {
      return;
    }
    deleteTree(node->left_);
    deleteTree(node->right_);
    delete node;
  }

  std::pair<iterator, bool> insertNode(Base_ptr node) {
    Base_ptr ptr = insertBST(root_, node);
    if (root_ && root_->parent_) {
      delete node;
      Base_ptr found_node = root_->parent_;
      root_->parent_ = nullptr;
      root_ = ptr;
      return std::make_pair<iterator, bool>(iterator(found_node), false);
    }
    root_ = ptr;
    balanceTree(node);
    return std::make_pair<iterator, bool>(iterator(node), true);
  }

  Base_ptr insertBST(Base_ptr root, Base_ptr node) noexcept {
    if (!root) {
      return node;
    }
    value_type key1 = static_cast<Node*>(node)->data_;
    value_type key2 = static_cast<Node*>(root)->data_;
    if (key_compare(key1,key2)) {
      root->left_ = insertBST(root->left_, node);
      root->left_->parent_ = root;
    } else if (key_compare(key2,key1)) {
      root->right_ = insertBST(root->right_, node);
      root->right_->parent_ = root;
    } else {
      root_->parent_ = root;
    }
    return root;
  }

  Base_ptr findBST(Base_ptr root, value_type key) const noexcept {
    if (!root) {
      return nullptr;
    }
    value_type val = static_cast<Node*>(root)->data_;
    if (key_compare(val, key)) {
      return findBST(root->right_, key);
    } else if(key_compare(key,val)) {
      return findBST(root->left_, key);
    }
    return root;
  }

  Base_ptr copyTree(Base_ptr root) noexcept {
    if (!root) {
      return nullptr;
    }
    Base_ptr new_root = new Node(static_cast<Node*>(root)->data_, root->color_);
    new_root->left_ = copyTree(root->left_);
    new_root->right_ = copyTree(root->right_);
    if (new_root->left_) {
      new_root->left_->parent_ = new_root;
    }
    if (new_root->right_) {
      new_root->right_->parent_ = new_root;
    }
    return new_root;
  }

  void rotateLeft(Base_ptr node) noexcept {
    Base_ptr right_child = node->right_;
    node->right_ = right_child->left_;
    if (node->right_) {
      node->right_->parent_ = node;
    }
    right_child->parent_ = node->parent_;
    if (!node->parent_) {
      root_ = right_child;
    } else if (node == node->parent_->left_) {
      node->parent_->left_ = right_child;
    } else {
      node->parent_->right_ = right_child;
    }
    right_child->left_ = node;
    node->parent_ = right_child;
  }

  void rotateRight(Base_ptr node) noexcept {
    Base_ptr left_child = node->left_;
    node->left_ = left_child->right_;
    if (node->left_) {
      node->left_->parent_ = node;
    }
    left_child->parent_ = node->parent_;
    if (!node->parent_) {
      root_ = left_child;
    } else if (node == node->parent_->left_) {
      node->parent_->left_ = left_child;
    } else {
      node->parent_->right_ = left_child;
    }
    left_child->right_ = node;
    node->parent_ = left_child;
  }

  void balanceTree(Base_ptr node) noexcept {
    Base_ptr parent;
    Base_ptr grandparent;
    Base_ptr uncle;
    while (node != root_ && node == RED && node->parent_ == RED) {
      parent = node->parent_;
      grandparent = parent->parent_;
      uncle = (parent == grandparent->left_) ? grandparent->right_
                                             : grandparent->left_;
      if (uncle == RED) {
        uncle->color_ = BLACK;
        parent->color_ = BLACK;
        grandparent->color_ = RED;
        node = grandparent;
      } else {
        if (parent == grandparent->left_) {
          if (node == parent->right_) {
            rotateLeft(parent);
            node = parent;
            parent = node->parent_;
          }
          rotateRight(grandparent);
        } else {
          if (node == parent->left_) {
            rotateRight(parent);
            node = parent;
            parent = node->parent_;
          }
          rotateLeft(grandparent);
        }
        std::swap(parent->color_, grandparent->color_);
        node = parent;
      }
    }
    root_->color_ = BLACK;
  }

  void deleteNode(Base_ptr node) noexcept {
    if (node == root_) {
      root_ = nullptr;
      return;
    }
    if (node == RED || node->left_ == RED || node->right_ == RED) {
      Base_ptr child = (node->left_) ? node->left_ : node->right_;
      if (node == node->parent_->left_) {
        node->parent_->left_ = child;
      } else {
        node->parent_->right_ = child;
      }
      if (child) {
        child->parent_ = node->parent_;
        child->color_ = BLACK;
      }
    } else {
      Base_ptr sibling;
      Base_ptr parent;
      Base_ptr ptr = node;
      ptr->color_ = DOUBLE_BLACK;
      while (ptr != root_ && ptr == DOUBLE_BLACK) {
        parent = ptr->parent_;
        sibling = (ptr == parent->left_) ? parent->right_ : parent->left_;
        if (sibling == RED) {
          sibling->color_ = BLACK;
          parent->color_ = RED;
          rotateLeft(parent);
        } else {
          if (sibling->left_ == BLACK && sibling->right_ == BLACK) {
            sibling->color_ = RED;
            if (parent == RED) {
              parent->color_ = BLACK;
            } else {
              parent->color_ = DOUBLE_BLACK;
            }
            ptr = parent;
          } else {
            if (ptr == parent->left_) {
              if (sibling->right_ == BLACK) {
                sibling->left_->color_ = BLACK;
                sibling->color_ = RED;
                rotateRight(sibling);
                sibling = parent->right_;
              }
              sibling->color_ = parent->color_;
              parent->color_ = BLACK;
              sibling->right_->color_ = BLACK;
              rotateLeft(parent);
            } else {
              if (sibling->left_ == BLACK) {
                sibling->right_->color_ = BLACK;
                sibling->color_ = RED;
                rotateLeft(sibling);
                sibling = parent->left_;
              }
              sibling->color_ = parent->color_;
              parent->color_ = BLACK;
              sibling->left_->color_ = BLACK;
              rotateRight(parent);
            }
            break;
          }
        }
      }
      if (node == node->parent_->left_) {
        node->parent_->left_ = nullptr;
      } else {
        node->parent_->right_ = nullptr;
      }
      root_->color_ = BLACK;
    }
    delete node;
  }

 private:
  Base_ptr root_;
  Compare key_compare;
};

}

#endif