# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /homes/aap221/Desktop/bitmex-book-builder/xbtusd_builder

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /homes/aap221/Desktop/bitmex-book-builder/xbtusd_builder/build

# Include any dependencies generated for this target.
include CMakeFiles/xbtusd-builder.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/xbtusd-builder.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/xbtusd-builder.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/xbtusd-builder.dir/flags.make

CMakeFiles/xbtusd-builder.dir/xbtusd_builder.cpp.o: CMakeFiles/xbtusd-builder.dir/flags.make
CMakeFiles/xbtusd-builder.dir/xbtusd_builder.cpp.o: ../xbtusd_builder.cpp
CMakeFiles/xbtusd-builder.dir/xbtusd_builder.cpp.o: CMakeFiles/xbtusd-builder.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/homes/aap221/Desktop/bitmex-book-builder/xbtusd_builder/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/xbtusd-builder.dir/xbtusd_builder.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/xbtusd-builder.dir/xbtusd_builder.cpp.o -MF CMakeFiles/xbtusd-builder.dir/xbtusd_builder.cpp.o.d -o CMakeFiles/xbtusd-builder.dir/xbtusd_builder.cpp.o -c /homes/aap221/Desktop/bitmex-book-builder/xbtusd_builder/xbtusd_builder.cpp

CMakeFiles/xbtusd-builder.dir/xbtusd_builder.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/xbtusd-builder.dir/xbtusd_builder.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /homes/aap221/Desktop/bitmex-book-builder/xbtusd_builder/xbtusd_builder.cpp > CMakeFiles/xbtusd-builder.dir/xbtusd_builder.cpp.i

CMakeFiles/xbtusd-builder.dir/xbtusd_builder.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/xbtusd-builder.dir/xbtusd_builder.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /homes/aap221/Desktop/bitmex-book-builder/xbtusd_builder/xbtusd_builder.cpp -o CMakeFiles/xbtusd-builder.dir/xbtusd_builder.cpp.s

CMakeFiles/xbtusd-builder.dir/OrderBook/OrderBook.cpp.o: CMakeFiles/xbtusd-builder.dir/flags.make
CMakeFiles/xbtusd-builder.dir/OrderBook/OrderBook.cpp.o: ../OrderBook/OrderBook.cpp
CMakeFiles/xbtusd-builder.dir/OrderBook/OrderBook.cpp.o: CMakeFiles/xbtusd-builder.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/homes/aap221/Desktop/bitmex-book-builder/xbtusd_builder/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/xbtusd-builder.dir/OrderBook/OrderBook.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/xbtusd-builder.dir/OrderBook/OrderBook.cpp.o -MF CMakeFiles/xbtusd-builder.dir/OrderBook/OrderBook.cpp.o.d -o CMakeFiles/xbtusd-builder.dir/OrderBook/OrderBook.cpp.o -c /homes/aap221/Desktop/bitmex-book-builder/xbtusd_builder/OrderBook/OrderBook.cpp

CMakeFiles/xbtusd-builder.dir/OrderBook/OrderBook.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/xbtusd-builder.dir/OrderBook/OrderBook.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /homes/aap221/Desktop/bitmex-book-builder/xbtusd_builder/OrderBook/OrderBook.cpp > CMakeFiles/xbtusd-builder.dir/OrderBook/OrderBook.cpp.i

CMakeFiles/xbtusd-builder.dir/OrderBook/OrderBook.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/xbtusd-builder.dir/OrderBook/OrderBook.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /homes/aap221/Desktop/bitmex-book-builder/xbtusd_builder/OrderBook/OrderBook.cpp -o CMakeFiles/xbtusd-builder.dir/OrderBook/OrderBook.cpp.s

CMakeFiles/xbtusd-builder.dir/ThroughputMonitor/ThroughputMonitor.cpp.o: CMakeFiles/xbtusd-builder.dir/flags.make
CMakeFiles/xbtusd-builder.dir/ThroughputMonitor/ThroughputMonitor.cpp.o: ../ThroughputMonitor/ThroughputMonitor.cpp
CMakeFiles/xbtusd-builder.dir/ThroughputMonitor/ThroughputMonitor.cpp.o: CMakeFiles/xbtusd-builder.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/homes/aap221/Desktop/bitmex-book-builder/xbtusd_builder/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/xbtusd-builder.dir/ThroughputMonitor/ThroughputMonitor.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/xbtusd-builder.dir/ThroughputMonitor/ThroughputMonitor.cpp.o -MF CMakeFiles/xbtusd-builder.dir/ThroughputMonitor/ThroughputMonitor.cpp.o.d -o CMakeFiles/xbtusd-builder.dir/ThroughputMonitor/ThroughputMonitor.cpp.o -c /homes/aap221/Desktop/bitmex-book-builder/xbtusd_builder/ThroughputMonitor/ThroughputMonitor.cpp

CMakeFiles/xbtusd-builder.dir/ThroughputMonitor/ThroughputMonitor.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/xbtusd-builder.dir/ThroughputMonitor/ThroughputMonitor.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /homes/aap221/Desktop/bitmex-book-builder/xbtusd_builder/ThroughputMonitor/ThroughputMonitor.cpp > CMakeFiles/xbtusd-builder.dir/ThroughputMonitor/ThroughputMonitor.cpp.i

CMakeFiles/xbtusd-builder.dir/ThroughputMonitor/ThroughputMonitor.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/xbtusd-builder.dir/ThroughputMonitor/ThroughputMonitor.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /homes/aap221/Desktop/bitmex-book-builder/xbtusd_builder/ThroughputMonitor/ThroughputMonitor.cpp -o CMakeFiles/xbtusd-builder.dir/ThroughputMonitor/ThroughputMonitor.cpp.s

# Object files for target xbtusd-builder
xbtusd__builder_OBJECTS = \
"CMakeFiles/xbtusd-builder.dir/xbtusd_builder.cpp.o" \
"CMakeFiles/xbtusd-builder.dir/OrderBook/OrderBook.cpp.o" \
"CMakeFiles/xbtusd-builder.dir/ThroughputMonitor/ThroughputMonitor.cpp.o"

# External object files for target xbtusd-builder
xbtusd__builder_EXTERNAL_OBJECTS =

xbtusd-builder: CMakeFiles/xbtusd-builder.dir/xbtusd_builder.cpp.o
xbtusd-builder: CMakeFiles/xbtusd-builder.dir/OrderBook/OrderBook.cpp.o
xbtusd-builder: CMakeFiles/xbtusd-builder.dir/ThroughputMonitor/ThroughputMonitor.cpp.o
xbtusd-builder: CMakeFiles/xbtusd-builder.dir/build.make
xbtusd-builder: CMakeFiles/xbtusd-builder.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/homes/aap221/Desktop/bitmex-book-builder/xbtusd_builder/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable xbtusd-builder"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/xbtusd-builder.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/xbtusd-builder.dir/build: xbtusd-builder
.PHONY : CMakeFiles/xbtusd-builder.dir/build

CMakeFiles/xbtusd-builder.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/xbtusd-builder.dir/cmake_clean.cmake
.PHONY : CMakeFiles/xbtusd-builder.dir/clean

CMakeFiles/xbtusd-builder.dir/depend:
	cd /homes/aap221/Desktop/bitmex-book-builder/xbtusd_builder/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /homes/aap221/Desktop/bitmex-book-builder/xbtusd_builder /homes/aap221/Desktop/bitmex-book-builder/xbtusd_builder /homes/aap221/Desktop/bitmex-book-builder/xbtusd_builder/build /homes/aap221/Desktop/bitmex-book-builder/xbtusd_builder/build /homes/aap221/Desktop/bitmex-book-builder/xbtusd_builder/build/CMakeFiles/xbtusd-builder.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/xbtusd-builder.dir/depend
