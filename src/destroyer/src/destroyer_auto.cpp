#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <random>

class DestroyerAuto : public rclcpp::Node
{
public:
  DestroyerAuto() : Node("auto_mode_destroyer")
  {
    twistPublisher = this->create_publisher<geometry_msgs::msg::Twist>("/autonomous_destroyer", 10);
    
    publishTimer = this->create_wall_timer(
      std::chrono::milliseconds(500), std::bind(&DestroyerAuto::publishRandomTwist, this));
      
    RCLCPP_INFO(this->get_logger(), "Auto Mode Destroyer aktif! Memulai pergerakan acak...");
  }

private:
  void publishRandomTwist()
  {
    auto twistMsg = geometry_msgs::msg::Twist();

    std::random_device rd;
    std::mt19937 gen(rd());
    
    std::uniform_real_distribution<> linearDist(-5.0, 5.0);
    std::uniform_real_distribution<> angularDist(-1.0, 1.0);

    twistMsg.linear.x = linearDist(gen);
    twistMsg.linear.y = linearDist(gen);
    twistMsg.linear.z = 0.0;
    
    twistMsg.angular.x = 0.0;
    twistMsg.angular.y = 0.0;
    twistMsg.angular.z = angularDist(gen);

    twistPublisher->publish(twistMsg);
  }

  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr twistPublisher;
  rclcpp::TimerBase::SharedPtr publishTimer;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<DestroyerAuto>());
  rclcpp::shutdown();
  return 0;
}