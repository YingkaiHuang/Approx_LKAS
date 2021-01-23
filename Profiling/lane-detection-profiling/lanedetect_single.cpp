#include <opencv2/opencv.hpp>
#include "config.hpp"
//#include "../Profiler.h"

#ifdef PROFILEWITHCHRONO
#include "../my_profiler.hpp" 
#endif

#ifdef PROFILEWITHVALGRIND
#include "valgrind/callgrind.h" 
#endif

#include "halide_benchmark.h"
#include "Halide.h"

#include <fstream>

using namespace std;

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

using namespace std;
using namespace cv;
using namespace Halide;
using namespace Halide::Tools;

int main(){
	Mat input;
	laneDetection LaneDetection;
	long double yL = 0.0L;
	vector<vector<double>> wc_avg_bc_tuples;
	string in_string, out_string;

    in_string = "imgs/output_fwd_v0.png";
    //in_string = "imgs/dem.raw.png";
	//in_string = "imgs/isp.raw_v0.png";
	//in_string = "imgs/dem.output.png";
	//in_string = "imgs/img_isp_v5_136.png";
	// in_string = "imgs/img_isp_v7_136.png";
	input = imread(in_string);

    // Timing code
    // statistical
	#ifdef PROFILEWITHCHRONO
    wc_avg_bc_tuples = do_benchmarking( [&]() {
        yL=LaneDetection.lane_detection_pipeline(input);
    } );
    // printf("bc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[0][0], wc_avg_bc_tuples[0][1]);  
    // printf("avg {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[1][0], wc_avg_bc_tuples[1][1]);  
    // printf("wc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[2][0], wc_avg_bc_tuples[2][1]);  
    // std::ofstream outfile("results_ld.csv");
    // write_container(wc_avg_bc_tuples[0], outfile);
	out_string = "chrono/runtime_lanedetect_single.csv";
	std::ofstream outfile(out_string);
	write_container(wc_avg_bc_tuples[0], outfile, "v0");
	#endif
	
	#ifdef PROFILEWITHVALGRIND
	CALLGRIND_TOGGLE_COLLECT;
	yL=LaneDetection.lane_detection_pipeline(input);
	CALLGRIND_TOGGLE_COLLECT;
	CALLGRIND_DUMP_STATS;
	#endif
	
	#ifdef NOPROFILE
	yL=LaneDetection.lane_detection_pipeline(input);
	cout << yL*10 <<endl;
	#endif
  
  // // sampling 
  // BenchmarkConfig config;
  // config.accuracy = 0.001;
  // BenchmarkResult result{0, 0, 0};
  // result = Halide::Tools::benchmark( [&](){ yL=LaneDetection.lane_detection_pipeline(input); }, config );
  // cout << "\nBest elapsed wall-clock time per iteration (ms)           : " << result.wall_time * 1e3 << endl;
  // cout << "Number of samples used for measurement                    : " << result.samples << endl;
  // cout << "Total number of iterations across all samples             : " << result.iterations << endl;
  // cout << "Measured accuracy between the best and third-best result  : " << result.accuracy << endl;

  // // instruction
  // cout << "\n";
  // do_instr_benchmarking([&](){yL = LaneDetection.lane_detection_pipeline(input);});


	return 0;
}
