#include <Eigen/Eigen>
#include <opencv2/opencv.hpp>
#include "Halide.h"

// ------------ defs -------------//
#define VREP_CAM 1               // select when VREP camera frames are used
//#define CARND_CAM 1            // select when CARND camera frames are used

class laneDetection {
private:
    std::vector<long double> m_yL_container;
    // private methods
    void bev_transform(cv::Mat& src, cv::Mat& dst);
    void bev_rev_transform(cv::Mat& src, cv::Mat& dst);
    std::vector<std::vector<cv::Point2f>> get_bev_points();
    std::vector<std::vector<cv::Point>> sliding_window_lane_tracking(cv::Mat& src);
    long double calculate_lateral_deviation(std::vector<cv::Point> left_lane_inds, 
                                            std::vector<cv::Point> right_lane_inds);
    void lane_identification(std::vector<std::vector<cv::Point>> lanes, cv::Mat& src, cv::Mat& img_roi, 
                            cv::Mat& img_warped, cv::Mat& img_lanes_temp, cv::Mat& img_detected_lanes, 
                            cv::Mat& draw_lines, cv::Mat& diff_src_rebev, cv::Mat& img_rev_warped);
public:
    // constructor
    laneDetection();
    // destructor
    ~laneDetection();
    // class public methods
    long double lane_detection_pipeline(cv::Mat src);
    std::vector<long double> get_yL_container();
};


// ------------ function declarations -------------//

void write_yL_2_file(std::vector<long double> yL_container);
std::string get_timestamp();



