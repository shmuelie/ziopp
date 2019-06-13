#include <ziopp/upath.h>
#include <utility>
#include <sstream>

namespace ziopp {
	struct text_slice {
	public:
		text_slice(int start, int end) : start_(start), end_(end)
		{
		}

		int start() const {
			return start_;
		}

		int end() const {
			return end_;
		}

		int size() const {
			return end_ - start_ + 1;
		}
	private:
		int start_;
		int end_;
	};

	bool is_dot_dot(text_slice& slice, const std::string& path)
	{
		if (slice.size() != 2)
		{
			return false;
		}
		return path.at(slice.start()) == '.' && path.at(slice.end()) == '.';
	}

	std::pair<std::string, std::string> validate_and_normalize(const std::string& path)
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
		int lastIndex = 0;
		int i = 0;
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

				int endIndex = i - 1;
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
			text_slice part{ lastIndex, (int)path.size() - 1 };
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
			int partLength = part.size();
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
				// We have a '.'
				if (parts.size() > 1)
				{
					parts.erase(parts.begin() + i);
					i--;
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
					for (int j = part.start() + 2; j <= part.end(); j++)
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

						parts.erase(parts.begin() + i);
						i--;
						parts.erase(parts.begin() + i);
						i--;
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

	upath::upath() : upath(std::string{ })
	{
	}

	upath::upath(const std::string& path) : upath(path, false)
	{
	}

	upath::upath(const std::string& path, bool safe)
	{
		if (safe)
		{
			full_name_ = path;
		}
		else
		{
			auto result = validate_and_normalize(path);
			if (result.second.empty())
			{
				full_name_ = result.first;
			}
			else
			{
				throw std::invalid_argument(result.second.c_str());
			}
		}
	}

	const std::string& upath::full_name() const
	{
		return full_name_;
	}

	bool upath::empty() const
	{
		return full_name_.empty();
	}

	bool upath::absolute() const
	{
		return !empty() && full_name_.at(0) == '/';
	}

	bool upath::relative() const
	{
		return !absolute();
	}

	bool upath::equals(const upath& other) const
	{
		return full_name_.compare(other.full_name_) == 0;
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
		return full_name_;
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
			builder << path1.full_name_ << '/';
		}
		if (!path2.empty())
		{
			builder << path2.full_name_;
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

	const upath upath::to_relative() const
	{
		if (relative())
		{
			return upath{full_name_, true};
		}
		return full_name_ == "/" ? upath{} : upath{full_name_.substr(1), true};
	}

	const upath upath::to_absolute() const
	{
		if (absolute())
		{
			return upath{full_name_, true};
		}
		return empty() ? upath{"/", true} : upath{"/", true} / *this;
	}

	const upath upath::directory() const
	{
		if (full_name_ == "/")
		{
			return upath{};
		}

		auto const last_index = full_name_.find_last_of(directory_seperator);
		if (last_index != std::string::npos && last_index > 0)
		{
			return upath{full_name_.substr(0, last_index)};
		}
		return last_index == 0 ? upath{"/", true} : upath{};
	}

	const std::string upath::first_directory() const
	{
		auto const index = full_name_.find_first_of(directory_seperator);
		if (index == std::string::npos)
		{
			return full_name_.substr(1, full_name_.size() - 1);
		}
		return full_name_.substr(1, index - 1);
	}

	const std::vector<std::string> upath::Split() const
	{
		if (empty())
		{
			return std::vector<std::string>{};
		}

		std::vector<std::string> paths;
		size_t previous_index = absolute() ? 1 : 0;
		size_t next_index = 0;
		while ((next_index = full_name_.find_first_of(directory_seperator, previous_index)) != std::string::npos)
		{
			if (next_index != 0)
			{
				paths.push_back(full_name_.substr(previous_index, next_index - previous_index));
			}

			previous_index = next_index + 1;
		}

		if (previous_index < full_name_.size())
		{
			paths.push_back(full_name_.substr(previous_index, full_name_.size() - previous_index));
		}
		return paths;
	}

	bool upath::in_directory(const upath& directory, bool recursive) const
	{
		if (absolute() != directory.absolute())
		{
			throw new std::invalid_argument("Cannot mix absolute and relative paths");
		}

		const std::string& target = full_name_;
		const std::string& dir = directory.full_name_;

		if (target.size() < dir.length() || target.find_first_of(dir) == std::string::npos)
		{
			return false;
		}

		if (target.size() == dir.size())
		{
			// exact match due to the StartsWith above
            // the directory parameter is interpreted as a directory so trailing separator isn't important
			return true;
		}

		bool dir_has_trailing_separator = dir.at(dir.size() - 1) == directory_seperator;

		if (!recursive)
		{
			// need to check if the directory part terminates
			const auto last_separator_in_target = target.find_last_of(directory_seperator);
			const auto expected_last_separator = dir.size() - (dir_has_trailing_separator ? 1 : 0);

			if (last_separator_in_target != expected_last_separator)
			{
				return false;
			}
		}

		if (!dir_has_trailing_separator)
		{
			// directory is missing ending slash, check that target has it
			return target.size() > dir.size() && target.at(dir.size()) == directory_seperator;
		}

		return true;
	}
}