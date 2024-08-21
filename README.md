# FUSE-Based WAD Filesystem

# Overview
This project implements a custom filesystem using the FUSE (Filesystem in UserSpace) API to interact with WAD files, a format popularized by classic PC games like DOOM. The primary goal was to create a userspace filesystem daemon that allows for both reading from and writing to WAD files, effectively treating them as a mounted directory structure. This implementation demonstrates how to manipulate and interact with the WAD format in a modern Linux environment, providing a practical example of filesystem operations.

# Features
WAD File Parsing: The system interprets WAD files, organizing their contents into a directory structure that mirrors the internal file descriptors and lumps of the WAD format.
Read/Write Access: Users can navigate through the mounted filesystem, read file contents, and create new directories and files. The system supports adding new files and directories while preserving the integrity of the WAD format.
Custom Filesystem Interface: Implemented using FUSE, the daemon runs in userspace, translating filesystem calls to operations on WAD files.

# WAD File Structure
WAD files are composed of a header, a descriptor list, and data lumps. The header provides basic metadata, while the descriptors specify the location and size of each lump. The lumps contain the actual data, such as game textures or level information.

# Filesystem Operations
The following core FUSE operations were implemented:

get_attr: Retrieve attributes of files and directories.
readdir: List the contents of a directory.
read: Read data from files.
mknod: Create new files.
mkdir: Create new directories.
write: Write data to newly created files.
Directory and File Creation
The system supports creating new directories and files within the mounted filesystem. These new entries are added directly to the WAD file, with the appropriate descriptors and lumps being managed to ensure consistency.

# Extensibility
While the current implementation focuses on basic filesystem operations, it is designed to be extensible, allowing for future enhancements such as more complex data manipulation within WAD files or support for additional file formats.

# Prerequisites
Linux Environment: The system requires a Linux environment with FUSE support.
FUSE Library: Ensure that the FUSE library is installed (libfuse-dev).
