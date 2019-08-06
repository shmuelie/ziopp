#include <ziopp/filesystem.h>
#include <type_traits>

namespace ziopp {

	file_access operator | (file_access lhs, file_access rhs)
	{
		using T = std::underlying_type<file_access>::type;
		return static_cast<file_access>(static_cast<T>(lhs) | static_cast<T>(rhs));
	}

	file_access& operator |= (file_access& lhs, file_access rhs)
	{
		lhs = lhs | rhs;
		return lhs;
	}

	file_access operator & (file_access lhs, file_access rhs)
	{
		using T = std::underlying_type<file_access>::type;
		return static_cast<file_access>(static_cast<T>(lhs) & static_cast<T>(rhs));
	}

	file_access& operator &= (file_access& lhs, file_access rhs)
	{
		lhs = lhs & rhs;
		return lhs;
	}

	void filesystem::copy_file_cross(filesystem& dest_filesystem, const upath& src, const upath& dest, bool overwrite)
	{
		if (this == &dest_filesystem)
		{
			copy_file(src, dest, overwrite);
			return;
		}

		if (!src.absolute())
		{
			throw std::invalid_argument("src must be absolute");
		}

		if (!file_exists(src))
		{
			throw std::ios_base::failure("src file must exist", std::make_error_code(std::errc::no_such_file_or_directory));
		}

		if (!dest.absolute())
		{
			throw std::invalid_argument("dest must be absolute");
		}

		upath dest_directory = dest.directory();
		if (!dest_filesystem.directory_exists(dest_directory))
		{
			throw std::ios_base::failure("dest directory must exist", std::make_error_code(std::errc::no_such_file_or_directory));
		}

		if (dest_filesystem.file_exists(dest) && !overwrite)
		{
			throw std::ios_base::failure("the destination file path already exists and overwrite is false", std::make_error_code(std::errc::file_exists));
		}

		std::iostream& source_stream = open_file(src, file_mode::open, file_access::read);
		std::iostream& dest_stream = dest_filesystem.open_file(dest, file_mode::create, file_access::write);
		dest_stream << source_stream.rdbuf();
		dest_filesystem.write_time(dest, write_time(src));
	}

	void filesystem::move_file_cross(filesystem& dest_filesystem, const upath& src, const upath& dest)
	{
		if (this == &dest_filesystem)
		{
			move_file(src, dest);
			return;
		}

		if (!src.absolute())
		{
			throw std::invalid_argument("src must be absolute");
		}

		if (!file_exists(src))
		{
			throw std::ios_base::failure("src file must exist", std::make_error_code(std::errc::no_such_file_or_directory));
		}

		if (!dest.absolute())
		{
			throw std::invalid_argument("dest must be absolute");
		}

		upath dest_directory = dest.directory();
		if (!dest_filesystem.directory_exists(dest_directory))
		{
			throw std::ios_base::failure("dest directory must exist", std::make_error_code(std::errc::no_such_file_or_directory));
		}

		if (dest_filesystem.file_exists(dest))
		{
			throw std::ios_base::failure("the destination file path already exists and overwrite is false", std::make_error_code(std::errc::file_exists));
		}

		std::iostream& source_stream = open_file(src, file_mode::open, file_access::read);
		std::iostream& dest_stream = dest_filesystem.open_file(dest, file_mode::create, file_access::write);
		dest_stream << source_stream.rdbuf();
		dest_filesystem.creation_time(dest, creation_time(src));
		dest_filesystem.access_time(dest, access_time(src));
		dest_filesystem.write_time(dest, write_time(src));
		delete_file(src);
	}

	const std::vector<uint8_t> filesystem::read_all_binary(const upath& path)
	{
		//source_stream.seekg(0, std::ios_base::end);
		//std::streampos stream_length = source_stream.tellg();
		//source_stream.seekg(0, std::ios_base::beg);
		std::iostream& source_stream = open_file(path, file_mode::open, file_access::read);
		size_t stream_length = file_length(path);
		std::vector<uint8_t> bytes(stream_length);
		source_stream.read((char*)& bytes[0], stream_length);
		return bytes;
	}

	const std::string filesystem::read_all_text(const upath& path)
	{
		std::iostream& source_stream = open_file(path, file_mode::open, file_access::read);
		std::string str{ std::istreambuf_iterator<std::iostream::char_type>(source_stream), {} };
		return str;
	}

	void filesystem::write_all_binary(const upath& path, const std::vector<uint8_t>& content)
	{
		std::iostream& destination_stream = open_file(path, file_mode::create, file_access::write);
		size_t content_length = content.size();
		destination_stream.write((char*)& content[0], content_length);
	}

	const std::vector<std::string> filesystem::read_all_lines(const upath& path)
	{
		std::vector<std::string> lines{};
		std::iostream& source_stream = open_file(path, file_mode::open, file_access::read);
		std::string line;
		while (!std::getline(source_stream, line).eof())
		{
			lines.push_back(line);
		}
		return lines;
	}

	void filesystem::write_all_text(const upath& path, std::string& content)
	{
		std::iostream& destination_stream = open_file(path, file_mode::create, file_access::write);
		size_t content_length = content.length();
		destination_stream.write(content.c_str(), content_length);
	}

	void filesystem::append_all_text(const upath& path, std::string& content)
	{
		std::iostream& destination_stream = open_file(path, file_mode::append, file_access::write);
		size_t content_length = content.length();
		destination_stream.write(content.c_str(), content_length);
	}

	std::iostream& filesystem::create_file(const upath& path)
	{
		return open_file(path, file_mode::create_new, file_access::write);
	}

	std::ios_base::openmode filesystem::convert(file_mode mode, file_access access)
	{
		switch (mode)
		{
			case file_mode::append:
				if (access == file_access::write) {
					return std::ios_base::app | std::ios_base::out;
				}
				break;
			case file_mode::create:
			case file_mode::truncate:
				if (access == file_access::write)
				{
					return std::ios_base::trunc | std::ios_base::out;
				}
				break;
			case file_mode::create_new:
				if (access == file_access::write)
				{
					return std::ios_base::out;
				}
				break;
			case file_mode::open:
			case file_mode::open_or_create:
				std::ios_base::openmode openmode = 0;
				if ((access & file_access::read) == file_access::read)
				{
					openmode &= std::ios_base::in;
				}
				if ((access & file_access::write) == file_access::write)
				{
					openmode &= std::ios_base::out;
				}
				return openmode;
		}
		return std::ios_base::failbit;
	}
}