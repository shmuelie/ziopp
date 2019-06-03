Zio++
=====

Zio++ provides a simple, powerful, cross-platform filesystem abstraction for C++ with many built-ins filesystems.

Zio++ is based on [Zio](https://github.com/xoofx/zio) by Alexandre MUTEL aka [xoofx](http://xoofx.com/)

# Features

- Compatible with C++ 11.
- All paths are normalized through a lightweight uniform path class [upath](ziopp/includes/upath.h).
- Multiple built-in filesystems:
  - `StdFileSystem` provides access to physical disks, directories, and folders using [std::filesystem](https://en.cppreference.com/w/cpp/filesystem). (Requires C++ 17)
  - `BoostFileSystem` provides access to physical disks, directories, and fodlers using [Boost Filesystem](http://www.boost.org/doc/libs/release/libs/filesystem/doc/index.htm).
  - `PocoFileSystem` provices access to physical disks, directories, and folders using [Poco Filesystem](https://pocoproject.org/docs/package-Foundation.Filesystem.html).