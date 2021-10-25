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

file(GLOB TEST_SOURCE
	"include/test/*.cpp"
	"include/ai/*.cpp"
	"include/game/*.cpp"
	"include/*.cpp"
)

# Add source to this project's executable.
add_executable (train "AlphaZeroPytorch.cpp" "AlphaZeroPytorch.h" ${SOURCES_Files} ${HEADER_Files})
add_executable (GameReplayer "Replay.cpp" "include/game/game.cpp")
add_executable (runTest "test.cpp" ${TEST_SOURCE} ${HEADER_Files})

message("linking libs ...")
target_link_libraries(train "${TORCH_LIBRARIES}")
target_link_libraries(GameReplayer "${TORCH_LIBRARIES}")
target_link_libraries(runTest "${TORCH_LIBRARIES}")

message("adding includes ...")

if (WIN32)
	target_include_directories(train PUBLIC "D:/MyCode/CppLibs/include")
	target_include_directories(GameReplayer PUBLIC "D:/MyCode/CppLibs/include")
	target_include_directories(runTest PUBLIC "D:/MyCode/CppLibs/include")

	target_link_libraries (train "D:/MyCode/CppLibs/x64/sockpp-static_d.lib")
endif (WIN32)
if (UNIX)
	target_include_directories(train PUBLIC "/spdlog/spdlog/include")
	target_include_directories(train PUBLIC "/sockpp/include")

	target_include_directories(GameReplayer PUBLIC "/spdlog/spdlog/include")

	target_include_directories(runTest PUBLIC "/spdlog/spdlog/include")

	find_library(sockpp_location sockpp)
	message("sockpp is at: ${sockpp_location}")
	target_link_libraries(train "${sockpp_location}")
endif(UNIX)


target_include_directories(train PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_include_directories(GameReplayer PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_include_directories(runTest PUBLIC "${CMAKE_SOURCE_DIR}/include")


message("done")

