#!/usr/bin/env python3

import rospy
from geometry_msgs.msg import Twist 


def move_robot():
    rospy.init_node('robot_teleop', anonymous=True)
    pub_vx = rospy.Publisher('robot/vx', Twist, queue_size=10)
    pub_vy = rospy.Publisher('robot/vy', Twist, queue_size=10)
    pub_omega = rospy.Publisher('robot/omega', Twist, queue_size=10)
    rate = rospy.Rate(10)  # 10 Hz

    twist = Twist()

    while not rospy.is_shutdown():
        key = input("""
        Enter command (w/a/s/d for move, q to quit
        ,x for stop,f for rotate): """)

        if key == 'w':
            twist.linear.x = 1.0
            twist.angular.z = 0.0
            pub_vy.publish(twist)
        elif key == 's':
            twist.linear.x = -1.0
            twist.angular.z = 0.0
            pub_vy.publish(twist)
        elif key == 'a':
            twist.linear.y = -1.0
            twist.angular.z = 0.0
            pub_vx.publish(twist)
        elif key == 'd':
            twist.linear.y = 1.0
            twist.angular.z = 0.0
            pub_vx.publish(twist)
        elif key == 'f':
            twist.linear.y = 0.0
            twist.linear.x = 0.0
            twist.angular.z = 1.0
            pub_omega.publish(twist)
            pub_vx.publish(twist)
            pub_vy.publish(twist)

        elif key == 'q':

            rospy.signal_shutdown('User requested shutdown.')
            break
        elif key == 'x':
    
            twist.linear.x = 0.0
            twist.angular.z = 0.0
            twist.linear.y = 0.0
            pub_vx.publish(twist)
            pub_vy.publish(twist)
            pub_omega.publish(twist)

        rate.sleep()



if __name__ == '__main__':
    try:
        move_robot()
    except rospy.ROSInterruptException:
        pass
