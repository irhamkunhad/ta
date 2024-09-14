#include <ros/ros.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Int64.h>
#include <geometry_msgs/Vector3.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
#include <cmath>

// Deklarasi variabel global
const float L = 0.2;
const float X_max = 857.9617834;
const float Y_max = 859.5541401;
const float Theta_max = 4671.66;

long enc1 = 0, enc2 = 0, enc3 = 0;
float v1 = 0, v2 = 0, v3 = 0;
float SX = 0, SY = 0, ST = 0;
float Pos_X = 0, Pos_Y = 0;

void encoderMotor1Callback(const std_msgs::Int64::ConstPtr& msg) {
    enc1 = msg->data;
}

void encoderMotor2Callback(const std_msgs::Int64::ConstPtr& msg) {
    enc2 = msg->data;
}

void encoderMotor3Callback(const std_msgs::Int64::ConstPtr& msg) {
    enc3 = msg->data;
}

void velocity1Callback(const std_msgs::Float64::ConstPtr& msg) {
    v1 = msg->data;
}

void velocity2Callback(const std_msgs::Float64::ConstPtr& msg) {
    v2 = msg->data;
}

void velocity3Callback(const std_msgs::Float64::ConstPtr& msg) {
    v3 = msg->data;
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "Odometri");
    ros::NodeHandle nh;

    ros::Subscriber enc1_sub = nh.subscribe("/encoderValue_1", 10, encoderMotor1Callback);
    ros::Subscriber enc2_sub = nh.subscribe("/encoderValue_2", 10, encoderMotor2Callback);
    ros::Subscriber enc3_sub = nh.subscribe("/encoderValue_3", 10, encoderMotor3Callback);
    
    ros::Subscriber vel1_sub = nh.subscribe("/v1", 10, velocity1Callback);
    ros::Subscriber vel2_sub = nh.subscribe("/v2", 10, velocity2Callback);
    ros::Subscriber vel3_sub = nh.subscribe("/v3", 10, velocity3Callback);

    ros::Publisher pos_pub = nh.advertise<geometry_msgs::Vector3>("/Error_Position", 10);
    ros::Publisher odom_pub = nh.advertise<nav_msgs::Odometry>("odom", 50);
    tf::TransformBroadcaster odom_broadcaster;

    ros::Rate rate(10);

    double x = 0.0;
    double y = 0.0;
    double th = 0.0;

    ros::Time current_time, last_time;
    ros::Time last_info_time;  // Waktu terakhir informasi ditampilkan
    current_time = ros::Time::now();
    last_time = ros::Time::now();
    last_info_time = ros::Time::now();  // Inisialisasi waktu terakhir informasi

    while (ros::ok()) {
        current_time = ros::Time::now();
        
        SX = (((2 * enc2) - enc1 - enc3) / 3.0);
        SY = (((std::sqrt(3) * enc3) - (std::sqrt(3) * enc1)) / 3.0);
        ST = ((v1 + v2 + v3) / (3 * L));

        Pos_X = std::round((1 * ((SX) / X_max)) * 1000.0) / 1000.0;
        Pos_Y = std::round((1 * ((SY) / Y_max)) * 1000.0) / 1000.0;
        float omega = std::round(ST * 10.0) / 10.0;
          /*
        // Hitung odometri
        double dt = (current_time - last_time).toSec();
        double delta_x = (v1 * cos(th) - v2 * sin(th)) * dt;
        double delta_y = (v1 * sin(th) + v2 * cos(th)) * dt;
        double delta_th = omega * dt;

        x += delta_x;
        y += delta_y;
        th += delta_th;
          */
        // Buat quaternion dari yaw
        geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(omega);

        // Publikasikan transformasi TF
        geometry_msgs::TransformStamped odom_trans;
        odom_trans.header.stamp = current_time;
        odom_trans.header.frame_id = "odom";
        odom_trans.child_frame_id = "base_agv";

        odom_trans.transform.translation.x = Pos_X;
        odom_trans.transform.translation.y = Pos_Y;
        odom_trans.transform.translation.z = 0.0;
        odom_trans.transform.rotation = odom_quat;

        odom_broadcaster.sendTransform(odom_trans);

        // Publikasikan pesan odometri
        nav_msgs::Odometry odom;
        odom.header.stamp = current_time;
        odom.header.frame_id = "odom";

        odom.pose.pose.position.x = Pos_X;
        odom.pose.pose.position.y = Pos_Y;
        odom.pose.pose.position.z = 0.0;
        odom.pose.pose.orientation = odom_quat;

        odom.child_frame_id = "base_agv";
        odom.twist.twist.linear.x = v1;
        odom.twist.twist.linear.y = v2;
        odom.twist.twist.angular.z = v3;

        odom_pub.publish(odom);

        geometry_msgs::Vector3 posisi_msg;
        posisi_msg.x = Pos_X;
        posisi_msg.y = Pos_Y;
        posisi_msg.z = omega;

        pos_pub.publish(posisi_msg);

        // Periksa apakah 10 detik telah berlalu sejak terakhir kali informasi ditampilkan
        /*if ((current_time - last_info_time).toSec() >= 10.0) {
            ROS_INFO("X = %f, Y = %f, H = %f", SX, SY, ST);
            ROS_INFO("Posisi_X = %f, Posisi_Y = %f, Omega = %f", Pos_X, Pos_Y, omega);

            // Perbarui waktu terakhir informasi ditampilkan
            last_info_time = current_time;
        }   
        */
        last_time = current_time;
        ros::spinOnce();
        rate.sleep();
    }

    return 0;
}
