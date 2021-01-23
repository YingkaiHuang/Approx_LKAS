#include <iostream>
#include <opencv2/opencv.hpp>
#include "config.hpp"

int main(int argc, char **argv) {
	using namespace std;
	using namespace cv;

	if (argc != 3) {
		cout << "Usage: ./cpp_vrep_main pipeline_version simulation_time\n";
		return -1;
	}
	// ------- simulation parameters ----------//		
	float simstep = 0.005; // V-rep simulation step time
	float simulation_time = atoi(argv[2]);
	//vector<float> period_s {0.07, 0.025, 0.06, 0.07, 0.025, 0.06, 0.025, 0.02};
	//vector<float> tau_s {0.065, 0.020, 0.055, 0.065, 0.020, 0.055, 0.020, 0.015};
	vector<float> period_s {0.040, 0.025, 0.035, 0.040, 0.020, 0.035, 0.025, 0.020};
	vector<float> tau_s {0.038, 0.0205, 0.0329, 0.0379, 0.0193, 0.0328, 0.0205, 0.0191};
	int pipeline_version = atoi(argv[1]);
	cout << "pipeline_version: v" << pipeline_version << endl;

	// simulation main loop parameters
    Mat img_vrep, img_isp;
	vector<long double> steering_angles(2);
	float time = 0.0f;
	long double yL = 0.0L; // lateral deviation
    int it_counter = 0;

	// ------- init simulation ----------//		
	vrepAPI VrepAPI;
    laneDetection Lane_detection;
    lateralController Controller;
    //IBCController Controller;
    imageSignalProcessing ISP;

	// --- delay 2.5 sec to reach desired velocity ---//		
	int time_step = 2.5 / simstep; 
	VrepAPI.sim_delay(time_step); 
	

	// ------- simulation main loop -------//
	while(time < simulation_time - period_s[pipeline_version]) {
		cout << "simulation_time: " << time << "\t";
		// ------- sensing ----------//	
		img_vrep = VrepAPI.sim_sense();
		
		
		if (it_counter <= 200) {
			string out_string;
			out_string = "v"+to_string(pipeline_version)+"/vrep/img_vrep_"+to_string(it_counter)+".png";
			imwrite(out_string, img_vrep);
		}

		// ------- imageSignalProcessing ----------//
		img_isp = ISP.approximate_pipeline(img_vrep, pipeline_version);
		//imwrite("img_isp.png", img_isp);

		// ------- laneDetection -----------------//
	    yL = Lane_detection.lane_detection_pipeline(img_isp);

	    // ------- control_compute --------------//
	    Controller.compute_steering_angles(yL, it_counter, pipeline_version);
	    steering_angles = Controller.get_steering_angles();

	    // ------- control_next_state  -----------//
	    Controller.estimate_next_state(it_counter, pipeline_version);


		// ------- sensor-to-actuator delay ----------//	
		time_step = tau_s[pipeline_version] / simstep;
		VrepAPI.sim_delay(time_step);  

		// ----- actuating -----//
		// [TODO] add actuation delay here (too small, neglected)
		VrepAPI.sim_actuate(steering_angles); 

		// ------- remaining delay to sampling period ----------//	
		time_step = (period_s[pipeline_version] - tau_s[pipeline_version])/ simstep;
		VrepAPI.sim_delay(time_step);  


		// ------- handle simulation time -------//
		time += period_s[pipeline_version];
		it_counter++;
	}
	cout << "images: " << it_counter << endl;

	// write yLs to file
	// vector<long double> yL_container = Lane_detection.get_yL_container();
	// write_yL_2_file(yL_container, pipeline_version);

	return 0;
}
