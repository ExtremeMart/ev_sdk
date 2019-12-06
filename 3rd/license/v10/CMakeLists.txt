# license tools
install(
        PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/tools/clear.sh
        ${CMAKE_CURRENT_SOURCE_DIR}/tools/ev_codec
        ${CMAKE_CURRENT_SOURCE_DIR}/tools/ev_license
        ${CMAKE_CURRENT_SOURCE_DIR}/tools/oneKeyTest.sh
        ${CMAKE_CURRENT_SOURCE_DIR}/tools/oneKeyTest_n.sh DESTINATION bin/license-tools/v10/
)