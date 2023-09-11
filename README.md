# Containers
STL containers implementation.

## Brief
Use it just like standard one, but instead of `std` namespace use `s21`.
Also dont forget that it is an implementation which means it does not have all the pool
of methods that are used in `std` one.

## Usage example 
```
    s21::vector<int> vec(24);
    vec.push_back(5);
    s21::map<int, bool> mp;
    mp[4] = true;
```
