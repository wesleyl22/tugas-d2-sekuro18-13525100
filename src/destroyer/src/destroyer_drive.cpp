#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <iostream>
#include <termios.h>
#include <unistd.h>

char getch()
{
  char buf = 0;
  struct termios old = {};
  if (tcgetattr(0, &old) < 0) perror("tcsetattr()");
  old.c_lflag &= ~ICANON;
  old.c_lflag &= ~ECHO;
  old.c_cc[VMIN] = 1;
  old.c_cc[VTIME] = 0;
  if (tcsetattr(0, TCSANOW, &old) < 0) perror("tcsetattr ICANON");
  if (read(0, &buf, 1) < 0) perror ("read()");
  old.c_lflag |= ICANON;
  old.c_lflag |= ECHO;
  if (tcsetattr(0, TCSADRAIN, &old) < 0) perror ("tcsetattr ~ICANON");
  return buf;
}

class DestroyerDrive : public rclcpp::Node
{
public:
  DestroyerDrive() : Node("drive_mode_destroyer")
  {
    drivePublisher = this->create_publisher<geometry_msgs::msg::Twist>("/drive_destroyer", 10);
    RCLCPP_INFO(this->get_logger(), "Panah: Maju/Mundur/Kiri/Kanan (5 m/s)");
    RCLCPP_INFO(this->get_logger(), "R/L  : Rotasi Kanan/Kiri (1 rad/s)");
    RCLCPP_INFO(this->get_logger(), "B: Rem/Berhenti");
  }

  void runKeyboardLoop()
  {
    while (rclcpp::ok())
    {
      char c = getch();
      auto msg = geometry_msgs::msg::Twist();
      bool isValidKey = false;

      if (c == '\x1B')
      {
        char seq1 = getch();
        char seq2 = getch();
        
        if (seq1 == '[')
        {
          switch(seq2)
          {
            case 'A':
              msg.linear.x = 5.0; isValidKey = true; break;
            case 'B':
              msg.linear.x = -5.0; isValidKey = true; break;
            case 'C':
              msg.linear.y = -5.0; isValidKey = true; break;
            case 'D':
              msg.linear.y = 5.0; isValidKey = true; break;
          }
        }
      } 
      else
      {
        if (c == 'r' || c == 'R')
        {
          msg.angular.z = -1.0; 
          isValidKey = true;
        } 
        else if (c == 'l' || c == 'L')
        {
          msg.angular.z = 1.0;  
          isValidKey = true;
        } 
        else if (c == 'b' || c == 'B')
        { 
          msg.linear.x = 0.0;
          msg.linear.y = 0.0;
          msg.angular.z = 0.0;
          isValidKey = true;
        }
        else if (c == '\x03')
        {
          break;
        }
      }

      if (isValidKey)
      {
        drivePublisher->publish(msg);
      }
    }
  }

private:
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr drivePublisher;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  
  auto node = std::make_shared<DestroyerDrive>();
  node->runKeyboardLoop();
  
  rclcpp::shutdown();
  return 0;
}