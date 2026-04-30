#include <rclcpp/rclcpp.hpp>
#include <QApplication>
#include <yaml-cpp/yaml.h>
#include <thread>
#include <string>
#include <ament_index_cpp/get_package_share_directory.hpp>
#include "turtlebot3_qt_controller/controller_node.hpp"
#include "turtlebot3_qt_controller/main_window.hpp"
 
int main(int argc, char ** argv)
{
  // ── 1. Initialisation ROS 2 ──
  rclcpp::init(argc, argv);
 
  // ── 2. Lecture du fichier YAML ──
  std::string pkg_share =
    ament_index_cpp::get_package_share_directory(
      "turtlebot3_qt_controller");
  std::string yaml_path = pkg_share + "/config/keybindings.yaml";
 
  YAML::Node cfg = YAML::LoadFile(yaml_path);
  auto kb = cfg["keybindings"];
  auto sp = cfg["speeds"];
  auto ro = cfg["ros"];
 
  tb3ctrl::KeyBindings bindings;
  bindings.forward      = kb["forward"].as<int>();
  bindings.backward     = kb["backward"].as<int>();
  bindings.turn_left    = kb["turn_left"].as<int>();
  bindings.turn_right   = kb["turn_right"].as<int>();
  bindings.stop         = kb["stop"].as<int>();
  bindings.arrow_up     = kb["arrow_up"].as<int>();
  bindings.arrow_down   = kb["arrow_down"].as<int>();
  bindings.arrow_left   = kb["arrow_left"].as<int>();
  bindings.arrow_right  = kb["arrow_right"].as<int>();
  bindings.linear_speed = sp["linear"].as<double>();
  bindings.angular_speed= sp["angular"].as<double>();
 
  std::string cmd_topic = ro["cmd_vel_topic"].as<std::string>();
  std::string cam_topic = ro["camera_topic"].as<std::string>();
  std::string bat_topic = ro["battery_topic"].as<std::string>();
 
  // ── 3. Création du nœud ROS 2 ──
  auto node = std::make_shared<tb3ctrl::ControllerNode>(
    "turtlebot3_qt_controller");
  node->set_topics(cmd_topic, cam_topic, bat_topic);
 
  // ── 4. Thread ROS 2 (spin séparé du thread Qt) ──
  std::thread ros_thread([&node](){
    rclcpp::spin(node);
  });
 
  // ── 5. Application Qt ──
  QApplication app(argc, argv);
 
  tb3ctrl::MainWindow win(node, bindings);
  win.show();
 
  int ret = app.exec();   // boucle Qt (thread principal)
 
  // ── 6. Arrêt propre ──
  node->stop();
  rclcpp::shutdown();
  ros_thread.join();
 
  return ret;
}
