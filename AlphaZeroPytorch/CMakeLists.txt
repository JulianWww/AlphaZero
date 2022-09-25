# CMakeList.txt : CMake project for AlphaZeroPytorch, include source and define
# project specific logic here.
#
project(AlphaZero)

set (CMAKE_CXX_STANDARD 17)
cmake_minimum_required (VERSION 3.8)
set(CMAKE_BUILD_TYPE Debug)

message(STATUS "adding source files ...")
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

find_library(spdlog_location spdlog "/" REQUIRED)
find_library(sockpp_location sockpp "/" REQUIRED)


# Add source to this project's executable.
add_executable (train "AlphaZeroPytorch.cpp" "AlphaZeroPytorch.h" ${SOURCES_Files} ${HEADER_Files})
add_executable (GameReplayer "Replay.cpp" "include/game/game.cpp" "include/config.cpp")
add_executable (runTest "test.cpp" ${TEST_SOURCE} ${SOURCES_Files})
add_executable (runServer "runServer.cpp" ${SOURCES_Files} ${SERVER_Files})
add_executable (convert "convertToJceFormat.cpp" "include/ai/modelWorker.cpp" "${OUTER_Files}")
add_executable (eloRaiting "doEloRaiting.cpp" ${SOURCES_Files} ${SERVER_Files})

target_compile_definitions(train PRIVATE cpuct_=2.0f)
target_compile_definitions(GameReplayer PRIVATE cpuct_=2.0f)
target_compile_definitions(runTest PRIVATE cpuct_=2.0f)
target_compile_definitions(runServer PRIVATE cpuct_=1.0f)
target_compile_definitions(convert PRIVATE cpuct_=2.0f)
target_compile_definitions(eloRaiting PRIVATE cpuct_=2.0f)


# Settting model device macro
find_package(CUDA 11.3)
message(STATUS "Using cuda: " ${CUDA_VERSION_STRING})


if (CUDA_VERSION_STRING EQUAL "")
  target_compile_definitions(train PUBLIC DEVICES="cpu")
  target_compile_definitions(GameReplayer PUBLIC DEVICES="cpu")
  target_compile_definitions(runTest PUBLIC DEVICES="cpu")
  target_compile_definitions(runServer PUBLIC DEVICES="cpu")
  target_compile_definitions(convert PUBLIC DEVICES="cpu")
  target_compile_definitions(eloRaiting PUBLIC DEVICES="cpu")
else()
  target_compile_definitions(train PUBLIC DEVICES="cuda:0")
  target_compile_definitions(GameReplayer PUBLIC DEVICES="cuda:0")
  target_compile_definitions(runTest PUBLIC DEVICES="cuda:0")
  target_compile_definitions(runServer PUBLIC DEVICES="cuda:0")
  target_compile_definitions(convert PUBLIC DEVICES="cuda:0")
  target_compile_definitions(eloRaiting PUBLIC DEVICES="cuda:0")
endif()

# Download libtorch from offical website
if (EXISTS "${PROJECT_SOURCE_DIR}/dependencies/libtorch.zip" OR EXISTS "${PROJECT_SOURCE_DIR}/dependencies/libtorch")
  message(STATUS "libtorch already downloaded")
else ()
  if (CUDA_VERSION_STRING EQUAL "")
	  message(STATUS "did not find cuda11.3 +")
    message(STATUS "downloading libtorch cpu (this may take a while, go have a coffe)")
    file(
      DOWNLOAD
      "https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-1.11.0%2Bcpu.zip"
      "${PROJECT_SOURCE_DIR}/dependencies/libtorch.zip"
    )
    set(libtorch_hash "5b5ea3067f878dea051f6fcd8fb00338f55517cb6baecdc810983a814e030845")
  else ()
    message(STATUS "found cuda version " ${CUDA_VERSION_STRING})
    message(STATUS "downloading libtorch cuda " ${CUDA_VERSION_STRING} "  (this may take a while, go have a coffe)")
    file(
      DOWNLOAD
      "https://download.pytorch.org/libtorch/cu113/libtorch-cxx11-abi-shared-with-deps-1.11.0%2Bcu113.zip"
      "${PROJECT_SOURCE_DIR}/dependencies/libtorch.zip"
    )
    set(libtorch_hash "8d9e829ce9478db4f35bdb7943308cf02e8a2f58cf9bb10f742462c1d57bf287")
  endif()
  file(SHA256 "${PROJECT_SOURCE_DIR}/dependencies/libtorch.zip" libtorch_checksum)
  message(STATUS "check sum" ${libtorch_checksum})
  if (libtorch_checksum MATCHES "${libtorch_hash}")
    message(STATUS "libtorch checksum is valid")
  else()
    message(FATAL_ERROR "libtorch checksum is not valid")
  endif()
endif()

 #unzip libtorch
if (EXISTS "${PROJECT_SOURCE_DIR}/dependencies/libtorch")
  message(STATUS "libtorch already installed")
else()
  message(STATUS "installing libtorch")
  file(ARCHIVE_EXTRACT INPUT "${PROJECT_SOURCE_DIR}/dependencies/libtorch.zip" DESTINATION "${PROJECT_SOURCE_DIR}/dependencies")
  file(REMOVE "${PROJECT_SOURCE_DIR}/dependencies/libtorch.zip")
  message(STATUS "libtorch installed")
endif()
message(STATUS "libtorch Path:\t" "${PROJECT_SOURCE_DIR}/dependencies/libtorch")
set (CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/dependencies/libtorch/")


message(STATUS "searching for Pytorch ...")
find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")


message(STATUS "linking libs")

message(STATUS "sockpp is at: ${sockpp_location} ${spdlog_location}")
target_link_libraries(train        PRIVATE "${TORCH_LIBRARIES}" ${spdlog_location})
target_link_libraries(runServer    PRIVATE "${TORCH_LIBRARIES}" ${spdlog_location} ${sockpp_location})
target_link_libraries(eloRaiting   PRIVATE "${TORCH_LIBRARIES}" ${spdlog_location} ${sockpp_location})
target_link_libraries(GameReplayer PRIVATE "${TORCH_LIBRARIES}" ${spdlog_location})
target_link_libraries(convert      PRIVATE "${TORCH_LIBRARIES}" ${spdlog_location})
target_link_libraries(runTest	   PRIVATE "${TORCH_LIBRARIES}" ${spdlog_location})


target_include_directories(train PUBLIC "include/")
target_include_directories(runServer PUBLIC "include/")
target_include_directories(GameReplayer PUBLIC "include/")
target_include_directories(runTest PUBLIC "include/")
target_include_directories(convert PUBLIC "include/")
target_include_directories(eloRaiting PUBLIC "include/")


message(STATUS "done")

