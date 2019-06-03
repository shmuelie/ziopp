#include <ziopp/upath.h>
#include <utility>
#include <sstream>
#include <vector>

namespace ziopp {
	struct text_slice {
	public:
		text_slice(size_t start, size_t end) : start_(start), end_(end)
		{
		}

		size_t start() const {
			return start_;
		}

		size_t end() const {
			return end_;
		}

		size_t size() const {
			return end_ - start_ + 1;
		}
	private:
		size_t start_;
		size_t end_;
	};

	bool is_dot_dot(text_slice& slice, std::string& path)
	{
		if (slice.size() != 2)
		{
			return false;
		}
		return path.at(slice.start()) == '.' && path.at(slice.end()) == '.';
	}

	std::pair<std::string, std::string> validate_and_normalize(std::string& path)
	{
		if (path == "/" || path == ".." || path == ".")
		{
			return std::make_pair(path, std::string{});
		}
		if (path == "\\")
		{
			return std::make_pair("/", std::string{});
		}

		std::vector<text_slice> parts;
		size_t lastIndex = 0;
		size_t i = 0;
		bool processParts = false;
		int dotCount = 0;
		for (; i < path.size(); i++)
		{
			char c = path.at(i);

			if (c == '.')
			{
				dotCount++;
			}

			if (c == upath::directory_seperator || c == '\\')
			{
				// optimization: If we don't expect to process the path
				// and we only have a trailing / or \\, then just perform
				// a substring on the path

				if (!processParts && i + 1 == path.size())
				{
					return std::make_pair(path.substr(0, path.size() - 1), std::string{});
				}

				if (c == '\\')
				{
					processParts = true;
				}

				size_t endIndex = i - 1;
				for (i++; i < path.size(); i++)
				{
					c = path.at(i);
					if (c == upath::directory_seperator || c == '\\')
					{
						// If we have consecutive / or \\, we need to process parts
						processParts = true;
						continue;
					}
					break;
				}

				if (endIndex >= lastIndex || endIndex == -1)
				{
					text_slice part{ lastIndex, endIndex };
					parts.push_back(part);

					// If the previous part had only dots, we need to process it
					if (part.size() == dotCount)
					{
						processParts = true;
					}
				}
				dotCount = c == '.' ? 1 : 0;
				lastIndex = i;
			}
		}

		if (lastIndex < path.size())
		{
			text_slice part{ lastIndex, path.size() - 1 };
			parts.push_back(part);

			// If the previous part had only dots, we need to process it
			if (part.size() == dotCount)
			{
				processParts = true;
			}
		}

		// Optimized path if we don't need to compact the path
		if (!processParts)
		{
			return std::make_pair(path, std::string{});
		}

		// Slow path, we need to process the parts
		for (i = 0; i < parts.size(); i++)
		{
			text_slice& part = parts.at(i);
			size_t partLength = part.size();
			if (partLength < 1)
			{
				continue;
			}

			if (path.at(part.start()) != '.')
			{
				continue;
			}

			if (partLength == 1)
			{
				if (parts.size() > 1)
				{
					parts.erase(parts.begin() + i--);
				}
			}
			else
			{
				if (path.at(part.start() + 1) != '.')
				{
					continue;
				}

				// Throws an exception if our slice path contains only `.`  and is longer than 2 characters
				if (partLength > 2)
				{
					bool isValid = false;
					for (size_t j = part.start() + 2; j < part.end(); j++)
					{
						if (path.at(j) != '.')
						{
							isValid = true;
							break;
						}
					}

					if (!isValid)
					{
						return std::make_pair(std::string{}, std::string{ "The path contains invalid dots" });
					}

					continue;
				}

				if (i > 0)
				{
					text_slice& previousSlice = parts.at(i - 1);
					if (!is_dot_dot(previousSlice, path))
					{
						if (previousSlice.size() == 0)
						{
							return std::make_pair(std::string{}, std::string{ "The path cannot go to the parent of a root path" });
						}

						parts.erase(parts.begin() + i--);
						parts.erase(parts.begin() + i--);
					}
				}
			}
		}

		// If we have a single part and it is empty, it is a root
		if (parts.size() == 1 && parts.at(0).start() == 0 && parts.at(0).end() < 0)
		{
			return std::make_pair(std::string{ "/" }, std::string{});
		}

		std::string builder;
		for (i = 0; i < parts.size(); i++)
		{
			text_slice& slice = parts.at(i);
			if (slice.size() > 0)
			{
				builder += path.substr(slice.start(), slice.size());
			}
			if (i + 1 < parts.size())
			{
				builder += '/';
			}
		}
		return std::make_pair(builder, std::string{});
	}

	upath::upath() : upath(std::string{ "" })
	{
	}

	upath::upath(std::string& path) : upath(path, false)
	{
	}

	upath::upath(std::string& path, bool safe)
	{
		if (safe)
		{
			full_name = path;
		}
		else
		{
			auto result = validate_and_normalize(path);
			if (result.second.empty())
			{
				full_name = result.first;
			}
			else
			{
				throw std::exception(result.second.c_str());
			}
		}
	}

	const std::string& upath::get_full_name() const
	{
		return full_name;
	}

	bool upath::empty() const
	{
		return full_name.empty();
	}

	bool upath::absolute() const
	{
		return !empty() && full_name.at(0) == '/';
	}

	bool upath::relative() const
	{
		return !absolute();
	}

	bool upath::equals(const upath& other) const
	{
		return full_name.compare(other.full_name) == 0;
	}

	bool upath::operator==(const upath& other) const
	{
		return equals(other);
	}

	bool upath::operator!=(const upath& other) const
	{
		return !equals(other);
	}

	upath::operator std::string() const
	{
		return full_name;
	}

	const upath upath::combine(const upath& path1, const upath& path2)
	{
		if (path1.empty() && path2.empty())
		{
			return upath{};
		}

		// If the right path is absolute, it takes priority over path1
		if (path2.absolute())
		{
			return path2;
		}

		std::stringstream builder;
		if (!path1.empty())
		{
			builder << path1.full_name << '/';
		}
		if (!path2.empty())
		{
			builder << path2.full_name;
		}

		return upath{ builder.str() };
	}

	const upath upath::combine(const upath& path1, const upath& path2, const upath& path3)
	{
		return combine(combine(path1, path2), path3);
	}

	const upath upath::combine(const upath& path1, const upath& path2, const upath& path3, const upath& path4)
	{
		return combine(combine(path1, path2), combine(path3, path4));
	}

	const upath upath::operator/(const upath& other)
	{
		return combine(*this, other);
	}
}