#include <ctime>
#include <sstream>
#include <iomanip>
#include "config.hpp"


void write_yL_2_file(std::vector<long double> yL_container, int pipeline_version){
    time_t now = time(0);
    tm *ltm = localtime(&now);
    std::ostringstream oss;
    oss << "/home/kbimpisi/Approx_IBC/final_app/results/"
        << 1900 + ltm->tm_year 
        << std::setfill('0') << std::setw(2) << 1 + ltm->tm_mon 
        << std::setfill('0') << std::setw(2) << ltm->tm_mday 
        << "_"  
        << std::setfill('0') << std::setw(2) << ltm->tm_hour 
        << std::setfill('0') << std::setw(2) << ltm->tm_min
        << "_v"
        << pipeline_version
        << ".txt";
    std::string file_name = oss.str();   
    std::ofstream outfile(file_name);
    write_container(yL_container, outfile);
}

std::string get_timestamp(){
    time_t now = time(0);
    tm *ltm = localtime(&now);
    std::ostringstream oss;
    oss << "["
        << 1900 + ltm->tm_year 
        << "-"
        << std::setfill('0') << std::setw(2) << 1 + ltm->tm_mon 
        << "-"
        << std::setfill('0') << std::setw(2) << ltm->tm_mday 
        << " "  
        << std::setfill('0') << std::setw(2) << ltm->tm_hour 
        << ":"
        << std::setfill('0') << std::setw(2) << ltm->tm_min
        << ":"
        << std::setfill('0') << std::setw(2) << ltm->tm_sec
        << "]";
    std::string timestamp = oss.str();   
    return timestamp;
}
