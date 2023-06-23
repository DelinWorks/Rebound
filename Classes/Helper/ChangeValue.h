
#ifndef __CHANGE_VALUE_H__
#define __CHANGE_VALUE_H__

#include "CCRef.h"

// This class is designed to stop calling functions over and over
// and detect if a value is changed and if so only call the function when
// the change happened to avoid possible memory leaks or performance issues
template <typename T>
class ChangeValue {
private:
	T oldValue;
	T value;

public:
	void setValue(T value) {
		oldValue = this->value;
		this->value = value;
	}

	T getOldValue() {
		return this->oldValue;
	}

	T getValue() {
		return this->value;
	}

	bool isChanged() {
		if (this->oldValue != this->value)
		{
			this->oldValue = this->value;
			return true;
		}
		return false;
	}
};

#endif