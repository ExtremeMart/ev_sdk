add_library(cjson SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/cJSON.c)
target_include_directories(cjson PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS cjson DESTINATION lib)