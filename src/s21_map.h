#ifndef S21_CONTAINER_SRC_S21_MAP_H_
#define S21_CONTAINER_SRC_S21_MAP_H_

#include "s21_tree.h"
#include "s21_vector.h"

namespace s21 {

template<typename Key, typename Tp, typename Compare = std::less<Key>>
class map {
 public:
  using key_type = Key;
  using mapped_type = Tp;
  using value_type = std::pair<key_type, mapped_type>;
  using reference = value_type&;
  using const_reference = const value_type&;
  using tree = RBTree<key_type, value_type, KeyGetters::select_first<value_type>, Compare>;
  using iterator = typename tree::iterator;
  using const_iterator = typename tree::const_iterator;
  using size_type = typename tree::size_type;

  map() = default;

  map(std::initializer_list<value_type> const& items) {
    for(auto& item : items) {
      m_tree.insert_unique(item);
    }
  }

  map(const map& other) noexcept : m_tree(other.m_tree) {}

  map(map&& other) noexcept : m_tree(std::move(other.m_tree)) {}

  ~map() = default;

  map& operator=(map&& other)  noexcept {
    m_tree = std::move(other.m_tree);
    return *this;
  }

  map& operator=(const map& other) {
    m_tree = other.m_tree;
    return *this;
  }

  mapped_type& at(const Key& key) {
    iterator it = m_tree.lower_bound(key);
    if (it == end() || Compare()(key, (*it).first)) {
      throw std::out_of_range("Missing key in map");
    }
    return (*it).second;
  }

  const mapped_type& at(const Key& key) const {
    const_iterator it = m_tree.lower_bound(key);
    if (it == end() || Compare()(key, (*it).first)) {
      throw std::out_of_range("Missing key in map");
    }
    return (*it).second;
  }

  mapped_type& operator[](const Key& key) {
    iterator it = m_tree.lower_bound(key);
    if (it == end() || Compare()(key, (*it).first)) {
      auto ins_it = insert(value_type(key, mapped_type())).first;
      return (*ins_it).second;
    }
    return (*it).second;
  }

  const mapped_type& operator[](const Key& key) const {
    const_iterator it = m_tree.lower_bound(key);
    if (it == end() || Compare()(key, (*it).first)) {
      auto ins_it = insert(value_type(key, mapped_type())).first;
      return (*ins_it).second;
    }
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
    m_tree.erase((*pos).first);
  }

  void swap(map& other) noexcept {
    m_tree.swap(other.m_tree);
  }

  void merge(map& other) {
    m_tree.merge_unique(other.m_tree);
  }

  std::pair<iterator, bool> insert(const Key& key, const Tp& obj) {
    return m_tree.insert_unique(value_type(key, obj));
  }

  std::pair<iterator, bool> insert_or_assign(const Key& key, const Tp& obj) {
    iterator it = m_tree.lower_bound(key);
    if (it == end() || Compare()(key, (*it).first)) {
      m_tree.insert_unique(value_type(key, obj));
      return std::make_pair(it, true);
    }
    (*it).second = obj;
    return std::pair<iterator, bool>(it, false);
  }

  bool contains(const Key& key) const noexcept {
    return m_tree.count(key) != 0;
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

} //namespace s21

#endif // S21_CONTAINER_SRC_S21_MAP_H_