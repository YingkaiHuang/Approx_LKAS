#include <Eigen/Eigen>
#include "config.hpp"

using namespace std;
using namespace Eigen;

// constructor
lateralController::lateralController() {
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

    m_desired_steering_angle = 0.0L;
    m_steering_angle_left    = 0.0L;
    m_steering_angle_right   = 0.0L;

    // controller design time parameters
    // Eigen::Matrix<typename Scalar, int RowsAtCompileTime, int ColsAtCompileTime>
   // v0  
    m_phi_aug[0] <<   
0.065327631161289923,   -0.0057488315421935186,   0,   0,   0,   0.76150260085199573,
     0,   0.065327631161289923,   0,   0,   0,   4.2279342994073188,
     -0.013703164614691236,   -0.0080272205354882439,   1,   0.088000000000000009,   0.0038720000000000009,   -0.10637424391358273,
     0,   -0.013703164614691236,   0,   1,   0.088000000000000009,   -0.13706217728596651,
     0,   0,   0,   0,   1,   0,
     0,   0,   0,   0,   0,   0;

// // v0 q=7.5
    m_K2c[0] <<
-0.62487449999615796,   -0.52499560951294966,   -1.5351289237584498,   0.093226254489242155,   -0.29566173047848859;
//v0 q=12.5
//     m_K2c[0] <<
// -0.10971256332977458,   -0.93475959871833403,   -2.1549625509490422,   0.21623195373822424,   -0.43675359007892589;
// //v0 q=10
//     m_K2c[0] <<
// -0.10010595490980388,   -0.85053499727153059,   -2.0303662024836084,   0.20519639640544476,   -0.41282981911566269;

          

    m_T[0] <<
1.6653345369377348e-16,   -2.0816681711721685e-17,   -5.5511151231257827e-17,   0,   1,   -2.6020852139652106e-18,
     -0.47226521616603823,   0.08818207658680037,   -0.64150645980119636,   0.59800795423286013,   1.1102230246251565e-16,   0.0067405990868722801,
     0.85763394722311603,   -0.15673472717902309,   -0.42251786027657834,   0.24732311919072517,   0,   -0.014428108943549133,
     -0.099645956833292901,   -0.025394343165814029,   -0.63949542132535697,   -0.76129804247072108,   0,   0.029945070647882585,
     0.13545052167686777,   0.81397018430409607,   -0.031520499932350218,   -0.040530648337571674,   0,   -0.56255614819391209,
     0.11468835003776268,   0.55178420878463841,   -0.00042891555909085828,   -0.00056432577801871582,   0,   0.82606311312719061;
   
    
    m_Gamma_aug[0] <<
0.13883727310325264,
     0.66796858498592593,
     -0.00051922855805427699,
     -0.00068315092279374711,
     0,
     1;

   
   // v1  
    m_phi_aug[1] <<   
0.18173374202393422,   -0.0099953558113163761,   0,   0,   0,   0.52997224575840463,
     0,   0.18173374202393422,   0,   0,   0,   2.901707133569547,
     -0.011996542965771949,   -0.0067771168753804597,   1,   0.055000000000000007,   0.0015125000000000004,   -0.049743549748402914,
     0,   -0.011996542965771949,   0,   1,   0.055000000000000007,   -0.064839232051367129,
     0,   0,   0,   0,   1,   0,
     0,   0,   0,   0,   0,   0;

// // v1 q=7.5
    m_K2c[1] <<
-0.88180153381057946,   -0.5111178545706222,   -1.5185272890981876,   0.056362076505496586,   -0.13365416304110522;

// v1 q=2.5
//     m_K2c[1] <<
// -0.68642575997363919,   -0.42230262218973824,   -1.3150249826288494,   0.042576727323744915,   -0.10937487465110513;        

    m_T[1] <<
-8.3266726846886741e-17,   1.7347234759768071e-17,   -5.5511151231257827e-17,   0,   1,   2.1684043449710089e-18,
     -0.159409820453009,   0.030659947721202938,   -0.74542009377145757,   0.64651960882872639,   -2.2204460492503131e-16,   0.0031234517084458057,
     0.94516366600774293,   -0.17740880372498224,   -0.26544801404504947,   -0.064480499186531925,   0,   -0.023903511956805876,
     -0.22073908944297288,   0.0067300364987392808,   -0.61056227783979444,   -0.75894692523045904,   0,   0.049418929196479028,
     0.074678175791478674,   0.57280784939730101,   -0.033134174665934754,   -0.043019170823817843,   0,   -0.8144727221584126,
     0.16416935784209952,   0.7996448950904198,   -0.0014377028829073131,   -0.0018910974247756532,   0,   0.5775905301011216;
   

    m_Gamma_aug[1] <<
0.28423138761183908,
     1.3844494558288933,
     -0.0024891385990272515,
     -0.003274114318398831,
     0,
     1;
   
    


    // v2
    m_phi_aug[2] <<   
0.091877632704218226,   -0.0070745777182248011,   0,   0,   0,   0.73734382690593392,
     0,   0.091877632704218226,   0,   0,   0,   4.0577967811886833,
     -0.01331391694482047,   -0.0076964103967119704,   1,   0.077000000000000013,   0.002964500000000001,   -0.087196785954254799,
     0,   -0.01331391694482047,   0,   1,   0.077000000000000013,   -0.11284226553309537,
     0,   0,   0,   0,   1,   0,
     0,   0,   0,   0,   0,   0;
  

     // q=10
    m_K2c[2] <<
-0.8602608405550316,   -0.56443801949306016,   -1.6963128830225653,   0.092164841715324949,   -0.28388943974479186;
    

    m_T[2] <<
-5.5511151231257827e-17,   6.9388939039072284e-18,   5.5511151231257827e-17,   -5.5511151231257827e-17,   1,   8.6736173798840355e-19,
     -0.32761073528808554,   0.062060030041148545,   -0.69789986999295506,   0.63382748762617236,   0,   0.0042715557590363084,
     0.91775558774499633,   -0.17030680537937859,   -0.33828186445320019,   0.11866032440592833,   0,   -0.014331112945371864,
     -0.13568530818828883,   -0.012772533691878744,   -0.63066818708741945,   -0.76348986437622712,   0,   0.027698814335219105,
     0.13422611030943887,   0.80206998079695402,   -0.027542210236249018,   -0.035571003087654957,   0,   -0.58020964108692019,
     0.11818940620835461,   0.56891541814247393,   -0.00046486240233698661,   -0.00061161710466062647,   0,   0.81385866161598053;
 


    m_Gamma_aug[2] <<
0.14522104608886302,
     0.69903466655112756,
     -0.00057118320939653783,
     -0.00075150285117837597,
     0,
     1;
  


    // v3
    m_phi_aug[3] <<   
0.065327631161289923,   -0.0057488315421935186,   0,   0,   0,   0.75511882786638584,
     0,   0.065327631161289923,   0,   0,   0,   4.1968682178421188,
     -0.013703164614691236,   -0.0080272205354882439,   1,   0.088000000000000009,   0.0038720000000000009,   -0.10632228926224048,
     0,   -0.013703164614691236,   0,   1,   0.088000000000000009,   -0.13699382535758189,
     0,   0,   0,   0,   1,   0,
     0,   0,   0,   0,   0,   0;

    // q=7.5
    m_K2c[3] <<
-0.62397391877152453,   -0.52567914320271902,   -1.5354539127918503,   0.097085698092269371,   -0.29128636940792801;
    
    m_T[3] <<
-1.1102230246251565e-16,   2.7755575615628914e-17,   5.5511151231257827e-17,   -8.3266726846886741e-17,   1,   2.6020852139652106e-18,
     -0.47475744722592139,   0.088555558442959814,   -0.64042136878447598,   0.59713733247587453,   -1.1102230246251565e-16,   0.0071243203323152985,
     0.85635104924195504,   -0.15630738386514703,   -0.42396194435262025,   0.24951475378994095,   0,   -0.0151505641257129,
     -0.099030867483337393,   -0.02595045811430452,   -0.63958956615482654,   -0.76121421589988514,   0,   0.031584258531100459,
     0.13225883074994954,   0.80211454016338712,   -0.032275546684635024,   -0.041496231650383235,   0,   -0.57996225566165915,
     0.11818940620835439,   0.56891541814247282,   -0.00046486240233698423,   -0.00061161710466062301,   0,   0.81385866161598119;
  

    m_Gamma_aug[3] <<
0.14522104608886258,
     0.69903466655112556,
     -0.00057118320939653436,
     -0.0007515028511783712,
     0,
     1;
   


    // v4
    m_phi_aug[4] <<   
0.25559270561048886,   -0.011246079046861503,   0,   0,   0,   0.70088874200339324,
     0,   0.25559270561048886,   0,   0,   0,   3.6550551331398697,
     -0.010913701993855197,   -0.0061084728899776829,   1,   0.044000000000000004,   0.00096800000000000033,   -0.036321366547281152,
     0,   -0.010913701993855197,   0,   1,   0.044000000000000004,   -0.047508736733892339,
     0,   0,   0,   0,   1,   0,
     0,   0,   0,   0,   0,   0;

    // q=7.5
    m_K2c[4] <<
-0.95173778262620989,   -0.69768476768675169,   -1.4428370287714245,   -0.0081997449306354924,   -0.1815377787551716;

    m_T[4] <<
-9.7144514654701197e-17,   1.3877787807814457e-17,   -2.2204460492503131e-16,   1.6653345369377348e-16,   1,   2.574980159653073e-19,
     -0.073093734249085246,   0.014732501623011211,   -0.76460910643459346,   0.64016650455265589,   -2.2204460492503131e-16,   0.00014218150046005942,
     0.90350339720682793,   -0.1793152532295903,   -0.29875180585886685,   -0.24953790460616454,   0,   -0.0024174068254423243,
     -0.3781829630452529,   0.05662576115490206,   -0.57097378653105679,   -0.72645095412925131,   0,   0.0053959220740070115,
     0.18126885642758161,   0.95303413821626259,   -0.010450067529119205,   -0.013663267186685465,   0,   -0.24201579349754171,
     0.049579889078085859,   0.2369566372211373,   -6.3532531970265176e-05,   -8.3594764985212125e-05,   0,   0.97025428401599345;

    m_Gamma_aug[4] <<
0.051099891950869746,
     0.24422117080519004,
     -6.5480290081582293e-05,
     -8.6157584009012396e-05,
     0,
     1;


    // v5
    m_phi_aug[5] <<   
0.091877632704218226,   -0.0070745777182248011,   0,   0,   0,   0.7310102365982627,
     0,   0.091877632704218226,   0,   0,   0,   4.0269418757017572,
     -0.01331391694482047,   -0.0076964103967119704,   1,   0.077000000000000013,   0.002964500000000001,   -0.087142477239069829,
     0,   -0.01331391694482047,   0,   1,   0.077000000000000013,   -0.11277081756736147,
     0,   0,   0,   0,   1,   0,
     0,   0,   0,   0,   0,   0;

    // q=10
    m_K2c[5] <<
 -0.85965085412595177,   -0.56463221203537872,   -1.6966587358264962,   0.095503378776192666,   -0.27965678103806424;
        

    m_T[5] <<
-1.6653345369377348e-16,   3.4694469519536142e-17,   0,   5.5511151231257827e-17,   1,   2.6020852139652106e-18,
     -0.32963235520096879,   0.062382293885120607,   -0.69728508672118494,   0.63342254873589565,   -1.6653345369377348e-16,   0.0045102553635109413,
     0.91718022846752723,   -0.17000580884884861,   -0.33923667274908387,   0.12071049420342853,   0,   -0.015030299970328957,
     -0.13483101981401463,   -0.013341629934629078,   -0.63080567371298624,   -0.76346389294573347,   0,   0.029149325656893019,
     0.13106992803352835,   0.79030832330287359,   -0.02821237712592442,   -0.036432131139082856,   0,   -0.59674968762543601,
     0.12150806135444348,   0.58518478226639403,   -0.0005014845665948733,   -0.00065979610212276764,   0,   0.80174427020843164;
  
 
    m_Gamma_aug[5] <<
0.15155463639653416,
     0.72988957203805382,
     -0.00062549192458151404,
     -0.00082295081691227884,
     0,
     1;
    

    // v6
    m_phi_aug[6] <<   
0.18173374202393422,   -0.0099953558113163761,   0,   0,   0,   0.52997224575840463,
     0,   0.18173374202393422,   0,   0,   0,   2.901707133569547,
     -0.011996542965771949,   -0.0067771168753804597,   1,   0.055000000000000007,   0.0015125000000000004,   -0.049743549748402914,
     0,   -0.011996542965771949,   0,   1,   0.055000000000000007,   -0.064839232051367129,
     0,   0,   0,   0,   1,   0,
     0,   0,   0,   0,   0,   0;

    // q=7.5  
    m_K2c[6] <<
-0.88180153381057946,   -0.5111178545706222,   -1.5185272890981876,   0.056362076505496586,   -0.13365416304110522;
    

    m_T[6] <<
-8.3266726846886741e-17,   1.7347234759768071e-17,   -5.5511151231257827e-17,   0,   1,   2.1684043449710089e-18,
     -0.159409820453009,   0.030659947721202938,   -0.74542009377145757,   0.64651960882872639,   -2.2204460492503131e-16,   0.0031234517084458057,
     0.94516366600774293,   -0.17740880372498224,   -0.26544801404504947,   -0.064480499186531925,   0,   -0.023903511956805876,
     -0.22073908944297288,   0.0067300364987392808,   -0.61056227783979444,   -0.75894692523045904,   0,   0.049418929196479028,
     0.074678175791478674,   0.57280784939730101,   -0.033134174665934754,   -0.043019170823817843,   0,   -0.8144727221584126,
     0.16416935784209952,   0.7996448950904198,   -0.0014377028829073131,   -0.0018910974247756532,   0,   0.5775905301011216;
  
    m_Gamma_aug[6] <<
0.28423138761183908,
     1.3844494558288933,
     -0.0024891385990272515,
     -0.003274114318398831,
     0,
     1;
   

    // v7
    m_phi_aug[7] <<   
0.25559270561048886,   -0.011246079046861503,   0,   0,   0,   0.68679869573310148,
     0,   0.25559270561048886,   0,   0,   0,   3.5873927607523894,
     -0.010913701993855197,   -0.0061084728899776829,   1,   0.044000000000000004,   0.00096800000000000033,   -0.036279089993364634,
     0,   -0.010913701993855197,   0,   1,   0.044000000000000004,   -0.047453110878709438,
     0,   0,   0,   0,   1,   0,
     0,   0,   0,   0,   0,   0;
    
    // q=7.5
    m_K2c[7] <<
-0.9517055908767188,   -0.69151130838670538,   -1.4456636723598613,   0.0015717803958913701,   -0.17827378851579448;
    

    m_T[7] <<
4.8572257327350599e-16,   -9.4542429440735987e-17,   1.27675647831893e-15,   -1.3322676295501878e-15,   1,   -1.6263032587282567e-18,
     -0.074229102970017322,   0.014941938817411295,   -0.76437683481816443,   0.6403083616993952,   1.9984014443252818e-15,   0.00018726410260516354,
     0.90527184983385567,   -0.17936159059127482,   -0.29688513807477646,   -0.24527870630567425,   0,   -0.0031414554417953022,
     -0.37390017171321011,   0.055159127136420009,   -0.57224797915496828,   -0.72776321163155133,   0,   0.007006456676863752,
     0.17695628170201289,   0.93608613171035782,   -0.010932016985627449,   -0.014291526047889662,   0,   -0.30348883286471084,
     0.06211323634850463,   0.29716389921730257,   -0.00010267115604122801,   -0.00013509183332793094,   0,   0.95280403760747867;
  

    m_Gamma_aug[7] <<
0.065189938221161453,
     0.31188354319267014,
     -0.00010775684399809907,
     -0.00014178343919191489,
     0,
     1;
}
// destructor
lateralController::~lateralController(){}

// class methods
void lateralController::compute_steering_angles(long double the_yL, int the_it_counter, int the_pipe_version) {
    m_z3 = the_yL;
    m_z5 = 2 * m_z3 / ( pow( m_LL + m_l_r, 2 ) );   // curvature calculate

    Matrix<long double, 6, 1> zt_temp, zt;       // zt is the transferred state vector
    if (the_it_counter == 0){
        zt_temp <<  m_z1,
                    m_z2,
                    m_z3,
                    m_z4,
                    m_z5,
                    0.0L;
    } else {
        zt_temp <<  m_z1,
                    m_z2,
                    m_z3,
                    m_z4,
                    m_z5,
                    m_input[the_it_counter-1];
    }

    zt = m_T[the_pipe_version] * zt_temp;        

    Matrix<long double, 5, 1> zt_temp_2;
    zt_temp_2 << zt[1], 
                 zt[2], 
                 zt[3], 
                 zt[4], 
                 zt[5];

    // calculate the desired steering angle 
    m_desired_steering_angle = m_K2c[the_pipe_version] * zt_temp_2;   
    // calculate left front tire steering angle according to desired steering angle                     
    m_steering_angle_left    = atan( m_l / (-m_d + m_l / tan(m_desired_steering_angle) ) );  
    // calculate right front tire steering angle according to desired steering angle 
    m_steering_angle_right   = atan( m_l / ( m_d + m_l / tan(m_desired_steering_angle) ) );         
}

vector<long double> lateralController::get_steering_angles() {
    // return steering angles
    vector<long double> steering_angles(2);
    steering_angles[0] = m_steering_angle_left;
    steering_angles[1] = m_steering_angle_right;
    return steering_angles;     
}

void lateralController::estimate_next_state(int the_it_counter, int the_pipe_version) {
    // transfer state vector
    Matrix<long double, 6, 1> zkp_temp, zkp;  
    if (the_it_counter == 0){
        zkp_temp << m_z1,
                    m_z2,
                    m_z3,
                    m_z4,
                    m_z5,
                    0.0L;       
    } else {
        zkp_temp << m_z1,
                    m_z2,
                    m_z3,
                    m_z4,
                    m_z5,
                    m_input[the_it_counter-1]; 
    }

    // given the control design, estimate next states
    zkp = m_phi_aug[the_pipe_version]   * zkp_temp + 
          m_Gamma_aug[the_pipe_version] * m_desired_steering_angle;  

        m_z1 = zkp[0];
        m_z2 = zkp[1];
        m_z4 = zkp[3];
        m_z5 = zkp[4];
        m_input[the_it_counter+1] = m_desired_steering_angle;
}

