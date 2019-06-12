#pragma once

#include <string>

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
	private:
		explicit upath(const std::string& path, bool safe);

		std::string full_name_;
	};
}