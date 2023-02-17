#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>

#define TINI_UNICODE -1

#if defined(TINI_UNICODE) && TINI_UNICODE != 0
#define TINI_UNICODE 1
#endif

#if TINI_UNICODE
#include <stringapiset.h>
#endif

#ifndef TINI_H
#define TINI_H

#define COMMENT_PREFIXES "#;"
#define DEFAULT_SECTION_NAME "Global"

typedef std::string str;

namespace tINI {
	namespace utils {
		

		inline void split(str& str, const char* delim, std::vector<std::string>& out)
		{
			size_t start;
			size_t end = 0;

			while ((start = str.find_first_not_of(delim, end)) != str::npos)
			{
				end = str.find(delim, start);
				out.push_back(str.substr(start, end - start));
			}
		}

		inline void split_first(str& str, const char* delim, std::string& first, std::string& second)
		{
			size_t start;
			size_t end = 0;

			while ((start = str.find_first_not_of(delim, end)) != str::npos)
			{
				end = str.find(delim, start);
				first = str.substr(start, end - start);

				if (!first.empty()) {
					second = str.substr(end - start + strlen(delim), str.length());
					break;
				}
			}
		}

		inline void left_trim(str& s) {
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
				return !std::isspace(ch);
			}));
		}

		inline void right_trim(str& s) {
			s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
				return !std::isspace(ch);
			}).base(), s.end());
		}

		inline void trim(str& s) {
			left_trim(s);
			right_trim(s);
		}

		inline bool replace(std::string& str, const std::string& from, const std::string& to) {
			size_t start_pos = str.find(from);
			if (start_pos == std::string::npos)
				return false;
			str.replace(start_pos, from.length(), to);
			return true;
		}

		inline bool replace_all(std::string& str, const std::string& from, const std::string& to) {
			bool cond = false;
			size_t start_pos = 0;
			while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
				str.replace(start_pos, from.length(), to);
				start_pos += to.length();
				cond = true;
			}
			return true;
		}

		inline str format_comment(str _comment) {

			bool hadNewLine = false;

			if (_comment.at(_comment.length() - 1) == '\n')
			{
				hadNewLine = true;
				_comment.erase(_comment.length() - 1, _comment.length());
			}

			const size_t totalLength = strlen(_comment.c_str()) + 1; // +1 for '\0' 

			size_t n = std::count(_comment.begin(), _comment.end(), '\n');
			const char* ocmt = _comment.c_str();
			char* cmt = new char[totalLength + n * 2];

			size_t t = 0;

			cmt[t++] = COMMENT_PREFIXES[0];
			cmt[t++] = ' ';

			for (size_t i = 0; i < totalLength - 1; i++)
			{
				if (ocmt[i] == '\n')
				{
					cmt[t++] = '\n';
					cmt[t++] = COMMENT_PREFIXES[0];
					cmt[t++] = ' ';
				}
				else
					cmt[t++] = ocmt[i];
			}
			cmt[t] = '\0';

			str comment = str(cmt);
			if (hadNewLine)
				comment += '\n';
			return comment;
		}
	}

#if TINI_UNICODE
	namespace unicode {
		// Multi-Byte to Wide Char
		inline const wchar_t* MB2WC(const char* str)
		{
			int str_len = (int)strlen(str);
			int num_chars = MultiByteToWideChar(CP_UTF8, 0, str, str_len, NULL, 0);
			wchar_t* wstrTo = (wchar_t*)malloc((num_chars + 1) * sizeof(WCHAR));
			if (wstrTo)
			{
				MultiByteToWideChar(CP_UTF8, 0, str, str_len, wstrTo, num_chars);
				wstrTo[num_chars] = L'\0';
			}
			return wstrTo;
		}

		// Wide Char to Multi-Byte
		inline const char* WC2MB(const wchar_t* wstr)
		{
			int wstr_len = (int)wcslen(wstr);
			int num_chars = WideCharToMultiByte(CP_UTF8, 0, wstr, wstr_len, NULL, 0, NULL, NULL);
			char* strTo = (char*)malloc((num_chars + 1) * sizeof(char));
			if (strTo)
			{
				WideCharToMultiByte(CP_UTF8, 0, wstr, wstr_len, strTo, num_chars, NULL, NULL);
				strTo[num_chars] = '\0';
			}
			return strTo;
		}

		// Multi-Byte to Wide Char
		inline std::wstring MB2WC(std::string data)
		{
			return std::wstring(MB2WC(data.c_str()));
		}

		// Wide Char to Multi-Byte
		inline std::string WC2MB(std::wstring data)
		{
			return std::string(WC2MB(data.c_str()));
		}
	}
#endif

	class Identifier;
	class Tag;
	class Section;
	class File;

	class Identifier {
		// Name or filename
		str name;
		str comment;

	protected:
		void SetName(str _name) {
			name = _name;
		}

	public:
		// Gets the name of the Section/Tag or the filename of a File
		str GetName() {
			return name;
		}

		void SetComment(str _comment) {

			if (_comment.empty())
			{
				comment = "";
				return;
			}

			comment = utils::format_comment(_comment);
		}

		str GetComment() {
			return comment;
		}
	};

	class Tag : public Identifier {
		str val;

	public:
		Tag(str name, str comment = "") {
			this->SetName(name);
			this->SetComment(comment);
		}

		// Sets the value of the tag
		void Set(str tag_name) {
			val = tag_name;
		}

		// Sets the value of the tag and it's comment
		void Set(str tag_name, str _comment) {
			val = tag_name;
			SetComment(_comment);
		}

		// Gets the value of the tag
		str Get() {
			return val;
		}
	};

	class Section : public Identifier {
	public:
		Section(str name, str comment = "") {
			this->SetName(name);
			this->SetComment(comment);
		}

		// Gets or creates a new tag and saves it
		Tag* Get(str tag_name) {
			if (tag_name.empty())
				tag_name = DEFAULT_SECTION_NAME;
			auto it = tags.find(tag_name);
			if (it != tags.end())
				return it->second;
			else
			{
				auto tag = new Tag(tag_name);
				tags.emplace(tag_name, tag);
				tagsInsertionOrder.push_back(tag_name);
				return tag;
			}
		}

		void DestroyTag(str tag_name) {
			auto it = tags.find(tag_name);
			if (it != tags.end())
			{
				tags.erase(tag_name);
				auto itr = std::find(tagsInsertionOrder.begin(), tagsInsertionOrder.end(), tag_name);
				if (itr != tagsInsertionOrder.end()) tagsInsertionOrder.erase(itr);
			}
		}

	//private:
		std::unordered_map<std::string, tINI::Tag*> tags;
		std::vector<std::string> tagsInsertionOrder;
	};

	class File : public Identifier {
	private:
		void WriteToFile(str data) {
			std::ofstream stream;
			stream.open(GetName(), std::ios_base::trunc /* using trunc to overwrite file data */ );
			stream << data;
			stream.close();
		}
	public:
		File(str filename, str comment = "") {
			this->SetName(filename);
			this->SetComment(comment);
			this->Get(DEFAULT_SECTION_NAME);
		}

		void SetFilename(str filename) {
			this->SetName(filename);
		}

		void Save(bool pretty_print = true, bool pretty_comments = false) {
			str data = pretty_comments ? GetComment().empty() ? "" : GetComment() + "\n" : "";

			for (auto& si : sectionsInsertionOrder)
			{
				auto& s = sections[si];
				if (s->tags.size() == 0)
					continue;
				if (pretty_comments)
					data += s->GetComment().empty() ? "" : s->GetComment() + "\n";
				data += "[" + s->GetName() + "]\n";
				for (auto& ti : s->tagsInsertionOrder)
				{
					auto& t = s->tags[ti];
					auto comment = t->GetComment();
					if (pretty_comments) {
						char inc = COMMENT_PREFIXES[0];
						str in { inc };
						utils::replace_all(comment, in, "  " + in);
					}
					if (pretty_comments)
						data += t->GetComment().empty() ? "" : comment + "\n";
					data += (pretty_print ? "  " : "") + t->GetName() + (pretty_print ? " = " : "=") + t->Get() + "\n";
				}
				if (pretty_print)
					data += "\n";
			}

			if (!data.empty())
				if (data.at(data.length() - 1) == '\n')
					data.erase(data.length() - 1, data.length());

			WriteToFile(data);
		}

		void Load() {
			std::ifstream infile(GetName());
			std::string line;
			str currentComment = "";
			str currentSection = "";

			auto stripNewLine = [&](str& s) {
				if (!s.empty())
					if (s.at(s.length() - 1) == '\n')
						s.erase(s.length() - 1, s.length());
			};

			bool isFirstComment = true;
			while (std::getline(infile, line))
			{
				utils::trim(line);

				if (line.empty() && isFirstComment) {
					SetComment(currentComment);
					currentComment.clear();
					isFirstComment = false;
					continue;
				}

				if (!line.empty() && line.at(0) == '[' && line.at(line.length() - 1) == ']') {
					line.erase(0, 1);
					line.erase(line.length() - 1, line.length());
					utils::trim(line);
					currentSection = line;
					stripNewLine(currentComment);
					Get(line)->SetComment(currentComment);
					currentComment.clear();
					continue;
				}

				if (!currentSection.empty() && !line.empty() && (line.at(0) != COMMENT_PREFIXES[0] && line.at(0) != COMMENT_PREFIXES[1]) && line.find('=') != std::string::npos) {
					str first, second;
					utils::split_first(line, "=", first, second);
					utils::trim(first);
					utils::trim(second);
					Get(currentSection)->Get(first)->Set(second);
					stripNewLine(currentComment);
					Get(currentSection)->Get(first)->SetComment(currentComment);
					currentComment = "";
				}

				if (!line.empty() && (line.at(0) == COMMENT_PREFIXES[0] || line.at(0) == COMMENT_PREFIXES[1])) {
					line.erase(0, 1);
					utils::trim(line);
					currentComment += line + '\n';
				}
			}
		}

		// Gets or creates a new section and saves it
		Section* Get(str section_name = DEFAULT_SECTION_NAME) {
			if (section_name.empty())
				section_name = DEFAULT_SECTION_NAME;
			auto it = sections.find(section_name);
			if (it != sections.end())
				return it->second;
			else
			{
				auto section = new Section(section_name);
				sections.emplace(section_name, section);
				sectionsInsertionOrder.push_back(section_name);
				return section;
			}
		}

		// Shortcut: Gets or creates a new section and saves it and then gets or creates a new tag and saves it
		Tag* Get(str section_name, str tag_name) {
			return Get(section_name)->Get(tag_name);
		}

		// Shortcut: Gets or creates a new section and saves it and then gets or creates a new tag and saves it and gets the value
		template<typename T>
		T Get(str section_name, str tag_name) {
			auto s = Get(section_name)->Get(tag_name)->Get();
			std::istringstream convert(s);
			T value;
			convert >> value;
			return value;
		}

		// Shortcut: gets or creates a new tag and saves it in a global section and gets the value
		template<typename T>
		T Get(str tag_name) {
			auto s = Get(DEFAULT_SECTION_NAME)->Get(tag_name)->Get();
			std::istringstream convert(s);
			T value;
			convert >> value;
			return value;
		}

		// Shortcut: Gets or creates a new section and saves it and then gets or creates a new tag and saves it and sets the value
		void Set(str section_name, str tag_name, str value) {
			Get(section_name)->Get(tag_name)->Set(value);
		}

		// Shortcut: Gets or creates a new section and saves it and then gets or creates a new tag and saves it and sets the value
		void Set(str section_name, str tag_name, const char* value) {
			Get(section_name)->Get(tag_name)->Set(value);
		}

		// Shortcut: Gets or creates a new section and saves it and then gets or creates a new tag and saves it and sets the value
		template<typename T>
		void Set(str section_name, str tag_name, T value) {
			std::string sv = std::to_string(value);
			sv.erase(sv.find_last_not_of('0') + 1, std::string::npos);
			Get(section_name)->Get(tag_name)->Set(sv);
		}

		// Shortcut: gets or creates a new tag and saves it in a global section and sets the value
		void Set(str tag_name, str value) {
			Get(DEFAULT_SECTION_NAME)->Get(tag_name)->Set(value);
		}

		// Shortcut: gets or creates a new tag and saves it in a global section and sets the value
		void Set(str tag_name, const char* value) {
			Get(DEFAULT_SECTION_NAME)->Get(tag_name)->Set(value);
		}

		// Shortcut: gets or creates a new tag and saves it in a global section and sets the value
		template<typename T>
		void Set(str tag_name, T value) {
			std::string sv = std::to_string(value);
			sv.erase(sv.find_last_not_of('0') + 1, std::string::npos);
			Get(DEFAULT_SECTION_NAME)->Get(tag_name)->Set(sv);
		}

		// Shortcut: returns true if the section exists, Otherwise false.
		bool SectionExists(str section_name) {
			auto it = sections.find(section_name);
			if (it != sections.end())
				return true;
			else
				return false;
		}

		// Shortcut: removes an entire section along with it's tags
		void DestroySection(str section_name) {
			auto it = sections.find(section_name);
			if (it != sections.end())
			{
				sections.erase(section_name);
				auto itr = std::find(sectionsInsertionOrder.begin(), sectionsInsertionOrder.end(), section_name);
				if (itr != sectionsInsertionOrder.end()) sectionsInsertionOrder.erase(itr);
			}
		}

		// Shortcut: removes a tag by name and section name
		void DestroyTag(str section_name, str tag_name) {
			Get(section_name)->DestroyTag(tag_name);
		}

		// Returns approx. file size and mem usage in bytes
		int GetFileSize() {
			int size = sizeof(File) + sizeof(char) * GetComment().length();
			for (auto& s : sections)
			{
				for (auto& t : s.second->tags)
				{
					size += sizeof(char) * t.second->Get().length();
					size += sizeof(char) * t.second->GetName().length();
					size += sizeof(Tag);
				}
				size += sizeof(char) * s.second->GetName().length();
				size += sizeof(Section);
			}
			return size;
		}

	private:
		std::unordered_map<std::string, tINI::Section*> sections;
		std::vector<std::string> sectionsInsertionOrder;
	};
}

#endif
