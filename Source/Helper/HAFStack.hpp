#pragma once

#include <iostream>
#include <list>

template<class T>
class HeapAllocatedFixedStack {
public:
	HeapAllocatedFixedStack() {}
	~HeapAllocatedFixedStack() {}

	void push(const T& e) {
		pool.push_front(e);

		if (pool.size() > capacity && capacity != 0)
			pool.pop_back();
	}

	T& top() {
		return pool.front();
	}

	void pop() {
		pool.pop_front();
	}

	bool empty() const {
		return pool.empty();
	}

	size_t size() const {
		return pool.size();
	}

	void reset() {
		pool.clear();
	}

	void set_capacity(int c) { capacity = c; }

	int capacity = 0;
	std::list<T> pool;
};
