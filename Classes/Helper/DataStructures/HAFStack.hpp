#pragma once

#include <iostream>
#include <list>

template<class T>
class HeapAllocatedFixedStack {
public:

	FixedStack() {
		pool = new std::list<T>();
	}

	~FixedStack() {
		delete pool;
	}

	void push(const T& e) {
		pool->push_front(e);

		if (pool->size() > capacity && capacity != 0)
			pool->pop_back();
	}

	T& top() {
		return pool->front();
	}

	void pop() {
		pool->pop_front();
	}

	bool empty() const {
		return pool->empty();
	}

	size_t size() const {
		return pool->size();
	}

	void reset() {
		delete pool;
		pool = new std::list<T>();
	}

	void set_capacity(int c) { capacity = c; }

private:
	int capacity = 0;
	std::list<T>* pool;
};
