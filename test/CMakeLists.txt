find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

set(EXECUTABLE_OUTPUT_PATH bin)
add_executable(Test-Skipping-List test.cpp)

target_link_libraries(Test-Skipping-List PRIVATE
    SkippingList 
    GTest::gtest
    Threads::Threads    
)
