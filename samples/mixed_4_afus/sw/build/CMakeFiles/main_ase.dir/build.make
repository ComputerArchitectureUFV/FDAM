# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/lucas/Documentos/afu-manager/samples/mixed_4_afus/sw

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lucas/Documentos/afu-manager/samples/mixed_4_afus/sw/build

# Include any dependencies generated for this target.
include CMakeFiles/main_ase.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/main_ase.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/main_ase.dir/flags.make

CMakeFiles/main_ase.dir/main.cpp.o: CMakeFiles/main_ase.dir/flags.make
CMakeFiles/main_ase.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lucas/Documentos/afu-manager/samples/mixed_4_afus/sw/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/main_ase.dir/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main_ase.dir/main.cpp.o -c /home/lucas/Documentos/afu-manager/samples/mixed_4_afus/sw/main.cpp

CMakeFiles/main_ase.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main_ase.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lucas/Documentos/afu-manager/samples/mixed_4_afus/sw/main.cpp > CMakeFiles/main_ase.dir/main.cpp.i

CMakeFiles/main_ase.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main_ase.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lucas/Documentos/afu-manager/samples/mixed_4_afus/sw/main.cpp -o CMakeFiles/main_ase.dir/main.cpp.s

CMakeFiles/main_ase.dir/main.cpp.o.requires:

.PHONY : CMakeFiles/main_ase.dir/main.cpp.o.requires

CMakeFiles/main_ase.dir/main.cpp.o.provides: CMakeFiles/main_ase.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/main_ase.dir/build.make CMakeFiles/main_ase.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/main_ase.dir/main.cpp.o.provides

CMakeFiles/main_ase.dir/main.cpp.o.provides.build: CMakeFiles/main_ase.dir/main.cpp.o


# Object files for target main_ase
main_ase_OBJECTS = \
"CMakeFiles/main_ase.dir/main.cpp.o"

# External object files for target main_ase
main_ase_EXTERNAL_OBJECTS =

main_ase: CMakeFiles/main_ase.dir/main.cpp.o
main_ase: CMakeFiles/main_ase.dir/build.make
main_ase: ../../../../sw/AFUManagerLIB/libafumanager.a
main_ase: /usr/local/lib/libopae-c-ase.so
main_ase: /usr/lib64/libuuid.so
main_ase: /usr/local/lib/libMPF.so
main_ase: CMakeFiles/main_ase.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lucas/Documentos/afu-manager/samples/mixed_4_afus/sw/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable main_ase"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/main_ase.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/main_ase.dir/build: main_ase

.PHONY : CMakeFiles/main_ase.dir/build

CMakeFiles/main_ase.dir/requires: CMakeFiles/main_ase.dir/main.cpp.o.requires

.PHONY : CMakeFiles/main_ase.dir/requires

CMakeFiles/main_ase.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/main_ase.dir/cmake_clean.cmake
.PHONY : CMakeFiles/main_ase.dir/clean

CMakeFiles/main_ase.dir/depend:
	cd /home/lucas/Documentos/afu-manager/samples/mixed_4_afus/sw/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lucas/Documentos/afu-manager/samples/mixed_4_afus/sw /home/lucas/Documentos/afu-manager/samples/mixed_4_afus/sw /home/lucas/Documentos/afu-manager/samples/mixed_4_afus/sw/build /home/lucas/Documentos/afu-manager/samples/mixed_4_afus/sw/build /home/lucas/Documentos/afu-manager/samples/mixed_4_afus/sw/build/CMakeFiles/main_ase.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/main_ase.dir/depend

