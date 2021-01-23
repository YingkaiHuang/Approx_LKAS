#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <chrono> 
#include "polyfit.hpp"
#include "config.hpp"

using namespace cv;
using namespace std;
using namespace std::chrono; 

// constructor
laneDetection::laneDetection() {}
// destructor
laneDetection::~laneDetection() {}

/// <summary>
/// "main" function for processing of the input image 
/// goes through the image processing pipeline steps
/// performs the control computations
/// </summary>
/// <param name="src">Image to be processed</param>  
/// <returns>0 when succesful</returns>  
long double laneDetection::lane_detection_pipeline(Mat src_in){
    setUseOptimized(true);

    // scale src
    Mat src;
    copyMakeBorder(src_in, src, 256, 0, 0, 0, BORDER_CONSTANT, Scalar(0));

    // Processing time ##########################################################################
    auto t_proc_start = high_resolution_clock::now(); 
    // get_warped_image
    Mat img_warped;
    bev_transform(src, img_warped);

    // rgb2gray
    Mat img_gray;
    cvtColor(img_warped, img_gray, COLOR_RGB2GRAY);

    // get_white_mask
    Mat mask, img_white_mask;
    #ifdef VREP_CAM
    inRange(img_gray, 200, 255, mask);
    #endif
    #ifdef CARND_CAM
    inRange(img_gray, 150, 255, mask);
    #endif
    bitwise_and(img_gray, img_gray, img_white_mask, mask); 

    // early check on img_white_mask
    Mat img_lanes, img_thresholding;
    double minVal, maxVal;
    minMaxLoc(img_white_mask, &minVal, &maxVal);
    if (maxVal){
        img_lanes = img_white_mask;
    } else {
        // get_threshed_img
        threshold(img_gray, img_thresholding, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
        img_lanes = img_thresholding;
    }
    // make a temp copy of img_lanes for reverting back to original image 
    Mat img_lanes_temp = img_lanes.clone();

    // perform sliding window lane tracking
    vector<vector<Point>> lanes = sliding_window_lane_tracking(img_lanes);

    #ifdef RE_DRAW_IMAGE
    // lane identification -> reverting back to original image
    Mat img_detected_lanes, draw_lines, diff_src_rebev, img_rev_warped, img_roi;
    lane_identification(lanes, src, img_roi, img_warped, img_lanes_temp, 
                        img_detected_lanes, draw_lines, diff_src_rebev, img_rev_warped);
    #endif

    // calculate lateral deviation
    long double yL = calculate_lateral_deviation(lanes[0], lanes[1]);
   

    auto t_proc_stop = high_resolution_clock::now(); 
    auto t_proc = duration<double, milli>(t_proc_stop - t_proc_start); 
    
    cout << "lateral_deviation: " << yL << endl;
    m_yL_container.push_back(yL);
    
    // Saving time    ##########################################################################
    auto t_save_start = high_resolution_clock::now(); 

    #ifdef RE_DRAW_IMAGE
    Rect out_roi = Rect(0, 256, 512, 256);
    Mat cropped = Mat(img_detected_lanes, out_roi);
    imwrite("output_images/img_roi.png", img_roi);
    imwrite("output_images/img_diff_src_rebev.png", diff_src_rebev);
    imwrite("output_images/img_output.png", cropped); 
    imwrite("output_images/img_draw_lines.png", draw_lines); 
    imwrite("output_images/img_rev_warped.png", img_rev_warped); 
    #endif
    imwrite("output_images/img_warped.png", img_warped);
    imwrite("output_images/img_gray.png", img_gray);
    if (maxVal){
        imwrite("output_images/img_white_mask.png", img_white_mask);
    } else {
        imwrite("output_images/img_thresholding.png", img_thresholding);
    }
    imwrite("output_images/img_lanes.png", img_lanes_temp);
    //imwrite("img_bottom_half.png", img_bottom_half);
    auto t_save_stop = high_resolution_clock::now(); 
    //auto t_save = duration_cast<milliseconds>(t_save_stop - t_save_start); 
    auto t_save = duration<double, milli>(t_save_stop - t_save_start);

    //auto t_total = t_access.count() + t_proc.count() + t_control.count() + t_save.count();

    // print relevant durations
    //cout << "total: " << t_total << " ms" << endl;
   
    //cout << "\tprocessed in " << t_proc.count() << " ms" << endl;
    
    // cout << "high_resolution_clock steady = " << boolalpha << chrono::high_resolution_clock::is_steady << endl << endl;
    // cout << "system_clock steady = " << boolalpha << chrono::system_clock::is_steady << endl << endl;

    return yL;
}

/// <summary> .... </summary>
/// <param name="bar"> .... </param>
/// <returns> .... </returns>
vector<vector<Point2f>> laneDetection::get_bev_points(){
    // declare variables
    vector<vector<Point2f>> vertices(2);
    vector<Point2f> src_vertices(4), dst_vertices(4);
    
    #ifdef VREP_CAM
    int warp_offset = 70;
    // assign src points
    src_vertices[1] = Point(233, 280);
    src_vertices[0] = Point(277, 280);
    src_vertices[2] = Point(462, 512);
    src_vertices[3] = Point(50, 512);
    // assign dst points
    dst_vertices[1] = Point(50 + warp_offset, 0);
    dst_vertices[0] = Point(462 - warp_offset, 0);
    dst_vertices[2] = Point(462 - warp_offset, 512);
    dst_vertices[3] = Point(50 + warp_offset, 512);
    #endif

    #ifdef CARND_CAM
    int warp_offset = 70;
    int height = 720, width = 1280;
    // assign src points
    src_vertices[1] = Point(707, 464);
    src_vertices[0] = Point(575, 464);
    src_vertices[2] = Point(258, 682);
    src_vertices[3] = Point(1049, 682);
    // assign dst points
    dst_vertices[1] = Point(width - 350 + warp_offset, 0);
    dst_vertices[0] = Point(250 - warp_offset, 0);
    dst_vertices[2] = Point(250 - warp_offset, height);
    dst_vertices[3] = Point(width - 350 + warp_offset, height);  
    #endif

    // return result 
    vertices[0] = src_vertices;
    vertices[1] = dst_vertices;
    return vertices;   
}

/// <summary> .... </summary>
/// <param name="bar"> .... </param>
/// <returns> .... </returns>
void laneDetection::bev_transform(Mat& src, Mat& dst){
    // get transform points 
    vector<vector<Point2f>> vertices = get_bev_points();
    // calculate transform matrix
    Mat M = getPerspectiveTransform(vertices[0], vertices[1]);
    // perform bev 
    warpPerspective(src, dst, M, dst.size(), INTER_LINEAR, BORDER_CONSTANT);
}

/// <summary> .... </summary>
/// <param name="bar"> .... </param>
/// <returns> .... </returns>
void laneDetection::bev_rev_transform(Mat& src, Mat& dst){
    // get transform points 
    vector<vector<Point2f>> vertices = get_bev_points();
    // calculate rev transform matrix
    Mat M = getPerspectiveTransform(vertices[1], vertices[0]);
    // perform bev
    warpPerspective(dst, src, M, dst.size(), INTER_LINEAR, BORDER_CONSTANT);
}

/// <summary> .... </summary>
/// <param name="bar"> .... </param>
/// <returns> .... </returns>
vector<vector<Point>> laneDetection::sliding_window_lane_tracking(Mat& src){
    // sliding window polyfit 
    Size shape = src.size();
    // get bottom half of BEV image
    Mat img_bottom_half = src(Range(shape.height/2, shape.height), Range(0, shape.width));
    // get histogram of bottom half
    Mat histogram;
    reduce(img_bottom_half, histogram, 0, CV_REDUCE_SUM, CV_32S);
    // midpoint of histogram
    int midpoint = shape.width/2;
    // get left and right halves of histogram
    Point min_loc, max_loc;
    double minVal, maxVal; 
    Size histo = histogram.size();
    // left half of histogram
    Mat hist_half = histogram(Range(0, histo.height), Range(0, histo.width/2));
    minMaxLoc(hist_half, &minVal, &maxVal, &min_loc, &max_loc);
    int leftx_base = max_loc.x;
    // right half of histogram
    hist_half = histogram(Range(0, histo.height), Range(histo.width/2, histo.width));
    minMaxLoc(hist_half, &minVal, &maxVal, &min_loc, &max_loc);
    int rightx_base = max_loc.x + midpoint;

    // Get x and y positions of all nonzero pixels in the image
    Mat nonzero;
    findNonZero(src, nonzero);
    
    // Current positions to be updated for each window
    int leftx_current = leftx_base;
    int rightx_current = rightx_base;
    // width of the windows
    int margin   = 65;
    // minimum number of pixels needed to recenter window
    int minpix   = 400;
    // number of sliding windows
    int nwindows = 8;
    // Set height of sliding windows
    int window_height = shape.height/nwindows;   
    // declare rectangle parameters
    int win_y_low, win_y_high, win_xleft_low, win_xleft_high, win_xright_low, win_xright_high;
    // declare variables for mean calculation
    int total_good_left, counter_good_left, total_good_right, counter_good_right;
    // Create vectors for points of left box and right box
    vector<Point> point_1_left(nwindows), point_1_right(nwindows);
    vector<Point> point_2_left(nwindows), point_2_right(nwindows);
    // Create vectors to receive left and right lane pixel indices per window
    vector<int> good_left_inds;
    vector<int> good_right_inds;
    good_left_inds.reserve(nonzero.total());
    good_right_inds.reserve(nonzero.total());
    // Create empty vectors to accumulate left and right lane pixel Points
    vector<Point> left_lane_inds, right_lane_inds;

    // draw sliding windows and track the two lanes
    for (int window = 0; window < nwindows; ++window){
        // set rectangle parameters
        win_y_low       = shape.height - (window + 1) * window_height;
        win_y_high      = shape.height - window * window_height;
        win_xleft_low   = leftx_current - margin;
        win_xleft_high  = leftx_current + margin;
        win_xright_low  = rightx_current - margin;
        win_xright_high = rightx_current + margin;

        #ifdef DRAW_SLIDING_WINDOWS
        // top left corner
        point_1_left[window]  = Point(win_xleft_low, win_y_high);
        point_1_right[window] = Point(win_xright_low, win_y_high);
        // bottom right corner
        point_2_left[window]  = Point(win_xleft_high, win_y_low);
        point_2_right[window] = Point(win_xright_high, win_y_low);
        // draw rectangles 
        rectangle(src, point_1_left[window], point_2_left[window], Scalar(255, 255, 255));
        rectangle(src, point_1_right[window], point_2_right[window], Scalar(255, 255, 255));
        #endif

        // initialize variables for mean calculation
        total_good_left    = 0;
        counter_good_left  = 0;
        total_good_right   = 0;
        counter_good_right = 0;

        // Identify the nonzero pixels in x and y within the 2 rectangles
        for (int i = nonzero.total(); i > 0; --i) {
            // left rectangle
            if ((nonzero.at<Point>(i).y >= win_y_low) && (nonzero.at<Point>(i).y < win_y_high) && 
                (nonzero.at<Point>(i).x >= win_xleft_low) && (nonzero.at<Point>(i).x < win_xleft_high)){
                    // // Append candidate point.x to the current window vector
                    // good_left_inds[i] = nonzero.at<Point>(i).x;
                    good_left_inds.push_back(nonzero.at<Point>(i).x);
                    // mean calculation
                    total_good_left += nonzero.at<Point>(i).x;
                    counter_good_left++;
                    // Append candidate point to the accumulation vector
                    left_lane_inds.push_back(nonzero.at<Point>(i));
                }
            // right rectangle
            if ((nonzero.at<Point>(i).y >= win_y_low) && (nonzero.at<Point>(i).y < win_y_high) && 
                (nonzero.at<Point>(i).x >= win_xright_low) && (nonzero.at<Point>(i).x < win_xright_high)){
                    // // Append candidate point.x to the current window vector
                    // good_right_inds[i] = nonzero.at<Point>(i).x;
                    good_right_inds.push_back(nonzero.at<Point>(i).x);
                    // mean calculation
                    total_good_right += nonzero.at<Point>(i).x;
                    counter_good_right++;
                    // Append candidate point to the accumulation vector
                    right_lane_inds.push_back(nonzero.at<Point>(i));
                }
        }
        // If you found > minpix pixels, recenter next window on their mean position
        if ((good_left_inds.size() > minpix) && counter_good_left ){
            leftx_current  = total_good_left / counter_good_left;
        }
        if ((good_right_inds.size() > minpix) && counter_good_right ){
            rightx_current = total_good_right / counter_good_right;
        }
    }   

    // return identified points for both lanes
    vector<vector<Point>> lanes(2);
    lanes[0] = left_lane_inds;
    lanes[1] = right_lane_inds;
    return lanes;
}

/// <summary> .... </summary>
/// <param name="bar"> .... </param>
/// <returns> .... </returns>
long double laneDetection::calculate_lateral_deviation(vector<Point> left_lane_inds, vector<Point> right_lane_inds){
    //  Extract left and right line pixel positions
    vector<long double> leftx(left_lane_inds.size()), lefty(left_lane_inds.size());
    vector<long double> rightx(right_lane_inds.size()), righty(right_lane_inds.size());
    for (int i = 0; i < left_lane_inds.size(); ++i){
        leftx[i] = left_lane_inds[i].x;
        lefty[i] = left_lane_inds[i].y;
    }
    for (int i = 0; i < right_lane_inds.size(); ++i){
        rightx[i] = right_lane_inds[i].x;
        righty[i] = right_lane_inds[i].y;
    }   

    // Fit a second order polynomial to left and right lane positions
    vector<long double> left_fit = mathalgo::polyfit( lefty, leftx, 2 );
    vector<long double> right_fit = mathalgo::polyfit( righty, rightx, 2 );
    
    // calculate lateral deviation yL at look-ahead distance LL = 0.55
    vector<long double> vec1{483.0L}, vec2(512.0L);
    vector<long double> yL_leftx = mathalgo::polyval(left_fit, vec1); 
    vector<long double> yL_rightx = mathalgo::polyval(right_fit, vec1);  
    vector<long double> scale_leftx = mathalgo::polyval(left_fit, vec2); 
    vector<long double> scale_rightx = mathalgo::polyval(right_fit, vec2); 
    long double scale = 0.32 / (scale_rightx[0] - scale_leftx[0]);
    long double yL = (256 - (yL_leftx[0] + yL_rightx[0]) / 2) * scale;

    return yL;
}

/// <summary> .... </summary>
/// <param name="bar"> .... </param>
/// <returns> .... </returns>
void laneDetection::lane_identification(vector<vector<Point>> lanes, Mat& src, Mat& img_roi, 
										Mat& img_warped, Mat& img_lanes_temp, Mat& img_detected_lanes, 
										Mat& draw_lines, Mat& diff_src_rebev, Mat& img_rev_warped) {
    // reverting back to original image
    draw_lines = img_warped.clone();

    // draw circes iso lines;
    for (int i = 0; i < lanes[0].size(); ++i){
        circle(draw_lines, lanes[0][i], 1, Scalar(0, 0, 255), 1, 8);
    }
    for (int i = 0; i < lanes[1].size(); ++i){
        circle(draw_lines, lanes[1][i], 1, Scalar(0, 0, 255), 1, 8);
    }

    // get reverse bev on bev with drawed detected lines 
    bev_rev_transform(img_rev_warped, draw_lines);
    // reverse bev transform on bev of src image 
    bev_rev_transform(img_roi, img_warped);
    // substract: diff_src_rebev = src - img_roi 
    absdiff(src, img_roi, diff_src_rebev);
    // combine: diff_src_rebev + img_rev_warped
    bitwise_or(diff_src_rebev, img_rev_warped, img_detected_lanes);    
}

/// <summary> .... </summary>
/// <param name="bar"> .... </param>
/// <returns> .... </returns>
vector<long double> laneDetection::get_yL_container(){
    return m_yL_container;
}
