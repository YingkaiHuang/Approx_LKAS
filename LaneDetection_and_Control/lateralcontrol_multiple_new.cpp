#include "config.hpp"

using namespace std;
using namespace Eigen;

// constructor
lateralController::lateralController() {
    // initialize member variables 
    m_d   = 1.628L;   // 2*d = distance between left andController right wheels
    m_l   = 2.995L;    // l = distance between front and rear wheels 
    m_vx  = 5.55556L;     // vehicle longitudinal speed is 5.55556m/s (20 km/hr)
    m_LL  = 5.5L;    // look-ahead distance is 0.25s*vx
    m_l_r = 1.2975L;    // distance from float delay, float period, int pipe_version, int qCG to rear axle (m)

    m_z1    = 0.0L;         // z1 is vy
    m_z2    = 0.0L;         // z2 is yaw rate
    m_z3    = 0.0L;         // z3 is yL
    m_z4    = 0.0L;         // z4 is epsilon_L
    m_z5    = 0.0L;         // z5 is curvature at lookahead distance KL (which is K_ref of CoG)
	m_input = 0.0L;
	
	m_desired_steering_angle = 0.0L;
    //m_phi_aug = readCSV(control_csv_dir+"phi_aug_pipe"+to_string(pipe_version)+"_q"+to_string(q)+"by10_d"+to_string(delay)+"ms_p"+to_string(period)+".csv", 6, 6).cast<long double>();
    //m_K2c = readCSV(control_csv_dir+"k_pipe"+to_string(pipe_version)+"_q"+to_string(q)+"by10_d"+to_string(delay)+"ms_p"+to_string(period)+".csv", 1, 5).cast<long double>(); 
    //m_T = readCSV(control_csv_dir+"t_pipe"+to_string(pipe_version)+"_q"+to_string(q)+"by10_d"+to_string(delay)+"ms_p"+to_string(period)+".csv", 6, 6).cast<long double>();
	//m_Gamma_aug = readCSV(control_csv_dir+"gamma_aug_pipe"+to_string(pipe_version)+"_q"+to_string(q)+"by10_d"+to_string(delay)+"ms_p"+to_string(period)+".csv", 6, 1).cast<long double>();
	m_phi_aug << 0.34595,-0.19687,0,0,0,1.0971,
-0.027176,0.46639,0,0,0,0.77086,
-0.025416,-0.19055,1,0.27778,0.03858,-0.20936,
0.00094433,-0.034897,0,1,0.27778,-0.028962,
0,0,0,0,1,0,
0,0,0,0,0,0;


	 

	 
	 //m_K2c << 0.84491815106022972,   -0.16600846736848518,   -1.4070069562954657,   -0.24986504997769693,   -0.62936811041126894;
	m_K2c << -0.0081542,-0.13729,0.31539,0.78234,0,-0.19017;
	//m_K2c << -0.033077,-0.28474,0.8471,1.45,0,-0.42034; 
	//m_K2c << -0.00095331,-0.057715,0.0931,0.44822,0,-0.075374;

  
	 
	 m_T << -2.2551405187698492e-17,   3.3610267347050637e-17,   -1.9775847626135601e-16,   1.4988010832439613e-15,   0.99999999999999989,   1.395910297075087e-18,
     -0.0045733947275615281,   0.0068512745564913314,   -0.13549538251305887,   0.99074365864510661,   -1.3600232051658168e-15,   0.00038505813289358803,
     0.49338315720916087,   -0.7917473587370093,   -0.35650530456396462,   -0.040991579598957661,   0,   -0.030542402247160531,
     -0.35425156060217355,   0.18615406435977117,   -0.89959026361928462,   -0.12599884832076336,   0,   0.12130187119945876,
     0.65444197171710006,   0.52438198562500093,   -0.21258371249238039,   -0.029483901839656764,   0,   -0.50066765775540412,
     0.45029288436410519,   0.25191144762443951,   -0.0095127240965814739,   -0.0012973089999781466,   0,   0.8565540064213315;
	 
	 m_Gamma_aug << 0.5257,
					0.2941,
					-0.011106,
					-0.0015146,
					0,
					1;
}
// destructor
lateralController::~lateralController(){}

// class methods
void lateralController::compute_steering_angles(long double the_yL) {
    m_z3 = the_yL;
    m_z5 = 2 * m_z3 / ( pow( m_LL + m_l_r, 2 ) );   // curvature calculate
	
    Matrix<long double, 6, 1> zt_temp;       // zt is the transferred state vector
        zt_temp <<  m_z1,
                    m_z2,
                    m_z3,
                    m_z4,
                    m_z5,
					m_input;
                    //m_input[the_it_counter-1]; SD: changed
   

    // calculate the desired steering angle 
    //m_desired_steering_angle = m_K2c * zt_temp_2;
	
	m_desired_steering_angle = m_K2c * zt_temp;   	
    // calculate left front tire steering angle according to desired steering angle                     
    //m_steering_angle_left    = atan( m_l / (-m_d + m_l / tan(m_desired_steering_angle) ) );  
    // calculate right front tire steering angle according to desired steering angle 
    //m_steering_angle_right   = atan( m_l / ( m_d + m_l / tan(m_desired_steering_angle) ) );         
}

long double lateralController::get_steering_angles() {
    // return steering angles
    long double steering_angles;
    steering_angles = m_desired_steering_angle;
	//m_steering_angle_left_container.push_back(m_steering_angle_left);
	//m_steering_angle_right_container.push_back(m_steering_angle_right);
    return steering_angles;     
}

void lateralController::estimate_next_state() {
    // transfer state vector
    Matrix<long double, 6, 1> zkp_temp, zkp;
        zkp_temp << m_z1,
                    m_z2,
                    m_z3,
                    m_z4,
                    m_z5,
					m_input;
                    //m_input[the_it_counter-1]; SD: changed

    // given the control design, estimate next states
    zkp = m_phi_aug   * zkp_temp + 
          m_Gamma_aug * m_desired_steering_angle;  

        m_z1 = zkp[0];
        m_z2 = zkp[1];
        m_z4 = zkp[3];
        m_z5 = zkp[4];
		m_input = m_desired_steering_angle;
		//m_input[the_it_counter] = m_desired_steering_angle; // SD: changed 
        //m_input[the_it_counter+1] = m_desired_steering_angle;
}

//vector<long double> lateralController::get_steering_angle_left_container(){
//    return m_steering_angle_left_container;
//}

//vector<long double> lateralController::get_steering_angle_right_container(){
//    return m_steering_angle_right_container;
//}

