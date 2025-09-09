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
		(IS_REGULAR_FILE)
		(CREATE_DIRECTORY)
		(CREATE_DIRECTORIES)
		(REMOVE)
		(RENAME)
		(CURRENT_PATH)
		[DIRECTORY_ENTRY]
		[DIRECTORY_ITERATOR]
		(COPY_FILE)
		(COPY_DIRECTORY)
		(COPY)

*/

#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <string>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <fstream>
#include <cerrno>

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

	enum copy_options {
		copy_options_none = 0,
		copy_options_recursive = 1 << 0,
		copy_options_overwrite_existing = 1 << 1
	};

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
			// avoid double separator if already ends with one
			if (!m_path.empty() && (m_path[m_path.size() - 1] == '/' || m_path[m_path.size() - 1] == '\\')) {
				return m_path + other.m_path;
			}
			std::string s = m_path;
			s += PATH_SEP;
			s += other.m_path;
			return path(s);
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
	inline bool exists(const path& p) { return exists(p.string()); }

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

// (IS_REGULAR_FILE
	inline bool is_regular_file(const path& p) {
		if (!exists(p)) return false;
		return !is_directory(p);
	}

// (CREATE_DIRECTORY)
	inline bool create_directory(const std::string& path) {
#if defined(_WIN32)
		return _mkdir(path.c_str()) == 0;
#else
		return mkdir(path.c_str(), 0755) == 0;
#endif
	}
	inline bool create_directory(const path& p) { return create_directory(p.string()); }

// (CREATE_DIRECTORIES)
	inline bool create_directories(const std::string& pstr) {
		if (pstr.empty()) return false;
		// if already exists and is directory, done
		if (exists(pstr)) {
			// if it's a directory, return true; otherwise can't create
			path pp(pstr);
			if (is_directory(pp)) return true;
			return false;
		}

		// iterate components
		std::string cur;
#if defined(_WIN32)
		// handle drive letter "C:\"
		if (pstr.size() >= 2 && pstr[1] == ':') {
			cur = pstr.substr(0, 2); // "C:"
			// if it begins with "C:\" include the separator
			if (pstr.size() >= 3 && (pstr[2] == '\\' || pstr[2] == '/')) {
				cur += PATH_SEP;
			}
		}
		else if (!pstr.empty() && (pstr[0] == '\\' || pstr[0] == '/')) {
			cur = std::string(1, PATH_SEP);
		}
#else
		if (!pstr.empty() && pstr[0] == '/') {
			cur = std::string(1, PATH_SEP);
		}
#endif

		size_t i = 0;
		while (i < pstr.size()) {
			// find next separator
			size_t pos = pstr.find_first_of("/\\", i);
			std::string part;
			if (pos == std::string::npos) {
				part = pstr.substr(i);
				i = pstr.size();
			}
			else {
				part = pstr.substr(i, pos - i);
				i = pos + 1;
			}
			if (!part.empty()) {
				if (!cur.empty() && cur[cur.size() - 1] != PATH_SEP) cur += PATH_SEP;
				cur += part;
			}
			// create cur if not exists
			if (!cur.empty() && !exists(cur)) {
#if defined(_WIN32)
				int r = _mkdir(cur.c_str());
#else
				int r = mkdir(cur.c_str(), 0755);
#endif
				if (r != 0 && errno != EEXIST) return false;
			}
		}
		// final check
		return exists(pstr) && is_directory(path(pstr));
	}
	inline bool create_directories(const path& p) { return create_directories(p.string()); }

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

// (FREE)
	inline void free(void* ptr) {
		if (!ptr) return;
		std::free(ptr);
	}

// (CURRENT_PATH)
	inline std::string current_path() {
#if defined(_WIN32)
		char buf[MAX_PATH];
		if (_getcwd(buf, MAX_PATH) == NULL) return std::string();
		return std::string(buf);
#else
		char buf[PATH_MAX];
		if (getcwd(buf, PATH_MAX) == NULL) return std::string();
		return std::string(buf);
#endif
	}

// [DIRECTORY_ENTRY]
	class directory_entry {
		path _p;
	public:
		directory_entry(const path& p) : _p(p) {}

		const path& path_() const { return _p; }
		path getpath() const { return _p; }

		bool is_directory() const { return fs::is_directory(_p); }
		bool is_regular_file() const { return fs::is_regular_file(_p); }
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

	// (COPY_FILE)
	inline bool copy_file(const path& src, const path& dst, int options = copy_options_none) {
		// if src doesn't exist or is directory -> fail
		if (!exists(src) || is_directory(src)) return false;

		// if dst exists and not overwrite -> fail
		if (exists(dst) && !(options & copy_options_overwrite_existing)) {
			return false;
		}

		std::ifstream in(src.string().c_str(), std::ios::binary);
		if (!in.is_open()) return false;

		// ensure destination directory exists
		std::string dstPath = dst.string();
		// find last sep
		size_t pos = dstPath.find_last_of("/\\");
		if (pos != std::string::npos) {
			std::string dir = dstPath.substr(0, pos);
			if (!dir.empty() && !exists(dir)) {
				// try create directories
				create_directories(dir);
			}
		}

		std::ofstream out(dst.string().c_str(), std::ios::binary | std::ios::trunc);
		if (!out.is_open()) {
			in.close();
			return false;
		}

		const std::size_t bufsize = 4096;
		char buffer[bufsize];
		while (in.good()) {
			in.read(buffer, bufsize);
			std::streamsize s = in.gcount();
			if (s > 0) out.write(buffer, s);
		}
		in.close(); out.close();
		return true;
	}

	// (COPY_DIRECTORY)
	inline bool copy_directory(const path& src, const path& dst, int options = copy_options_none) {
		if (!exists(src) || !is_directory(src)) return false;

		// create destination dir
		if (!exists(dst)) {
			if (!create_directories(dst.string())) return false;
		}

		// iterate entries
		fs::directory_iterator dit(src);
		for (std::vector<fs::directory_entry>::iterator it = dit.begin(); it != dit.end(); ++it) {
			path child = it->getpath();
			// compute destination path
			path destChild = dst / child.filename();
			if (it->is_directory()) {
				if (!(options & copy_options_recursive)) continue;
				if (!copy_directory(child, destChild, options)) return false;
			}
			else {
				if (!copy_file(child, destChild, options)) return false;
			}
		}
		return true;
	}

	// (COPY)
	inline bool copy(const path& src, const path& dst, int options = copy_options_none) {
		if (!exists(src)) return false;
		if (is_directory(src)) {
			// if recursion not requested fail
			if (!(options & copy_options_recursive)) return false;
			return copy_directory(src, dst, options);
		}
		else {
			return copy_file(src, dst, options);
		}
	}

};

#endif
