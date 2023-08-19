add_executable(driver ./src/driver.cpp)
add_executable(generator ./src/generator.cpp)
add_executable(ans_generator ./src/ans_generator.cpp)

target_include_directories(driver
                           PRIVATE ${INCLUDE_DIR}
                           PRIVATE ./include)

target_include_directories(generator
                           PRIVATE ./include)

target_include_directories(ans_generator
                           PRIVATE ./include)

install(TARGETS driver generator ans_generator
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
