#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <std_msgs/msg/string.hpp>
#include <string>

class DestroyerMove : public rclcpp::Node
{
public:
  DestroyerMove() : Node("move_destroyer")
  {
    powerSubscriber = this->create_subscription<geometry_msgs::msg::Twist>(
      "/power_destroyer", 10, std::bind(&DestroyerMove::powerCallback, this, std::placeholders::_1));
      
    moveTypeSubscriber = this->create_subscription<std_msgs::msg::String>(
      "/move_type", 10, std::bind(&DestroyerMove::moveTypeCallback, this, std::placeholders::_1));
  }

private:
  void powerCallback(const geometry_msgs::msg::Twist::SharedPtr msg)
  {
    std::string moveStr = "";

    if (msg->linear.x > 0.0) moveStr += "Maju ";
    else if (msg->linear.x < 0.0) moveStr += "Mundur ";

    if (msg->linear.y > 0.0) moveStr += "Geser Kiri ";
    else if (msg->linear.y < 0.0) moveStr += "Geser Kanan ";

    if (msg->angular.z > 0.0) moveStr += "Berputar Kiri";
    else if (msg->angular.z < 0.0) moveStr += "Berputar Kanan";

    if (moveStr.empty()) moveStr = "Diam";

    if (moveStr.back() == ' ') moveStr.pop_back();

    RCLCPP_INFO(this->get_logger(), "Orientasi Gerak: %s", moveStr.c_str());
  }

  void moveTypeCallback(const std_msgs::msg::String::SharedPtr msg)
  {
    RCLCPP_INFO(this->get_logger(), "Tipe Kendali Aktif: %s", msg->data.c_str());
  }

  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr powerSubscriber;
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr moveTypeSubscriber;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<DestroyerMove>());
  rclcpp::shutdown();
  return 0;
}