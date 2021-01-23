#include <string>
#include <fstream>
#include <vector>
#include <utility> // std::pair
#include <sstream>
#include <iostream>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/point_xy.hpp>

using namespace std;

vector<vector<double>> read_csv( string filename ){
    // Reads a CSV file with 2 columns into a vector of <vector<double>>
    vector<vector<double>> result(2);

    // Create an input filestream
	//const char *c = filename.c_str();
    ifstream myFile(filename.c_str());
	
    // Make sure the file is open
    if(!myFile.is_open()) throw runtime_error("Could not open file");

    // Helper vars
    string line;
    double val;

	if(myFile.good())
	{
		// Read data, line by line
		while(getline(myFile, line))
		{
			// Create a stringstream of the current line
			stringstream ss(line);
			
			// Keep track of the current column index
			int colIdx = 0;
			// Extract each integer
			while(ss >> val){
				
				// Add the current integer to the 'colIdx' column's values vector
				result[colIdx].push_back(val);
				
				// If the next token is a comma, ignore it and move on
				if(ss.peek() == ',') ss.ignore();
				
				// Increment the column index
				colIdx++;
			}
		}
	}

    // Close file
    myFile.close();
    return result;
}

double getyL(string ref, double positionx , double positiony, bool debug)
{
	typedef boost::geometry::model::d2::point_xy<double> point_type;
	typedef boost::geometry::model::linestring<point_type> linestring_type;
	
	if (debug) cout << "Entry: Read CSV" << endl;
    vector<vector<double>> gold_ref = read_csv(ref);
	if (debug) cout << "Exit: Read CSV" << endl;
	
	point_type p(positionx, positiony);
	linestring_type line;
	double x = 0;
	double y = 0;
	
	for (int i = 0; i < gold_ref[0].size(); ++i)
    {
		for(int j = 0; j < gold_ref.size(); ++j)
		{
			if (j == 0) x = gold_ref[j][i];
			else y = gold_ref[j][i];
			
		}
		line.push_back(point_type(x,y));
        //cout << x << " , " << y << "\n" ;
    }
	
	double yl = boost::geometry::distance(p, line);
	//cout << "Point-Line: " << yl << endl;
	return yl;
}
 
/*int main(int argc , char *argv[])
  {
	typedef boost::geometry::model::d2::point_xy<double> point_type;
	typedef boost::geometry::model::linestring<point_type> linestring_type;
    vector<vector<double>> gold_ref = read_csv("/home/sayandipde/Approx_IBC/hil/client/Webots/worlds/city_ref.csv");
	
	point_type p(1,2);
	linestring_type line;
	double x = 0;
	double y = 0;
	
	for (int i = 0; i < gold_ref[0].size(); ++i)
    {
		for(int j = 0; j < gold_ref.size(); ++j)
		{
			//cout << gold_ref[j][i];
			//if(j != gold_ref.size() - 1) cout << ","; // No comma at end of line
			if (j == 0) x = gold_ref[j][i];
			else y = gold_ref[j][i];
			
		}
		line.push_back(point_type(x,y));
        cout << x << " , " << y << "\n" ;
    }
	
	cout << "Point-Line: " << boost::geometry::distance(p, line) << endl;
	return 0;
  }*/
