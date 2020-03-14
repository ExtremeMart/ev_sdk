set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-fext-numeric-literals ${CMAKE_CXX_FLAGS}")
find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED)

LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF("${isSystemDir}" STREQUAL "-1")

# libWKTParser.so
add_library(WKTParser SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/WKTParser.cpp)
target_link_libraries(WKTParser
        ${OpenCV_LIBS}
        boost_system
        boost_filesystem
        boost_thread
        boost_coroutine
        boost_context
        boost_regex
)
target_include_directories(WKTParser
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE ${OpenCV_INCLUDE_DIR}
        PRIVATE ${Boost_INCLUDE_DIRS}
)
target_compile_options(WKTParser
        PRIVATE -std=c++11 -Wall -O3 -fPIC)

# Install libWKTParser.so
install(TARGETS WKTParser DESTINATION lib)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    # testWkt
    add_executable(testWktParser ${CMAKE_CURRENT_SOURCE_DIR}/src/test-wkt-parser.cpp)
    target_link_libraries(testWktParser
            ${OpenCV_LIBS}
            WKTParser
    )
    target_include_directories(testWktParser
            PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    install(TARGETS testWktParser DESTINATION bin)
endif ()