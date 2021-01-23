#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>
#include "meanshift.hpp"

MeanShiftClass::MeanShiftClass()
{
}


MeanShiftClass::~MeanShiftClass()
{
}

// void MeanShiftClass::DoMeanShiftCluster(float emb_float[256][512][7], int bin_seg_pred[256][512], int bandwidth)
// {
//     vector<MeanShift> dataset;
//     LoadData(emb_float, bin_seg_pred, dataset);
// 	for (int i = 0; i < dataset.size(); i++)
// 	{
// 		stop = false;
// 		while (!stop)
// 		{
// 			ShiftOnce(dataset[i], bandwidth, dataset);
// 		}
// 		// cout <<"("<< dataset[i].res.x << "," << dataset[i].res.y<<")" << endl;
// 	}
// 	LabelClusters(dataset);
//     int result[256][512];
//     int idx= 0;
//     for(int y0 = 0; y0<256; y0++)
// 	{
// 		for(int x0 = 0; x0<512; x0++)
// 		{
//             if (bin_seg_pred[y0][x0]>0)
//             {
//                 result[y0][x0] = dataset[idx].label +1;
//                 idx++;
//             }
//         }
//     }
//     cout << "current_index = " << idx << endl;
// 	// ShowClusterResult();
// }


void MeanShiftClass::LoadData(float emb_float[256][512][7], int bin_seg_pred[256][512], vector<MeanShift>& dataset)
{
    
	for(int y2 = 0; y2<256; y2++)
	{
		for(int x2 = 0; x2<512; x2++)
		{
            if (bin_seg_pred[y2][x2]>0)
            {
                MeanShift tmp;
                for(int z2 = 0; z2<7; z2++)
                {
                    tmp.pos.push_back(emb_float[y2][x2][z2]);
                    tmp.res.push_back(emb_float[y2][x2][z2]);
                }
                dataset.push_back(tmp);
            }
        }
    }
}

// int MeanShiftClass::GetManhattanDistance(Point2f p0, Point2f p1)
// {
// 	return abs(p0.x - p1.x) + abs(p0.y - p1.y);
// }
float MeanShiftClass::GetEuclideanDistance(const vector<float> &point_a, const vector<float> &point_b)
{
    float total = 0;
    for(int i=0; i<point_a.size(); i++){
        const float temp = (point_a[i] - point_b[i]);
        total += temp*temp;
    }
    return sqrt(total);
}

float MeanShiftClass::GaussianKernel(int distance, int bandwidth)
{
	return exp(-0.5*(distance*distance) / (bandwidth*bandwidth));
}

bool MeanShiftClass::ShiftOnce(MeanShift& p, int bandwidth, vector<MeanShift>& dataset)
{
	float k1_sum = 0;
	float k2_sum = 0;
    float k3_sum = 0;
	float k4_sum = 0;
    float k5_sum = 0;
	float k6_sum = 0;	
    float k7_sum = 0;
	float weight_sum = 0;
	for (int i = 0; i < dataset.size(); i++)
	{
        const Point7f k= dataset[i].pos;
		float tmp_distance = GetEuclideanDistance(p.res, k);
		float weight = GaussianKernel(tmp_distance, bandwidth);
		k1_sum += k[0] * weight;
        k2_sum += k[1] * weight;
        k3_sum += k[2] * weight;
        k4_sum += k[3] * weight;
        k5_sum += k[4] * weight;
        k6_sum += k[5] * weight;
        k7_sum += k[6] * weight;
		weight_sum += weight;
	}
	Point7f shift_vector = {k1_sum/ weight_sum, k2_sum/ weight_sum, k3_sum/ weight_sum, k4_sum/ weight_sum, k5_sum/ weight_sum, k6_sum/ weight_sum, k7_sum/ weight_sum};
	float shift_distance = GetEuclideanDistance(p.res, shift_vector);
	// cout << "shift_distance = " << shift_distance << endl;
    bool stop = false;
	if (shift_distance < EPSILON)
		stop = true;

	p.res = shift_vector;
    return stop;
}

void MeanShiftClass::LabelClusters(vector<MeanShift>& dataset, vector<MeanShift>& shifted)
{
	int current_label = -1;
	for (int i = 0; i< dataset.size(); i++)
	{
		if (dataset[i].label != -1)
			continue;
		current_label++;
		for (int j = 0; j < dataset.size(); j++)
		{
			if (GetEuclideanDistance(shifted[i].res, shifted[j].res) < CLUSTER_EPSILON)
			{
				dataset[j].label = current_label;
			}
		}
	}
    cout << "current_label = " << current_label << endl;
}