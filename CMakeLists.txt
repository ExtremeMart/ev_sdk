cmake_minimum_required(VERSION 3.5)
project(ji)
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
    message(STATUS "optional:-std=c++11")
endif(CMAKE_COMPILER_IS_GNUCXX)
set(CMAKE_INSTALL_PREFIX "/usr/local/ev_sdk")
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# 使用pkg-config查找必要的依赖包
find_package(OpenCV REQUIRED)
find_library(glog REQUIRED)
set(ENV{PKG_CONFIG_PATH} "/usr/local/ev_sdk/3rd/ev_encrypt_module/lib/pkgconfig:/usr/local/ev_sdk/3rd/license/v20_0/lib/pkgconfig:/usr/local/cuda/pkgconfig/:ENV{PKG_CONFIG_PATH}")
find_package(PkgConfig REQUIRED)
pkg_search_module(EV_ENCRYPT_MODULE REQUIRED ev_encrypt_module)
pkg_search_module(JI_LICENSE REQUIRED ji_license)
pkg_search_module(CUDA REQUIRED cuda-10.0)
message(${CUDA_CFLAGS})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rd/cJSON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rd/wkt_parser)

# 编译libji.so
list(APPEND libji_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ji.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SampleDetector.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ji_utils.cpp)
add_library (${PROJECT_NAME} SHARED ${libji_SOURCES})
target_include_directories(${PROJECT_NAME}
        PUBLIC ${OpenCV_INCLUDE_DIR}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
        PUBLIC ${JI_LICENSE_INCLUDE_DIRS}
        PUBLIC ${EV_ENCRYPT_MODULE_INCLUDE_DIRS}
        PUBLIC "/usr/local/ev_sdk/3rd/darknet/include"
        PUBLIC ${CUDA_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}
        ${OpenCV_LIBS}
        glog
        ${JI_LICENSE_LDFLAGS}
        ${EV_ENCRYPT_MODULE_LDFLAGS}
        cjson
        WKTParser
        "/usr/local/ev_sdk/3rd/darknet/lib/libdarknet.so"
)

# 安装SampleDetector所依赖的libdarknet.so, 请在此处将自己依赖的库安装到/usr/local/ev_sdk/lib下
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/3rd/darknet/lib/libdarknet.so DESTINATION lib)



# 以下是EV_SDK所依赖和需要测试接口的程序&库，请勿修改！！！
# --- BEGIN EV_SDK DEPENDENCIES ---
# 编译 JI API 测试程序
add_executable(test-ji-api ${CMAKE_CURRENT_SOURCE_DIR}/test/test.cpp)
target_link_libraries(test-ji-api
        ${OpenCV_LIBS}
        cjson
        glog
        ${PROJECT_NAME}
)
# Install libji_license_impl.so
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/3rd/license/v20_0/lib/libji_license_impl.so DESTINATION lib)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/3rd/license/v20_0/lib/license_lib.sig DESTINATION lib)
# Install libev_encrypt_module
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/3rd/ev_encrypt_module/lib/libev_encrypt_module.so DESTINATION lib)
# Install libji.so
install(TARGETS ${PROJECT_NAME} DESTINATION lib)
# Install test-ji-api
install(TARGETS test-ji-api DESTINATION bin)
# ---  END EV_SDK DEPENDENCIES  ---