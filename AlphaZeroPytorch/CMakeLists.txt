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
)
file(GLOB HEADER_Files 
	"include/*.hpp"
	"include/ai/*.hpp"
	"include/game/*.hpp"
)
file(GLOB SERVER_Files
	"include/Server/*.cpp"
)

file(GLOB OUTER_Files
	"include/*.cpp"
)

file(GLOB GAME_Files
	"include/game/*.cpp"
)

file(GLOB TEST_SOURCE
	"include/test/*.cpp"
	"include/ai/*.cpp"
	"include/game/*.cpp"
	"include/*.cpp"
)


# Add source to this project's executable.
add_executable (train "AlphaZeroPytorch.cpp" "AlphaZeroPytorch.h" ${SOURCES_Files} ${HEADER_Files})
add_executable (GameReplayer "Replay.cpp" "include/game/game.cpp" "include/config.cpp")
add_executable (runTest "test.cpp" ${TEST_SOURCE} ${SOURCES_Files})
add_executable (runServer "runServer.cpp" ${SOURCES_Files} ${SERVER_Files})
add_executable (convert "convertToJceFormat.cpp" "include/ai/modelWorker.cpp" "${OUTER_Files}")
add_executable (eloRaiting "doEloRaiting.cpp" ${SOURCES_Files} ${SERVER_Files})

message("linking libs ...")
target_link_libraries(train "${TORCH_LIBRARIES}")
target_link_libraries(GameReplayer "${TORCH_LIBRARIES}")
target_link_libraries(runTest "${TORCH_LIBRARIES}")
target_link_libraries(runServer "${TORCH_LIBRARIES}")
target_link_libraries(convert "${TORCH_LIBRARIES}")
target_link_libraries(eloRaiting "${TORCH_LIBRARIES}")

message("adding includes ...")

if (WIN32)
	add_executable(showLoss "showLoss.cpp" "include/log.cpp")
	target_include_directories(showLoss PUBLIC "${CMAKE_SOURCE_DIR}/include")
	target_include_directories(showLoss PUBLIC "C:/Users/Julia/AppData/Local/Programs/Python/Python39/include")
	target_link_libraries(showLoss "C:/Users/Julia/AppData/Local/Programs/Python/Python39/libs/python39_d.lib")
	target_link_libraries(showLoss "${TORCH_LIBRARIES}")


	target_include_directories(train PUBLIC "D:/MyCode/CppLibs/include")
	target_include_directories(convert PUBLIC "D:/MyCode/CppLibs/include")
	target_include_directories(GameReplayer PUBLIC "D:/MyCode/CppLibs/include")
	target_include_directories(runTest PUBLIC "D:/MyCode/CppLibs/include")
	target_include_directories(showLoss PUBLIC "D:/MyCode/CppLibs/include")
	target_include_directories(runServer PUBLIC "D:/MyCode/CppLibs/include")
	target_include_directories(eloRaiting PUBLIC "D:/MyCode/CppLibs/include")

	target_link_libraries (runServer "lib/sockpp-static_d")
	target_link_libraries (eloRaiting "lib/sockpp-static_d")
endif (WIN32)
if (UNIX)
	target_include_directories(train PUBLIC "/cpp/spdlog/include")

	target_include_directories(runServer PUBLIC "/cpp/spdlog/include")
	target_include_directories(runServer PUBLIC "/cpp/sockpp/include")

	target_include_directories(eloRaiting PUBLIC "/cpp/spdlog/include")
	target_include_directories(eloRaiting PUBLIC "/cpp/sockpp/include")

	target_include_directories(convert PUBLIC "/cpp/spdlog/include")


	target_include_directories(GameReplayer PUBLIC "/cpp/spdlog/include")

	target_include_directories(runTest PUBLIC "/cpp/spdlog/include")

	find_library(sockpp_location sockpp)
	message("sockpp is at: ${sockpp_location}")
	target_link_libraries(train "${sockpp_location}")
	target_link_libraries(runServer "${sockpp_location}")
	target_link_libraries(eloRaiting "${sockpp_location}")
endif(UNIX)


target_include_directories(train PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_include_directories(runServer PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_include_directories(GameReplayer PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_include_directories(runTest PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_include_directories(convert PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_include_directories(eloRaiting PUBLIC "${CMAKE_SOURCE_DIR}/include")


message("done")

