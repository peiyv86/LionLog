
add_executable(lion-collector collector/main.cpp)
add_executable(lion-query query/main.cpp)
add_executable(lion-write write/main.cpp)

target_link_libraries(lion-write PRIVATE lionlog)
target_link_libraries(lion-collector PRIVATE lionlog)

# if(TARGET zlibstatic)
#     target_link_libraries(lion-query PRIVATE lionlog zlibstatic)
# else()
#     target_link_libraries(lion-query PRIVATE lionlog zlib)
# endif()

target_link_libraries(lion-query ${ZLIB_LIBRARIES})


install(TARGETS lion-collector lion-query
    RUNTIME DESTINATION bin
)