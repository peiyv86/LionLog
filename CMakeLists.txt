cmake_minimum_required(VERSION 3.14)
project(LionLog LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)

include(FetchContent)

message(STATUS "Fetching ZLIB...")

# FetchContent_Declare(
#     zlib
#     URL https://github.com/madler/zlib/archive/refs/tags/v1.3.1.tar.gz
# )

# set(ZLIB_BUILD_EXAMPLES OFF CACHE INTERNAL "")
# set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
# FetchContent_MakeAvailable(zlib)
find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})

# FetchContent_Declare(
#     googletest
#     URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
# )
# FetchContent_MakeAvailable(googletest)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

find_package(Threads REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/sinks)
include_directories(${PROJECT_SOURCE_DIR}/include/tool)

add_library(threadpool STATIC src/ThreadPool.cpp)
target_link_libraries(threadpool PRIVATE Threads::Threads)

add_library(lionlog STATIC 
    src/Logger.cpp
    src/FileSink.cpp 
    src/ConsoleSink.cpp
    src/RollingFileSink.cpp
    src/LogFormatter.cpp
    src/GzipUtil.cpp
)

target_link_libraries(lionlog 
    PUBLIC  threadpool 
)
target_link_libraries(lionlog PRIVATE ${ZLIB_LIBRARIES})
# if(TARGET zlibstatic)
#     target_link_libraries(lionlog PRIVATE zlibstatic)
# else()
#     target_link_libraries(lionlog PRIVATE zlib)
# endif()

add_subdirectory(apps)

enable_testing()

add_executable(test_ tests/test.cpp)
# target_link_libraries(test_ PRIVATE lionlog GTest::gtest_main)

target_link_libraries(test_ 
    PRIVATE 
    lionlog 
    GTest::GTest      # 核心庫
    GTest::Main       # 自動提供 main() 函數，如果不選這個則需自寫 main
)

include(GoogleTest)
gtest_discover_tests(test_)




#####################3#####################3#####################3
# --------------------------------------------------------------
# 5. 配置安装规则 (Install Rules)
# --------------------------------------------------------------

# A. 安装核心库 (liblionlog.a) 到 /usr/local/lib
install(TARGETS lionlog 
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# B. 安装头文件 到 /usr/local/include/lionlog
# 这样用户在代码里就可以写 #include <lionlog/Logger.h>
install(DIRECTORY include/ 
    DESTINATION include/lionlog
    FILES_MATCHING PATTERN "*.h"
)

# --------------------------------------------------------------
# 6. 配置打包工具 (CPack)
# --------------------------------------------------------------
# 设置包的元数据
set(CPACK_PACKAGE_NAME "lionlog-system")
set(CPACK_PACKAGE_VENDOR "Peiyv")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "High performance asynchronous logging system")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_CONTACT "your_email@example.com")

# 设置打包类型
# DEB: 适用于 Ubuntu/Debian (最常用)
# TGZ: 通用的 .tar.gz 压缩包
# RPM: 适用于 CentOS/RedHat (可选)
set(CPACK_GENERATOR "DEB;TGZ") 

# Debian 包的特定配置
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Peiyv")
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON) # 自动分析依赖

# 这一行必须放在最后！
include(CPack)