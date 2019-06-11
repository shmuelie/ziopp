#pragma once

#include <string>

namespace ziopp {
	class upath {
	public:
		static const char directory_seperator = '/';

		explicit upath();

		upath(std::string& path);

		const std::string& full_name() const;

		bool empty() const;

		bool absolute() const;

		bool relative() const;

		bool equals(const upath& other) const;
		bool operator==(const upath& other) const;
		bool operator!=(const upath& other) const;

		explicit operator std::string() const;

		static const upath combine(const upath& path1, const upath& path2);
		static const upath combine(const upath& path1, const upath& path2, const upath& path3);
		static const upath combine(const upath& path1, const upath& path2, const upath& path3, const upath& path4);

		const upath operator/(const upath& other);
	private:
		explicit upath(std::string& path, bool safe);

		std::string full_name_;
	};
}