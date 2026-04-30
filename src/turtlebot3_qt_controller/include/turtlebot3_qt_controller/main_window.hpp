#pragma once
 
#include <QMainWindow>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QStatusBar>
#include <QKeyEvent>
#include <QGridLayout>
#include <QGroupBox>
#include <map>
#include <string>
#include <memory>
#include "turtlebot3_qt_controller/controller_node.hpp"
 
namespace tb3ctrl {
 
struct KeyBindings {
  int forward, backward, turn_left, turn_right, stop;
  int arrow_up, arrow_down, arrow_left, arrow_right;
  double linear_speed;
  double angular_speed;
};
 
class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  explicit MainWindow(
    std::shared_ptr<ControllerNode> node,
    const KeyBindings & bindings,
    QWidget * parent = nullptr);
 
public slots:
  void on_new_image(QImage img);
  void on_battery_updated(float percentage);
 
protected:
  void keyPressEvent(QKeyEvent * event) override;
  void keyReleaseEvent(QKeyEvent * event) override;
 
private:
  void build_ui();
  void handle_key(int key, bool pressed);
 
  std::shared_ptr<ControllerNode> node_;
  KeyBindings bindings_;
 
  // Widgets UI
  QLabel *       camera_label_;
  QProgressBar * battery_bar_;
  QLabel *       battery_label_;
  QLabel *       status_label_;
  QLabel *       speed_label_;
 
  // État courant du mouvement
  bool key_forward_  {false};
  bool key_backward_ {false};
  bool key_left_     {false};
  bool key_right_    {false};
};
 
}  // namespace tb3ctrl
