add_executable(driver ./src/driver.cpp)
target_include_directories(driver
                           PRIVATE ${INCLUDE_DIR})

install(TARGETS driver
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
