#include <Eigen/Eigen>
#include "config.hpp"

using namespace std;
using namespace Eigen;

// constructor
IBCController::IBCController() {
    // initialize member variables 
    m_d   = 0.135L;   // 2*d = distance between left and right wheels
    m_l   = 0.42L;    // l = distance between front and read wheels 
    m_vx  = 2.2L;     // vehicle longitudinal speed is 2.2m/s to simulate ~80km/h in reality
    m_LL  = 0.55L;    // look-ahead distance is 0.25s*vx
    m_l_r = 0.21L;    // distance from CG to rear axle (m)

    m_z1    = 0.0L;         // z1 is vy
    m_z2    = 0.0L;         // z2 is yaw rate
    m_z3    = 0.0L;         // z3 is yL
    m_z4    = 0.0L;         // z4 is epsilon_L
    m_z5    = 0.0L;         // z5 is curvature at lookahead distance KL (which is K_ref of CoG)
    fill(m_input.begin(), m_input.end(), 0.0L); // m_input is the input of the last sampling period.
    m_z7    = 0.0L;         // z5 is curvature at lookahead distance KL (which is K_ref of CoG)

    m_desired_steering_angle = 0.0L;
    m_steering_angle_left    = 0.0L;
    m_steering_angle_right   = 0.0L;

    // controller design time parameters
    // Eigen::Matrix<typename Scalar, int RowsAtCompileTime, int ColsAtCompileTime>
    m_phi_aug[0] <<   
    0.0330271529900219,  -0.00363298682890240,    0,   0,  0,   0.919330246258046,   0,
    0,  0.0330271529900219,  0,   0,  0,   5.06509586529036,    0,
    -0.0141767196102926, -0.00854864783294572,    1,   0.110000000000000,   0.00605000000000000, -0.147027023768347,  0,
    0,   -0.0141767196102926, 0,   1,   0.110000000000000,  -0.187645754422277,  0,
    0,   0,   0,   0,   1,   0,   0,
    0,   0,   0,   0,   0,   0,  0,
    0,   0,   1,   0,   0,   0,  1;


    m_K2c[0] <<
    -0.0192615381694946, -0.0194677133283466, 1.47201690303982,    0.681677102722397,   0,   -0.411892001408546,  0.154545427895890;


    m_T[0] <<
    0,   0,   0,   0,   1,   0,   0,
    0.127095860327213,   -0.0238786592019740, 0.766845244512323,   -0.622724472349080,  0,   0,   -0.0862499532104074,
    -0.967050372451366,  0.179765811169486,   0.151507839979264,   -0.00417455202570528,    0,   0,   -0.0976000912727692,
    0.116942196185946,   -0.00257641076152601,    0.175621972614182,   0.365782614115727,  0,   0,   -0.906464498369663,
    -0.0562164527763084, -0.0327369430171113, -0.597776455748552,  -0.690705458934311,  0,   0,   -0.401693018299031,
    0.178394093562023,   0.982871160141946,   -0.0285302835852897, -0.0364122625217403, 0,   0,   0,
    0,   0,   0,   0,  0,   1,   0;


    m_Gamma_aug[0] <<
    0,
    0,
    0,
    0,
    0,
    1,
    0;
}
// destructor
IBCController::~IBCController(){}

// class methods
void IBCController::compute_steering_angles(long double the_yL, int the_it_counter) {
    m_z3 = the_yL;
    m_z5 = 2 * m_z3 / ( pow( m_LL + m_l_r, 2 ) );   // curvature calculate

    Matrix<long double, 7, 1> zt_temp, zt;       // zt is the transferred state vector
    if (the_it_counter == 0){
        zt_temp <<  m_z1,
                    m_z2,
                    m_z3,
                    m_z4,
                    m_z5,
                    0.0L,
                    m_z7;
    } else {
        zt_temp <<  m_z1,
                    m_z2,
                    m_z3,
                    m_z4,
                    m_z5,
                    m_input[the_it_counter-1],
                    m_z7;
    }

    zt = m_T[SELECT_PERIOD] * zt_temp;        

    // calculate the desired steering angle *** |TODO| replace SELECT_PERIOD with variable **********|TODO|
    m_desired_steering_angle = m_K2c[SELECT_PERIOD] * zt_temp;   
    // calculate left front tire steering angle according to desired steering angle                     
    m_steering_angle_left    = atan( m_l / (-m_d + m_l / tan(m_desired_steering_angle) ) );  
    // calculate right front tire steering angle according to desired steering angle 
    m_steering_angle_right   = atan( m_l / ( m_d + m_l / tan(m_desired_steering_angle) ) );         
}

vector<long double> IBCController::get_steering_angles() {
    // return steering angles
    vector<long double> steering_angles(2);
    steering_angles[0] = m_steering_angle_left;
    steering_angles[1] = m_steering_angle_right;
    return steering_angles;     
}

void IBCController::estimate_next_state(int the_it_counter) {
    // transfer state vector
    Matrix<long double, 7, 1> zkp_temp, zkp;  
    if (the_it_counter == 0){
        zkp_temp << m_z1,
                    m_z2,
                    m_z3,
                    m_z4,
                    m_z5,
                    0.0L,       
                    m_z7;
    } else {
        zkp_temp << m_z1,
                    m_z2,
                    m_z3,
                    m_z4,
                    m_z5,
                    m_input[the_it_counter-1], 
                    m_z7;
    }

    // given the control design, estimate next states
    zkp = m_phi_aug[SELECT_PERIOD]   * zkp_temp + 
          m_Gamma_aug[SELECT_PERIOD] * m_desired_steering_angle;  

        m_z1 = zkp[0];
        m_z2 = zkp[1];
        m_z4 = zkp[3];
        m_z5 = zkp[4];
        m_input[the_it_counter+1] = m_desired_steering_angle;
        m_z7 = zkp[6];
}

