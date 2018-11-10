add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/simple.pre.cpp
        COMMAND ../processor/processor ${CMAKE_CURRENT_SOURCE_DIR}/simple.cpp --outfile ${CMAKE_BINARY_DIR}/simple.pre.cpp
        DEPENDS ../processor/processor ${CMAKE_CURRENT_SOURCE_DIR}/simple.cpp
)

add_executable(simple ${CMAKE_BINARY_DIR}/simple.pre.cpp)
target_link_libraries(simple PRIVATE tdomp)