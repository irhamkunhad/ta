#include <ros.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Int64.h>
#include <geometry_msgs/Twist.h>

// Pin definitions
const int leftPwmPin = 6;   // PWM pin to control left direction
const int rightPwmPin = 5; // PWM pin to control right direction
const int ENCA1 = 2;        // Encoder pin A
const int ENCB1 = 3;


unsigned int v2 = 100;
float VX = 0.0;
float VY = 0.0;
float omega = 0.0;
volatile long pulse = 0;

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

std_msgs::Float64 v2_msg;
std_msgs::Int64 encoder_msg;
ros::Publisher v2_pub("/v2", &v2_msg);
ros::Publisher encoder_pub("/encoderValue_2", &encoder_msg);



void readEncoder() {
  if (digitalRead(ENCA1) == digitalRead(ENCB1)) {
    pulse--;
  } else {
    pulse++;
  }
}

void setup() {
  Serial.begin(57600);
  pinMode(leftPwmPin, OUTPUT);
  pinMode(rightPwmPin, OUTPUT);
  pinMode(ENCA1, INPUT_PULLUP);
  pinMode(ENCB1, INPUT_PULLUP);
  
   nh.initNode();
  nh.subscribe(sub_vx);
  nh.subscribe(sub_vy);
  nh.subscribe(sub_omega);

  nh.advertise(v2_pub);
  nh.advertise(encoder_pub);

    attachInterrupt(digitalPinToInterrupt(ENCA1), readEncoder, CHANGE);
}

void loop() {
  
float s2 = 0.67 * VX*35 + 0.33 * omega*35;
if (s2 < 0){
  s2 = abs(s2);
  s2 = min(v2,s2);
    analogWrite(leftPwmPin,s2);
    analogWrite(rightPwmPin,0);
}
else if (s2 > 0){
  s2 = abs(s2);
  s2 = min(v2,s2);
    analogWrite(leftPwmPin,0);
    analogWrite(rightPwmPin,s2);
}
else if (s2 == 0){
    analogWrite(leftPwmPin,0);
    analogWrite(rightPwmPin,0);
}

  v2_msg.data = v2;
  v2_pub.publish(&v2_msg);

  // Publikasikan nilai encoder
  encoder_msg.data = pulse;
  encoder_pub.publish(&encoder_msg);
  
  nh.spinOnce();
  
  delay(1);
}
