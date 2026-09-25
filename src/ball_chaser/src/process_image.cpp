#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

ros::ServiceClient client;

void drive_robot(float lin_x, float ang_z)
{
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if (!client.call(srv))
        ROS_ERROR("Failed to call service command_robot");
}

void process_image_callback(const sensor_msgs::Image img)
{
    const int white_pixel = 255;
    int ball_column = -1;

    for (int i = 0; i + 2 < (int)img.data.size(); i += 3) {
        if (img.data[i] == white_pixel && img.data[i + 1] == white_pixel && img.data[i + 2] == white_pixel) {
            ball_column = (i % img.step) / 3;
            break;
        }
    }

    if (ball_column < 0)
        drive_robot(0.0, 0.0);
    else if (ball_column < (int)img.width / 3)
        drive_robot(0.0, 0.5);
    else if (ball_column < 2 * (int)img.width / 3)
        drive_robot(0.5, 0.0);
    else
        drive_robot(0.0, -0.5);
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    ros::spin();

    return 0;
}
