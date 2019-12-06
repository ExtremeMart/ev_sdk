# Add library file
#add_library(darknet0 SHARED IMPORTED GLOBAL)
#set_property(TARGET darknet0 PROPERTY IMPORTED_LOCATION "/usr/local/ev_sdk/darknet/lib/libdarknet.so")
#set_target_properties(darknet0 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include")
#
#install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/lib/libdarknet.so DESTINATION lib)
#install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/darknet.h DESTINATION include)