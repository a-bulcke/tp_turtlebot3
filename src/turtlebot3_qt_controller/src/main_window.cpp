#include "turtlebot3_qt_controller/main_window.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QPalette>
#include <QSizePolicy>
#include <QPixmap>
 
namespace tb3ctrl {
 
MainWindow::MainWindow(
  std::shared_ptr<ControllerNode> node,
  const KeyBindings & bindings,
  QWidget * parent)
: QMainWindow(parent), node_(node), bindings_(bindings)
{
  setWindowTitle("TurtleBot3 — Contrôle Qt");
  setMinimumSize(900, 600);
  build_ui();
 
  // Connecter les signaux ROS → slots Qt
  connect(node_.get(), &ControllerNode::new_image,
          this, &MainWindow::on_new_image,
          Qt::QueuedConnection);  // thread-safe cross-thread
 
  connect(node_.get(), &ControllerNode::battery_updated,
          this, &MainWindow::on_battery_updated,
          Qt::QueuedConnection);
 
  setFocusPolicy(Qt::StrongFocus);
  setFocus();
}
 
void MainWindow::build_ui()
{
  auto * central = new QWidget(this);
  setCentralWidget(central);
 
  auto * main_layout = new QHBoxLayout(central);
 
  // ── Zone caméra (gauche) ──
  auto * cam_box = new QGroupBox("Caméra");
  cam_box->setMinimumWidth(640);
  auto * cam_layout = new QVBoxLayout(cam_box);
 
  camera_label_ = new QLabel("En attente du flux caméra...");
  camera_label_->setAlignment(Qt::AlignCenter);
  camera_label_->setMinimumSize(640, 480);
  camera_label_->setStyleSheet(
    "background-color: #111; color: #888;");
  cam_layout->addWidget(camera_label_);
 
  // ── Panneau droite ──
  auto * right_layout = new QVBoxLayout();
 
  // Groupe batterie
  auto * bat_box = new QGroupBox("Batterie");
  auto * bat_layout = new QVBoxLayout(bat_box);
 
  battery_label_ = new QLabel("---%");
  battery_label_->setAlignment(Qt::AlignCenter);
  QFont bf; bf.setPointSize(20); bf.setBold(true);
  battery_label_->setFont(bf);
 
  battery_bar_ = new QProgressBar();
  battery_bar_->setRange(0, 100);
  battery_bar_->setValue(0);
  battery_bar_->setTextVisible(false);
  battery_bar_->setStyleSheet(
    "QProgressBar::chunk { background-color: #4CAF50; }");
 
  bat_layout->addWidget(battery_label_);
  bat_layout->addWidget(battery_bar_);
 
  // Groupe commandes
  auto * ctrl_box = new QGroupBox("Commandes clavier");
  auto * ctrl_layout = new QGridLayout(ctrl_box);
 
  auto mk_btn = [](const QString & t) {
    auto * b = new QPushButton(t);
    b->setFixedSize(70, 70);
    b->setEnabled(false);
    b->setStyleSheet(
      "QPushButton { font-size:16px; background:#2a2a2a; }");
    return b;
  };
 
  // Grille fléchée
  ctrl_layout->addWidget(mk_btn("⬆ Avance"),  0, 1);
  ctrl_layout->addWidget(mk_btn("⬅ Gauche"),  1, 0);
  ctrl_layout->addWidget(mk_btn("⏹ Stop"),    1, 1);
  ctrl_layout->addWidget(mk_btn("Droite ➡"),  1, 2);
  ctrl_layout->addWidget(mk_btn("⬇ Recule"),  2, 1);
 
  // Indicateur vitesse
  status_label_ = new QLabel("Statut : arrêté");
  speed_label_  = new QLabel("v=0.00 m/s  ω=0.00 rad/s");
  status_label_->setAlignment(Qt::AlignCenter);
  speed_label_->setAlignment(Qt::AlignCenter);
 
  right_layout->addWidget(bat_box);
  right_layout->addWidget(ctrl_box);
  right_layout->addWidget(status_label_);
  right_layout->addWidget(speed_label_);
  right_layout->addStretch();
 
  main_layout->addWidget(cam_box, 3);
  main_layout->addLayout(right_layout, 1);
}
 
void MainWindow::on_new_image(QImage img)
{
  // Redimensionner l'image pour le label
  QPixmap pix = QPixmap::fromImage(img).scaled(
    camera_label_->size(),
    Qt::KeepAspectRatio,
    Qt::SmoothTransformation);
  camera_label_->setPixmap(pix);
}
 
void MainWindow::on_battery_updated(float percentage)
{
  int pct = static_cast<int>(percentage * 100.0f);
  battery_label_->setText(
    QString("%1 %").arg(pct));
  battery_bar_->setValue(pct);
 
  // Couleur selon niveau
  QString color = (pct > 50) ? "#4CAF50" :
                  (pct > 20) ? "#FF9800" : "#F44336";
  battery_bar_->setStyleSheet(
    QString("QProgressBar::chunk { background-color: %1; }")
    .arg(color));
}
 
void MainWindow::keyPressEvent(QKeyEvent * event)
{
  if (!event->isAutoRepeat())
    handle_key(event->key(), true);
  QMainWindow::keyPressEvent(event);
}
 
void MainWindow::keyReleaseEvent(QKeyEvent * event)
{
  if (!event->isAutoRepeat())
    handle_key(event->key(), false);
  QMainWindow::keyReleaseEvent(event);
}
 
void MainWindow::handle_key(int key, bool pressed)
{
  if (key == bindings_.forward   || key == bindings_.arrow_up)
    key_forward_  = pressed;
  else if (key == bindings_.backward  || key == bindings_.arrow_down)
    key_backward_ = pressed;
  else if (key == bindings_.turn_left || key == bindings_.arrow_left)
    key_left_     = pressed;
  else if (key == bindings_.turn_right|| key == bindings_.arrow_right)
    key_right_    = pressed;
  else if (key == bindings_.stop)
  { node_->stop(); status_label_->setText("Statut : arrêté"); return; }
  else return;
 
  double lin = 0.0, ang = 0.0;
  if (key_forward_)  lin += bindings_.linear_speed;
  if (key_backward_) lin -= bindings_.linear_speed;
  if (key_left_)     ang += bindings_.angular_speed;
  if (key_right_)    ang -= bindings_.angular_speed;
 
  node_->publish_twist(lin, ang);
 
  QString st = (lin>0)?"Avance":(lin<0)?"Recule":
               (ang>0)?"Rotation G":(ang<0)?"Rotation D":"Arrêté";
  status_label_->setText("Statut : " + st);
  speed_label_->setText(
    QString("v=%1 m/s  ω=%2 rad/s")
    .arg(lin,4,'f',2).arg(ang,4,'f',2));
}
 
}  // namespace tb3ctrl
