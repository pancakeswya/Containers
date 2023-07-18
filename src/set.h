#include <initializer_list>
#include <limits>

#include "rb_tree.h"

namespace s21 {

template<typename Key, typename Compare = std::less<Key>>
class set {
 public:
  typedef rb_tree<Key, Compare>         tree;
  typedef typename tree::iterator       iterator;
  typedef typename tree::const_iterator const_iterator;
  typedef typename tree::size_type      size_type;
  typedef Key                           key_type;
  typedef Key                           value_type;
  typedef Key&                          reference;
  typedef const Key&                    const_reference;

  set() noexcept : m_tree() {}

  set(std::initializer_list<value_type> const &items) : m_tree() {
    for(auto &item : items) {
      m_tree.insert(item);
    }
  }

  set(const set &other) noexcept : m_tree(other.m_tree) {}

  set(set &&other) noexcept : m_tree(std::move(other)) {}

  ~set() = default;

  set& operator=(set &&other)  noexcept {
    *this = std::move(other);
    return *this;
  }

  set& operator=(const set &other) {
    *this = other;
    return *this;
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

  void swap(set& other) noexcept {
    m_tree.swap(other.m_tree);
  }

  void merge(set& other) {
    for(auto &i : other.m_tree) {
      m_tree.insert(i);
    }
    other.clear();
  }

  iterator find(const Key& key) noexcept {
    return m_tree.find(key);
  }

  bool contains(const Key& key) const noexcept {
    return m_tree.find(key) != end();
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