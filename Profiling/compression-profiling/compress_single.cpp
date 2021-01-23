#include "Halide.h"
#include "halide_benchmark.h"
//#include "halide_image_io.h"
//#include "halide_image.h"
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <math.h>
//#include <chrono> 
//#include "../Profiler.h"

#ifdef PROFILEWITHCHRONO
#include "../my_profiler.hpp" 
#endif

#ifdef PROFILEWITHVALGRINDCOMPRESS
#include "valgrind/callgrind.h" 
#endif

#ifdef PROFILEWITHVALGRINDDECOMPRESS
#include "valgrind/callgrind.h" 
#endif

using namespace Halide;
using namespace Halide::Tools;

using namespace cv;
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


int main(){
    //setUseOptimized(true);
    Mat src;

    // auto t_access_start = high_resolution_clock::now(); 

    // Mat src = imread("1.png", IMREAD_COLOR);// takes ~90ms  VREP_CAM

    // auto t_access_stop = high_resolution_clock::now(); 
    // auto t_access = duration<double, milli>(t_access_stop - t_access_start); 

    // auto t_save_start = high_resolution_clock::now(); 

    // imwrite("2.jpg", src);// takes ~90ms  VREP_CAM

    // auto t_save_stop = high_resolution_clock::now(); 
    // auto t_save = duration<double, milli>(t_save_stop - t_save_start);
    // cout << t_access.count() << "\n" << t_save.count() << "\n";
	
	string in_string, out_string1, out_string2,out_string3;
	
	//for (int version = 0; version <= 7; version++){
	//  cout << "- profiling version: " << version << endl;
	//  for (int i = 1; i < 201; i++){
		
    in_string = "imgs/output_fwd_v0.png";
	//in_string = "imgs/vrep.png";
	vector<int> compression_params;
    compression_params.push_back(IMWRITE_JPEG_QUALITY);
	compression_params.push_back(100);
	//compression_params.push_back(90);

	
	#ifdef PROFILEWITHCHRONO
	vector<vector<double>> wc_avg_bc_tuples;
	//////////////////////////////////////////////////////////////////////////////////////////////////
    // COMPRESS Timing code
    // statistical
	cout << "Compress: \n";
        out_string1 = "./jpegs/single_v0.jpg";
	src = imread(in_string, IMREAD_COLOR);
	wc_avg_bc_tuples = do_benchmarking( [&]() {imwrite(out_string1, src, compression_params);} );
	
	out_string2 = "chrono/runtime_compress_v0_single.csv";
    std::ofstream outfile2(out_string2);
    write_container(wc_avg_bc_tuples[0], outfile2, "v0");
	
	//////////////////////////////////////////////////////////////////////////////////////////////////
	// DECOMPRESS Timing code
	// statistical
    
	cout << "\nDecompress: \n";
    wc_avg_bc_tuples = do_benchmarking( [&]() {src = imread(out_string1, IMREAD_COLOR);} );
    // printf("bc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[0][0], wc_avg_bc_tuples[0][1]);  
    // printf("avg {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[1][0], wc_avg_bc_tuples[1][1]);  
    // printf("wc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[2][0], wc_avg_bc_tuples[2][1]);  
    // std::ofstream outfile("decompress.csv");
    // write_container(wc_avg_bc_tuples[0], outfile);
    out_string3 = "chrono/runtime_decompress_v0_single.csv";
    std::ofstream outfile(out_string3);
    write_container(wc_avg_bc_tuples[0], outfile, "v0");
	#endif
	
	#ifdef PROFILEWITHVALGRINDCOMPRESS
	out_string1 = "./jpegs/single_v0.jpg";
	src = imread(in_string, IMREAD_COLOR);
	CALLGRIND_TOGGLE_COLLECT;
	imwrite(out_string1, src, compression_params);
	CALLGRIND_TOGGLE_COLLECT;
	CALLGRIND_DUMP_STATS;
	src = imread(out_string1, IMREAD_COLOR);
	#endif
	
	#ifdef PROFILEWITHVALGRINDDECOMPRESS
	out_string1 = "./jpegs/single_v0.jpg";
	src = imread(in_string, IMREAD_COLOR);
	imwrite(out_string1, src, compression_params);
	CALLGRIND_TOGGLE_COLLECT;
	src = imread(out_string1, IMREAD_COLOR);
	CALLGRIND_TOGGLE_COLLECT;
	CALLGRIND_DUMP_STATS;
	#endif
	
    // sampling 
    // BenchmarkConfig config;
    // config.accuracy = 0.001;
    // BenchmarkResult result{0, 0, 0};
    // result = Halide::Tools::benchmark( [&](){ src = imread("2.jpg", IMREAD_COLOR); }, config );
    // cout << "\nBest elapsed wall-clock time per iteration (ms)           : " << result.wall_time * 1e3 << endl;
    // cout << "Number of samples used for measurement                    : " << result.samples << endl;
    // cout << "Total number of iterations across all samples             : " << result.iterations << endl;
    // cout << "Measured accuracy between the best and third-best result  : " << result.accuracy << endl;

    // // instruction
    // cout << "\n";
    // do_instr_benchmarking([&](){ src = imread("2.jpg", IMREAD_COLOR); });

    // printf("bc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[0][0], wc_avg_bc_tuples[0][1]);  
    // printf("avg {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[1][0], wc_avg_bc_tuples[1][1]);  
    // printf("wc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[2][0], wc_avg_bc_tuples[2][1]);  
    // std::ofstream outfile2("compress.csv");
    // write_container(wc_avg_bc_tuples[0], outfile2);
  

    // // sampling 
    // result = Halide::Tools::benchmark( [&](){ imwrite("2.jpg", src); }, config );
    // cout << "\nBest elapsed wall-clock time per iteration (ms)           : " << result.wall_time * 1e3 << endl;
    // cout << "Number of samples used for measurement                    : " << result.samples << endl;
    // cout << "Total number of iterations across all samples             : " << result.iterations << endl;
    // cout << "Measured accuracy between the best and third-best result  : " << result.accuracy << endl;

    // // instruction
    // cout << "\n";
    // do_instr_benchmarking([&](){ imwrite("2.jpg", src); });

    return 0;
}



