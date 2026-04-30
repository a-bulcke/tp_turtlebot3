#include "turtlebot3_qt_controller/controller_node.hpp"
#include <QImage>
 
namespace tb3ctrl {
 
ControllerNode::ControllerNode(const std::string & node_name)
: QObject(nullptr), rclcpp::Node(node_name) {}
 
void ControllerNode::set_topics(
  const std::string & cmd,
  const std::string & cam,
  const std::string & bat)
{
  pub_cmd_ = create_publisher<geometry_msgs::msg::Twist>(cmd, 10);
 
  // Souscription caméra JPEG compressée
  sub_cam_ = create_subscription<sensor_msgs::msg::CompressedImage>(
    cam, 10,
    [this](const sensor_msgs::msg::CompressedImage::SharedPtr msg){
      on_image(msg);
    });
 
  // Souscription état batterie
  sub_bat_ = create_subscription<sensor_msgs::msg::BatteryState>(
    bat, 10,
    [this](const sensor_msgs::msg::BatteryState::SharedPtr msg){
      on_battery(msg);
    });
}
 
void ControllerNode::publish_twist(
  double linear, double angular)
{
  geometry_msgs::msg::Twist msg;
  msg.linear.x  = linear;
  msg.angular.z = angular;
  pub_cmd_->publish(msg);
}
 
void ControllerNode::stop() {
  publish_twist(0.0, 0.0);
}
 
void ControllerNode::on_image(
  const sensor_msgs::msg::CompressedImage::SharedPtr msg)
{
  // Décoder JPEG depuis le vecteur d'octets ROS
  QImage img;
  const uchar * data = msg->data.data();
  img.loadFromData(data,
    static_cast<int>(msg->data.size()), "JPEG");
  if (!img.isNull()) {
    emit new_image(img);   // signal Qt → thread GUI
  }
}
 
void ControllerNode::on_battery(
  const sensor_msgs::msg::BatteryState::SharedPtr msg)
{
  emit battery_updated(msg->percentage);
}
 
}  // namespace tb3ctrl
