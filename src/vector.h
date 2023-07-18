#ifndef S21_CONTAINER_SRC_VECTOR_H_
#define S21_CONTAINER_SRC_VECTOR_H_

#include <initializer_list>
#include <iterator>
#include <cstring>
#include <limits>
#include <memory>

namespace s21 {

template <typename Tp>
class VectorBase {
 public:
  typedef Tp*                pointer;
  typedef Tp                 value_type;
  typedef size_t             size_type;
  typedef std::allocator<Tp> allocator_type;

  struct VectorImpl {
    pointer m_start_;
    pointer m_finish_;
    pointer m_capacity_;

    VectorImpl() noexcept : m_start_(), m_finish_(), m_capacity_() {}

    VectorImpl(VectorImpl&& other) noexcept
        : m_start_(other.m_start_), m_finish_(other.m_finish_), m_capacity_(other.m_capacity_) {
      other.m_start_ = other.m_finish_ = other.m_capacity_ = nullptr;
    }

    void swap(VectorImpl& other) noexcept {
      VectorImpl tmp(std::move(*this));
      move(other);
      other.move(tmp);
    }

    void copy(VectorImpl& other, size_type size) noexcept {
      memcpy(m_start_, other.m_start_, size * sizeof(value_type));
      m_finish_ += size;
    }

    void move(VectorImpl &other) noexcept {
      m_start_ = other.m_start_;
      m_finish_ = other.m_finish_;
      m_capacity_ = other.m_capacity_;
    }
  };

  VectorBase() = default;

  explicit VectorBase(size_type n) : m_impl_() { create_storage(n); }

  VectorBase(const VectorBase& other)  {
    auto n = other.m_size();
    create_storage(n);
    m_impl_.copy(other, n);
  }

  VectorBase(VectorBase&& other) noexcept : m_impl_(std::move(other.m_impl_)) {}

  ~VectorBase() { destroy_storage(); }

 protected:
  allocator_type m_allocator_;
  VectorImpl m_impl_;

   size_type m_size() const noexcept {
    return size_type(m_impl_.m_finish_ - m_impl_.m_start_);
  }

   size_type m_capacity() const noexcept {
    return size_type(m_impl_.m_capacity_ - m_impl_.m_start_);
  }

  void create_storage(size_type size) {
    m_impl_.m_start_ = (size != 0) ? m_allocator_.allocate(size) : nullptr;
    m_impl_.m_finish_ = m_impl_.m_start_;
    m_impl_.m_capacity_ = m_impl_.m_start_ + size;
  }

  void grow(size_type new_size) {
    VectorBase tmp(new_size);
    tmp.m_impl_.copy(m_impl_, m_size());
    m_impl_.swap(tmp.m_impl_);
  }

  void shrink(size_type new_size) {
    VectorBase tmp(new_size);
    tmp.m_impl_.copy(m_impl_, new_size);
    m_impl_.swap(tmp.m_impl_);
  }

  void destroy_storage() {
    m_allocator_.deallocate(m_impl_.m_start_, m_impl_.m_capacity_ - m_impl_.m_start_);
  }

};

template<typename Tp>
class vector : protected VectorBase<Tp> {
 public:
  typedef VectorBase<Tp>                Base;
  typedef typename Base::value_type     value_type;
  typedef Tp&                           reference;
  typedef const Tp&                     const_reference;
  typedef typename Base::size_type      size_type;
  typedef typename Base::pointer        iterator;
  typedef const Tp*                     const_iterator;

  vector() = default;

  explicit vector(size_type n) : Base(n) {}

  vector(std::initializer_list<value_type> const &items) : Base(items.size()) {
    for (auto &item : items) {
      push_back(item);
    }
  }

  vector(const vector& other) noexcept : Base(other) {}

  vector(vector&& other) noexcept : Base(std::move(other)) {}

  ~vector() = default;

  iterator begin() noexcept { return m_impl_.m_start_; }

  const_iterator begin() const noexcept { return m_impl_.m_start_; }

  const_iterator cbegin() const noexcept {
    return m_impl_.m_start_;
  }

  iterator end() noexcept { return m_impl_.m_finish_; }

  const_iterator end() const noexcept { return m_impl_.m_finish_; }

  const_iterator cend() const noexcept {
    return m_impl_.m_finish_;
  }

  size_type size() const noexcept {
    return this->m_size();
  }

  size_type max_size() const noexcept {
    return std::numeric_limits<size_type>::max()/sizeof(value_type);
  }

  bool empty() const noexcept {
    return m_impl_.m_start_ == m_impl_.m_finish_;
  }

  size_type capacity() const noexcept {
    return this->m_capacity();
  }

  void reserve(size_type new_cap) {
    new_cap < size() ? this->shrink(new_cap) : this->grow(new_cap);
  }

  void shrink_to_fit() {
    if (!full() && !empty()) {
      this->shrink(size());
    }
  }

  void clear() noexcept {
    size_type cap = capacity();
    this->destroy_storage();
    this->create_storage(cap);
  }

  reference at(size_type pos) noexcept {
    return *(m_impl_.m_start_ + pos);
  }

  reference front() { return *m_impl_.m_start_; }

  const_reference front() const { return *m_impl_.m_start_; }

  reference back() { return *m_impl_.m_finish_; }

  const_reference back() const { return *m_impl_.m_finish_; }

  vector& operator=(const vector& other) noexcept {
    vector tmp(other);
    swap(tmp);
    return *this;
  }

  vector& operator=(vector&& other)  noexcept {
    vector tmp(std::move(other));
    swap(tmp);
    return *this;
  }

  reference operator[](size_type pos) { return at(pos); }

  iterator insert(iterator pos, const_reference value) {
    const auto offset = pos - begin();
    if (full()) {
      grow_cap();
    }
    m_impl_.m_finish_++;
    iterator it = begin() + offset;
    value_type prev_val = value, cur_val;
    for (; it != end(); ++it) {
      cur_val = *it;
      *it = prev_val;
      prev_val = cur_val;
    }
    return begin() + offset;
  }

  void push_back(const value_type& value) {
    insert(end(), value);
  }

  template <typename... Args>
  iterator emplace(const_iterator pos, Args&&... args) {
    auto n = pos - cbegin();
    if (full()) {
      grow_cap();
    }
    Base::m_allocator_.construct(m_impl_.m_start_ + n, std::forward<Args>(args)...);
    m_impl_.m_finish_++;
    return m_impl_.m_start_ + n;
  }

  template <typename... Args>
  void emplace_back(Args&&... args) {
    emplace(cend(), std::forward<Args>(args)...);
  }

  void erase(iterator pos) {
    const auto n = pos - begin();
    if (pos != end()) {
      iterator cur = begin() + n;
      iterator next = cur + 1;
      for (; next != end(); next++, cur++) {
        *cur = *next;
      }
    }
    pop_back();
  }

  void pop_back() {
    m_impl_.m_finish_--;
    this->shrink(capacity());
  }

  void swap(vector& other) noexcept {
    m_impl_.swap(other.m_impl_);
  }

 private:
  using Base::m_impl_;

  bool full() noexcept { return m_impl_.m_finish_ == m_impl_.m_capacity_; }

  void grow_cap() {
    auto new_cap = capacity() * 2;
    if (new_cap == 0) {
      new_cap++;
    }
    this->grow(new_cap);
  }
};

}


#endif // S21_CONTAINER_SRC_VECTOR_H_