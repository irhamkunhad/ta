#include <ros.h>
#include <geometry_msgs/Twist.h>

// Pin definitions
const int leftPwmPin = 6;   // PWM pin to control left direction
const int rightPwmPin = 5; // PWM pin to control right direction




float VX = 0.0;
float VY = 0.0;
float omega = 0.0;

ros::NodeHandle nh;

// Variables to store speed and direction

void vxCallback(const geometry_msgs::Twist& msg) {
  VX = msg.linear.y;
}

void vyCallback(const geometry_msgs::Twist& msg) {
  VY = msg.linear.x;
}

void omegaCallback(const geometry_msgs::Twist& msg) {
  omega = msg.angular.z;
}

ros::Subscriber<geometry_msgs::Twist> sub_vx("robot/vx", &vxCallback);
ros::Subscriber<geometry_msgs::Twist> sub_vy("robot/vy", &vyCallback);
ros::Subscriber<geometry_msgs::Twist> sub_omega("robot/omega", &omegaCallback);


void setup() {
  Serial.begin(57600);
  pinMode(leftPwmPin, OUTPUT);
  pinMode(rightPwmPin, OUTPUT);
  
   nh.initNode();
  nh.subscribe(sub_vx);
  nh.subscribe(sub_vy);
  nh.subscribe(sub_omega);
}

void loop() {
  
float s2 = 0.67 * VX*35 + 0.33 * omega*35;
if (s2 < 0){
  s2 = abs(s2);
  s2 = min(100,s2);
    analogWrite(leftPwmPin,s2);
    analogWrite(rightPwmPin,0);
}
else if (s2 > 0){
  s2 = abs(s2);
  s2 = min(100,s2);
    analogWrite(leftPwmPin,0);
    analogWrite(rightPwmPin,s2);
}
else if (s2 == 0){
    analogWrite(leftPwmPin,0);
    analogWrite(rightPwmPin,0);
}
  nh.spinOnce();
  
  delay(1);
}
