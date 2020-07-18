cmake_minimum_required(VERSION 3.5)

project(bayan)

set(BOOST_ROOT "/home/max/Загрузки/boost_1_73_0")
set(BOOST_INCLUDE_DIRS "/home/max/Загрузки/boost_1_73_0")
set(BOOST_LIBRARIES "/home/max/Загрузки/boost_1_73_0/stage/lib")
set(Boost_NO_SYSTEM_PATHS ON)

find_package(Boost COMPONENTS program_options filesystem REQUIRED)

# message("Boost include dirs: ${BOOST_INCLUDE_DIRS}")
# message("Boost libs: ${BOOST_LIBRARIES}")

add_executable(boost commandhandler.h commandhandler.cpp hash.h hash.cpp main.cpp)

set_target_properties(boost PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    COMPILE_OPTIONS "-Wall;-Wextra;-Wpedantic"
#    INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
    INCLUDE_DIRECTORIES ${BOOST_ROOT}
)

target_link_libraries(boost
    ${Boost_LIBRARIES}
)