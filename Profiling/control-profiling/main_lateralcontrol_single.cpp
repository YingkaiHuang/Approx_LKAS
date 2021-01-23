#include <opencv2/opencv.hpp>
#include "config.hpp"
#include "halide_benchmark.h"
#include "Halide.h"
#include <iostream>

#ifdef PROFILEWITHCHRONO
#include "../my_profiler.hpp" 
#endif

#ifdef PROFILEWITHVALGRIND
#include "valgrind/callgrind.h" 
#endif


#include <fstream>

using namespace std;
using namespace cv;
using namespace Halide;
using namespace Halide::Tools;

#ifdef PROFILEWITHCHRONO
template<class Container>
std::ostream& write_container(const Container& c, std::ostream& out, string pipeversion, char delimiter = '\n')
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


int main(){
	Mat input;
	laneDetection LaneDetection;
	lateralController Controller;
	long double yL = 0.0L;
	string in_string, out_string;
	// input = imread("./input_images/1.png");
	
	in_string = "imgs/output_fwd_v0.png";
	input = imread(in_string);
	
	//for (int version = 0; version <= 7; version++){
	//	cout << "- profiling version: " << version << endl;
	//	for (int i = 1; i < 201; i++){
	//		cout << i << endl;
	//		in_string = "../multiple-images/images/v"+to_string(version)+"/"+to_string(i)+".png";
	//		input = imread(in_string);
	
	yL = LaneDetection.lane_detection_pipeline(input);
	int pipe_version = 0;

	#ifdef PROFILEWITHCHRONO
		// Timing code
		// statistical
		vector<vector<double>> wc_avg_bc_tuples = do_benchmarking( [&]() {controller_call(yL, Controller, pipe_version);} );
		// printf("bc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[0][0], wc_avg_bc_tuples[0][1]);  
		// printf("avg {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[1][0], wc_avg_bc_tuples[1][1]);  
		// printf("wc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[2][0], wc_avg_bc_tuples[2][1]);  
		// std::ofstream outfile("results.csv");
		// write_container(wc_avg_bc_tuples[0], outfile);

		out_string = "chrono/runtime_lateralcontrol_single.csv";
		std::ofstream outfile(out_string);
		write_container(wc_avg_bc_tuples[0], outfile, "v0");
	#endif
	
	#ifdef PROFILEWITHVALGRIND
	CALLGRIND_TOGGLE_COLLECT;
	controller_call(yL, Controller, pipe_version);
	CALLGRIND_TOGGLE_COLLECT;
	CALLGRIND_DUMP_STATS;
	#endif
	
	cout << "\nLateral Deviation : " << yL << endl;
	
      // sampling 
    //   BenchmarkConfig config;
    //   config.accuracy = 0.001;
    //   BenchmarkResult result{0, 0, 0};
    //   result = Halide::Tools::benchmark( [&](){ controller_call(yL, Controller); }, config );
    //   cout << "\nBest elapsed wall-clock time per iteration (ms)           : " << result.wall_time * 1e3 << endl;
    //   cout << "Number of samples used for measurement                    : " << result.samples << endl;
    //   cout << "Total number of iterations across all samples             : " << result.iterations << endl;
    //   cout << "Measured accuracy between the best and third-best result  : " << result.accuracy << endl;

    //   // instruction
    //   cout << "\n";
    // do_instr_benchmarking([&](){controller_call(yL, Controller);});

	return 0;
}