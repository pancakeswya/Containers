#ifndef S21_CONTAINER_SRC_S21_MAP_H_
#define S21_CONTAINER_SRC_S21_MAP_H_

#include "s21_tree.h"

namespace s21 {

template<typename Key, typename Tp, typename Compare = std::less<Key>>
class map {
 public:
  using key_type = Key;
  using mapped_type = Tp;
  using value_type = std::pair<const key_type, mapped_type>;
  using reference = value_type&;
  using const_reference = const value_type&;

  struct key_compare {
    bool operator()(const_reference lhs, const_reference rhs) const noexcept {
      return cmp(lhs.first, rhs.first);
    }
    Compare cmp;
  };

  typedef RBTree<value_type, key_compare>      tree;
  typedef typename tree::iterator                iterator;
  typedef typename tree::const_iterator          const_iterator;
  typedef typename tree::size_type               size_type;

  map() = default;

  map(std::initializer_list<value_type> const &items) {
    for(auto &item : items) {
      m_tree.insert(item);
    }
  }

  map(const map &other) noexcept : m_tree(other.m_tree) {}

  map(map &&other) noexcept : m_tree(std::move(other)) {}

  ~map() = default;

  map& operator=(map &&other)  noexcept {
    *this = std::move(other);
    return *this;
  }

  map& operator=(const map &other) {
    *this = other;
    return *this;
  }

  Tp& at(const Key& key) {
    (*this)[key];
  }

  Tp& operator[](const Key& key) {
    for(auto it = begin(); it != end();++it) {
      if ((*it).first == key) {
        return (*it).second;
      }
    }
    auto it = insert(value_type(key, Tp())).first;
    return (*it).second;
  }

  void clear() {
    m_tree.clear();
  }

   bool empty() const noexcept {
    return m_tree.empty();
  }

  size_type size() const noexcept {
    return m_tree.size();
  }

  size_type max_size() const noexcept {
    return m_tree.max_size();
  }

  std::pair<iterator, bool> insert(const value_type& value) {
    return m_tree.insert(value);
  }

  void erase(iterator pos) {
    m_tree.erase(*pos);
  }

  void swap(map& other) noexcept {
    m_tree.swap(other.m_tree);
  }

  void merge(map& other) {
    for(auto &i : other.m_tree) {
      m_tree.insert(i);
    }
    other.clear();
  }

  std::pair<iterator, bool> insert(const Key& key, const Tp& obj) {
    return m_tree.insert(value_type(key, obj));
  }

  std::pair<iterator, bool> insert_or_assign(const Key& key, const Tp& obj) {
    auto it = m_tree.insert(value_type(key, obj));
    if (!it.second) {
      (*it.first).second = obj;
    }
    return std::pair<iterator, bool>(it.first, true);
  }

  bool contains(const Key& key) const noexcept {
    for(auto it = begin(); it != end(); ++it) {
      if ((*it).first == key) {
        return true;
      }
    }
    return false;
  }

  iterator begin() noexcept {
    return m_tree.begin();
  }

  const_iterator begin() const noexcept {
    return m_tree.cbegin();
  }

  iterator end() noexcept {
    return m_tree.end();
  }

  const_iterator end() const noexcept {
    return m_tree.cend();
  }

 private:
  tree m_tree{};

};

} //namespace s21

#endif // S21_CONTAINER_SRC_S21_MAP_H_