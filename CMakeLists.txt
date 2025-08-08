cmake_minimum_required(VERSION 3.10)

project(RootAI VERSION 0.1.0 LANGUAGES CXX)

find_package(fmt 9.0.0 REQUIRED)

add_executable(RootAI
    main.cpp
    src/board_data.cpp
    src/card_data.cpp
    src/clearing_data.cpp
    src/forest_data.cpp
    src/card_pile.cpp
    src/deck_data.cpp
    src/discard_pile_data.cpp
    src/factions_data.cpp
    src/game_data.cpp
    src/token_data.cpp
)

target_include_directories(RootAI PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_BINARY_DIR}
)

target_compile_features(RootAI PRIVATE cxx_std_23)
target_compile_options(RootAI PRIVATE 
    -Wall 
    -Wextra 
    -Wpedantic
    -fno-exceptions
)

target_link_libraries(RootAI PRIVATE fmt::fmt)