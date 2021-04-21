project(card_recognize)
cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall)

# -DCMAKE_BUILD_TYPE=Debug
if (${CMAKE_BUILD_TYPE} MATCHES Debug)
  add_definitions(-DDEBUG)
  find_package(OpenCV REQUIRED)
  include_directories(${OpenCV_INCLUDE_DIRS})
  link_directories(${OpenCV_LIBRARY_DIRS})
else()
  # TODO: Modify this!
  set(OPENCV_PATH /home/chenx/devp/repo/opencv)
  include_directories(${OPENCV_PATH}/include)
  include_directories(${OPENCV_PATH}/build_wasm)
  file(GLOB opencv_include_modules "${OPENCV_PATH}/modules/*/include")
  include_directories(${opencv_include_modules})
  file(GLOB opencv_core "${OPENCV_PATH}/build_wasm/lib/*.a")
endif()

include_directories(./include)
include_directories(./third_party/jsonxx/include)
aux_source_directory(./src SRC)
add_executable(${PROJECT_NAME} ${SRC})
if (${CMAKE_BUILD_TYPE} MATCHES Debug)
  target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
else()
  target_link_libraries(${PROJECT_NAME} ${opencv_core})
  set_target_properties(${PROJECT_NAME}
    PROPERTIES
    LINK_FLAGS
    "${target_properties} \
    --bind \
    -s DEMANGLE_SUPPORT=1 \
    -s USE_ZLIB=1 \
    -s WASM=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s NO_FILESYSTEM=1 \
    -flto \
    -O2"
  )
endif()
