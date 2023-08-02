#ifndef S21_CONTAINER_SRC_S21_VECTOR_H_
#define S21_CONTAINER_SRC_S21_VECTOR_H_

#include <memory>
#include <initializer_list>
#include <stdexcept>

namespace s21 {

template<typename Tp, typename Alloc = std::allocator<Tp>>
class vector {
 public:
  using value_type = Tp;
  using iterator = Tp*;
  using const_iterator = const Tp*;
  using reference = Tp&;
  using const_reference = const Tp&;
  using size_type = size_t;
  using allocator_type = Alloc;

  vector() = default;

  explicit vector(size_type n) {
    create_storage(n);
    for(;n != 0; --n) {
      m_allocator.construct(m_finish++);
    }
  }

  vector(std::initializer_list<value_type> const &items) {
    create_storage(items.size());
    for (auto &item : items) {
      m_allocator.construct(m_finish++, item);
    }
  }

  vector(const vector& other) {
    create_storage(other.capacity());
    for (auto &val : other) {
      m_allocator.construct(m_finish++, val);
    }
  }

  vector(vector&& other) noexcept
    : m_start(other.m_start),m_finish(other.m_finish),
      m_capacity(other.m_capacity), m_allocator(std::move(other.m_allocator)) {
    other.m_start = other.m_finish = other.m_capacity = nullptr;
  }

  ~vector() {
    destroy_storage();
  }

  vector& operator=(const vector& other) {
    if (this != &other) {
      vector tmp(other);
      swap(tmp);
    }
    return *this;
  }

  vector& operator=(vector&& other)  noexcept {
    vector tmp(std::move(other));
    if (this != &other) {
      swap(tmp);
    }
    return *this;
  }

  const Tp* data() const noexcept {
    return m_start;
  }

  iterator begin() noexcept { return m_start; }

  const_iterator begin() const noexcept { return m_start; }

  const_iterator cbegin() const noexcept {
    return m_start;
  }

  iterator end() noexcept { return m_finish; }

  const_iterator end() const noexcept { return m_finish; }

  const_iterator cend() const noexcept {
    return m_finish;
  }

  size_type size() const noexcept {
    return m_finish - m_start;
  }

  size_type max_size() const noexcept {
    return m_allocator.max_size();
  }

  bool empty() const noexcept {
    return begin() == end();
  }

  size_type capacity() const noexcept {
    return m_capacity - m_start;
  }

  void clear() noexcept {
    for(iterator p = begin(); p != end(); ++p) {
      m_allocator.destroy(p);
    }
    m_finish = m_start;
  }

  void reserve(size_type n) {
    if (capacity() > max_size()) {
      throw std::out_of_range("Invalid reserve size");
    }
    if (capacity() < n) {
      reallocate_storage(end(), n);
    }
  }

  reference at(size_type pos) {
    if (pos >= size()) {
      throw std::out_of_range("Invalid position number");
    }
    return (*this)[pos];
  }

  reference operator[](size_type pos) { return *(m_start + pos); }

  reference front() { return *begin(); }

  const_reference front() const { return *begin(); }

  reference back() { return *(end() - 1); }

  const_reference back() const { return *(end() - 1); }

  template <typename... Args>
  iterator emplace(const_iterator pos, Args&&... args) {
    auto offset = pos - cbegin();
    iterator new_pos = make_gap(offset);
    m_allocator.construct(new_pos, std::forward<Args>(args)...);
    ++m_finish;
    return new_pos;
  }

  template <typename... Args>
  void emplace_back(Args&&... args) {
    emplace(cend(), std::forward<Args>(args)...);
  }

  iterator insert(iterator pos, const_reference value) {
    auto offset = pos - begin();
    iterator new_pos = make_gap(offset);
    m_allocator.construct(new_pos, value);
    ++m_finish;
    return new_pos;
  }

  template <typename... Args>
  iterator insert_many(iterator pos, Args&&... args) {
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

  void push_back(const_reference value) {
    insert(end(), value);
  }

  void pop_back() {
    --m_finish;
    m_allocator.destroy(end());
  }

  void shrink_to_fit() {
    if (!storage_is_full() && !empty()) {
      shrink_storage(size());
    }
  }

  void erase(iterator pos) {
    if (pos + 1 != end()) {
      std::move(pos + 1, end(), pos);
    }
    pop_back();
  }

  void swap(vector& other) noexcept {
    std::swap(m_start, other.m_start);
    std::swap(m_finish, other.m_finish);
    std::swap(m_capacity, other.m_capacity);
    std::swap(m_allocator, other.m_allocator);
  }

 private:
  iterator make_gap(size_type offset) {
    if (storage_is_full()) {
      grow_storage();
    }
    iterator new_pos = begin() + offset;
    std::move(new_pos, end(), new_pos + 1);
    return new_pos;
  }

  bool storage_is_full() noexcept {
    return m_finish == m_capacity;
  }

  void reallocate_storage(iterator end_of_storage, size_type new_cap) {
    size_type old_size = size();
    size_type new_size = (new_cap < old_size) ? new_cap : old_size;
    iterator new_start = m_allocator.allocate(new_cap);
    std::move(begin(), end_of_storage, new_start);
    destroy_storage();
    m_start = new_start;
    m_finish = m_start + new_size;
    m_capacity = m_start + new_cap;
  }

  void grow_storage() {
    size_type old_cap = capacity();
    size_type new_cap = (old_cap == 0) ? 1 : old_cap * 2;
    reallocate_storage(end(), new_cap);
  }

  void shrink_storage(size_type new_cap) {
    reallocate_storage(begin() + new_cap, new_cap);
  }

  void create_storage(size_type n) {
    m_start = (n != 0) ? m_allocator.allocate(n) : nullptr;
    m_finish = m_start;
    m_capacity = m_start + n;
  }

  void destroy_storage() {
    m_allocator.deallocate(m_start, m_capacity - m_start);
    m_start = m_finish = m_capacity = nullptr;
  }

  iterator m_start{},
           m_finish{},
           m_capacity{};
  allocator_type m_allocator;

};

} // namespace s21

#endif // S21_CONTAINER_SRC_S21_VECTOR_H_