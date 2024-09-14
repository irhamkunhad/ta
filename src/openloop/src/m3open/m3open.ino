#include <ros.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Int64.h>
#include <geometry_msgs/Twist.h>

// Pin definitions
const int leftPwmPin = 6;   // PWM pin to control left direction
const int rightPwmPin = 5; // PWM pin to control right direction
const int ENCA1 = 2;        // Encoder pin A
const int ENCB1 = 3;        // Encoder pin B

unsigned int v3 = 100;
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

std_msgs::Float64 v3_msg;
std_msgs::Int64 encoder_msg;
ros::Publisher v3_pub("/v3", &v3_msg);
ros::Publisher encoder_pub("/encoderValue_3", &encoder_msg);

// Fungsi untuk membaca encoder
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
  
  // Initialize publisher
  nh.advertise(v3_pub);
  nh.advertise(encoder_pub);

   // Attach interrupt to encoder pin
  attachInterrupt(digitalPinToInterrupt(ENCA1), readEncoder, CHANGE);
   
}

void loop() {


float s3 = -0.33 * VX*35 + 0.58 * VY*35 + 0.33 * omega*35;
if (s3 < 0){
  s3 = abs(s3);
  s3 = min(v3,s3);
   analogWrite(leftPwmPin,s3);
   analogWrite(rightPwmPin,0);
}
else if (s3 > 0){
   s3 = abs(s3);
   s3 = min(v3,s3);
    analogWrite(leftPwmPin,0);
    analogWrite(rightPwmPin,s3);
}
else if (s3 == 0){
    analogWrite(leftPwmPin,0);
    analogWrite(rightPwmPin,0);
}

  // Publikasikan nilai s1
  v3_msg.data = v3;
  v3_pub.publish(&v3_msg);

  // Publikasikan nilai encoder
  encoder_msg.data = pulse;
  encoder_pub.publish(&encoder_msg);
  nh.spinOnce();
  
  delay(1);
}
