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

using namespace std;
using namespace cv;
using namespace Halide;
using namespace Halide::Tools;

int main(int argc, char **argv){
	if (argc < 2) {
        printf("Usage: ./lanedetect_multiple.o input.png\n"
               "e.g.: ./lanedetect_multiple.o input.png\n");
        return 0;
    }
	const char * img_path = argv[1];
	Mat input;
	laneDetection LaneDetection;
	long double yL = 0.0L;
	vector<vector<double>> wc_avg_bc_tuples;
	string in_string, out_string;
		
	for (int version = 0; version <= 7; version++){
		cout << "- profiling version: " << version << endl;
		out_string = "chrono/runtime_lanedetect_multiple_v" + to_string(version) + "_.csv";
		std::ofstream outfile(out_string);
		for (int i = 0; i < 200; i++){
			cout << i << endl;
			in_string = std::string(img_path)+"v"+to_string(version)+"/isp_fwd/img_ispf_"+to_string(i)+".png";
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
			
			if (yL == -1000) { cout << "FAIL" << endl; continue; }
			
			write_container(wc_avg_bc_tuples[0], outfile, "img_"+to_string(i));
			outfile << "\n";
			
			#endif
		}
	}
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