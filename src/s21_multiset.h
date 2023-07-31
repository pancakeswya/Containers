#ifndef S21_CONTAINER_SRC_S21_MULTISET_H_
#define S21_CONTAINER_SRC_S21_MULTISET_H_

#include "s21_tree.h"
#include "s21_vector.h"

namespace s21 {

template<typename Key, typename Compare = std::less<Key>>
class multiset {
 public:
  using tree = RBTree<Key>;
  using iterator = typename tree::iterator;
  using const_iterator = typename tree::const_iterator;
  using size_type = typename tree::size_type;
  using key_type = Key;
  using value_type = Key;
  using reference = Key&;
  using const_reference = const Key&;

  multiset() = default;

  multiset(std::initializer_list<value_type> const &items) {
    for(auto &item : items) {
      insert(item);
    }
  }

  multiset(const multiset &other) noexcept : m_tree(other.m_tree) {}

  multiset(multiset &&other) noexcept : m_tree(std::move(other)) {}

  ~multiset() = default;

  multiset& operator=(multiset &&other)  noexcept {
    *this = std::move(other);
    return *this;
  }

  multiset& operator=(const multiset &other) {
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
    return m_tree.insert_equal(value);
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

  void swap(multiset& other) noexcept {
    m_tree.swap(other.m_tree);
  }

  void merge(multiset& other) {
    for(auto &i : other.m_tree) {
      m_tree.insert_equal(i);
    }
    other.clear();
  }

  iterator find(const Key& key) noexcept {
    return m_tree.find(key);
  }

  size_type count(const Key& key) const noexcept {
    return m_tree.count(key);
  }

  std::pair<iterator,iterator> equal_range(const Key& key) {
    return m_tree.equal_range(key);
  }

  iterator lower_bound(const Key& key) {
    return m_tree.lower_bound(key);
  }

  iterator upper_bound(const Key& key) {
    return m_tree.upper_bound(key);
  }

  bool contains(const Key& key) const noexcept {
    return find(key) != end();
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

} // namespace s21

#endif // S21_CONTAINER_SRC_S21_MULTISET_H_