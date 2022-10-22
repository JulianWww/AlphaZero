project(AlphaZeroConsoleClient)

set (CMAKE_CXX_STANDARD 23)
cmake_minimum_required (VERSION 3.8)

file(GLOB Files
	"scr/*"
    "inlcude/*"
)

find_library(sockpp_location sockpp "/" REQUIRED)

add_executable (ConsoleClient "ConsoleClient.cpp" ${Files})
target_link_libraries(ConsoleClient PRIVATE ${sockpp_location})
target_include_directories(ConsoleClient PUBLIC "include/")