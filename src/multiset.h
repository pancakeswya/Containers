#include "rb_tree.h"

namespace s21 {

template<typename Key, typename Compare = std::less<Key>>
class multiset {
 public:
  struct compare_keys {
    bool equal(const Key& lhs, const Key& rhs) const noexcept {
      return !cmp(lhs, rhs) && !cmp(rhs, lhs);
    }

    bool operator()(const Key& lhs, const Key& rhs) const noexcept {
      return cmp(lhs, rhs) || equal(lhs,rhs);
    }
    Compare cmp;
  };

  typedef rb_tree<Key, compare_keys>   tree;
  typedef typename tree::iterator       iterator;
  typedef typename tree::const_iterator const_iterator;
  typedef typename tree::size_type      size_type;
  typedef Key                           key_type;
  typedef Key                           value_type;
  typedef Key&                          reference;
  typedef const Key&                    const_reference;

  multiset() noexcept : m_tree() {}

  multiset(std::initializer_list<value_type> const &items) : m_tree() {
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
    return m_tree.insert(value);
  }

  template <typename... Args>
  std::pair<iterator,bool> emplace(Args&&... args) {
    return m_tree.emplace(std::forward<Args>(args)...);
  }

  void erase(iterator pos) {
    m_tree.erase(*pos);
  }

  void swap(multiset& other) noexcept {
    m_tree.swap(other.m_tree);
  }

  void merge(multiset& other) {
    for(auto &i : other.m_tree) {
      m_tree.insert(i);
    }
    other.clear();
  }

  iterator find(const Key& key) noexcept {
    compare_keys cmp;
    for (auto it = begin(); it != end();++it) {
      if (m_cmp.equal(*it,key)) {
        return it;
      }
    }
    return end();
  }

  size_type count(const Key& key) const noexcept {
    size_type count_keys;
    for(auto &i : m_tree) {
      if (m_cmp.equal(i,key)) {
        ++count_keys;
      }
    }
    return count_keys;
  }

  std::pair<iterator,iterator> equal_range(const Key& key) {
    for (auto first = begin(); first != end();++first) {
      if (m_cmp.equal(*first,key)) {
        auto second = first;
        for(;m_cmp.equal(*second,key); ++second)
          ;
        return std::pair<iterator,iterator>(first, second);
      }
    }
    return std::make_pair<iterator, iterator>(iterator(end()), iterator(end()));
  }

  iterator lower_bound(const Key& key) {
    for(auto it = begin(); it != end(); ++it) {
      if (!m_cmp(*it, key) || m_cmp.equal(*it,key)) {
        return it;
      }
    }
    return end();
  }

  iterator upper_bound(const Key& key) {
    for(auto it = begin(); it != end(); ++it) {
      if (!m_cmp(*it, key) && !m_cmp.equal(*it,key)) {
        return it;
      }
    }
    return end();
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
  tree m_tree;
  compare_keys m_cmp;
};

}