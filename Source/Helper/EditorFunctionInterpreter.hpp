#include <axmol.h>
#include "shared_scenes/GameSingleton.h"
#include "string_manipulation_lib/stringFunctions.hpp"
#include "Helper/short_types.h"
#include <format.h>
#include "shared_scenes/GameUtils.h"

#ifndef __H_EDITORFUNCTIONINTERPRETER__
#define __H_EDITORFUNCTIONINTERPRETER__

struct EditorFunction {
	I32         _int = 0;
	F32         _float = 0.0f;
	ax::Vec2    _vector = ax::Vec2::ZERO;
	std::string _enum = "<unknown>";
	char preferred = '\0';
	std::string bind;
	std::string format;
	int idx;

	bool set(int i) {
		_int = i;
		_float = i;
		_vector = { F32(i), F32(i) };
		_enum = "<int>";
		return true;
	}

	bool set(float f) {
		_int = f;
		_float = f;
		_vector = { f, f };
		_enum = "<float>";
		return true;
	}

	bool set(ax::Vec2 v) {
		_int = v.x + v.y;
		_float = v.x + v.y;
		_vector = v;
		_enum = "<vector>";
		return true;
	}

	bool set(std::string e) {
		try {
			_int = std::stoi(e);
			_float = std::stof(e);
			_vector = { std::stof(e), std::stof(e) };
		}
		catch (std::exception& ex) {};
		_enum = e;
		return true;
	}
};

#define TTI_PARAM_ERR { Rebound::getInstance()->MessageBoxWin32("Failed to load tmx map!", \
	ax::StringUtils::format("Couldn't interpret function '%s' with format of (%s) at param (null), expected %d parameters and got %d parameters, %s.\nMake sure you're following the documentation correctly!", bind.c_str(), format.c_str(), in, out, explain.c_str())); }
#define TTI_ERR { Rebound::getInstance()->MessageBoxWin32("Failed to load tmx map!", \
	ax::StringUtils::format("Couldn't interpret function '%s' with format of (%s) at param %d, expected %c and got %c, this parameter is strict.\nMake sure you're following the documentation correctly!", bind.c_str(), format.c_str(), i + 1, in, out)); }
#define TTI_RETURN_ERR { TTI_ERR return false; }

class EditorFunctionInterpreter {
public:
	// possible format is 'i,f,v,e'
	bool interpret(std::string bind, std::string format, std::string data) {
		VECTOR_CHARS formats;
		Strings::split_single_char(format, ",", formats);
		VECTOR_STRING values;
		Strings::split(data, ",", values);

		if (formats.size() != values.size())
		{
			int in = formats.size();
			int out = values.size();
			std::string explain = "(null)";
			if (in < out)
				explain = "Too many function parameters";
			else
				explain = "Too few function parameters";
			TTI_PARAM_ERR;
			return false;
		}

		int n = formats.size();

		for (U8 i = 0; i < (n > 255 ? 255 : n); i++)
		{
			switch (formats[i])
			{
				case 'i':
				{
					char in = 'i';
					char out = 'i';
					try {
						EditorFunction t;
						t.preferred = 'i';
						t.bind = bind;
						t.format = format;
						t.idx = i;
						if (!t.set((int)std::stoi(values[i])))
							TTI_RETURN_ERR;
						_its[bind].push_back(t);
					}
					catch (std::exception& e)
					{
						TTI_RETURN_ERR;
					};

					break;
				}
				case 'f':
				{
					char in = 'i';
					char out = 'i';
					try {
						EditorFunction t;
						t.preferred = 'f';
						t.bind = bind;
						t.format = format;
						t.idx = i;
						if (!t.set((float)std::stof(values[i])))
							TTI_RETURN_ERR;
						_its[bind].push_back(t);
					}
					catch (std::exception& e)
					{
						TTI_RETURN_ERR;
					};

					break;
				}
				case 'v':
				{
					char in = 'i';
					char out = 'i';
					try {
						EditorFunction t;
						t.preferred = 'v';
						t.bind = bind;
						t.format = format;
						t.idx = i;
						if (!t.set(GameUtils::parseVector2D(values[i])))
							TTI_RETURN_ERR;
						_its[bind].push_back(t);
					}
					catch (std::exception& e)
					{
						TTI_RETURN_ERR;
					};

					break;
				}
				case 'e':
				{
					char in = 'i';
					char out = 'i';
					try {
						EditorFunction t;
						t.preferred = 'e';
						t.bind = bind;
						t.format = format;
						t.idx = i;
						if (!t.set(values[i]))
							TTI_RETURN_ERR;
						_its[bind].push_back(t);
					}
					catch (std::exception& e)
					{
						TTI_RETURN_ERR;
					};

					break;
				}
				default:
					continue;
			}
		}

		return true;
	}

	void bind(std::string bind) { bound = bind; }

	I32 asInt(int idx, bool strict = true) {
		auto it = _its.find(bound);
		if (it == _its.end()) return 0;
		auto type = it->second[idx > it->second.size() - 1 ? it->second.size() - 1 : idx];
		if (strict && type.preferred != 'i') {
			char in = type.preferred;
			char out = 'i';
			std::string bind = type.bind;
			std::string format = type.format;
			int i = type.idx;
			lastError = true;
			TTI_ERR;
		}
		return type._int;
	}

	F32 asFloat(int idx, bool strict = true) {
		auto it = _its.find(bound);
		if (it == _its.end()) return 0;
		auto type = it->second[idx > it->second.size() - 1 ? it->second.size() - 1 : idx];
		if (strict && type.preferred != 'f') {
			char in = type.preferred;
			char out = 'f';
			std::string bind = type.bind;
			std::string format = type.format;
			int i = type.idx;
			lastError = true;
			TTI_ERR;
		}
		return type._float;
	}

	ax::Vec2 asVector(int idx, bool strict = true) {
		auto it = _its.find(bound);
		if (it == _its.end()) return ax::Vec2::ZERO;
		auto type = it->second[idx > it->second.size() - 1 ? it->second.size() - 1 : idx];
		if (strict && type.preferred != 'v') {
			char in = type.preferred;
			char out = 'v';
			std::string bind = type.bind;
			std::string format = type.format;
			int i = type.idx;
			lastError = true;
			TTI_ERR;
		}
		return type._vector;
	}

	std::string asEnum(int idx, bool strict = true) {
		auto it = _its.find(bound);
		if (it == _its.end()) return "";
		auto type = it->second[idx > it->second.size() - 1 ? it->second.size() - 1 : idx];
		if (strict && type.preferred != 'e') {
			char in = type.preferred;
			char out = 'e';
			std::string bind = type.bind;
			std::string format = type.format;
			int i = type.idx;
			lastError = true;
			TTI_ERR;
		}
		return type._enum;
	}

	bool lastError = false;
	std::string bound;
	std::map<std::string, std::vector<EditorFunction>> _its;
};

#endif