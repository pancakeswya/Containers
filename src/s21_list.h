#ifndef S21_CONTAINER_SRC_S21_LIST_H_
#define S21_CONTAINER_SRC_S21_LIST_H_

#include <initializer_list>
#include <iterator>
#include <limits>

namespace s21 {

struct ListNodeBase {
  ListNodeBase() noexcept : m_prev(this), m_next(this) {}

  ListNodeBase(ListNodeBase* const prev, ListNodeBase* const next) noexcept
    : m_prev(prev), m_next(next) {
    m_prev->m_next = m_next->m_prev = this;
  }

  virtual ~ListNodeBase() noexcept {
    m_prev->m_next = m_next;
    m_next->m_prev = m_prev;
  }

  ListNodeBase *m_prev;
  ListNodeBase *m_next;
};

template<typename Tp>
struct ListNode : public ListNodeBase {
  Tp m_data;
  ListNode(ListNodeBase* const prev, ListNodeBase* const next, const Tp& data)
    : ListNodeBase(prev, next), m_data(data) {}
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

  ListIterator() noexcept : m_node() {}

  explicit ListIterator(ListNodeBase* node) noexcept : m_node(node) {}

  reference operator*() const noexcept { return static_cast<Node*>(m_node)->m_data; }

  pointer operator->() const noexcept { return &static_cast<Node*>(m_node)->m_data; }

  ListIterator& operator++() noexcept {
    m_node = m_node->m_next;
    return *this;
  }

  ListIterator operator++(int) noexcept {
    ListIterator ret(*this);
    m_node = m_node->m_next;
    return ret;
  }

  ListIterator& operator--() noexcept {
    m_node = m_node->m_prev;
    return *this;
  }

  ListIterator operator--(int) noexcept {
    ListIterator ret(*this);
    m_node = m_node->m_prev;
    return ret;
  }

  friend bool operator==(const ListIterator& lhs, const ListIterator& rhs) {
    return lhs.m_node == rhs.m_node;
  }

  friend bool operator!=(const ListIterator& lhs, const ListIterator& rhs) {
    return lhs.m_node != rhs.m_node;
  }

  ListNodeBase* m_node;

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

  ListConstIterator() noexcept : m_node() {}

  explicit ListConstIterator(const ListNodeBase* node) noexcept : m_node(node) {}

  ListConstIterator(const iterator& it) noexcept : m_node(it.m_node) {}

  iterator const_cast_() noexcept {
    return iterator(const_cast<ListNodeBase*>(m_node));
  }

  reference operator*() const noexcept { return static_cast<Node*>(m_node)->m_data; }

  pointer operator->() const noexcept { return &static_cast<Node*>(m_node)->m_data; }

  ListConstIterator& operator++() noexcept {
    m_node = m_node->m_next;
    return *this;
  }

  ListConstIterator operator++(int) noexcept {
    ListConstIterator ret(*this);
    m_node = m_node->m_next;
    return ret;
  }

  ListConstIterator& operator--() noexcept {
    m_node = m_node->m_prev;
    return *this;
  }

  ListConstIterator operator--(int) noexcept {
    ListConstIterator ret(*this);
    m_node = m_node->m_prev;
    return ret;
  }

  friend bool operator==(const ListConstIterator& lhs, const ListConstIterator& rhs) {
    return lhs.m_node == rhs.m_node;
  }

  friend bool operator!=(const ListConstIterator& lhs, const ListConstIterator& rhs) {
    return lhs.m_node != rhs.m_node;
  }

  const ListNodeBase* m_node;
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

  list() : m_base(new ListNodeBase) {}

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

  list(list&& other) noexcept : m_size(other.m_size), m_base(other.m_base) {
    other.m_base->m_next = other.m_base->m_prev = other.m_base;
    other.m_size = 0;
  }

  ~list() {
    clear();
    delete m_base;
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
      ListNodeBase *next = m_base->m_next->m_next;
      delete m_base->m_next;
      m_base->m_next = next;
    }
    m_size = 0;
  }

  bool empty() const noexcept { return m_base == m_base->m_next; }

  void erase(iterator pos) {
    delete pos.m_node;
    --m_size;
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
    ListNodeBase* node = pos.const_cast_().m_node;

    node->m_prev->m_next = other.m_base->m_next;
    other.m_base->m_next->m_prev = node->m_prev;

    node->m_prev = other.m_base->m_prev;
    other.m_base->m_prev->m_next = node;

    other.m_base->m_next = other.m_base;
    other.m_base->m_prev = other.m_base;

    m_size += other.m_size;
    other.m_size = 0;
  }

  void reverse() noexcept {
    std::swap(m_base->m_next, m_base->m_prev);
    for(auto it = begin(); it != end(); ++it) {
      std::swap(it.m_node->m_next, it.m_node->m_prev);
    }
  }

  size_type size() const noexcept {
    return m_size;
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
    ++m_size;
    return iterator(new Node(pos.m_node->m_prev, pos.m_node, value));
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

  template <typename... Args>
  void insert_many_back(Args&&... args) {
    ([&]
    {
      push_back(args);
    } (), ...);
  }

  template <typename... Args>
  void insert_many_front(Args&&... args) {
    ([&]
    {
      push_front(args);
    } (), ...);
  }

  void push_front(const_reference value) {
    ++m_size;
    new Node(m_base, m_base->m_next, value);
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
    new (node_ptr) ListNodeBase(ListNodeBase(place.m_node->m_prev, place.m_node));
    new (&node_ptr->m_data) value_type(std::forward<Args>(args)...);
    ++m_size;
    return iterator(node_ptr);
  }

  void swap(list& other) noexcept {
    std::swap(m_base, other.m_base);
  }

  void push_back(const_reference value) {
    ++m_size;
    new Node(m_base->m_prev, m_base, value);
  }

  iterator begin() noexcept {
    return iterator(m_base->m_next);
  }

  iterator end() noexcept {
    return iterator(m_base);
  }

  const_iterator begin() const noexcept {
    return const_iterator(m_base->m_next);
  }

  const_iterator end() const noexcept {
    return const_iterator(m_base);
  }

  const_iterator cbegin() const noexcept {
    return const_iterator(m_base->m_next);
  }

  const_iterator cend() const noexcept {
    return const_iterator(m_base);
  }

 private:
  size_type m_size{};
  ListNodeBase* m_base;

  iterator tail() noexcept {
    return iterator(m_base->m_prev);
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

} // namespace s21

#endif // S21_CONTAINER_SRC_S21_LIST_H_