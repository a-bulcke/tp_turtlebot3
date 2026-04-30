#pragma once
 
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <sensor_msgs/msg/compressed_image.hpp>
#include <sensor_msgs/msg/battery_state.hpp>
#include <QObject>
#include <QImage>
#include <memory>
#include <map>
#include <string>
 
namespace tb3ctrl {
 
class ControllerNode : public QObject, public rclcpp::Node {
  Q_OBJECT
public:
  explicit ControllerNode(const std::string & node_name);
 
  // Appelées depuis la GUI Qt
  void publish_twist(double linear, double angular);
  void stop();
 
  // Getters pour topics (lus depuis YAML)
  void set_topics(const std::string & cmd, const std::string & cam,
                  const std::string & bat);
 
signals:
  // Signaux Qt émis depuis les callbacks ROS
  void new_image(QImage img);
  void battery_updated(float percentage);
 
private:
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_cmd_;
  rclcpp::Subscription<sensor_msgs::msg::CompressedImage>::SharedPtr sub_cam_;
  rclcpp::Subscription<sensor_msgs::msg::BatteryState>::SharedPtr sub_bat_;
 
  void on_image(const sensor_msgs::msg::CompressedImage::SharedPtr msg);
  void on_battery(const sensor_msgs::msg::BatteryState::SharedPtr msg);
};
 
}  // namespace tb3ctrl
