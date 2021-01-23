#include <Eigen/Eigen>
#include <opencv2/opencv.hpp>
#include "Halide.h"
extern "C" {
    #include "extApi.h"
}

// ------------ defs -------------//
#define SELECT_PERIOD 1          // controller period: 0 -> 0.035, 1 -> 0.065, 2 -> 0.1 
#define DRAW_SLIDING_WINDOWS 1   // select whether to draw the sliding window while tracking
#define VREP_CAM 1               // select when VREP camera frames are used
//#define CARND_CAM 1            // select when CARND camera frames are used
#define RE_DRAW_IMAGE 1          // run re-draw image function

// ------------ class declarations -------------//
class vrepAPI {
private:
    // private membcvers
    // std::vector<std::vector<cv::Mat>> m_three_channels = std::vector<std::vector<cv::Mat>> (400);
    // std::vector<cv::Mat> m_out_Mat = std::vector<cv::Mat> (400, cv::Mat(256,512,CV_8UC3));
    int m_clientID, m_ping_time, m_cam, m_car, m_floor;
    int m_resolution[2];
    simxUChar* m_image;
    int m_nakedCar_steeringLeft, m_nakedCar_steeringRight;
    int m_nakedCar_motorLeft, m_nakedCar_motorRight;
    simxFloat m_position[3];
    double m_vx, m_desired_wheel_rot_speed;  
    // private methods
    cv::Mat vrep_img_2_Mat(int the_iter);
public:
    // constructor
    vrepAPI();
    // destructor
    ~vrepAPI();
    // public methods
    void sim_delay(int time_step);
    cv::Mat sim_sense(int the_iter);
    void sim_actuate(std::vector<long double> the_steering_angles);
};

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

class IBCController {
private:
    // member variables
    long double m_d, m_l, m_vx, m_LL, m_l_r;
    long double m_z1, m_z2, m_z3, m_z4, m_z5, m_z7;
    std::vector<long double> m_input = std::vector<long double> (400);
    long double m_desired_steering_angle, m_steering_angle_left, m_steering_angle_right;
    std::vector< Eigen::Matrix<long double, 7, 7> > m_phi_aug    
                                    = std::vector< Eigen::Matrix<long double, 7, 7> > (11);
    std::vector< Eigen::Matrix<long double, 7, 7> > m_T         
                                    = std::vector< Eigen::Matrix<long double, 7, 7> > (10);  
    std::vector< Eigen::Matrix<long double, 1, 7> > m_K2c        
                                    = std::vector< Eigen::Matrix<long double, 1, 7> > (10);
    std::vector< Eigen::Matrix<long double, 7, 1> > m_Gamma_aug  
                                    = std::vector< Eigen::Matrix<long double, 7, 1> > (10); 

public:
    // constructor
    IBCController();
    // destructor
    ~IBCController();
    // class public methods
    void compute_steering_angles(long double the_yL, int the_it_counter);
    std::vector<long double> get_steering_angles();
    void estimate_next_state(int the_it_counter);    
};

class imageSignalProcessing {
private:
    // class private methods
    cv::Mat run_pipeline(bool direction, cv::Mat the_img_in, int the_pipe_version);
    Halide::Runtime::Buffer<uint8_t> Mat2Image(cv::Mat *InMat);
    cv::Mat Image2Mat(Halide::Runtime::Buffer<uint8_t> *InImage);
public:
    // constructor
    imageSignalProcessing();
    // destructor
    ~imageSignalProcessing();
    // class public methods
    cv::Mat approximate_pipeline(cv::Mat the_img_vrep, int the_pipe_version);
};


// ------------ function declarations -------------//
template<class Container>
std::ostream& write_container(const Container& c,
                     std::ostream& out,
                     char delimiter = '\n')
{
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

void write_yL_2_file(std::vector<long double> yL_container, int pipeline_version);
std::string get_timestamp();



