cmake_minimum_required(VERSION 3.8)
project(turtlebot3_qt_controller)
 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)   # Génération automatique du code Meta-Object Qt
 
# Dépendances ROS 2
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
 
# Dépendances Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)
 
# yaml-cpp
find_package(yaml-cpp REQUIRED)
 
# Fichiers sources
set(SOURCES
  src/main.cpp
  src/controller_node.cpp
  src/main_window.cpp
)
 
# Headers avec Q_OBJECT (nécessaires pour MOC)
set(HEADERS
  include/turtlebot3_qt_controller/main_window.hpp
  include/turtlebot3_qt_controller/controller_node.hpp
)
 
add_executable(turtlebot3_qt_controller ${SOURCES} ${HEADERS})
 
target_include_directories(turtlebot3_qt_controller PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
 
ament_target_dependencies(turtlebot3_qt_controller
  rclcpp geometry_msgs sensor_msgs)
 
target_link_libraries(turtlebot3_qt_controller
  Qt6::Core Qt6::Widgets Qt6::Gui
  yaml-cpp)
 
install(TARGETS turtlebot3_qt_controller
  DESTINATION lib/${PROJECT_NAME})
 
install(DIRECTORY config/
  DESTINATION share/${PROJECT_NAME}/config)
 
ament_package()
