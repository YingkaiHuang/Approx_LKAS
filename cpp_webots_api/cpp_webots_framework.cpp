#include <iostream>
#include <time.h>
#include <cmath>
#include <memory>
#include <torch/script.h> // One-stop header.
#include <fstream>

//#include <sstream>
#include <opencv2/opencv.hpp>
#include "config.hpp"
#include "config_webots.hpp"
#include <webots/Camera.hpp>		 //#include <webots/camera.h>
#include <webots/Device.hpp>		 //#include <webots/device.h>
#include <webots/Display.hpp>		 //#include <webots/display.h>
#include <webots/GPS.hpp>			 //#include <webots/gps.h>
#include <webots/vehicle/Driver.hpp> //#include <webots/vehicle/driver.h>
#include <webots/Robot.hpp>			 //#include <webots/robot.h>
using namespace std;
extern "C"
{
	// #include "extApi.h"
	// #include <webots/camera.h>
	// #include <webots/device.h>
	// #include <webots/display.h>
	// #include <webots/keyboard.h>
	// #include <webots/robot.h>
	// #include <webots/supervisor.h>
	// #include <webots/vehicle/driver.h>

	// #include <math.h>
	// #include <stdio.h>
	// #include <string.h>
	// #include <signal.h>
	// #include <stdlib.h>
	// #include <unistd.h>
	// #include <sys/types.h>
	// #include <sys/socket.h>
	// #include <sys/time.h>
	// #include <netinet/in.h>
	// #include <netdb.h>
	// #include <errno.h>
	// #include <pthread.h>
	// // #include <Python.h>

	// #include <unistd.h> // for close
	// #include <arpa/inet.h>
}
float kkkk = 0.0f;
long double ssum = 0.0L;
long double asum = 0.0L;
long double MAE = 0.0L;
long double MSE = 0.0L;
long double Meanv = 0.0L;
long double Meansum = 0.0L;
string out_string = "/home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/cpp_webots_api/out_imgs";
std::string yl_csv_dir = "/home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/cpp_webots_api/result";
//torch::jit::script::Module module_lanenet = torch::jit::load("/home/yingkai/lanenet/exp0/expwebots.pt");
//--- convert image_2_Mat ---//
Mat webots_img_2_Mat(Camera *camera, const unsigned char *image)
{
	// 3 color image (RGB)
	const int width = camera_width;
	const int height = camera_height;
	Mat out_Mat=cv::Mat::zeros(256, 512, CV_8UC3);
	vector<Mat> three_channels;
	split(out_Mat, three_channels);
	// Convert to BGR memory storage
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			// Blue channel
			three_channels[0].at<char>(y, x) = camera->imageGetBlue(image, width, x, y);
			// Green channel
			three_channels[1].at<char>(y, x) = camera->imageGetGreen(image, width, x, y);
			// Red channel
			three_channels[2].at<char>(y, x) = camera->imageGetRed(image, width, x, y);
		}
	}
	merge(three_channels, out_Mat);
	return out_Mat;
}

Mat image_for_classifier(Mat image)
{

	Mat src = cv::Mat::zeros(256, 512, CV_8UC3) ;
	Mat imageROI;
	Mat clsimg;
	int x_begin, y_begin, width, height;
	int srcWidth, srcHeight;
	srcWidth = image.cols;
	srcHeight = image.rows;
	x_begin = 0;
	y_begin = srcHeight - 100;
	height = 100;
	width = srcWidth;

	src = image;
	imageROI = src(Rect(x_begin, y_begin, width, height));
	imageROI.convertTo(clsimg, clsimg.type());
	return clsimg;
}

/* Function to classify turns */
// torch::Tensor imagetotensor(Mat image)
// {

// 	// Deserialize the ScriptModule from a file using torch::jit::load().

// 	// assert(module != nullptr);
// 	//std::cout << "ok\n";

// 	// //输入图像
// 	cv::Mat image_transfomed = image;
// 	// cv::resize(image, image_transfomed, cv::Size(512, 256));
// 	// cv::cvtColor(image_transfomed, image_transfomed, cv::COLOR_BGR2RGB);
// 	// image_transfomed.convertTo(image_transfomed, CV_32FC3, 1.0f / 255.0f);
// 	// 图像转换为Tensor
// 	torch::Tensor tensor_image = torch::from_blob(image_transfomed.data, {image_transfomed.rows, image_transfomed.cols, 3}, torch::kByte);
// 	tensor_image = tensor_image.permute({2, 0, 1});
// 	tensor_image = tensor_image.toType(torch::kFloat);
// 	tensor_image = tensor_image.div(255);

// 	tensor_image = tensor_image.unsqueeze(0);
// 	tensor_image[0][0] = tensor_image[0][0].sub(0.485).div(0.229);
// 	tensor_image[0][1] = tensor_image[0][1].sub(0.456).div(0.224);
// 	tensor_image[0][2] = tensor_image[0][2].sub(0.406).div(0.225);
// 	return tensor_image;
// }

torch::Tensor preprocessing(Mat image)
{

	// Deserialize the ScriptModule from a file using torch::jit::load().

	// assert(module != nullptr);
	//std::cout << "ok\n";

	// //输入图像
	cv::Mat image_transfomed;
	cv::resize(image, image_transfomed, cv::Size(224, 224));
	cv::cvtColor(image_transfomed, image_transfomed, cv::COLOR_BGR2RGB);

	// 图像转换为Tensor
	torch::Tensor tensor_image = torch::from_blob(image_transfomed.data, {image_transfomed.rows, image_transfomed.cols, 3}, torch::kByte);
	tensor_image = tensor_image.permute({2, 0, 1});
	tensor_image = tensor_image.toType(torch::kFloat);
	tensor_image = tensor_image.div(255);

	tensor_image = tensor_image.unsqueeze(0);
	tensor_image[0][0] = tensor_image[0][0].sub(0.485).div(0.229);
	tensor_image[0][1] = tensor_image[0][1].sub(0.456).div(0.224);
	tensor_image[0][2] = tensor_image[0][2].sub(0.406).div(0.225);
	return tensor_image;

	// 网络前向计算
	// Execute the model and turn its output into a tensor.
	// at::Tensor output = module.forward({tensor_image}).toTensor();

	// auto max_result = output.max(1,true);
	// auto max_index = std::get<1>(max_result).item<float>();

	// // if (max_index == 0){
	// //     std::cout << "left\n";
	// // }else{
	// //     std::cout << "right\n";;
	// // }
	// return max_index;
}

int bevclassifier(torch::Tensor tensor_image)
{
	torch::jit::script::Module module = torch::jit::load("/home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/cpp_webots_api/curve_cpu_new.pt");
	//torch::jit::script::Module module = torch::jit::load("/home/yingkai/transfer_tutorial/transfer.pt");
	at::Tensor output = module.forward({tensor_image}).toTensor();

	auto max_result = output.max(1, true);
	auto max_index = std::get<1>(max_result).item<float>();

	// if (max_index == 0){
	//     std::cout << "left\n";
	// }else{
	//     std::cout << "right\n";;
	// }
	return max_index;
}

int pipelineswitcher(int kfturn, int lane, int light, int speed)
{
	int pversion = 0;

	///////////////////////////straight 50km/h////////////////////////////////////////////////
	if (kfturn == 2 && lane == 1 && light == 4) //white_con_noon straight
	{
		pversion = 1;
	}
	if (kfturn == 2 && lane == 3 && light == 4 ) //yellow_con_noon straight
	{
		pversion = 2;
	}
	if (kfturn == 2 && lane == 2 && light == 4 ) //white_dashed_noon straight
	{
		pversion = 5;
	}
	if (kfturn == 2 && lane == 4 && light == 4 ) //yellow_double_noon straight
	{
		pversion = 4;
	}
	if (kfturn == 2 && lane == 1 && light == 3 ) //white_con_night straight
	{
		pversion = 4;
	}
	if (kfturn == 2 && lane == 3 && light == 3 ) //yellow_con_night straight
	{
		pversion = 6;
	}
	if (kfturn == 2 && lane == 1 && light == 0 ) //white_con_dark straight
	{
		pversion = 6;
	}



	///////////////////////////right 50km/h////////////////////////////////////////////////
	if (kfturn == 1 && lane == 1 && light == 4 ) //white_con_noon 
	{
		pversion = 4;
	}
	if (kfturn == 1 && lane == 3 && light == 4 ) //yellow_con_noon 
	{
		pversion = 1;
	}
	 if(kfturn == 1 && lane == 2 && light ==4 )//white_dashed_noon 
	{
		pversion = 1;
	}
	if (kfturn == 1 && lane == 4 && light == 4 ) //yellow_double_noon 
	{
		pversion = 1;
	}
	if (kfturn == 1 && lane == 1 && light == 3 ) //white_con_night 
	{
		pversion = 6;
	}
	if (kfturn == 1 && lane == 3 && light == 3 ) //yellow_con_night 
	{
		pversion = 1;
	}
	if (kfturn == 1 && lane == 2 && light == 3 ) //white_dashed_night 
	{
		pversion = 6;
	}




	///////////////////////////left 50km/h////////////////////////////////////////////////
	if (kfturn == 0 && lane == 1 && light == 4 ) //white_con_noon straight
	{
		pversion = 1;
	}
	if (kfturn == 0 && lane == 3 && light == 4 ) //yellow_con_noon straight
	{
		pversion = 6;
	}
	if(kfturn == 2 && lane == 2 && light ==4 )//white_dashed_noon straight
	{
		pversion = 3;
	}
	if (kfturn == 0 && lane == 4 && light == 4 ) //yellow_double_noon straight
	{
		pversion = 6;
	}
	if (kfturn == 0 && lane == 1 && light == 3 ) //white_con_night straight
	{
		pversion = 1;
	}
	if (kfturn == 0 && lane == 3 && light == 3 ) //yellow_con_night straight
	{
		pversion = 6;
	}
	if(kfturn == 0 && lane == 2 && light ==3)
	{
		pversion = 3;
	}
	return pversion;
}
int laneclassifier(torch::Tensor tensor_image)
{
	torch::jit::script::Module module = torch::jit::load("/home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/cpp_webots_api/lane_cpu.pt");
	at::Tensor output = module.forward({tensor_image}).toTensor();

	auto max_result = output.max(1, true);
	auto max_index = std::get<1>(max_result).item<float>();

	// if (max_index == 0){
	//     std::cout << "left\n";
	// }else{
	//     std::cout << "right\n";;
	// }
	return max_index;
}

int lightclassifier(torch::Tensor tensor_image)
{
	torch::jit::script::Module module = torch::jit::load("/home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/cpp_webots_api/light_cpu.pt");
	at::Tensor output = module.forward({tensor_image}).toTensor();

	auto max_result = output.max(1, true);
	auto max_index = std::get<1>(max_result).item<float>();

	// if (max_index == 0){
	//     std::cout << "left\n";
	// }else{
	//     std::cout << "right\n";;
	// }
	return max_index;
}

/* Function to set vehicle speed */
void set_speed(Driver *driver, double kmh)
{
	// Limit at Max vehicle speed.
	if (kmh > TOP_SPEED)
	{
		kmh = TOP_SPEED;
	}
	//update vehicle speed.
	speed = kmh;

	printf("setting speed to %g km/h\n", kmh);
	driver->setCruisingSpeed(kmh); //wbu_driver_set_cruising_speed(kmh);
}

// positive: turn right, negative: turn left
void set_steering_angle(Driver *driver, double wheel_angle)
{
	// limit the difference with previous steering_angle
	/*if (wheel_angle - steering_angle > 0.1)
    wheel_angle = steering_angle + 0.1;
  if (wheel_angle - steering_angle < -0.1)
    wheel_angle = steering_angle - 0.1;
  steering_angle = wheel_angle;
  // limit range of the steering angle
  if (wheel_angle > 0.5)
    wheel_angle = 0.5;
  else if (wheel_angle < -0.5)
    wheel_angle = -0.5;*/
	driver->setSteeringAngle(-wheel_angle); //wbu_driver_set_steering_angle(wheel_angle);
}

void compute_gps_speed(GPS *gps)
{
	const double *coords = gps->getValues(); //wb_gps_get_values(gps);
	/* Print position of car using GPS. For QoC measurments */
	//printf("Positional vertices are: %f,%f,%f \n", coords[0], coords[1], coords[2]);
	cout << " Positional vertices are: " << coords[0] << " , " <<coords[1] << ", "<<coords[2]<<endl;
	const double speed_ms = gps->getSpeed(); //wb_gps_get_speed(gps);
	// store into global variables
	gps_speed = speed_ms * 3.6; // convert from m/s to km/h
	memcpy(gps_coords, coords, sizeof(gps_coords));
	positionx.push_back(coords[0]);
	positiony.push_back(coords[2]);
}

void update_display(Display *display, Driver *driver)
{
	const double NEEDLE_LENGTH = 50.0;

	// display background
	display->imagePaste(speedometer_image, -50, 0, false); //wb_display_image_paste(display, speedometer_image, 0, 0, false);

	// draw speedometer needle
	double current_speed = driver->getCurrentSpeed(); //wbu_driver_get_current_speed();
	if (isnan(current_speed))
		current_speed = 0.0;
	double alpha = current_speed / 260.0 * 3.72 - 0.27;
	int x = -NEEDLE_LENGTH * cos(alpha);
	int y = -NEEDLE_LENGTH * sin(alpha);
	display->drawLine(100, 95, 100 + x, 95 + y); //wb_display_draw_line(display, 100, 95, 100 + x, 95 + y);

	// draw text
	char txt[64];
	sprintf(txt, "GPS coords: %.1f %.1f", gps_coords[X], gps_coords[Z]);
	display->drawText(txt, 10, 130); //wb_display_draw_text(display, txt, 10, 130);
	sprintf(txt, "GPS speed:  %.1f", gps_speed);
	display->drawText(txt, 10, 140); //wb_display_draw_text(display, txt, 10, 140);
}
/* Variables required for ca/snap/webots/current/usr/share/webots/include/controller/c/webots
mera object. */
// WbDeviceTag camera_front;
/* image quality is set as 100 (highest quality possible). */
// int image_quality = 100;

int main(int argc, char **argv)
{

	// wbu_driver_init();

	using namespace std;
	using namespace cv;

	if (argc != 2)
	{
		cout << "Usage: ./cpp_webots_main pipeline_version\n";
		return -1;
	}
	// ------- simulation parameters ----------//
	// float simstep = 0.005; // V-rep simulation step time
	// int simulation_time = 7;
	//string analysis = argv[2];
	//string env = argv[3];
	//string result_dir = "/home/sayandipde/Approx_IBC/final_app/results/";
	//string ret_ref = argv[4];
	//string Q_value = argv[5];
	//bool ret_reference = (ret_ref == "True");
	// cout << ret_reference << endl;

	// ------- single image wo control -------- //
	// vector<float> period_s {0.040, 0.025, 0.035, 0.040, 0.020, 0.035, 0.025, 0.020};
	// vector<float> tau_s {0.038, 0.0205, 0.0329, 0.0379, 0.0193, 0.0328, 0.0205, 0.0191};

	// ------- multiple image wo control -------- //
	// vector<float> period_s {0.040, 0.025, 0.030, 0.040, 0.020, 0.030, 0.020, 0.020};
	// vector<float> tau_s {0.0374, 0.0201, 0.0298, 0.0368, 0.0159, 0.0294, 0.0193, 0.0155};

	// ------- multiple image with control -------- //
	//vector<float> tau_s{0.04149, 0.02308, 0.02253, 0.04068, 0.02247, 0.02245, 0.02301, 0.02240, 0.03893, 0.02498, 0.03048, 0.03598};
	//vector<float> period_s{0.045, 0.025, 0.025, 0.045, 0.025, 0.025, 0.025, 0.025, 0.040, 0.025, 0.035, 0.040};
	vector<float> tau_ms{45.f, 25.f, 25.f, 45.f, 25.f, 25.f, 25.f, 25.f, 40.f, 25.f, 35.f, 40.f};
	vector<float> period_ms{45.f, 25.f, 25.f, 45.f, 25.f, 25.f, 25.f, 25.f, 40.f, 25.f, 35.f, 40.f};

    vector<float> tau_ms_bev{35.f, 15.f, 15.f, 30.f, 15.f, 15.f, 15.f, 15.f, 30.f, 25.f, 35.f, 40.f};
	vector<float> period_ms_bev{35.f, 15.f, 15.f, 30.f, 15.f, 15.f, 15.f, 15.f, 30.f, 25.f, 35.f, 40.f};
	int pipeline_version = atoi(argv[1]);
	cout << "pipeline_version: v" << pipeline_version << endl;

	// simulation main loop parameters
	Mat img_isp = cv::Mat::zeros(256, 512, CV_8UC3);
	long double steering_angle = 0.0L;
	long double prev_steering_angle = 0.0L;
	float temp_steering_angle = 0.0f;
	int tsa = 0;
	//float time = 0.0f;
	float curr_period = 0.0f;
	long double yL = 0.0L; // lateral deviation
	int kfturn=2;
	int light=1;
	int lane=3;
	double diserror;
	int activate = 0;
	// ------- init simulation ----------//

	laneDetection Lane_detection;
	lateralController30 Controller30;
	lateralController30_bev Controller30_bev;
	lateralController50_bev Controller50_bev;
	lateralController50 Controller50;
	//IBCController Controller;
	imageSignalProcessing ISP;
	//cout << curr_period << endl;
	curr_period = period_ms[pipeline_version];
	cout << curr_period << endl;
	Driver *driver = new Driver();
	// --- delay 2.5 sec to reach desired velocity ---//
	//time_step = 2.5 / simstep;
	//WBAPI.sim_delay(time_step);

	// camera_front = wb_robot_get_device("camera_front");
	// wb_camera_enable(camera_front, period_ms[pipeline_version]);

	// /* Enable features of the car. */
	// wbu_driver_set_hazard_flashers(true);
	// wbu_driver_set_dipped_beams(true);
	// wbu_driver_set_antifog_lights(true);
	// wbu_driver_set_wiper_mode(SLOW);
	for (int device_index = 0; device_index < driver->getNumberOfDevices(); ++device_index) // wb_robot_get_number_of_devices()
	{
		Device *device = driver->getDeviceByIndex(device_index); // wb_robot_get_device_by_index(device_index)
		const string name = device->getName();					 // wb_device_get_name(device)
		if (name == "display")									 //(strcmp(name, "display") == 0)
		{
			enable_display = true;
		}
		/*if any one of the camera is found, raise a flag that the camera has been found.*/
		else if (name == "camera_front") //(strcmp(name, "camera_front") == 0)
		{
			has_camera = true;
		}
		else if (name == "gps") //(strcmp(name, "gps") == 0)
		{
			has_gps = true;
		}
	}

	/* enable gps to know position of car */
	if (has_gps)
	{
		gps = driver->getGPS("gps");			 // wb_robot_get_device("gps")
		gps->enable(driver->getBasicTimeStep()); //wb_gps_enable(gps, TIME_STEP)
	}

	/* Enable camera, if found.*/
	if (has_camera)
	{
		camera_front = driver->getCamera("camera_front"); //wb_robot_get_device("camera_front");
		camera_front->enable(driver->getBasicTimeStep()); //wb_camera_enable(camera_front, TIME_STEP);
		camera_width = camera_front->getWidth();		  //wb_camera_get_width(camera_front);
		camera_height = camera_front->getHeight();		  //wb_camera_get_height(camera_front);
		camera_fov = camera_front->getFov();			  //wb_camera_get_fov(camera_front);
	}

	/* Enable display, if found. */
	if (enable_display)
	{
		display = driver->getDisplay("display");												   //wb_robot_get_device("display");
		speedometer_image = display->imageLoad("/home/yingkai/all_city/20200122/speedometer.png"); //wb_display_image_load(display, "speedometer.png");
																								   //speedometer_image = display->imageLoad("speedometer.png");
	}

	/* If camera is found, start the car by setting speed at VEH_SPEED. 
	This speed is maintianed throughout. */
	if (has_camera)
	{
		set_speed(driver, VEH_SPEED);
		// cout << "set speed" << endl;
	}

	/* Enable features of the car. */
	driver->setHazardFlashers(true);	//wbu_driver_set_hazard_flashers(true);
	driver->setDippedBeams(true);		//wbu_driver_set_dipped_beams(true);
	driver->setAntifogLights(true);		//wbu_driver_set_antifog_lights(true);
	driver->setWiperMode(driver->SLOW); //wbu_driver_set_wiper_mode(SLOW);

	//static int loop_count_delay = tau_ms[pipeline_version] / driver->getBasicTimeStep() - 1;
	//static int loop_count_period = curr_period / driver->getBasicTimeStep() - 1;
	//static int it_counter_period = 0;
	//static int it_counter_delay = 0;
	static int loop_count_delay = tau_ms[pipeline_version] / driver->getBasicTimeStep() - 1;
	static int loop_count_period = curr_period / driver->getBasicTimeStep() - 1;
	static int it_counter_period = 0;
	static int it_counter_delay = 0;
	bool first_actuate = 0;
	int count1 = 0;
	// ------- simulation main loop -------				string out_string = "//home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/cpp_webots_api/out_imgs";
	cout << "control period :" << curr_period << " msec" << endl;
	set_speed(driver, 50);
	
	while(driver->step() != -1 && driver->getTime() < 3.5)// right//3.5:straight
	{		
		driver->setSteeringAngle(0);
		// compute_gps_speed(gps);
		// diserror = getyL(gold_ref_csv, gps_coords[0], gps_coords[2], false) - (ROAD_WIDTH / 2); // - FINE_TUNE; // As gold_ref is the middle of the entire road, not lane
		// cout << "		error = "<< diserror << " m" << endl;
	}
	while (driver->step() != -1)
	{
		// while (time < simulation_time - curr_period)
		// {
		int vel = 50;
		int pipchar = atoi(argv[1]);
		if (first_actuate)
				{
					if (has_gps)
					{
						//cout << "Compute GPS speed" << endl;
						compute_gps_speed(gps);
					}

					if (gps_coords[0] > 20  )//sector all
					//if (gps_coords[0] > 20 && gps_coords[0] < 120 )//sector1
					//if (gps_coords[0] > 150 && gps_coords[2] < 130) //sector2
					//if (gps_coords[2] > 170 && gps_coords[2] < 270 )//sector3
					//if (gps_coords[2] >289 && gps_coords[0] < 210 )//sector4
					//if (gps_coords[0] > 250 && gps_coords[0] < 350 )//sector5
					//if (gps_coords[0] > 200 && gps_coords[1] < 255 )//sector6
					//if (gps_coords[2] > 175 && gps_coords[2] < 275 )//sector7
					//if (gps_coords[0] < 449.5 && gps_coords[2] < 145.5 )//sector8
					//if (gps_coords[0] > 480 && gps_coords[0] < 580 )//sector9
					{
						diserror = getyL(gold_ref_csv, gps_coords[0], gps_coords[2], false) - (ROAD_WIDTH / 2); // - FINE_TUNE; // As gold_ref is the middle of the entire road, not lane
						cout << "		error = "<< diserror << " m" << endl;
						asum = asum + fabs(diserror);
						//ssum = ssum + fabs(diserror) * fabs(diserror);
						Meansum = Meansum + diserror;
						kkkk = kkkk + 1;
						//MSE = ssum / kkkk;
						MAE = asum / kkkk;
						Meanv = Meansum / kkkk;
						std::ofstream outfile(yl_csv_dir + "/MAE" + to_string(pipchar) +".csv", std::ios_base::app);
						// //std::ofstream outfile(yl_csv_dir+"/MAE_case0.csv", std::ios_base::app);
					    outfile << to_string(driver->getTime()) + "," + to_string(gps_coords[0])+ "," + to_string(gps_coords[2]) + "," + to_string(diserror) + "," + to_string(MAE) << endl;
					}

					cout << "        Mean = "<< Meanv << " m" << "        MAE = "<< MAE << " m" << endl;
					// if (enable_display)
					// {
					// 	cout << "Update Display" << endl;
					// 	update_display(display, driver);
					// }
					// }
				}
		//if (driver->getTime() > 3.5)
		//{
			if (loop_count_period == 0)
			{
				// cout << "t = " << driver->getTime() << " sec" << endl;
				const unsigned char *camera_front_image = NULL;
				// cout << "it_counter = " << it_counter << " sec" << endl;

				// ------- sensing ----------//
				// img_wb = WBAPI.sim_sense();

				// if (first_actuate)
				// {
				// 	if (has_gps)
				// 	{
				// 		//cout << "Compute GPS speed" << endl;
				// 		compute_gps_speed(gps);
				// 	}

				// 	//if (gps_coords[0] > 20  )//sector all
				// 	//if (gps_coords[0] > 20 && gps_coords[0] < 120 )//sector1
				// 	if (gps_coords[0] > 137 && gps_coords[2] < 130) //sector2
				// 	//if (gps_coords[2] > 170 && gps_coords[2] < 270 )//sector3
				// 	//if (gps_coords[2] >289 && gps_coords[0] < 220 )//sector4
				// 	//if (gps_coords[0] > 250 && gps_coords[0] < 350 )//sector5
				// 	//if (gps_coords[0] > 200 && gps_coords[1] < 255 )//sector6
				// 	//if (gps_coords[2] > 175 && gps_coords[2] < 275 )//sector7
				// 	//if (gps_coords[0] < 449.5 && gps_coords[2] < 145.5 )//sector8
				// 	//if (gps_coords[0] > 480 && gps_coords[0] < 590 )//sector9
				// 	{
				// 		diserror = getyL(gold_ref_csv, gps_coords[0], gps_coords[2], false) - (ROAD_WIDTH / 2); // - FINE_TUNE; // As gold_ref is the middle of the entire road, not lane
				// 		cout << "		error = "<< diserror << " m" << endl;
				// 		asum = asum + fabs(diserror);
				// 		//ssum = ssum + fabs(diserror) * fabs(diserror);
				// 		Meansum = Meansum + diserror;
				// 		kkkk = kkkk + 1;
				// 		//MSE = ssum / kkkk;
				// 		MAE = asum / kkkk;
				// 		Meanv = Meansum / kkkk;
				// 		std::ofstream outfile(yl_csv_dir + "/MAE" + to_string(pipchar) +".csv", std::ios_base::app);
				// 		// //std::ofstream outfile(yl_csv_dir+"/MAE_case0.csv", std::ios_base::app);
				// 	    outfile << to_string(driver->getTime()) + "," + to_string(gps_coords[0])+ "," + to_string(gps_coords[2]) + "," + to_string(diserror) + "," + to_string(MAE) << endl;
				// 	}

				// 	cout << "        Mean = "<< Meanv << " m" << "        MAE = "<< MAE << " m" << endl;
				// 	// if (enable_display)
				// 	// {
				// 	// 	cout << "Update Display" << endl;
				// 	// 	update_display(display, driver);
				// 	// }
				// 	// }
				// }

				if (has_camera)
				{
					//read and save front camera image
					camera_front_image = camera_front->getImage(); //wb_camera_get_image(camera_front);
					cout << " [capture] t = " << driver->getTime() << " sec, Img = " << it_counter_period << endl;
					//printf(" [capture] t = %f sec, Img = %d  \n",driver->getTime(),it_counter_period);
					// cout << "1" << endl;
					UNUSED(camera_front_image);
					Mat webots_rgb =cv::Mat::zeros(256, 512, CV_8UC3); 
					Mat img_cls;
					// if(it_counter_period>=0)
					// {
					// string Img_Name = "/home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/cpp_webots_api/out_imgs/straight/noclass" +to_string(it_counter_period)+".png";
					// // imwrite(out_string+"/zcls.png", img_cls);
					// webots_rgb=imread(Img_Name);
					// }
					webots_rgb = webots_img_2_Mat(camera_front, camera_front_image);
					// //string out_string = "/home/yingkai-huang3/Approx_IBC_sim/final_app/cpp/cpp_webots_api/out_imgs";
					// // imwrite(out_string+"/rev.png", webots_rgb);
					// // double dur;
					// // clock_t start,end;
					// // start = clock();
					img_cls = image_for_classifier(webots_rgb);
					// // end = clock();
					// // dur Mat histogram =cv::Mat::zeros(256, 512, CV_8UC3);= (double)(end - start);
					// // printf("Use Time:%f\n",(dur/CLOCKS_PER_SEC*1000));
					// if(gps_coords[2] > 58 && gps_coords[2] < 60 && pipeline_version==0)
					// {
					// string Img_Name = "/home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/cpp_webots_api/out_imgs/left/l" +to_string(gps_coords[0])+".png";
					// // imwrite(out_string+"/zcls.png", img_cls);
					//  imwrite(Img_Name, img_cls);
					// }
					// if(gps_coords[2] > 50 && gps_coords[2] < 58 && pipeline_version==0)
					// {
					// string Img_Name = "/home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/cpp_webots_api/out_imgs/straight/s" +to_string(count1)+".png";
					// // imwrite(out_string+"/zcls.png", img_cls);
					// imwrite(Img_Name, img_cls);
					// }

					// if(gps_coords[2] <70 && gps_coords[2] >60 && pipeline_version==0)
					// {
					// string Img_Name = "/home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/cpp_webots_api/out_imgs/right/r" +to_string(count1)+".png";
					// // imwrite(out_string+"/zcls.png", img_cls);
					// imwrite(Img_Name, img_cls);
					// }
					// if(gps_coords[2] <60 && gps_coords[2] >50 && pipeline_version==0)
					// {
					// string Img_Name = "/home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/cpp_webots_api/out_imgs/straight/sssw" +to_string(count1)+".png";
					// // imwrite(out_string+"/zcls.png", img_cls);
					// imwrite(Img_Name, img_cls);
					// }
					torch::Tensor tensor_image, tensor_image_full;
					tensor_image = preprocessing(img_cls);
					tensor_image_full = preprocessing(webots_rgb);
					kfturn = bevclassifier(tensor_image);
					//kfturn=2;
					
					lane = laneclassifier(tensor_image);
					
					light = lightclassifier(tensor_image_full);
					//lane = 1;
					//light =3;
										// if(it_counter_period>=0)
					// {
					// string Img_Name = "/home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/cpp_webots_api/out_imgs/straight/noclass" +to_string(it_counter_period)+".png";
					// // imwrite(out_string+"/zcls.png", img_cls);
					// webots_rgb=imread(Img_Name);
					// }

					//cout<<"temp"<<endl;
					// light = 3;
					// if (gps_coords[0] > 450) //sector2
					// {
					// 	light = 0;
					// }
					// else
					// {
					// 	light = 3;
					// }
					printf(" current curve = %d \n",kfturn);
					printf(" current lane = %d \n",lane);
					printf(" current light = %d \n",light);
					// kfturn = 2;
					// if (gps_coords[0] < 137) //sector2
					// {
					// 	kfturn = 2;
					// }
					// else
					// {
					// 	kfturn = 1;
					// }
					// // if (gps_coords[2] > 153)//sector8
					// {
					// 	 kfturn = 2;
					// }
					// else
					// {
					// 	 kfturn = 1;
					// }
					// if (gps_coords[2] < 289 )// sector4
					// {
					// 	 kfturn = 2;
					// }
					// else
					// {
					// 	 kfturn = 0;
					// }
					/////////////////////////dynamic speed setting///////////////////////////////
					// if(gps_coords[0] < 85)
					// {
					// 	set_speed(driver, 50);
					// }
					// else
					// {
					// 	set_speed(driver, 30);
					// }
					// set_speed(driver, 50);
					if(kfturn ==2)
					{
						set_speed(driver, 50);
					}
					else
					{
						set_speed(driver, 30);
						if(driver->getCurrentSpeed()>35)
						{
							driver->setBrakeIntensity(1);
						}
						//driver->setBrakeIntensity(0.8);
					}
					////////////////////////speed flag setting////////////////////////////////////
					float curr_speed = driver->getCurrentSpeed();
					cout << "current speed=" << curr_speed << "km/h" << endl;
					int speedflag;
					if (curr_speed < 40)
					{
						speedflag = 30;
					}
					else
					{
						speedflag = 50;
					}
					

					if (pipeline_version < 9)
					{
						img_isp = ISP.approximate_pipeline(webots_rgb, pipeline_version);
					}
					else
					{
						img_isp = ISP.approximate_pipeline(webots_rgb, 0);
					}
					
					// ------- laneDetection -----------------//
					// tensor_isp = imagetotensor(img_isp);
					//////////////////////choose pipeline version////////////////////////
					// pipeline_version= pipelineswitcher(kfturn,lane,light, speedflag);
					// curr_period = period_ms[pipeline_version];
					// cout<< "current period = "<< curr_period<<"ms"<<endl;
					// cout<< "current pipeline version = "<< pipeline_version<<endl;
					//printf(" current pipeline version = %d \n",pipeline_version);
					yL=0.0L;
					yL = Lane_detection.lane_detection_pipeline(img_isp, kfturn, lane, light, it_counter_period);
					cout << "lateral deviation: " << yL << endl;
					prev_steering_angle = steering_angle;// to make the actuation part use the old steering angle, only used for tau == period
					// ------- control_compute --------------//
					if (speedflag == 30)
					{
						Controller30.compute_steering_angles(yL, it_counter_period, pipeline_version);
						steering_angle = Controller30.get_steering_angles();
						// ------- control_next_state  -----------//
						Controller30.estimate_next_state(it_counter_period, pipeline_version);
					}
					else
					{
						Controller50.compute_steering_angles(yL, it_counter_period, pipeline_version);
						steering_angle = Controller50.get_steering_angles();
						// ------- control_next_state  -----------//
						Controller50.estimate_next_state(it_counter_period, pipeline_version);
					}
                    pipeline_version= pipelineswitcher(kfturn,lane,light, speedflag);
					curr_period = period_ms[pipeline_version];
					// cout<< "current period = "<< curr_period<<"ms"<<endl;
					 cout<< "current pipeline version = "<< pipeline_version<<endl;
					printf(" current pipeline version = %d \n",pipeline_version);
					//std::ofstream outfile(yl_csv_dir + "/yL_pipe" + to_string(pipeline_version) + "_" + to_string(vel) + "kmph.csv", std::ios_base::app);
					std::ofstream outfile1(yl_csv_dir+"/yL_pipe_Case0.csv", std::ios_base::app);
					outfile1 << to_string(driver->getTime()) + "," + to_string(yL) + "," + to_string(steering_angle) << endl;
					// end2 = clock();
					// dur2 = (double)(end2 - start2);
					// printf("Use Time2:%f\n",(dur2/CLOCKS_PER_SEC*1000));
					// ------- imageSignalProcessing ----------//

					// cout << " [capture] t = " << driver->getTime() << " sec, Img = " << it_counter_period << endl;

					// ------- sensor-to-actuator delay ----------//
					// time_step = tau_ms[pipeline_version];
				}
				count1++;
				// Print position of car using GPS. For QoC measurments
				//const double *coords = wb_gps_get_values(gps);
				//printf(" The vertices are: %f,%f,%f \n",coords[0],coords[1],coords[2]);
				it_counter_period++;
				loop_count_period = curr_period / driver->getBasicTimeStep() - 1;
			}
			else
			{
				loop_count_period--;
			}
		//}
		//else
		// {
			
		// 	compute_gps_speed(gps);
		// 	steering_angle = 0.0f;
		// }
		

		if (loop_count_delay == 0)
		{
			if (has_camera && it_counter_delay > 0)
			{

				// ----- actuating -----//
				// [TODO] add actuatioCalculated yLn delay here (too small, neglected)
				// WBAPI.sim_actuate(steering_angles);
				tsa = prev_steering_angle*1000000;
				temp_steering_angle = (float)tsa/1000000;
				if (fabs(temp_steering_angle) > 100.0f)
				{
					// transfer the steering angle to global variable.
					steer90 = temp_steering_angle;
					// Set the steering angle after removing the offset angle
					if (steer90 > 0)
					{
						// right turn
						driver->setSteeringAngle((steer90 - offset_value)); //wbu_driver_set_steering_angle((steer90 - offset_value));
					}
					else
					{
						// left turn
						driver->setSteeringAngle((steer90 + offset_value)); //wbu_driver_set_steering_angle(steer90 + offset_value);
					}
				}
				else
				{
					set_steering_angle(driver, temp_steering_angle);
				}

				cout << " [actuate] t = " << driver->getTime() << " sec for Img = " << it_counter_delay - 1 << endl;
				first_actuate = 1;
			}
			it_counter_delay++;
			loop_count_delay = curr_period / driver->getBasicTimeStep() - 1;
		}
		else
		{
			loop_count_delay--;
		}
	}

	cout << "images: " << it_counter_period << endl;
	delete driver;

	return 0;
}
