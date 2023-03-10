
#ifndef __CHANGE_VALUE_H__
#define __CHANGE_VALUE_H__

#include "CCRef.h"

// This class is designed to stop calling functions over and over
// and detect if a value is changed and if so only call the function when
// the change happened to avoid memory leaks and function multiple calls
class ChangeValueBool : public axmol::Ref {
private:
	bool oldValue;
	bool value;

public:
	void setValue(bool value) {
		oldValue = this->value;
		this->value = value;
	}

	bool getOldValue() {
		return this->oldValue;
	}

	bool getValue() {
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

// This class is designed to stop calling functions over and over
// and detect if a value is changed and if so only call the function when
// the change happened to avoid memory leaks and function multiple calls
class ChangeValueFloat : public axmol::Ref {
private:
	float oldValue;
	float value;

public:
	void setValue(float value) {
		oldValue = this->value;
		this->value = value;
	}

	float getOldValue() {
		return this->oldValue;
	}

	float getValue() {
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

// This class is designed to stop calling functions over and over
// and detect if a value is changed and if so only call the function when
// the change happened to avoid memory leaks and function multiple calls
class ChangeValueInt {
private:
	int oldValue = 0;
	int value = 0;

public:
	void setValue(int value) {
		oldValue = this->value;
		this->value = value;
	}

	int getOldValue() {
		return this->oldValue;
	}

	int getValue() {
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