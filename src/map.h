#ifndef MAP_H_
#define MAP_H_

#include "rb_tree.h"

namespace s21 {

template<typename Key, typename Tp, typename Compare = std::less<Key>>
class map {
 public:
  typedef Key                                    key_type;
  typedef Tp                                     mapped_type;
  typedef std::pair<const key_type, mapped_type> value_type;
  typedef value_type&                            reference;
  typedef const value_type&                      const_reference;

  struct compare_keys {
    bool operator()(const_reference lhs, const_reference rhs) const noexcept {
      return cmp(lhs.first, rhs.first);
    }
    Compare cmp;
  };

  typedef rb_tree<value_type, compare_keys>      tree;
  typedef typename tree::iterator                iterator;
  typedef typename tree::const_iterator          const_iterator;
  typedef typename tree::size_type               size_type;

  map() noexcept : m_tree() {}

  map(std::initializer_list<value_type> const &items) : m_tree() {
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
    for(auto it = begin(); it != end();++it) {
      if ((*it).first == key) {
        return (*it).second;
      }
    }
    auto it = insert(value_type(key, Tp())).first;
    return (*it).second;
  }

  Tp& operator[](const Key& key) {
    return at(key);
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

  template <typename... Args>
  std::pair<iterator,bool> emplace(Args&&... args) {
    return m_tree.emplace(std::forward<Args>(args)...);
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
  tree m_tree;

};

}




#endif