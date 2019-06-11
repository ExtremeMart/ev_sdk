project(ji)

cmake_minimum_required(VERSION 2.8)
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
    message(STATUS "optional:-std=c++11")   
endif(CMAKE_COMPILER_IS_GNUCXX)

set(LIBRARY_OUTPUT_PATH /usr/local/ev_sdk/lib)
set(SOURCE_DIR /usr/local/ev_sdk)

find_package(OpenCV REQUIRED)

include_directories(
    ${OpenCV_INCLUDE_DIR}	
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${SOURCE_DIR}/include
    ${SOURCE_DIR}/3rd/cJSON
    ${SOURCE_DIR}/3rd/license/v10/include
    ${SOURCE_DIR}/3rd/boost_interface
    ${SOURCE_DIR}/3rd/encrypted_module-master/include
)

link_directories(
    ${SOURCE_DIR}/3rd/license/v10/lib
    ${SOURCE_DIR}/3rd/encrypted_module-master/lib
)

list(APPEND SRCS
    ${SOURCE_DIR}/3rd/cJSON/cJSON.c
    ${SOURCE_DIR}/3rd/boost_interface/BoostInterface.cpp
    ${SOURCE_DIR}/src/ji.cpp
)

add_library (${PROJECT_NAME} SHARED ${SRCS})
target_link_libraries(${PROJECT_NAME}
    ${OpenCV_LIBS}
    ji_license
    encrypt_wrapper
    crypto
    curl
)

