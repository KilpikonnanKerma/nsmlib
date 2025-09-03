/* 

	Copyright (C) Nico Rajala 2025

	This is a cross-platform pre C++17 filesystem header
	designed to be used with older C++ versions.
	It has support for C++98 and up.

	I've tried to ease the reading of this file as much as possible
	and I added comments above each "section" of the file. You can
	search the method you are looking for in the table of contents
	below and use your IDEs/text editors find function to search for it.
	
	Also the toc is in all caps so the searching function doesn't try to find
	other possible instances of the same name.

	Table of contents: () - functions [] - classes/structs/other
		[PATH]
		(EXISTS)
		(IS_DIRECTORY)
		(CREATE_DIRECTORY)
		(REMOVE)
		(RENAME)
		[DIRECTORY_ENTRY]
		[DIRECTORY_ITERATOR]

*/

#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <string>
#include <vector>
#include <stdexcept>
#include <cstring>

#if defined(_WIN32)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <direct.h>						// _mkdir
	#include <io.h>							// _access
	#define PATH_SEP '\\'
#else
	#include <sys/stat.h>
	#include <unistd.h>
	#include <dirent.h>
	#define PATH_SEP '/'
#endif

namespace fs {

// [PATH]
	class path {
		std::string m_path;
	public:
		path() {}
		path(const std::string& s) : m_path(s) {}
		path(const char* s) : m_path(s) {}

		std::string string() const { return m_path; }
		const char* c_str() const { return m_path.c_str(); }

		operator std::string() const { return m_path; }

		path filename() const {
			size_t pos = m_path.find_last_of("/\\");
			if (pos == std::string::npos) return m_path;
			return path(m_path.substr(pos + 1));
		}

		path operator/(const path& other) const {
			if (m_path.empty()) return other;
			return m_path + PATH_SEP + other.m_path;
		}

		bool operator==(const std::string& rhs) const { return m_path == rhs; }
		bool operator!=(const std::string& rhs) const { return m_path != rhs; }
	};

// (EXISTS)
	inline bool exists(const std::string& path) {
#if defined(_WIN32)
		return _access(path.c_str(), 0) == 0;
#else
		return access(path.c_str(), F_OK) == 0;
#endif
	}

// (IS_DIRECTORY)
	inline bool is_directory(const path& p) {
#if defined(_WIN32)
		DWORD attrib = GetFileAttributesA(p.string().c_str());
		return(attrib != INVALID_FILE_ATTRIBUTES &&
			(attrib & FILE_ATTRIBUTE_DIRECTORY));
#else
		struct stat s;
		if (stat(p.string().c_str(), &s) != 0) return false;
		return S_ISDIR(s.st_mode);
#endif
	}

// (CREATE_DIRECTORY)
	inline bool create_directory(const std::string& path) {
#if defined(_WIN32)
		return _mkdir(path.c_str()) == 0;
#else
		return mkdir(path.c_str(), 0755) == 0;
#endif
	}

// (REMOVE)
	inline void remove(const path& p) {
#if defined(_WIN32)
		if (!DeleteFileA(p.string().c_str()))
			RemoveDirectoryA(p.string().c_str());
#else
		if (::remove(p.string().c_str()) != 0)
			throw std::runtime_error("Failed to file/dir");
#endif
	}

// (RENAME)
	inline bool rename(const std::string& from, const std::string& to) {
		return ::rename(from.c_str(), to.c_str()) == 0;
	}

// [DIRECTORY_ENTRY]
	class directory_entry {
		path _p;
	public:
		directory_entry(const path& p) : _p(p) {}

		const path& path_() const { return _p; }
		path getpath() const { return _p; }

		bool is_directory() const { return fs::is_directory(_p); }
	};

// [DIRECTORY_ITERATOR]
	class directory_iterator {
		std::vector<directory_entry> entries;
		size_t index;

	public:
		directory_iterator(const path& dir) : index(0) {
#ifdef _WIN32
			WIN32_FIND_DATA fd;
			HANDLE h = INVALID_HANDLE_VALUE;
			std::string search = dir.string() + "\\*";

			h = FindFirstFileA(search.c_str(), &fd);
			if (h == INVALID_HANDLE_VALUE) return;

			do {
				std::string name = fd.cFileName;
				if (name == "." || name == "..") continue;
				entries.push_back(directory_entry(path(dir.string() + PATH_SEP + name)));
			} while (FindNextFile(h, &fd) != 0);
			FindClose(h);
#else
			DIR* d = opendir(dir.string().c_str());
			if (d) {
				struct dirent* de;
				while ((de = readdir(d)) != NULL) {
					std::string name = de->d_name;
					if (name == "." || name == "..") continue;
					entries.push_back(directory_entry(dir / name));
				}
				closedir(d);
			}
#endif
		}

		std::vector<directory_entry>::iterator begin() { return entries.begin(); }
		std::vector<directory_entry>::iterator end() { return entries.end(); }
	};

};

#endif
