#ifndef S21_CONTAINER_SRC_LIST_H_
#define S21_CONTAINER_SRC_LIST_H_

#include <initializer_list>
#include <iterator>
#include <limits>

namespace s21 {

struct ListNodeBase {
  ListNodeBase() noexcept : m_prev_(this), m_next_(this) {}

  ListNodeBase(ListNodeBase* const prev, ListNodeBase* const next) noexcept
    : m_prev_(prev), m_next_(next) {
    m_prev_->m_next_ = m_next_->m_prev_ = this;
  }

  virtual ~ListNodeBase() noexcept {
    m_prev_->m_next_ = m_next_;
    m_next_->m_prev_ = m_prev_;
  }

  ListNodeBase *m_prev_;
  ListNodeBase *m_next_;
};

template<typename Tp>
struct ListNode : public ListNodeBase {
  ListNode(ListNodeBase* const prev, ListNodeBase* const next, const Tp& data)
    : ListNodeBase(prev, next), m_data_(data) {}

  Tp m_data_;
};

template<typename Tp>
class ListIterator {
 public:
  using Node = ListNode<Tp>;
  using difference_type = ptrdiff_t;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = Tp;
  using pointer = Tp*;
  using reference = Tp&;

  ListIterator() noexcept : m_node_() {}

  explicit ListIterator(ListNodeBase* node) noexcept : m_node_(node) {}

  reference operator*() const noexcept { return static_cast<Node*>(m_node_)->m_data_; }

  pointer operator->() const noexcept { return &static_cast<Node*>(m_node_)->m_data_; }

  ListIterator& operator++() noexcept {
    m_node_ = m_node_->m_next_;
    return *this;
  }

  ListIterator operator++(int) noexcept {
    ListIterator ret(*this);
    m_node_ = m_node_->m_next_;
    return ret;
  }

  ListIterator& operator--() noexcept {
    m_node_ = m_node_->m_prev_;
    return *this;
  }

  ListIterator operator--(int) noexcept {
    ListIterator ret(*this);
    m_node_ = m_node_->m_prev_;
    return ret;
  }

  friend bool operator==(const ListIterator& lhs, const ListIterator& rhs) {
    return lhs.m_node_ == rhs.m_node_;
  }

  friend bool operator!=(const ListIterator& lhs, const ListIterator& rhs) {
    return lhs.m_node_ != rhs.m_node_;
  }

  ListNodeBase* m_node_;

};

template<typename Tp>
class ListConstIterator {
 public:
  using iterator = ListIterator<Tp>;
  using Node = const ListNode<Tp>;
  using difference_type = ptrdiff_t;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = Tp;
  using pointer = const Tp*;
  using reference = const Tp&;

  ListConstIterator() noexcept : m_node_() {}

  explicit ListConstIterator(const ListNodeBase* node) noexcept : m_node_(node) {}

  ListConstIterator(const iterator& it) noexcept : m_node_(it.m_node_) {}

  iterator const_cast_() noexcept {
    return iterator(const_cast<ListNodeBase*>(m_node_));
  }

  reference operator*() const noexcept { return static_cast<Node*>(m_node_)->m_data_; }

  pointer operator->() const noexcept { return &static_cast<Node*>(m_node_)->m_data_; }

  ListConstIterator& operator++() noexcept {
    m_node_ = m_node_->m_next_;
    return *this;
  }

  ListConstIterator operator++(int) noexcept {
    ListConstIterator ret(*this);
    m_node_ = m_node_->m_next_;
    return ret;
  }

  ListConstIterator& operator--() noexcept {
    m_node_ = m_node_->m_prev_;
    return *this;
  }

  ListConstIterator operator--(int) noexcept {
    ListConstIterator ret(*this);
    m_node_ = m_node_->m_prev_;
    return ret;
  }

  friend bool operator==(const ListConstIterator& lhs, const ListConstIterator& rhs) {
    return lhs.m_node_ == rhs.m_node_;
  }

  friend bool operator!=(const ListConstIterator& lhs, const ListConstIterator& rhs) {
    return lhs.m_node_ != rhs.m_node_;
  }

  const ListNodeBase* m_node_;
};

template<typename Tp>
class list {
 public:
  using value_type = Tp;
  using reference = Tp&;
  using const_reference = const Tp&;
  using pointer = Tp*;
  using const_pointer = const Tp*;
  using Node = ListNode<value_type>;
  using iterator = ListIterator<value_type>;
  using const_iterator = ListConstIterator<value_type>;
  using size_type = size_t;

  list() : m_base_(new ListNodeBase) {}

  explicit list(size_type n) : list()  {
    for (;n != 0; --n) {
      emplace_back();
    }
  }

  list(std::initializer_list<value_type> const &items) : list()  {
    for (auto &item : items) {
      push_back(item);
    }
  }

  list(const list& other) : list()  {
    for (auto &val : other) {
      push_back(val);
    }
  }

  list(list&& other) noexcept : m_base_(other.m_base_) {
    other.m_base_->m_next_ = other.m_base_->m_prev_ = other.m_base_;
  }

  ~list() {
    clear();
    delete m_base_;
  }

  list& operator=(const list& other) {
    list tmp(other);
    swap(tmp);
    return *this;
  }

  list& operator=(list&& other) noexcept {
    list tmp(std::move(other));
    swap(tmp);
    return *this;
  }

  void clear() {
    while (!empty()) {
      ListNodeBase *next = m_base_->m_next_->m_next_;
      delete m_base_->m_next_;
      m_base_->m_next_ = next;
    }
  }

  bool empty() const noexcept { return m_base_ == m_base_->m_next_; }

  void erase(iterator pos) {
    delete pos.m_node_;
  }

  void pop_front() {
    erase(begin());
  };

  void pop_back() {
    erase(tail());
  }

  void merge(list& other) {
    if (this != &other) {
      list tmp(size() + other.size());
      auto it_tmp = tmp.begin();
      for (;;) {
        if (empty() && other.empty()) {
          break;
        } else if (other.empty() || front() < other.front()) {
          *it_tmp = std::move(front());
          pop_front();
        } else {
          *it_tmp = std::move(other.front());
          other.pop_front();
        }
        it_tmp++;
      }
      swap(tmp);
    }
  }

  void splice(const_iterator pos, list& other) {
    ListNodeBase* node = pos.const_cast_().m_node_;

    node->m_prev_->m_next_ = other.m_base_->m_next_;
    other.m_base_->m_next_->m_prev_ = node->m_prev_;

    node->m_prev_ = other.m_base_->m_prev_;
    other.m_base_->m_prev_->m_next_ = node;

    other.m_base_->m_next_ = other.m_base_;
    other.m_base_->m_prev_ = other.m_base_;
  }

  void reverse() noexcept {
    std::swap(m_base_->m_next_, m_base_->m_prev_);
    for(auto it = begin(); it != end(); ++it) {
      std::swap(it.m_node_->m_next_, it.m_node_->m_prev_);
    }
  }

   size_type size() const noexcept {
    size_type node_count = 0;
    for (auto it = begin(); it != end();++it, node_count++)
      ;
    return node_count;
  }

   size_type max_size() const noexcept {
    return std::numeric_limits<size_type>::max()/2/sizeof(ListNodeBase);
  }

  void sort() noexcept {
    qsort(begin(), tail());
  }

  void unique() {
    iterator curr = begin();
    iterator next = curr;
    while (next != end()) {
      ++next;
      if (*next == *curr) {
        iterator tmp = next--;
        erase(tmp);
      }
      if (next == tail()) {
        next = ++curr;
      }
    }
  }

  const_reference front() const noexcept {
    return *begin();
  }

  const_reference back() const noexcept {
    return *tail();
  }

  iterator insert(iterator pos, const_reference value) {
    return iterator(new Node(pos.m_node_->m_prev_, pos.m_node_, value));
  }

  template <typename... Args>
  iterator insert_many(iterator pos,Args&&... args) {
    ([&]
    {
      pos = insert(pos, args);
      ++pos;
    } (), ...);
    return pos;
  }

  void push_front(const_reference value) {
    new Node(m_base_, m_base_->m_next_, value);
  }

  template <typename... Args>
  void emplace_back(Args&&... args) {
    emplace(cend(), std::forward<Args>(args)...);
  }

  template <typename... Args>
  void emplace_front(Args&&... args) {
    emplace(cbegin(), std::forward<Args>(args)...);
  }

  template <typename... Args>
  iterator emplace(const_iterator pos, Args&&... args) {
    iterator place = pos.const_cast_();
    auto node_ptr = static_cast<Node*>(::operator new(sizeof(Node)));
    new (node_ptr) ListNodeBase(ListNodeBase(place.m_node_->m_prev_, place.m_node_));
    new (&node_ptr->m_data_) value_type(std::forward<Args>(args)...);
    return iterator(node_ptr);
  }

  void swap(list& other) noexcept {
    std::swap(m_base_, other.m_base_);
  }

  void push_back(const_reference value) {
    new Node(m_base_->m_prev_, m_base_, value);
  }

  iterator begin() noexcept {
    return iterator(m_base_->m_next_);
  }

  iterator end() noexcept {
    return iterator(m_base_);
  }

  const_iterator begin() const noexcept {
    return const_iterator(m_base_->m_next_);
  }

  const_iterator end() const noexcept {
    return const_iterator(m_base_);
  }

  const_iterator cbegin() const noexcept {
    return const_iterator(m_base_->m_next_);
  }

  const_iterator cend() const noexcept {
    return const_iterator(m_base_);
  }

 private:
  ListNodeBase* m_base_;

  iterator tail() noexcept {
    return iterator(m_base_->m_prev_);
  }

  void qsort(iterator left, iterator right) noexcept {
    if (left == right || left == end() || right == end() || left == tail()) {
      return;
    }
    iterator pivot = partition(left, right);
    qsort(left, --pivot);
    qsort(++pivot, right);
  }

  iterator partition(iterator left, iterator right) noexcept {
    iterator i = left;
    for (iterator j = i; j != right; ++j) {
      if (*j <= *right) {
        std::swap(*i, *j);
        ++i;
      }
    }
    std::swap(*i, *right);
    return i;
  }
};

}

#endif // S21_CONTAINER_SRC_LIST_H_