#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    // ROS_INFO_STREAM("Requesting velocities ...");
    // ROS_INFO_STREAM("{process_image} lin_x: "+std::to_string(lin_x)+ " ang_z: "+ std::to_string(ang_z));

    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = (float)lin_x;
    srv.request.angular_z = (float)ang_z;

    if(!client.call(srv))
        ROS_ERROR("Failed to call drive_bot");

}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
    int state;
    for(int x; x<(img.height*img.step); x=x+3) {
        state=0;
        // ROS_INFO_STREAM("pixel -> "+std::to_string(x));
        // ROS_INFO_STREAM("value -> "+std::to_string(img.data[x]));
        if((img.data[x]+img.data[x+1]+img.data[x+2])==(white_pixel*3)) {
            // ROS_INFO_STREAM("Ball found");
            state = 1;
            float img_index = x%img.step;
            if(img_index <= img.step/3) {
                drive_robot(0, 1);
                // ROS_INFO_STREAM("Turning left");
            }
            else if (img_index <= (img.step*2/3)) {
                drive_robot(0.5, 0);
                // ROS_INFO_STREAM("Forward");
            }
            else {
                drive_robot(0, -1);
                // ROS_INFO_STREAM("Turning right");
            }
            
          
            break;
        }
        //else 
         //ROS_ERROR("Ball NOT found");
         
      }
      if(state==0)
        drive_robot(0, 0);

}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}