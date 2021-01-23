// Halide tutorial lesson 21: Auto-Scheduler

// Before reading this file, see lesson_21_auto_scheduler_generate.cpp

// This is the code that actually uses the Halide pipeline we've
// compiled. It does not depend on libHalide, so we won't be including
// Halide.h.
//
// Instead, it depends on the header files that lesson_21_auto_scheduler_generator produced.
#include "auto_schedule_true_rev.h"
#include "auto_schedule_true_fwd_v0.h"
#include "auto_schedule_true_fwd_v1.h"
#include "auto_schedule_true_fwd_v2.h"
#include "auto_schedule_true_fwd_v3.h"
#include "auto_schedule_true_fwd_v4.h"
#include "auto_schedule_true_fwd_v5.h"
#include "auto_schedule_true_fwd_v6.h"

// We'll use the Halide::Runtime::Buffer class for passing data into and out of
// the pipeline.
#include "Halide.h"
#include "halide_benchmark.h"
#include "halide_image_io.h"
#include "halide_image.h"

#include "LoadCamModel.h"
#include "MatrixOps.h"
#include <stdio.h>
#include <math.h>

#ifdef PROFILEWITHCHRONO
#include "../my_profiler.hpp" 
#endif

#ifdef PROFILEWITHVALGRIND
#include "callgrind.h" 
#endif

#include <fstream>


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

/////////////////////////////////////////////////////////////////////////////////////////
// Image Processing Pipeline
//
// This is a Halide implementation of a pre-learned image 
// processing model. A description of the model can be found in
// "A New In-Camera Imaging Model for Color Computer Vision 
// and its Application" by Seon Joo Kim, Hai Ting Lin, 
// Michael Brown, et al. Code for learning a new model can 
// be found at the original project page. This particular 
// implementation was written by Mark Buckler.
//
// Original Project Page:
// http://www.comp.nus.edu.sg/~brown/radiometric_calibration/
//
// Model Format Readme:
// http://www.comp.nus.edu.sg/~brown/radiometric_calibration/datasets/Model_param/readme.pdf
//
/////////////////////////////////////////////////////////////////////////////////////////



// Function prototypes
int run_pipeline(bool direction, int version, int img_no, const char * path, std::ostream& out);


int main(int argc, char **argv) {
  using namespace std;

  if (argc != 2) {
    printf("Usage: ./isp_single.o path_to_imgs");
  }
  const char * img_path = argv[1];
  string out_string;

  printf("Starting script. Note that processing may take several minutes for large images\n");

  for (int version = 0; version <= 7; version++){
		cout << "- profiling version: " << version << endl;
		out_string = "chrono/runtime_isp_multiple_v" + to_string(version) + "_.csv";
		std::ofstream outfile(out_string);
		for (int i = 0; i < 200; i++){
			//Run backward pipeline
			printf("Running backward pipeline...\n");
			run_pipeline(false, version, i, img_path, outfile);
			printf("Backward pipeline complete\n");

			// Run forward pipeline
			printf("Running forward pipeline...\n");
			run_pipeline(true, version, i, img_path, outfile);
			printf("Forward pipeline complete\n");
		}
	}

  printf("Success!\n");
  return 0;
}


// Reversible pipeline function
int run_pipeline(bool direction, int version, int img_no, const char * path, std::ostream& out) {
    using namespace std;  

  ///////////////////////////////////////////////////////////////
  // Input Image Parameters
  ///////////////////////////////////////////////////////////////
  string denoised_image = std::string(path)+"v"+to_string(version)+"/isp_rev/img_ispr_"+to_string(img_no)+".png";
  string jpg_image = std::string(path)+"v"+to_string(version)+"/vrep/img_vrep_"+to_string(img_no)+".png";
  // Demosaiced raw input (to forward pipeline)
  //char denoised_image[] = 
  //"../../imgs/NikonD7000FL/DSC_0916.NEF.demos_3C.png";
  //"./imgs/output_rev.png";
  //in_string;


  // Jpeg input to backward pipeline. Must be converted to png
  //char jpg_image[] =
  //"/home/kbimpisi/Approx_IBC_offline/ReversiblePipeline/DSC_0799.NEF.dcraw.png";
  //"../../imgs/NikonD7000FL/DSC_0916.png";
  //"./imgs/vrep.png";
  //std::string(path)+"v"+to_string(version)+"/vrep/img_vrep_"+to_string(img_no)+".png";

  ///////////////////////////////////////////////////////////////////////////////////////
  // Import and format model data: cpp -> halide format

  // Declare model parameters
  vector<vector<float>> Ts, Tw, TsTw;
  vector<vector<float>> ctrl_pts, weights, coefs;
  vector<vector<float>> rev_tone;

  // Load model parameters from file
  // NOTE: Ts, Tw, and TsTw read only forward data
  // ctrl_pts, weights, and coefs are either forward or backward
  // tone mapping is always backward
  // This is due to the the camera model format
  ///////////////////////////////////////////////////////////////
  // Camera Model Parameters
  ///////////////////////////////////////////////////////////////

  // Path to the camera model to be used
  char cam_model_path[] =
  "./camera_models/NikonD7000/";

  // White balance index (select white balance from transform file)
  // The first white balance in the file has a wb_index of 1
  // For more information on model format see the readme
  int wb_index = 3 ; // Actual choice = Given no. + 1
  //4; //6 [SD]

  // Number of control points
  const int num_ctrl_pts = 
  3702;

  Ts        = get_Ts       (cam_model_path);
  Tw        = get_Tw       (cam_model_path, wb_index);
  TsTw      = get_TsTw     (cam_model_path, wb_index);
  ctrl_pts  = get_ctrl_pts (cam_model_path, num_ctrl_pts, direction);
  weights   = get_weights  (cam_model_path, num_ctrl_pts, direction);
  coefs     = get_coefs    (cam_model_path, num_ctrl_pts, direction);
  rev_tone  = get_rev_tone (cam_model_path);

  // Take the transpose of the color map and white balance transform for later use
  vector<vector<float>> TsTw_tran = transpose_mat (TsTw);

  // If we are performing a backward implementation of the pipeline, 
  // take the inverse of TsTw_tran
  if (direction == 0) { 
    TsTw_tran = inv_3x3mat(TsTw_tran);
  }

  using namespace Halide;
  using namespace Halide::Tools;

  // Convert control points to a Halide image
  int width  = ctrl_pts[0].size();
  int length = ctrl_pts.size();

  // cout << "\n- ctrl_pts_h(width,length): ";
  // cout << width << ", " << length << endl;

  Halide::Runtime::Buffer<float> ctrl_pts_h(width,length);
  for (int y=0; y<length; y++) {
    for (int x=0; x<width; x++) {
      ctrl_pts_h(x,y) = ctrl_pts[y][x];
    }
  }
  // Convert weights to a Halide image
  width  = weights[0].size();
  length = weights.size();

  // cout << "\n- weights_h(width,length): ";
  // cout << width << ", " << length << endl;

  Halide::Runtime::Buffer<float> weights_h(width,length);
  for (int y=0; y<length; y++) {
    for (int x=0; x<width; x++) {
      weights_h(x,y) = weights[y][x];
    }
  }
  // Convert the reverse tone mapping function to a Halide image
  width  = 3;
  length = 256;
  Halide::Runtime::Buffer<float> rev_tone_h(width,length);
  for (int y=0; y<length; y++) {
    for (int x=0; x<width; x++) {
      rev_tone_h(x,y) = rev_tone[y][x];
    }
  }
 

  // Convert the TsTw_tran to a Halide image
  width  = 3;
  length = 3;
  Halide::Runtime::Buffer<float> TsTw_tran_h(width,length);
  for (int y=0; y<length; y++) {
    for (int x=0; x<width; x++) {
      TsTw_tran_h(x,y) = TsTw_tran[x][y];
    }
  }

  // Convert the coefs to a Halide image
  width  = 4;
  length = 3;
  Halide::Runtime::Buffer<float> coefs_h(width,length);
  for (int y=0; y<length; y++) {
    for (int x=0; x<width; x++) {
      coefs_h(x,y) = coefs[x][y];
    }
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  // Import and format input image

  // Declare image handle variables
  Var x, y, c;

  Halide::Runtime::Buffer<uint8_t> input;
  // Load input image 
  if (direction == 1) {
    // If using forward pipeline
    input = load_and_convert_image(denoised_image);
  } else {
    // If using backward pipeline
    input = load_image(jpg_image);
  }

  ////////////////////////////////////////////////////////////////////////
  // run the generator

  Halide::Runtime::Buffer<uint8_t> output(input.width(), input.height(), input.channels());
  // int version = 0;
  //int samples          = 1; //2
  //int iterations       = 1; //5
  double auto_schedule_on = 0.0;
  if (direction == 1) {
    
    vector<vector<double>> wc_avg_bc_tuples;
    BenchmarkConfig config;
    config.accuracy = 0.001;
    BenchmarkResult result{0, 0, 0};
      switch(version){
        case 0: // full rev + fwd                   skip 0 stages
          {//////////////////////////////////////////////////////////////////////////////////////////////////
                    // Timing code
                    // statistical
					#ifdef PROFILEWITHCHRONO
                     wc_avg_bc_tuples = do_benchmarking( [&]() {
                        auto_schedule_true_fwd_v0(input, ctrl_pts_h, weights_h, rev_tone_h, TsTw_tran_h, coefs_h, output);
                    } );
                    // printf("bc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[0][0], wc_avg_bc_tuples[0][1]);  
                    // printf("avg {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[1][0], wc_avg_bc_tuples[1][1]);  
                    // printf("wc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[2][0], wc_avg_bc_tuples[2][1]);  
                    write_container(wc_avg_bc_tuples[0], out, "img_"+to_string(img_no));
					out << "\n";
					#endif
					
                    break;}
        case 1: // rev + fwd_transform_tonemap      skip 1 stage
          {/////////////////////////////////////////////////////////////////////////////////////////////////
                    // Timing code
                    // statistical
					#ifdef PROFILEWITHCHRONO
                    wc_avg_bc_tuples = do_benchmarking( [&]() {
                        auto_schedule_true_fwd_v1(input, ctrl_pts_h, weights_h, rev_tone_h, TsTw_tran_h, coefs_h, output);
                    } );
                    // printf("bc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[0][0], wc_avg_bc_tuples[0][1]);  
                    // printf("avg {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[1][0], wc_avg_bc_tuples[1][1]);  
                    // printf("wc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[2][0], wc_avg_bc_tuples[2][1]);  
                    write_container(wc_avg_bc_tuples[0], out, "img_"+to_string(img_no));
					out << "\n";
					#endif
					
					break;}      
        case 2: // rev + fwd_transform_gamut        skip 1 stage
          {//////////////////////////////////////////////////////////////////////////////////////////////////
                    // Timing code
                    // statistical
					#ifdef PROFILEWITHCHRONO
                     wc_avg_bc_tuples = do_benchmarking( [&]() {
                        auto_schedule_true_fwd_v2(input, ctrl_pts_h, weights_h, rev_tone_h, TsTw_tran_h, coefs_h, output);
                    } );
                    // printf("bc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[0][0], wc_avg_bc_tuples[0][1]);  
                    // printf("avg {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[1][0], wc_avg_bc_tuples[1][1]);  
                    // printf("wc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[2][0], wc_avg_bc_tuples[2][1]);  
                    write_container(wc_avg_bc_tuples[0], out, "img_"+to_string(img_no));
					out << "\n";
					#endif
					
					break;}
        case 3: // rev + fwd_gamut_tonemap          skip 1 stage
         {    //////////////////////////////////////////////////////////////////////////////////////////////////
                   // Timing code
                   // statistical
				   #ifdef PROFILEWITHCHRONO
                   wc_avg_bc_tuples = do_benchmarking( [&]() {
                       auto_schedule_true_fwd_v3(input, ctrl_pts_h, weights_h, rev_tone_h, TsTw_tran_h, coefs_h, output);
                   } );
                   // printf("bc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[0][0], wc_avg_bc_tuples[0][1]);  
                   // printf("avg {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[1][0], wc_avg_bc_tuples[1][1]);  
                   // printf("wc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[2][0], wc_avg_bc_tuples[2][1]);  
                   write_container(wc_avg_bc_tuples[0], out, "img_"+to_string(img_no));
				   out << "\n";
				   #endif
				   
				   break;}
        case 4: // rev + fwd_transform              skip 2 stages
          {//////////////////////////////////////////////////////////////////////////////////////////////////
                    // Timing code
                    // statistical
					#ifdef PROFILEWITHCHRONO
                     wc_avg_bc_tuples = do_benchmarking( [&]() {
                        auto_schedule_true_fwd_v4(input, ctrl_pts_h, weights_h, rev_tone_h, TsTw_tran_h, coefs_h, output);
                    } );
                    // printf("bc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[0][0], wc_avg_bc_tuples[0][1]);  
                    // printf("avg {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[1][0], wc_avg_bc_tuples[1][1]);  
                    // printf("wc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[2][0], wc_avg_bc_tuples[2][1]);  
                    write_container(wc_avg_bc_tuples[0], out, "img_"+to_string(img_no));
					out << "\n";
					#endif
					
					break;      }
        case 5: // rev + fwd_gamut                  skip 2 stages
       {   //////////////////////////////////////////////////////////////////////////////////////////////////
                 // Timing code
                 // statistical
				 #ifdef PROFILEWITHCHRONO
                  wc_avg_bc_tuples = do_benchmarking( [&]() {
                     auto_schedule_true_fwd_v5(input, ctrl_pts_h, weights_h, rev_tone_h, TsTw_tran_h, coefs_h, output);
                 } );
                 // printf("bc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[0][0], wc_avg_bc_tuples[0][1]);  
                 // printf("avg {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[1][0], wc_avg_bc_tuples[1][1]);  
                 // printf("wc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[2][0], wc_avg_bc_tuples[2][1]);  
                 write_container(wc_avg_bc_tuples[0], out, "img_"+to_string(img_no));
				 out << "\n";
				 #endif
				 
				 break;      }
        case 6: // rev + fwd_tonemap                skip 2 stages
          {//////////////////////////////////////////////////////////////////////////////////////////////////
                    // Timing code
                    // statistical
					#ifdef PROFILEWITHCHRONO
                     wc_avg_bc_tuples = do_benchmarking( [&]() {
                        auto_schedule_true_fwd_v6(input, ctrl_pts_h, weights_h, rev_tone_h, TsTw_tran_h, coefs_h, output);
                    } );
                    // printf("bc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[0][0], wc_avg_bc_tuples[0][1]);  
                    // printf("avg {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[1][0], wc_avg_bc_tuples[1][1]);  
                    // printf("wc  {mean, stdev}: {%f, %f}\n", wc_avg_bc_tuples[2][0], wc_avg_bc_tuples[2][1]);  
                    write_container(wc_avg_bc_tuples[0], out, "img_"+to_string(img_no));
					out << "\n";
					#endif
					
					break;}
        case 7: // rev only                         skip 3 stages
         { break;}
        default: // should not happen
          {cout << "Default pipe branch taken, pls check\n";
                    break;}
      }      
      //save_image(output, "output_fwd_v"+to_string(version)+".png"); 



  } else {
    double auto_schedule_on = Halide::Tools::benchmark(2, 5, [&]() {
        auto_schedule_true_rev(input, ctrl_pts_h, weights_h, rev_tone_h, TsTw_tran_h, coefs_h, output);
    });
    //printf("- Auto schedule rev: %gms\n", auto_schedule_on * 1e3);  
    //save_image(output, "output_rev.png");  
  }

  ////////////////////////////////////////////////////////////////////////
  // Save the output

  if (direction == 1) {
	save_image(output, std::string(path)+"v"+to_string(version)+"/isp_fwd/img_ispf_"+to_string(img_no)+".png"); 
    cout << "- output(c,x,y) fwd: "<< output.channels() << " " << output.width() << " " << output.height() << endl;
  } else {
    save_image(output, std::string(path)+"v"+to_string(version)+"/isp_rev/img_ispr_"+to_string(img_no)+".png");
    cout << "- output(c,x,y) rev: "<< output.channels() << " " << output.width() << " " << output.height() << endl;
  }

  return 0;
}

// benchmark(int samples, int iterations, std::function<void()> op)
//
// Benchmarks the operation 'op'. The number of iterations refers to
// how many times the operation is run for each time measurement, the
// result is the minimum over a number of samples runs. The result is the
// amount of time in seconds for one iteration.