cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokeOpenCV LANGUAGES CXX)

find_package(OpenCV 5.0 CONFIG REQUIRED COMPONENTS
   core imgproc geometry flann features video photo stereo ptcloud
   calib dnn objdetect imgcodecs videoio highgui
)

add_executable(opencv-consumer main.cpp)
set_property(TARGET opencv-consumer PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_compile_features(opencv-consumer PRIVATE cxx_std_23)
target_include_directories(opencv-consumer PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(opencv-consumer PRIVATE ${OpenCV_LIBS})
