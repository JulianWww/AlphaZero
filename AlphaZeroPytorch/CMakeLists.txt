# CMakeList.txt : CMake project for AlphaZeroPytorch, include source and define
# project specific logic here.
#
set (CMAKE_CXX_STANDARD 17)
cmake_minimum_required (VERSION 3.8)

message("searching for Pytorch ...")
find_package(Torch REQUIRED)

message("adding source files ...")
file(GLOB SOURCES_Files 
	"include/*.cpp" 
	"include/ai/*.cpp"
	"include/game/*.cpp"
)
file(GLOB HEADER_Files 
	"include/*.hpp"
	"include/ai/*.hpp"
	"include/game/*.hpp")

# Add source to this project's executable.
add_executable (AlphaZeroPytorch "AlphaZeroPytorch.cpp" "AlphaZeroPytorch.h" ${SOURCES_Files} ${HEADER_Files})

message("linking libs ...")
target_link_libraries(AlphaZeroPytorch "${TORCH_LIBRARIES}")

message("adding includes ...")
target_include_directories(AlphaZeroPytorch PUBLIC "/spdlog/spdlog/include")

target_include_directories(AlphaZeroPytorch PUBLIC "${CMAKE_SOURCE_DIR}/include")

message("done")

# TODO: Add tests and install targets if needed.
