#include <opencv2/opencv.hpp>
#include "config.hpp"
#include "halide_benchmark.h"
#include "Halide.h"
#include <iostream>

#ifdef PROFILEWITHCHRONO
#include "../my_profiler.hpp" 
#endif


#include <fstream>

using namespace std;
using namespace cv;
using namespace Halide;
using namespace Halide::Tools;

#ifdef PROFILEWITHCHRONO
template<class Container>
std::ostream& write_container(const Container& c, std::ostream& out, string pipeversion, char delimiter = ',')
{
    out << pipeversion;
    out << delimiter;
    bool write_sep = false;
    for (const auto& e: c) {
        if (write_sep)
            out << delimiter;
        else
            write_sep = true;
        out << e;
    }
    return out;
}
#endif

void controller_call(long double yL, lateralController Controller, int the_pipe_version){
    vector<long double> steering_angles(2);

      // ------- control_compute --------------//
    Controller.compute_steering_angles(yL, 0, the_pipe_version);
    steering_angles = Controller.get_steering_angles();

    // ------- control_next_state  -----------//
    Controller.estimate_next_state(0, the_pipe_version);
}


int main(int argc, char **argv){
	if (argc < 2) {
        printf("Usage: ./lateralcontrol_multiple.o input.png\n"
               "e.g.: ./lateralcontrol_multiple.o input.png\n");
        return 0;
    }
	const char * img_path = argv[1];
	Mat input;
	laneDetection LaneDetection;
	lateralController Controller;
	long double yL = 0.0L;
	vector<vector<double>> wc_avg_bc_tuples;
	string in_string, out_string;
		
	for (int version = 0; version <= 7; version++){
		cout << "- profiling version: " << version << endl;
		out_string = "chrono/runtime_lateralcontrol_multiple_v" + to_string(version) + "_.csv";
		std::ofstream outfile(out_string);
		for (int i = 0; i < 200; i++){
			cout << i << endl;
			in_string = std::string(img_path)+"v"+to_string(version)+"/isp_fwd/img_ispf_"+to_string(i)+".png";
			input = imread(in_string);

			// Timing code
			// statistical
			#ifdef PROFILEWITHCHRONO
			yL=LaneDetection.lane_detection_pipeline(input);
			if (yL == -1000) { cout << "FAIL" << endl; continue; }
			wc_avg_bc_tuples = do_benchmarking( [&]() {
				controller_call(yL, Controller, version);
			} );
			// printf("bc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[0][0], wc_avg_bc_tuples[0][1]);  
			// printf("avg {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[1][0], wc_avg_bc_tuples[1][1]);  
			// printf("wc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[2][0], wc_avg_bc_tuples[2][1]);  
			// std::ofstream outfile("results_ld.csv");
			// write_container(wc_avg_bc_tuples[0], outfile);
			
			write_container(wc_avg_bc_tuples[0], outfile, "img_"+to_string(i));
			outfile << "\n";
			
			#endif
		}
	}

	return 0;
}