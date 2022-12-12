#pragma once
#include "Types.h"
#include "Allocator.h"

#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

using namespace std;

template<typename T>
using Vector = vector<T, StlAllocator<T>>; 

template<typename T>
using List = list<T, StlAllocator<T>>;

// 기본적인 Key에 대한 less를 적용시키겠다는 의미
template<typename K, typename T, typename Pred = less<K>>
using Map = map< K, T, Pred, StlAllocator<pair<const K, T>>>;

template<typename K, typename T, typename Pred = less<K>>
using Set = set<K, Pred, StlAllocator<K>>;

template<typename T>
using Deque = deque<T, StlAllocator<T>>;

template<typename T, typename Container = Deque<T>>
using Queue = queue<T, Container>;

template<typename T, typename Container = Deque<T>>
using Stack = stack<T, Container>;

template<typename T, typename Container = Vector<T>, typename Pred = less<typename Container::value_type>>
using PriorityQueue = priority_queue<T, Container, Pred>;

using String = basic_string<char, char_traits<char>, StlAllocator<char>>;

using WString = basic_string<wchar_t, char_traits<wchar_t>, StlAllocator<wchar_t>>;

template<typename K, typename T, typename Hasher = hash<K>, typename KeyEq = equal_to<K>>
using HashMap = unordered_map<K, T, Hasher, KeyEq, StlAllocator<pair<const K, T>>>;

template<typename K, typename Hasher = hash<K>, typename KeyEq = equal_to<K>>
using HashSet = unordered_set<K, Hasher, KeyEq, StlAllocator<K>>;