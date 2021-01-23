#include <iostream>
//#include <sstream>
#include <opencv2/opencv.hpp>
#include "config.hpp"
extern "C"
{
#include "extApi.h"
#include <webots/camera.h>
#include <webots/device.h>
#include <webots/display.h>
#include <webots/keyboard.h>
#include <webots/robot.h>
#include <webots/supervisor.h>
#include <webots/vehicle/driver.h>

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <pthread.h>
// #include <Python.h>

#include <unistd.h> // for close
#include <arpa/inet.h>
}

/* Variables required for ca/snap/webots/current/usr/share/webots/include/controller/c/webots
mera object. */
WbDeviceTag camera_front;
/* image quality is set as 100 (highest quality possible). */
int image_quality = 100;
#define CAMERA_NO 1
volatile bool steer_delay = false;
/* call back function. This will be executed upon expiring of timer. */
void (*timer_func_handler_pntr)(void);
/* struct to store timer value. */
struct itimerval timervalue;
/* Struct to hold actions upon timer expiration. */
struct sigaction new_handler, old_handler;
/* Function for signal handler of the timer. */
void timer_sig_handler(int);

/* Function to start the timer. Once mSec (the timeout value in ms) has passed,a timeout function (the second argument) will be called. */
int start_timer(int mSec, void (*timer_func_handler)(void))
{
	//Make the second argument (timeout function) as the call back function.
	timer_func_handler_pntr = timer_func_handler;
	//declare the next timer value (in seconds and microseconds).
	timervalue.it_interval.tv_sec = mSec / 1000;
	timervalue.it_interval.tv_usec = (mSec % 1000) * 1000;
	//declare the current timer value (in seconds and microseconds).
	timervalue.it_value.tv_sec = mSec / 1000;
	timervalue.it_value.tv_usec = (mSec % 1000) * 1000;

	//set the timer to timervalue by decrementing in Real time.
	if (setitimer(ITIMER_REAL, &timervalue, NULL))
	{
		//error case.
		printf("\nsetitimer() error\n");
		return (1);
	}

	//Initalize the new handler with the call back function. This handler will be executed upon timer expiration.
	new_handler.sa_handler = &timer_sig_handler;
	new_handler.sa_flags = SA_NOMASK;

	//Upon raising the SIGALRM(which happens when timer expires), execute "new_handler" which calls the call back function.
	if (sigaction(SIGALRM, &new_handler, &old_handler))
	{
		printf("\nsigaction() error\n");
		return (1);
	}
	return (0);
}

/* Function which calls the call back function. */
void timer_sig_handler(int arg)
{
	timer_func_handler_pntr();
}

/* Function to stop the timer. */
void stop_timer(void)
{
	//Reset the values.
	timervalue.it_interval.tv_sec = 0;
	timervalue.it_interval.tv_usec = 0;
	timervalue.it_value.tv_sec = 0;
	timervalue.it_value.tv_usec = 0;

	setitimer(ITIMER_REAL, &timervalue, NULL);
	sigaction(SIGALRM, &old_handler, NULL);
}

void steering_delay(void)
{
	steer_delay = true;
	//stop the timer.
	stop_timer();
}

int main(int argc, char **argv)
{

	wbu_driver_init();

	using namespace std;
	using namespace cv;

	if (argc != 2)
	{
		cout << "Usage: ./cpp_vrep_main pipeline_version\n";
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
	vector<float> period_s{0.040, 0.025, 0.035, 0.040, 0.020, 0.030, 0.020, 0.020, 0.040};
	vector<float> tau_s{0.0379, 0.0206, 0.0303, 0.0373, 0.0164, 0.0299, 0.0198, 0.0160, 0.0359};
	vector<float> period_ms{40, 25, 35, 40, 20, 30, 20, 20, 40};
	vector<float> tau_ms{37.9, 20.6, 30.3, 37.3, 16.4, 29.9, 19.8, 16, 35.9};

	int pipeline_version = atoi(argv[1]);
	cout << "pipeline_version: v" << pipeline_version << endl;

	// simulation main loop parameters
	Mat img_wb, img_isp;
	long double steering_angles = 0.0L;
	float time = 0.0f;
	float curr_period = 0.0f;
	long double yL = 0.0L; // lateral deviation
	int it_counter = 0;
	int time_step;
	volatile bool steer_delay = false;
	// ------- init simulation ----------//

	laneDetection Lane_detection;
	lateralController Controller;
	//IBCController Controller;
	imageSignalProcessing ISP;

	// --- delay 2.5 sec to reach desired velocity ---//
	//time_step = 2.5 / simstep;
	//WBAPI.sim_delay(time_step);
	curr_period = period_ms[pipeline_version];
	camera_front = wb_robot_get_device("camera_front");
	wb_camera_enable(camera_front, period_ms[pipeline_version]);

	/* Enable features of the car. */
	wbu_driver_set_hazard_flashers(true);
	wbu_driver_set_dipped_beams(true);
	wbu_driver_set_antifog_lights(true);
	wbu_driver_set_wiper_mode(SLOW);

	// ------- simulation main loop -------//
	cout << "control period :" << curr_period << " msec" << endl;
	while (wbu_driver_step() != -1)
	{
		// while (time < simulation_time - curr_period)
		// {
		cout << "simulation_time: " << time << "\t";

		// ------- sensing ----------//
		// img_wb = WBAPI.sim_sense();

		const unsigned char *camera_front_image = NULL;
		camera_front_image = wb_camera_get_image(camera_front);
		const int b = 3;
		const int width = wb_camera_get_width(camera_front);
		const int height = wb_camera_get_height(camera_front);
		Mat out_Mat(height, width, CV_8UC3);
		vector<Mat> three_channels;
		split(out_Mat, three_channels);
		// Convert to BGR memory storage
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				// Blue channel
				three_channels[0].at<char>(height - y - 1, x) = camera_front_image[b * (y * width + x) + 2];
				// Green channel
				three_channels[1].at<char>(height - y - 1, x) = camera_front_image[b * (y * width + x) + 1];
				// Red channel
				three_channels[2].at<char>(height - y - 1, x) = camera_front_image[b * (y * width + x) + 0];
			}
		}
		merge(three_channels, out_Mat);
		img_wb = out_Mat;

		// ------- imageSignalProcessing ----------//
		img_isp = ISP.approximate_pipeline(img_wb, pipeline_version);

		// ------- laneDetection -----------------//
		yL = Lane_detection.lane_detection_pipeline(img_isp);
		cout << "lateral deviation: " << yL << endl;

		// ------- control_compute --------------//
		Controller.compute_steering_angles(yL, it_counter, pipeline_version);
		steering_angles = Controller.get_steering_angles();

		// ------- control_next_state  -----------//
		Controller.estimate_next_state(it_counter, pipeline_version);

		// ------- sensor-to-actuator delay ----------//
		time_step = tau_ms[pipeline_version];

		start_timer(time_step, &steering_delay);

		// ----- actuating -----//
		// [TODO] add actuation delay here (too small, neglected)
		// WBAPI.sim_actuate(steering_angles);
		wbu_driver_set_steering_angle(steering_angles);

		// ------- remaining delay to sampling period ----------//
		time_step = (period_ms[pipeline_version] - tau_ms[pipeline_version]);
		start_timer(time_step, &steering_delay);

		// ------- handle simulation time -------//
		time += period_ms[pipeline_version];

		it_counter++;
		// }
	}

	cout << "images: " << it_counter << endl;
	wbu_driver_cleanup();

	return 0;
}
