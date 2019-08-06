#pragma once

#include <chrono>
#include <istream>
#include <string>
#include <vector>
#include <ziopp/filesystem_watcher.h>
#include <ziopp/upath.h>
#include <ziopp/upath_iterator.h>

namespace ziopp {

	/**
	 * @brief Specifies how the file system should open a file.
	 *
	 */
	enum class file_mode {
		/**
		 * @brief Specifies that the file system should create a new file.
		 *
		 * If the file already exists, the operation should fail.
		 *
		 */
		create_new = 1,
		/**
		 * @brief Specifies that the file system should create a new file.
		 *
		 * If the file already exists, it will be overwritten.
		 *
		 */
		create = 2,
		/**
		 * @brief Specifies that the file system should open an existing file.
		 *
		 * If the file does not exist the operation should fail.
		 *
		 */
		open = 3,
		/**
		 * @brief Specifies that the file system should open a file if it exists; otherwise, a new file should be created.
		 *
		 */
		open_or_create = 4,
		/**
		 * @brief Specifies that the file system should open an existing file. When the file is opened, it should be truncated so that its size is zero bytes.
		 *
		 */
		truncate = 5,
		/**
		 * @brief Opens the file if it exists and seeks to the end of the file, or creates a new file.
		 *
		 */
		append = 6
	};

	/**
	 * @brief Defines constants for read, write, or read/write access to a file.
	 *
	 */
	enum class file_access {
		/**
		 * @brief Read access to the file.
		 *
		 */
		read = 0x01,
		/**
		 * @brief Write access to the file.
		 *
		 */
		write = 0x10,
		/**
		 * @brief Read and write access to the file.
		 *
		 */
		read_write = read | write
	};

	file_access operator | (file_access lhs, file_access rhs);
	file_access& operator |= (file_access& lhs, file_access rhs);
	file_access operator & (file_access lhs, file_access rhs);
	file_access& operator &= (file_access& lhs, file_access rhs);

	/**
	 * @brief Specifies whether to search the current directory, or the current directory and all subdirectories.
	 *
	 */
	enum class search_options {
		/**
		 * @brief Includes only the current directory in a search operation.
		 *
		 */
		top_directory_only = 0,
		/**
		 * @brief Includes the current directory and all its subdirectories in a search operation.
		 *
		 * This option includes reparse points such as mounted drives and symbolic links in the search.
		 *
		 */
		all_directories = 1
	};

	/**
	 * @brief Defines the behavior when looking for files and/or folders.
	 *
	 */
	enum class search_target {
		/**
		 * @brief Search for both files and folders.
		 *
		 */
		both = 0,
		/**
		 * @brief Search for files.
		 *
		 */
		file = 1,
		/**
		 * @brief Search for directories.
		 *
		 */
		directory = 2
	};

	/**
	 * @brief Interface of a file system.
	 *
	 */
	class filesystem {
	public:
		/**
		 * @brief Creates all directories and subdirectories in the specified path unless they already exist.
		 *
		 * @param path The directory to create.
		 */
		virtual void create_directory(const upath& path) = 0;

		/**
		 * @brief Determines whether the given path refers to an existing directory on disk.
		 *
		 * @param path The path to test.
		 * @return true if the given path refers to an existing directory on disk.
		 * @return false if the given path does not refer to an existing directory on disk.
		 */
		virtual bool directory_exists(const upath& path) const = 0;

		/**
		 * @brief Moves a directory and its contents to a new location.
		 *
		 * @param src the path of the directory to move.
		 * @param dest The path to the new location for src.
		 */
		virtual void move_directory(const upath& src, const upath& dest) = 0;

		/**
		 * @brief Deletes the specified directory and, if indicated, any subdirectories and files in the directory.
		 *
		 * @param path The path of the directory to remove.
		 * @param recursive true to remove directories, subdirectories, and files in path; otherwise false.
		 */
		virtual void delete_directory(const upath& path, bool recursive) = 0;

		/**
		 * @brief Copies and existing file to a new file.
		 *
		 * Overwriting a files of the same name is allowed.
		 *
		 * @param src The path of the file to copy.
		 * @param dest The path of the destination file. This cannot be a directoy.
		 * @param overwrite true if the destination file can be overwritten; otherwise false.
		 */
		virtual void copy_file(const upath& src, const upath& dest, bool overwrite) = 0;

		/**
		 * @brief Replaces the contents of a specified file with contents of another file, deleting the original file, and creating a backup of the replace file and optionally ignores merge errors.
		 *
		 * @param src The path of a file the replaces the file specified by dest.
		 * @param dest The path of the file being replaced.
		 * @param desk_backup The path of the backup file.
		 * @param ignore_metadata_errors true to ignore merge errors (such as attributes and access control lists (ACLs)) from the replaced file to the replacement file; otherwise false.
		 */
		virtual void replace_file(const upath& src, const upath& dest, const upath& desk_backup, bool ignore_metadata_errors) = 0;

		/**
		 * @brief Replaces the contents of a specified file with contents of another file, deleting the original file and optionally ignores merge errors.
		 *
		 * @param src The path of a file the replaces the file specified by dest.
		 * @param dest The path of the file being replaced.
		 * @param ignore_metadata_errors true to ignore merge errors (such as attributes and access control lists (ACLs)) from the replaced file to the replacement file; otherwise false.
		 */
		virtual void replace_file(const upath& src, const upath& dest, bool ignore_metadata_errors) = 0;

		/**
		 * @brief Gets the size, in bytes, of a file.
		 *
		 * @param path The path of a file.
		 * @return size_t The size, in bytes, of the file.
		 */
		virtual size_t file_length(const upath& path) const = 0;

		/**
		 * @brief Determines where the specified file exists.
		 *
		 * @param path The path
		 * @return true if the caller has the required permissions and path contains the name of an existing file.
		 * @return false if path is null, an invalid path, a zero-length string, the caller does not have sufficient permissions to read the specified file, or there is no file at the path.
		 */
		virtual bool file_exists(const upath& path) const = 0;

		/**
		 * @brief Moves a specified file to a new location, providing the option to specify a new file name.
		 *
		 * @param src The path of the file to move.
		 * @param dest The new path and name for the file.
		 */
		virtual void move_file(const upath& src, const upath& dest) = 0;

		/**
		 * @brief Deletes the specified file.
		 *
		 * @param path The path of the file to be deleted.
		 */
		virtual void delete_file(const upath& path) = 0;

		/**
		 * @brief Opens a file on the specifed path with the specified mode.
		 *
		 * @param path The path to the file to open.
		 * @param mode A value that specifies whether a files is created if one does not exist, and determines whether the contents of existing files are retained or overwritten.
		 * @param access A value that specifies the operations that can be performed on the file.
		 * @return std::iostream& A file on the specified path, have the specified mode, with read, write, or read/write access.
		 */
		virtual std::iostream& open_file(const upath& path, file_mode mode, file_access access) = 0;

		/**
		 * @brief Returns the creation date and time of the specified file or directory.
		 *
		 * @param path the path to a file or directory for which to optain creation date and time information.
		 * @return const std::chrono::system_clock::time_point&
		 */
		virtual const std::chrono::system_clock::time_point& creation_time(const upath& path) const = 0;

		/**
		 * @brief Sets the date and time the file was created.
		 *
		 * @param path the path to a file or directory for which to set the creation date and time.
		 * @param time the value to set for the creation date and time of path.
		 */
		virtual void creation_time(const upath& path, const std::chrono::system_clock::time_point& time) = 0;

		/**
		 * @brief Returns the last access date and time of the specified file or directory.
		 *
		 * @param path
		 * @return const std::chrono::system_clock::time_point&
		 */
		virtual const std::chrono::system_clock::time_point& access_time(const upath& path) const = 0;

		/**
		 * @brief Sets the date and time the file was last accessed.
		 *
		 * @param path The path to a file or directory for which to set the last access date and time.
		 * @param time the value to set for the accessed date and time of path.
		 */
		virtual void access_time(const upath& path, const std::chrono::system_clock::time_point& time) = 0;

		/**
		 * @brief Returns the last write date and time of the specified file or directory.
		 *
		 * @param path The path to a file or directory for which to obtain last write date and time.
		 * @return const std::chrono::system_clock::time_point&
		 */
		virtual const std::chrono::system_clock::time_point& write_time(const upath& path) const = 0;

		/**
		 * @brief Sets the date and time the file was last written to.
		 *
		 * @param path The path to a file or directory for which to set the last write date and time.
		 * @param time the value to set for the last written date and time of path.
		 */
		virtual void write_time(const upath& path, const std::chrono::system_clock::time_point& time) = 0;

		/**
		 * @brief Returns an iterator of file names and/or directory names that match a search pattern in a specified path, and optionally searches subdirectories.
		 *
		 * @param path The path to the directory to search.
		 * @param search_pattern The search string to match against file-system entries in path. This parameter can contain a combination of valid literal path and wildcard (* and ?) characters (see Remarks), but doesn't support regular expressions.
		 * @param options One of the enumeration values that specifies whether the search operation should include only the current directory or should include all subdirectories.
		 * @param target The search target either files and folders or only directories or files.
		 * @return const upath_iterator An iterator of file-system paths in the directory specified by path and that match the specified search pattern, option and target.
		 */
		virtual const upath_iterator enumerate_paths(const upath& path, const std::string& search_pattern, search_options options, search_target target) const = 0;

		/**
		 * @brief Checks if the file system and path can be watched.
		 *
		 * @param path The pack to check.
		 * @return true if the path can be watched on this file system.
		 * @return false if the path cannot be watched on this file system.
		 */
		virtual bool can_watch(const upath& path) const = 0;

		/**
		 * @brief Returns a filesystem_watcher that can be used to watch for changes to files and directories in the given path.
		 *
		 * @param path The path to watch changes for.
		 * @return const filesystem_watcher& A filesystem_watcher that watches the given path.
		 */
		virtual const filesystem_watcher& watch(const upath& path) = 0;

		virtual const std::string path_to_internal(const upath& path) const = 0;

		virtual const upath& path_from_internal(const std::string& system_path) const = 0;

		/**
		 * @brief Copies a file between two filesystems.
		 *
		 * @param dest_filesystem The destination filesystem.
		 * @param src The source path of the file to copy.
		 * @param dest The destination path of the file in the destination filesystem.
		 * @param overwrite true to overwrite an existing destination file.
		 */
		void copy_file_cross(filesystem& dest_filesystem, const upath& src, const upath& dest, bool overwrite);

		/**
		 * @brief Moves a file between two filesystems.
		 *
		 * @param dest_filesystem The destination filesystem.
		 * @param src The source path of the file to move.
		 * @param dest The destination path of the file in the destination filesystem.
		 */
		void move_file_cross(filesystem& dest_filesystem, const upath& src, const upath& dest);

		/**
		 * @brief Opens a binary file, reads the contents of the file into a vector of unsigned 8bit integers, and then closes the file.
		 *
		 * @param path The path of the file to open for reading.
		 * @return const std::vector<uint8_t> A vector of unsigned 8bit integers containing the contents of the file.
		 */
		const std::vector<uint8_t> read_all_binary(const upath& path);

		/**
		 * @brief Open a file, read all the lines of the file
		 *
		 * @param path
		 * @return const std::string
		 */
		const std::string read_all_text(const upath& path);

		/**
		 * @brief Creates a new file, writes the specified vector of unsigned 8bit integers to the file, and then closes the file.
		 *
		 * If the target file already exists, it is overwritten.
		 *
		 * @param path The path of the file to open for writing.
		 * @param content The content.
		 */
		void write_all_binary(const upath& path, const std::vector<uint8_t>& content);

		/**
		 * @brief Opens a file, reads all lines of the file, and then closes the file.
		 *
		 * @param path The path of the file to open for reading.
		 * @return const std::vector<std::string> lines of the file.
		 */
		const std::vector<std::string> read_all_lines(const upath& path);

		/**
		 * @brief Creates a new file, writes the specified std::string to the file, and then closes the file.
		 *
		 * If the target file already exists, it is overwritten.
		 *
		 * @param path The path of the file to open for writing.
		 * @param content The content.
		 */
		void write_all_text(const upath& path, std::string& content);

		/**
		 * @brief Opens a file, appends the specified std::string to the file, and then closes the file.
		 *
		 * If the file does not exist, this method creates a file, writes the specified std::string to the file, then closes the file.
		 *
		 * @param path The path of the file to open for appending.
		 * @param content The content to append.
		 */
		void append_all_text(const upath& path, std::string& content);

		/**
		 * @brief Creates or overwrites a file in the specified path.
		 *
		 * @param path The path and name of the file to create.
		 * @return std::iostream&
		 */
		std::iostream& create_file(const upath& path);
	protected:
		std::ios_base::openmode convert(file_mode mode, file_access access);
	};
}