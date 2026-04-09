#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <std_msgs/msg/string.hpp>

class DestroyerCheck : public rclcpp::Node
{
public:
  DestroyerCheck() : Node("check_move_destroyer")
  {
    powerPublisher = this->create_publisher<geometry_msgs::msg::Twist>("/power_destroyer", 10);
    moveTypePublisher = this->create_publisher<std_msgs::msg::String>("/move_type", 10);

    autoSubscriber = this->create_subscription<geometry_msgs::msg::Twist>(
      "/autonomous_destroyer", 10, std::bind(&DestroyerCheck::autoCallback, this, std::placeholders::_1));
      
    driveSubscriber = this->create_subscription<geometry_msgs::msg::Twist>(
      "/drive_destroyer", 10, std::bind(&DestroyerCheck::driveCallback, this, std::placeholders::_1));

    lastDriveTime = this->now();
  }

private:
  void autoCallback(const geometry_msgs::msg::Twist::SharedPtr msg)
  {
    auto now = this->now();
    
    if ((now - lastDriveTime).seconds() > 1.0)
    {
      RCLCPP_INFO(this->get_logger(), "Received from Autonomous: linear.x=%.2f, linear.y=%.2f, yaw=%.2f", msg->linear.x, msg->linear.y, msg->angular.z);

      powerPublisher->publish(*msg);

      auto typeMsg = std_msgs::msg::String();
      typeMsg.data = "Autonomous";
      moveTypePublisher->publish(typeMsg);
    }
  }

  void driveCallback(const geometry_msgs::msg::Twist::SharedPtr msg)
  {
    lastDriveTime = this->now();

    RCLCPP_INFO(this->get_logger(), "Received from Driver: linear.x=%.2f, linear.y=%.2f, yaw=%.2f", msg->linear.x, msg->linear.y, msg->angular.z);

    powerPublisher->publish(*msg);

    auto typeMsg = std_msgs::msg::String();
    typeMsg.data = "Driver";
    moveTypePublisher->publish(typeMsg);
  }

  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr powerPublisher;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr moveTypePublisher;
  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr autoSubscriber;
  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr driveSubscriber;
  
  rclcpp::Time lastDriveTime;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<DestroyerCheck>());
  rclcpp::shutdown();
  return 0;
}