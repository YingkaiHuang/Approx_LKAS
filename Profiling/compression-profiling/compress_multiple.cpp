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

using namespace Halide;
using namespace Halide::Tools;

using namespace cv;
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


int main(int argc, char **argv){
    //setUseOptimized(true);
	if (argc < 2) {
        printf("Usage: ./compress_multiple.o input.png\n"
               "e.g.: ./compress_multiple.o input.png\n");
        return 0;
    }
	const char * img_path = argv[1];
    
	Mat src;

	string in_string, out_string1, out_string2,out_string3;
	vector<vector<double>> wc_avg_bc_tuples;
	vector<int> compression_params;
    compression_params.push_back(IMWRITE_JPEG_QUALITY);
	compression_params.push_back(100);
	
	for (int version = 0; version <= 7; version++){
		cout << "- profiling version: " << version << endl;
		out_string2 = "chrono/runtime_compress_multiple_v" + to_string(version) + "_.csv";
		out_string3 = "chrono/runtime_decompress_multiple_v" + to_string(version) + "_.csv";;
		std::ofstream outfile2(out_string2);
		std::ofstream outfile3(out_string3);
		for (int i = 0; i < 200; i++){
			cout << i << endl;
			
			#ifdef PROFILEWITHCHRONO
			in_string = std::string(img_path)+"v"+to_string(version)+"/isp_fwd/img_ispf_"+to_string(i)+".png";
			out_string1 = std::string(img_path)+"v"+to_string(version)+"/jpegs/img_jpg_"+to_string(i)+".jpg";
			
			//////////////////////////////////////////////////////////////////////////////////////////////////
			// COMPRESS Timing code
			// statistical
			// cout << "Compress: \n";
			src = imread(in_string, IMREAD_COLOR);
			wc_avg_bc_tuples = do_benchmarking( [&]() {imwrite(out_string1, src, compression_params);} );

			write_container(wc_avg_bc_tuples[0], outfile2, "img_"+to_string(i));
			outfile2 << "\n";
	
			//////////////////////////////////////////////////////////////////////////////////////////////////
			// DECOMPRESS Timing code
			// statistical
    
			// cout << "\nDecompress: \n";
			wc_avg_bc_tuples = do_benchmarking( [&]() {src = imread(out_string1, IMREAD_COLOR);} );
			// printf("bc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[0][0], wc_avg_bc_tuples[0][1]);  
			// printf("avg {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[1][0], wc_avg_bc_tuples[1][1]);  
			// printf("wc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[2][0], wc_avg_bc_tuples[2][1]);  
			// std::ofstream outfile("decompress.csv");
			// write_container(wc_avg_bc_tuples[0], outfile);
			write_container(wc_avg_bc_tuples[0], outfile3, "img_"+to_string(i));
			outfile3 << "\n";
			#endif
		}
	}

    return 0;
}



