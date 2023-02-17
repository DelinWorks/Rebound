#ifndef __PROTECTED_TYPES_H__
#define __PROTECTED_TYPES_H__

#include <memoryapi.h>
#include <algorithm>
#include <string>

#define PROTECTED(T) ProtectedType<T>

// Define this if you don't want to use VirtualAlloc()
// but want to use base64 strings instead on Win32 Platforms
#undef FORCE_BASE64

#if defined(WIN32) && !defined(FORCE_BASE64)

/*
* class used to protect POD-like data types
* It prevents cheat engine and other software
* that reads process memory from reading
* values stored by this class.
*
* Can be useful to protect your game or other software
*
* It uses WIN32 Native Virtual Allocations
*/
template<typename _Ty>
class ProtectedType
{
	void* ptr;
	_Ty* data;
	DWORD output;
	bool isAutoProtect;

public:
	ProtectedType() : ptr(nullptr), isAutoProtect(true)
	{
		ptr = VirtualAlloc(NULL, sizeof(_Ty), MEM_RESERVE, PAGE_READWRITE);
		ptr = VirtualAlloc(ptr, sizeof(_Ty), MEM_COMMIT, PAGE_READWRITE);
		data = static_cast<_Ty*>(ptr);
		protect();
	};

	~ProtectedType()
	{
		VirtualFree(ptr, 0, MEM_RELEASE);
	};

	/*
	* Sets the allocated memory page to read & write.
	* The address can be read from/written to by any process at this state.
	*/
	void expose()
	{
		VirtualProtect(ptr, sizeof(_Ty), PAGE_READWRITE, &output);
	}

	/*
	* Sets the allocated memory page to no access.
	* The address cannot be read from/written to by any process at this state.
	*/
	void protect()
	{
		VirtualProtect(ptr, sizeof(_Ty), PAGE_NOACCESS, &output);
	}

	/*
	* If true, it will automatically expose the value when you access it and protect it as soon as possible.
	* Otherwise, You will have to call expose() set/get protect()
	* WARNING: getting or setting while this is false without calling expose() will throw an error.
	*/
	void setAutoProtect(bool protect = true)
	{
		isAutoProtect = protect;
	}

	operator _Ty() {
		if (isAutoProtect) expose();
		_Ty temp = *data;
		if (isAutoProtect) protect();
		return temp;
	}

	void operator=(const _Ty& other) {
		if (isAutoProtect) expose();
		*data = other;
		if (isAutoProtect) protect();
	}

	void operator++() {
		if (isAutoProtect) expose();
		*data += (_Ty)1;
		if (isAutoProtect) protect();
	}

	void operator--() {
		if (isAutoProtect) expose();
		*data -= (_Ty)1;
		if (isAutoProtect) protect();
	}

	void operator+=(const _Ty& other) {
		if (isAutoProtect) expose();
		*data += other;
		if (isAutoProtect) protect();
	}

	void operator-=(const _Ty& other) {
		if (isAutoProtect) expose();
		*data -= other;
		if (isAutoProtect) protect();
	}

	void operator*=(const _Ty& other) {
		if (isAutoProtect) expose();
		*data *= other;
		if (isAutoProtect) protect();
	}

	void operator/=(const _Ty& other) {
		if (isAutoProtect) expose();
		*data /= other;
		if (isAutoProtect) protect();
	}
};

#else

namespace internal_base64 {

	inline std::string get_base64_chars() {
		static std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789+/";
		return base64_chars;
	}

	inline std::string to_base64(std::string const& data) {
		int counter = 0;
		uint32_t bit_stream = 0;
		const std::string base64_chars = get_base64_chars();
		std::string encoded;
		int offset = 0;
		for (unsigned char c : data) {
			auto num_val = static_cast<unsigned int>(c);
			offset = 16 - counter % 3 * 8;
			bit_stream += num_val << offset;
			if (offset == 16) {
				encoded += base64_chars.at(bit_stream >> 18 & 0x3f);
			}
			if (offset == 8) {
				encoded += base64_chars.at(bit_stream >> 12 & 0x3f);
			}
			if (offset == 0 && counter != 3) {
				encoded += base64_chars.at(bit_stream >> 6 & 0x3f);
				encoded += base64_chars.at(bit_stream & 0x3f);
				bit_stream = 0;
			}
			counter++;
		}
		if (offset == 16) {
			encoded += base64_chars.at(bit_stream >> 12 & 0x3f);
			encoded += "==";
		}
		if (offset == 8) {
			encoded += base64_chars.at(bit_stream >> 6 & 0x3f);
			encoded += '=';
		}
		return encoded;
	}

	inline std::string from_base64(std::string const& data) {
		int counter = 0;
		uint32_t bit_stream = 0;
		std::string decoded;
		int offset = 0;
		const std::string base64_chars = get_base64_chars();
		for (unsigned char c : data) {
			auto num_val = base64_chars.find(c);
			if (num_val != std::string::npos) {
				offset = 18 - counter % 4 * 6;
				bit_stream += num_val << offset;
				if (offset == 12) {
					decoded += static_cast<char>(bit_stream >> 16 & 0xff);
				}
				if (offset == 6) {
					decoded += static_cast<char>(bit_stream >> 8 & 0xff);
				}
				if (offset == 0 && counter != 4) {
					decoded += static_cast<char>(bit_stream & 0xff);
					bit_stream = 0;
				}
			}
			else if (c != '=') {
				return std::string();
			}
			counter++;
		}
		return decoded;
	}
}

/*
* class used to protect POD-like data types
* It prevents cheat engine and other software
* that reads process memory from reading
* values stored by this class.
*
* Can be useful to protect your game or other software
*/
template<typename _Ty>
class ProtectedType
{
	std::string data;
	bool isProtected;
	bool isAutoProtect;

public:
	ProtectedType() : isProtected(false), isAutoProtect(true)
	{
		data = "";
		protect();
	};

	~ProtectedType()
	{

	};

	/*
	* Sets the allocated memory string to read & write.
	* The value can be read from/written to by any process at this state.
	*/
	void expose()
	{
		if (!isProtected)
			return;
		isProtected = false;

		data = internal_base64::from_base64(data);
	}

	/*
	* Sets the allocated memory string to no access.
	* The value cannot be read from/written to by any process at this state.
	*/
	void protect()
	{
		if (isProtected)
			return;
		isProtected = true;

		data = internal_base64::to_base64(data);
	}

	/*
	* If true, it will automatically expose the value when you access it and protect it as soon as possible.
	* Otherwise, You will have to call expose() set/get protect()
	* WARNING: getting or setting while this is false without calling expose() will throw an error.
	*/
	void setAutoProtect(bool protect = true)
	{
		isAutoProtect = protect;
	}

	operator _Ty() {
		if (isAutoProtect) expose();
		std::stringstream convert(data);
		_Ty temp;
		convert >> temp;
		if (isAutoProtect) protect();
		return temp;
	}

	void operator++() {
		if (isAutoProtect) expose();
		std::stringstream convert(data);
		_Ty temp;
		convert >> temp;
		temp += (_Ty)1;
		data = std::to_string(temp);
		if (isAutoProtect) protect();
	}

	void operator--() {
		if (isAutoProtect) expose();
		std::stringstream convert(data);
		_Ty temp;
		convert >> temp;
		temp -= (_Ty)1;
		data = std::to_string(temp);
		if (isAutoProtect) protect();
	}

	void operator=(const _Ty& other) {
		if (isAutoProtect) expose();
		data = std::to_string(other);
		if (isAutoProtect) protect();
	}

	void operator+=(const _Ty& other) {
		if (isAutoProtect) expose();
		std::stringstream convert(data);
		_Ty temp;
		convert >> temp;
		temp += other;
		data = std::to_string(temp);
		if (isAutoProtect) protect();
	}

	void operator-=(const _Ty& other) {
		if (isAutoProtect) expose();
		std::stringstream convert(data);
		_Ty temp;
		convert >> temp;
		temp -= other;
		data = std::to_string(temp);
		if (isAutoProtect) protect();
	}

	void operator*=(const _Ty& other) {
		if (isAutoProtect) expose();
		std::stringstream convert(data);
		_Ty temp;
		convert >> temp;
		temp *= other;
		data = std::to_string(temp);
		if (isAutoProtect) protect();
	}

	void operator/=(const _Ty& other) {
		if (isAutoProtect) expose();
		std::stringstream convert(data);
		_Ty temp;
		convert >> temp;
		temp /= other;
		data = std::to_string(temp);
		if (isAutoProtect) protect();
	}
};

#endif

#endif