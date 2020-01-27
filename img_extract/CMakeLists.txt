cmake_minimum_required (VERSION 3.8)
project(sudoku_image_extract)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

set (SRC
"image_extract.cpp" "image_extract.h"
"classifier/classifier.h" "classifier/classifier.cpp"
)

# Add source to this project's executable.
add_executable (sudoku_image_extract ${SRC})

target_include_directories(sudoku_image_extract PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(sudoku_image_extract PRIVATE ${OpenCV_LIBS})
target_compile_definitions(sudoku_image_extract PUBLIC "RES_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/res/\"")
set_target_properties(sudoku_image_extract PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
# TODO: Add tests and install targets if needed.
