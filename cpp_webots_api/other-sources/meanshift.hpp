#pragma once 
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#define PI 3.1415926535898
#define EPSILON 0.0015
#define CLUSTER_EPSILON 2
using namespace cv;
using namespace std;
typedef std::vector<float> Point7f;
struct MeanShift
{
	Point7f pos;
	Point7f res;
	int label = -1;
};

class MeanShiftClass
{
public:
	// bool stop;
	// int point_num;
	// int cluster_num;
	// int kernel_bandwidth;
	// vector<MeanShift> dataset;
	MeanShiftClass();
	~MeanShiftClass();
	// void DoMeanShiftCluster();
    void LoadData(float emb_float[256][512][7], int bin_seg_pred[256][512], vector<MeanShift>& dataset);
    void LabelClusters(vector<MeanShift>& dataset, vector<MeanShift>& shifted);
	// void ShowClusterResult();
	bool ShiftOnce(MeanShift& p, int bandwidth, vector<MeanShift>& dataset);
	// int GetManhattanDistance(Point2f p0, Point2f p1);
	// float GaussianKernel(int distance, int bandwidth);
	// float GetEuclideanDistance(const vector<float> &point_a, const vector<float> &point_b);

private:
	// bool stop;
	// int point_num;
	// int cluster_num;
	// int kernel_bandwidth;
	// vector<MeanShift> dataset;

	// void LabelClusters();
	// void ShowClusterResult();
	// void ShiftOnce(MeanShift& p);
	// int GetManhattanDistance(Point2f p0, Point2f p1);
	float GaussianKernel(int distance, int bandwidth);
	float GetEuclideanDistance(const vector<float> &point_a, const vector<float> &point_b);
	//void LoadData(float emb_float[256][512][7], int bin_seg_pred[256][512], vector<MeanShift>& dataset);
};