#ifndef S21_CONTAINER_SRC_S21_SET_H_
#define S21_CONTAINER_SRC_S21_SET_H_

#include <initializer_list>
#include <limits>

#include "s21_tree.h"
#include "s21_vector.h"

namespace s21 {

template<typename Key, typename Compare = std::less<Key>>
class set {
 public:
  using key_type = Key;
  using value_type = Key;
  using reference = Key&;
  using const_reference = const Key&;
  using tree = RBTree<key_type, value_type, KeyGetters::identity<value_type>, Compare>;
  using iterator = typename tree::iterator;
  using const_iterator = typename tree::const_iterator;
  using size_type = typename tree::size_type;

  set() = default;

  set(std::initializer_list<value_type> const& items) {
    for(auto& item : items) {
      m_tree.insert_unique(item);
    }
  }

  set(const set& other) noexcept : m_tree(other.m_tree) {}

  set(set&& other) noexcept : m_tree(std::move(other.m_tree)) {}

  ~set() = default;

  set& operator=(set&& other) noexcept {
    m_tree = std::move(other.m_tree);
    return *this;
  }

  set& operator=(const set& other) {
    m_tree = other.m_tree;
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
    return m_tree.insert_unique(value);
  }

  template <typename... Args>
  vector<std::pair<iterator,bool>> insert_many(Args&&... args) {
    vector<std::pair<iterator, bool>> inserted;
    ([&]
    {
      auto ins_pair = insert(args);
      inserted.push_back(ins_pair);
    } (), ...);
    return inserted;
  }

  void erase(iterator pos) {
    m_tree.erase(*pos);
  }

  void swap(set& other) noexcept {
    m_tree.swap(other.m_tree);
  }

  void merge(set& other) {
    m_tree.merge_unique(other.m_tree);
  }

  iterator find(const Key& key) noexcept {
    return m_tree.find(key);
  }

  const_iterator find(const Key& key) const noexcept {
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

  const_iterator cbegin() const noexcept {
    return m_tree.cbegin();
  }

  iterator end() noexcept {
    return m_tree.end();
  }

  const_iterator end() const noexcept {
    return m_tree.cend();
  }

  const_iterator cend() const noexcept {
    return m_tree.cend();
  }

 private:
  tree m_tree{};
};

} // namespace s21

#endif // S21_CONTAINER_SRC_S21_SET_H_