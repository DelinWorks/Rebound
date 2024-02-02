#pragma once

#include "Helper/Math.h"
#include <vector>

template <typename T>
class DLLNode {
public:
	T data;
	DLLNode* prev, next;

	void removeSelf()
	{
		if (prev && next)
		{
			prev->next = next;
			next->prev = prev;
		}
		else if (next) next.prev = nullptr;
		else if (prev) prev.next = nullptr;

		delete this;
	}
};

template <typename T>
class DoublyLinkedList
{
public:
	DLLNode<T>* head, tail;


};





































