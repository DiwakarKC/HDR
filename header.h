#ifndef HEADER_H
#define HEADER_H
#include<iostream>
#include<opencv2/opencv.hpp>
#include<filesystem>
#include <cstdlib>
#include<future>

using namespace std;
using namespace cv;
// Using a name fs as filesystem
namespace fs = filesystem;

// Creating global structure variable for callback function
struct Parameters
{
    Mat input;
    Mat output;
    Mat Clahe_output;
    Mat Sig_output;
    Mat final_output;
    int cliplimit;
    int st;
    int c;
    int val;
    int level;

};
void gamma_lut(int gam[],const Mat& input,Mat& output);
void Sigmoid_TM(int& st,int& c,Mat& input,Mat& output);
void WBC(Mat& input,Mat& output);
void unmask_sharp(int& level,Mat& input,Mat& output);
void Clahe(int& ClipLimit,Mat& image,Mat& Clahe_out);

#endif // HEADER_H
