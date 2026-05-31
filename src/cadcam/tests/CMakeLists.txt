set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Test REQUIRED)

add_executable(tst_cadcam tst_cadcam.cpp)
target_link_libraries(tst_cadcam PRIVATE cadcam Qt5::Test Qt5::Core Qt5::Gui)

add_test(NAME tst_cadcam COMMAND tst_cadcam)
