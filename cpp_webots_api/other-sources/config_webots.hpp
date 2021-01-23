//#include <iostream>
//#include <sstream>
#include <opencv2/opencv.hpp>
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>   
#include<sys/ioctl.h>
#include<unistd.h>  
//#include<fstream>
#include<errno.h>
#include <math.h>

// webots includes
#include <webots/Camera.hpp> //#include <webots/camera.h>
#include <webots/Device.hpp> //#include <webots/device.h>
#include <webots/Display.hpp> //#include <webots/display.h>
#include <webots/GPS.hpp> //#include <webots/gps.h>
#include <webots/vehicle/Driver.hpp> //#include <webots/vehicle/driver.h>
#include <webots/Robot.hpp> //#include <webots/robot.h>

using namespace std;
using namespace webots;
using namespace cv;

// to be used as array indices
enum { X, Y, Z };

// ------------ defs -------------//
#define FILE_NAME_LENGTH 25
/* Sampling time of 40 milliseconds */
//#define SAMPLING_PERIOD 40
#define UNUSED(x) (void)(x)
/* Define port of socket to receive steering angle. */
//#define PORT 8080
/* Define port of socket to send Images */
//#define PORT_SEND 6000
/* Duration in ms taken for the vehicle to take 90 degree turn. */
//#define TURN_DURATION 4800
/* Duration in ms the vehicle has to wait before taking a 90 degree turn. */
//#define WAIT_TURN_DURATION 4000
/* Sensor to actuator delay in ms */
//#define SENSOR_TO_ACTUATOR_DELAY 37.9



/* Define cruising speed and maximum speed. */
#define TOP_SPEED 60.0
#define VEH_SPEED 50.0
#define ROAD_WIDTH 3.25

/* No. of Cameras. */
//#define CAMERA_NO 3

/* Flags to enable various 'features' */
bool enable_display = false;
bool has_camera = false;
bool has_gps = false; 

/* Variables required for camera object. */
Camera *camera_front; //WbDeviceTag camera_front;
//WbDeviceTag camera_left;
//WbDeviceTag camera_right;
int camera_width = -1;
int camera_height = -1;
double camera_fov = -1.0;

/* to get location */
GPS *gps; //WbDeviceTag gps;
double gps_coords[3] = {0.0, 0.0, 0.0};
double gps_speed = 0.0;
std::vector<long double> positionx;
std::vector<long double> positiony;

/* golden reference */
string gold_ref_csv = "/home/yingkai/all_city/finaltest/final.csv";
//string gold_ref_csv = "/home/yingkai/all_city/20200122/city.csv";
// get yL
double getyL(string ref, double positionx , double positiony, bool debug=false);

vector<vector<double>> read_csv( string filename );

/* image quality is set as 100 (highest quality possible). */
int image_quality = 100;

/* For speedometer */
Display *display; //WbDeviceTag display;
int display_width = 0;
int display_height = 0;
ImageRef *speedometer_image = NULL;

/* steering variables. */
double steer90 = 0.0;
//bool turn90 = false;
//bool steer_flag = false;
//double start_time = 0.0;
//double end_time = 0.0;
//volatile bool steer_delay = false;
/* declare a offset value to detect 90 degree turns.  */
double offset_value = 100.0;

/* misc variables */
double speed = 0.0;
double steering_angle = 0.0;
//bool autodrive = true;

/* IP address of Px2 */
// char agx_ip[14]="192.168.1.139";

// socket functions for client
// int send_image(int socket, std::string img_addr, bool debug=false);

// // receive image TCP/IP
// int receive_image(int socket, std::string img_addr, bool debug=false);

//  /* Function to get steering angle */
// long double steering_angle_recv(int sock, bool debug=false);

// // Send data
// int send_data(int new_socket, std::string data, bool debug=false);

// // get data
// std::string get_data(int socket, bool debug=false);




