#include <Eigen/Eigen>
#include <opencv2/opencv.hpp>
#include "Halide.h"

// ------------ defs -------------//
//#define SELECT_PERIOD 0          // controller period: 0 -> 0.035, 1 -> 0.065, 2 -> 0.1 
//#define DRAW_SLIDING_WINDOWS 1   // select whether to draw the sliding window while tracking
#define VREP_CAM 1               // select when VREP camera frames are used
//#define CARND_CAM 1            // select when CARND camera frames are used
//#define RE_DRAW_IMAGE 1          // run re-draw image function

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

class lateralController {
private:
    // member variables
    long double m_d, m_l, m_vx, m_LL, m_l_r;
    long double m_z1, m_z2, m_z3, m_z4, m_z5;
    std::vector<long double> m_input = std::vector<long double> (400);
    long double m_desired_steering_angle, m_steering_angle_left, m_steering_angle_right;
    std::vector< Eigen::Matrix<long double, 6, 6> > m_phi_aug    
                                    = std::vector< Eigen::Matrix<long double, 6, 6> > (8);
    std::vector< Eigen::Matrix<long double, 6, 6> > m_T         
                                    = std::vector< Eigen::Matrix<long double, 6, 6> > (8);  
    std::vector< Eigen::Matrix<long double, 1, 5> > m_K2c        
                                    = std::vector< Eigen::Matrix<long double, 1, 5> > (8);
    std::vector< Eigen::Matrix<long double, 6, 1> > m_Gamma_aug  
                                    = std::vector< Eigen::Matrix<long double, 6, 1> > (8);  
public:
    // constructor
    lateralController();
    // destructor
    ~lateralController();
    // class public methods
    void compute_steering_angles(long double the_yL, int the_it_counter, int the_pipe_version);
    std::vector<long double> get_steering_angles();
    void estimate_next_state(int the_it_counter, int the_pipe_version);
};

// ------------ function declarations -------------//


//void write_yL_2_file(std::vector<double> yL_container);
//std::string get_timestamp();



