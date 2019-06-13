#pragma once

#include <string>
#include <vector>

namespace ziopp {
	/**
	 * @brief A uniform unix like path.
	 *
	 */
	class upath {
	public:
		/**
		 * @brief The directory separator `/`
		 *
		 */
		static const char directory_seperator = '/';

		/**
		 * @brief Construct a new upath object
		 *
		 */
		explicit upath();

		/**
		 * @brief Construct a new upath object
		 *
		 * @param path The path that will be normalized.
		 */
		upath(const std::string& path);

		/**
		 * @brief Gets the full name of this path.
		 *
		 * @return const std::string& The full name of this path.
		 */
		const std::string& full_name() const;

		/**
		 * @brief Gets a value indicating whether this path is empty.
		 *
		 * @return true if this instance is empty.
		 * @return false if this instance is not empty.
		 */
		bool empty() const;

		/**
		 * @brief Gets a value indicating whether this path is absolute by starting with a leading `/`.
		 *
		 * @return true if this path is absolute.
		 * @return false if this path is relative.
		 */
		bool absolute() const;

		/**
		 * @brief Gets a value indicating whether this path is relative by not starting with a leading `/`.
		 *
		 * @return true if this instance is relative.
		 * @return false if this instance is absolute.
		 */
		bool relative() const;

		bool equals(const upath& other) const;
		bool operator==(const upath& other) const;
		bool operator!=(const upath& other) const;

		/**
		 * @brief Performs an explicit conversion from upath to std::string.
		 *
		 * @return std::string
		 */
		explicit operator std::string() const;

		/**
		 * @brief Combines two paths into a new path.
		 *
		 * @param path1 The first path to combine.
		 * @param path2 The second path to combine.
		 * @return const upath The combined paths. If one of the specified paths is a zero-length string, this method returns the other path. If path2 contains an absolute path, this method returns path2.
		 */
		static const upath combine(const upath& path1, const upath& path2);
		static const upath combine(const upath& path1, const upath& path2, const upath& path3);
		static const upath combine(const upath& path1, const upath& path2, const upath& path3, const upath& path4);

		/**
		 * @brief Implements the / operator equivalent of upath::combine()
		 *
		 * @param other
		 * @return const upath
		 */
		const upath operator/(const upath& other);

		/**
		 * @brief Converts the path to a relative path (by removing the leading `/`). If the path is already relative, returns a copy.
		 *
		 * @return const upath A relative path.
		 */
		const upath to_relative() const;

		/**
		 * @brief Converts the path to an absolute path (by adding a leading `/`). If the path is already absolute, returns a copy.
		 *
		 * @return const upath An absolute path.
		 */
		const upath to_absolute() const;

		/**
		 * @brief Gets the directory.
		 *
		 * @return const upath The directory of the path.
		 */
		const upath directory() const;

		/**
		 * @brief Gets the first directory.
		 *
		 * @return const std::string& The first directory of the path.
		 */
		const std::string first_directory() const;

		/**
		 * @brief Splits the path by directories using the directory separator character `/`.
		 *
		 * @return const std::vector<const std::string&> A list of sub path for each directory entry in the path (/a/b/c returns [a,b,c], or a/b/c returns [a,b,c]).
		 */
		const std::vector<std::string> Split() const;

		/**
		 * @brief Checks if the path is in the given directory. Does not check if the paths exist.
		 *
		 * @param directory The directory to check the path against.
		 * @param recursive True to check if it is anywhere in the directory, false to check if it is directly in the directory.
		 * @return true The path is in the given directory.
		 * @return false The path is not in the given directory.
		 */
		bool in_directory(const upath& directory, bool recursive) const;
	private:
		explicit upath(const std::string& path, bool safe);

		std::string full_name_;
	};
}