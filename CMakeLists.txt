cmake_minimum_required(VERSION 3.24)
project(blufizz)

# use C++23
set(CMAKE_CXX_STANDARD 23)

find_package(PkgConfig REQUIRED)

# external dependencies
# dbus c++
pkg_check_modules(sdbus-c++ IMPORTED_TARGET sdbus-c++)
# nlohmann-json3
pkg_check_modules(nlohmann_json IMPORTED_TARGET nlohmann_json)
# sqlite3
pkg_check_modules(sqlite3 IMPORTED_TARGET sqlite3)

set(SOURCE_FILES
    src/main.cpp
    src/Bluez_AdapterController.cpp
    src/Bluez_AdapterManager.cpp
    src/Bluez_DeviceClient.cpp
    src/Bluez_DeviceConnector.cpp
    src/Bluez_ObjectManager.cpp
    src/Database.cpp
    src/DevicesManager.cpp
    src/Device.cpp
    src/BluFizz.cpp
    src/TaskManager.cpp
    src/Tools.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# SET(GCC_INCLUDE_ORDER_FLAGS "-H")
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_INCLUDE_ORDER_FLAGS}")

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::nlohmann_json PkgConfig::sdbus-c++ PkgConfig::sqlite3)
