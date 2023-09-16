#pragma once

#include <iostream>
#include <list>

// A vector class that relies on memory pooling instead of left shifting
// which could bring huge performance boosts for insertions and deletions.
template<class T>
class MemPoolVector {
private:
	void allocate_capacity(uint32_t old_capacity)
	{
		T* _newArray = new T[_capacity];
		if (_array)
		{
			memmove(_newArray, _array, old_capacity * sizeof(T));
			delete[] _array;
		}
		_array = _newArray;
	}

	void increase_capacity()
	{
		uint32_t old = _capacity;
		_capacity *= 1.5;
		allocate_capacity(old);
	}

public:
	MemPoolVector() { allocate_capacity(_capacity); }
	~MemPoolVector() { delete[] _array; }

	uint32_t shove(const T& e) {
		if (_free_indices.size() > 0)
		{
			uint32_t i = _free_indices.front();
			_free_indices.pop_front();
			_array[i] = e;
			return i;
		}

		if (_size + 1 > _capacity)
			increase_capacity();

		_array[_size] = e;
		return _size++;
	}

	void erase_index(uint32_t i) {
		if (i >= _size) return;
		_size--;
		_array[i] = 0;
		_free_indices.push_back(i);
	}

	void erase_match(const T& e) {
		for (uint32_t i = 0; i < _size; i++)
		{
			if (_array[i] == e)
			{
				erase_index(i);
				return;
			}
		}
	}

	T& get(uint32_t i)
	{
		return _array[i];
	}

	size_t size() const {
		return _size;
	}

	void clear() {
		_size = 0;
	}

	std::list<uint32_t> _free_indices;
	T* _array = nullptr;
	uint32_t _size = 0;
	uint32_t _capacity = 2;
};
