# CMakeList.txt : CMake project for AlphaZeroPytorch, include source and define
# project specific logic here.
#
project(AlphaZero)

set (CMAKE_CXX_STANDARD 17)
cmake_minimum_required (VERSION 3.8)
set(CMAKE_BUILD_TYPE Debug)

message("searching for Pytorch ...")
find_package(Torch REQUIRED)

message("adding source files ...")
file(GLOB SOURCES_Files 
	"include/*.cpp" 
	"include/ai/*.cpp"
	"include/game/*.cpp"
	"include/Server/*.cpp"
)
file(GLOB HEADER_Files 
	"include/*.hpp"
	"include/ai/*.hpp"
	"include/game/*.hpp"
	"include/Server/*.hpp"
)

# Add source to this project's executable.
add_executable (AlphaZeroPytorch "AlphaZeroPytorch.cpp" "AlphaZeroPytorch.h" ${SOURCES_Files} ${HEADER_Files})

message("linking libs ...")
target_link_libraries(AlphaZeroPytorch "${TORCH_LIBRARIES}")

message("adding includes ...")

if (WIN32)
	target_include_directories(AlphaZeroPytorch PUBLIC "D:/MyCode/CppLibs/include")

	target_link_libraries (AlphaZeroPytorch "D:/MyCode/CppLibs/x64/sockpp-static_d.lib")
endif (WIN32)
if (UNIX)
	target_include_directories(AlphaZeroPytorch PUBLIC "/spdlog/spdlog/include")
	target_include_directories(AlphaZeroPytorch PUBLIC "/sockpp/include")
	find_library(sockpp_location sockpp)
	message("Lib1 is at: ${sockpp_location}")
	target_link_libraries(AlphaZeroPytorch "${sockpp_location}")
endif(UNIX)

target_include_directories(AlphaZeroPytorch PUBLIC "${CMAKE_SOURCE_DIR}/include")

message("done")

#message("building cuda2cpu")

#add_executable (cuda2cpu "toCPU.cpp")
#target_link_libraries(cuda2cpu "${TORCH_LIBRARIES}")
#target_include_directories(cuda2cpu PUBLIC "${CMAKE_SOURCE_DIR}/include")

#if (WIN32)
#	target_include_directories(cuda2cpu PUBLIC "D:/MyCode/CppLibs/include")
#endif (WIN32)
#if (UNIX)
#	target_include_directories(cuda2cpu PUBLIC "/spdlog/spdlog/include")
#endif(UNIX)

# TODO: Add tests and install targets if needed.
