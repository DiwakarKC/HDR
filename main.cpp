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

void gamma_lut(int gam[],const Mat& input,Mat& output)
{
    output = Mat(input.rows,input.cols,CV_8UC3);

    for(int y=0; y<input.rows; y++)
    {
        for(int x=0; x<input.cols; x++)
        {
            Vec3b pixels = input.at<Vec3b>(y,x);
            unsigned char B = pixels[0];
            unsigned char G = pixels[1];
            unsigned char R = pixels[2];

            B = gam[B];
            G = gam[G];
            R = gam[R];

            pixels[0] = B;
            pixels[1] = G;
            pixels[2] = R;
            output.at<Vec3b>(y,x)= pixels;
        }
    }
}
void Sigmoid_TM(int& st,int& c,Mat& input,Mat& output)
{
    output = Mat::zeros(input.size(),input.type());
    int Sig[256];
    float Smin = (1.0/(1.0 + pow(2.71828,st*(c/10.0))));
    float Smax = (1.0/(1.0 + pow(2.71828,-st*(1-(c/10.0)))));
    float subs = Smax-Smin;
    for(int i=0; i<256; i++)
    {
        float S = (1.0/(1.0 + pow(2.71828,-st * ((i/255.0) - (c/10.0)))));
        float Snorm = (S-Smin)/subs;
        Sig[i] = 255*Snorm;
    }

    for(int y=0; y<input.rows; y++)
    {
        for(int x=0; x<input.cols; x++)
        {
            Vec3b pixels = input.at<Vec3b>(y,x);
            unsigned char B = pixels[0];
            unsigned char G = pixels[1];
            unsigned char R = pixels[2];

            B = Sig[B];
            G = Sig[G];
            R = Sig[R];

            pixels[0] = B;
            pixels[1] = G;
            pixels[2] = R;

            output.at<Vec3b>(y,x) = pixels;
        }
    }
}

void WBC(Mat& input,Mat& output)
{
    output = Mat(input.rows,input.cols,CV_8UC3);
    int Gx[3][3]= {{-1,0,1},{-2,0,2},{-1,0,1}};
    int Gy[3][3]= {{-1,-2,-1},{0,0,0},{1,2,1}};
    int GradX_R=0,GradX_G=0,GradX_B=0;
    int GradY_R=0,GradY_G=0,GradY_B=0;
    int MR=0,MG=0,MB=0;
    int ER=0,EG=0,EB=0;
    int gray=0;
    float kR=0.0,kG=0.0,kB=0.0;
    for(int y=1; y<input.rows-1; y++)
    {
        for(int x=1; x<input.cols-1; x++)
        {
            for(int i=-1; i<=1; i++)
            {
                for(int j=-1; j<=1; j++)
                {
                    Vec3b pixels = input.at<Vec3b>(y+i,x+j);
                    unsigned char B = pixels[0];
                    unsigned char G = pixels[1];
                    unsigned char R = pixels[2];

                    GradX_B = GradX_B + B * Gx[i+1][j+1];
                    GradX_G = GradX_G + G * Gx[i+1][j+1];
                    GradX_R = GradX_R + R * Gx[i+1][j+1];

                    GradY_B = GradY_B + B * Gy[i+1][j+1];
                    GradY_G = GradY_G + G * Gy[i+1][j+1];
                    GradY_R = GradY_R + R * Gy[i+1][j+1];
                }
            }
            MB = MB + sqrt(GradX_B*GradX_B+GradY_B*GradY_B);
            MG = MG + sqrt(GradX_G*GradX_G+GradY_G*GradY_G);
            MR = MR + sqrt(GradX_R*GradX_R+GradY_R*GradY_R);
        }
    }

    EB = MB/((input.rows-2)*(input.cols-2));
    EG = MG/((input.rows-2)*(input.cols-2));
    ER = MR/((input.rows-2)*(input.cols-2));

    gray = (EB+EG+ER)/3;
    kR = (gray*1.0)/ER;
    kG = (gray*1.0)/EG;
    kB = (gray*1.0)/EB;

//--------------------------------------------------------------------
//Doing WBC on each channels
    for(int y=0; y<input.rows; y++)
    {
        for(int x=0; x<input.cols; x++)
        {
            Vec3b pixels = input.at<Vec3b>(y,x);
            unsigned char B = pixels[0];
            unsigned char G = pixels[1];
            unsigned char R = pixels[2];

            B = kB * B;
            G = kG * G;
            R = kR * R;

            B = (B>255)?255:B;
            G = (G>255)?255:G;
            R = (R>255)?255:R;

            pixels[0] = B;
            pixels[1] = G;
            pixels[2] = R;

            output.at<Vec3b>(y,x) = pixels;
        }
    }
}
void unmask_sharp(int& level,Mat& input,Mat& output)
{
    int mask[3][3] = {{1,2,1},{2,4,2},{1,2,1}};
    int red=0,blue=0,green=0;
    int r_o=0,g_o=0,b_o=0;
    Vec3b pixels1;
    output = Mat::zeros(input.size(),input.type());
    for(int y=1; y<input.rows-1; y++)
    {
        for(int x=1; x<input.cols-1; x++)
        {
            for(int i=-1; i<=1; i++)
            {
                for(int j=-1; j<=1; j++)
                {
                    Vec3b pixels = input.at<Vec3b>(y+i,x+j);
                    unsigned char B = pixels[0];
                    unsigned char G = pixels[1];
                    unsigned char R = pixels[2];

                    blue = blue + B * mask[i+1][j+1];
                    green = green + G * mask[i+1][j+1];
                    red = red + R * mask[i+1][j+1];
                }
            }
            Vec3b pixels = input.at<Vec3b>(y,x);
            unsigned char B = pixels[0];
            unsigned char G = pixels[1];
            unsigned char R = pixels[2];

            blue=blue/16;
            green=green/16;
            red=red/16;

//            blue = max(0,min(255,blue));
//            green = max(0,min(255,green));
//            red = max(0,min(255,red));

//                blue = (blue<0)?0:blue;
//                blue = (blue>255)?255:blue;
//
//                 green = (green<0)?0:green;
//                green = (green>255)?255:green;
//
//                red= (red<0)?0:red;
//                red = (red>255)?255:red;
//                 cout << blue <<endl;
            b_o = B + ((B-blue)*level)/100;
            g_o = G + ((G-green)*level)/100;
            r_o = R + ((R-red)*level)/100;

            pixels1[0] = max(0,min(255,b_o));
            pixels1[1] = max(0,min(255,g_o));
            pixels1[2] = max(0,min(255,r_o));

            output.at<Vec3b>(y,x) = pixels1;
            blue=0;
            green=0;
            red=0;
        }
    }

}
void Clahe(int& ClipLimit,Mat& image,Mat& Clahe_out)
{
    Clahe_out = Mat::zeros(image.size(),image.type());
    int hist_r[8][8][256],hist_g[8][8][256],hist_b[8][8][256];
    int lut_r[8][8][256],lut_g[8][8][256],lut_b[8][8][256];
    int excess_r[8],excess_g[8],excess_b[8];
    int cdf_r[8][256],cdf_g[8][256],cdf_b[8][256];
    int redis_r[8],redis_g[8],redis_b[8];
    int tile_h = image.rows/8;
    int tile_w = image.cols/8;
    unsigned char B=0,G=0,R=0;
    unsigned int tile_total = tile_h*tile_w;

    memset(hist_r,0,sizeof(hist_r));
    memset(hist_g,0,sizeof(hist_g));
    memset(hist_b,0,sizeof(hist_b));
    memset(lut_r,0,sizeof(lut_r));
    memset(lut_g,0,sizeof(lut_g));
    memset(lut_b,0,sizeof(lut_b));

    for(int i =0; i<8; i++)
    {
        memset(excess_r,0,sizeof(excess_r));
        memset(excess_g,0,sizeof(excess_g));
        memset(excess_b,0,sizeof(excess_b));
        memset(cdf_r,0,sizeof(cdf_r));
        memset(cdf_g,0,sizeof(cdf_g));
        memset(cdf_b,0,sizeof(cdf_b));
        memset(redis_r,0,sizeof(redis_r));
        memset(redis_g,0,sizeof(redis_g));
        memset(redis_b,0,sizeof(redis_b));

        int a = tile_h*i;


        for(int y=0; y<tile_h; y++)
        {
            for(int x=0; x<tile_w; x++)
            {
                Vec3b pixels = image.at<Vec3b>(y+a,x);
                B = pixels[0];
                G = pixels[1];
                R = pixels[2];

                hist_b[i][0][B] = hist_b[i][0][B]+1;
                hist_g[i][0][G] = hist_g[i][0][G]+1;
                hist_r[i][0][R] = hist_r[i][0][R]+1;

                pixels = image.at<Vec3b>(y+a,x+(tile_w*1));
                B = pixels[0];
                G = pixels[1];
                R = pixels[2];

                hist_b[i][1][B] = hist_b[i][1][B]+1;
                hist_g[i][1][G] = hist_g[i][1][G]+1;
                hist_r[i][1][R] = hist_r[i][1][R]+1;

                pixels = image.at<Vec3b>(y+a,x+(tile_w*2));
                B = pixels[0];
                G = pixels[1];
                R = pixels[2];

                hist_b[i][2][B] = hist_b[i][2][B]+1;
                hist_g[i][2][G] = hist_g[i][2][G]+1;
                hist_r[i][2][R] = hist_r[i][2][R]+1;

                pixels = image.at<Vec3b>(y+a,x+(tile_w*3));
                B = pixels[0];
                G = pixels[1];
                R = pixels[2];

                hist_b[i][3][B] = hist_b[i][3][B]+1;
                hist_g[i][3][G] = hist_g[i][3][G]+1;
                hist_r[i][3][R] = hist_r[i][3][R]+1;

                pixels = image.at<Vec3b>(y+a,x+(tile_w*4));
                B = pixels[0];
                G = pixels[1];
                R = pixels[2];

                hist_b[i][4][B] = hist_b[i][4][B]+1;
                hist_g[i][4][G] = hist_g[i][4][G]+1;
                hist_r[i][4][R] = hist_r[i][4][R]+1;

                pixels = image.at<Vec3b>(y+a,x+(tile_w*5));
                B = pixels[0];
                G = pixels[1];
                R = pixels[2];

                hist_b[i][5][B] = hist_b[i][5][B]+1;
                hist_g[i][5][G] = hist_g[i][5][G]+1;
                hist_r[i][5][R] = hist_r[i][5][R]+1;

                pixels = image.at<Vec3b>(y+a,x+(tile_w*6));
                B = pixels[0];
                G = pixels[1];
                R = pixels[2];

                hist_b[i][6][B] = hist_b[i][6][B]+1;
                hist_g[i][6][G] = hist_g[i][6][G]+1;
                hist_r[i][6][R] = hist_r[i][6][R]+1;

                pixels = image.at<Vec3b>(y+a,x+(tile_w*7));
                B = pixels[0];
                G = pixels[1];
                R = pixels[2];

                hist_b[i][7][B] = hist_b[i][7][B]+1;
                hist_g[i][7][G] = hist_g[i][7][G]+1;
                hist_r[i][7][R] = hist_r[i][7][R]+1;
            }
        }

        for(int j=0; j<8; j++)
        {
            for(int m=0; m<256; m++)
            {
                if(hist_b[i][j][m]>ClipLimit)
                {
                    excess_b[j]+=(hist_b[i][j][m]-ClipLimit);
                    hist_b[i][j][m] = ClipLimit;
                }
                if(hist_g[i][j][m]>ClipLimit)
                {
                    excess_g[j]+=(hist_g[i][j][m]-ClipLimit);
                    hist_g[i][j][m] = ClipLimit;
                }
                if(hist_r[i][j][m]>ClipLimit)
                {
                    excess_r[j]+=(hist_r[i][j][m]-ClipLimit);
                    hist_r[i][j][m] = ClipLimit;
                }
            }
        }
        // Residual limit redistribute
        redis_b[0] = excess_b[0]/256;
        redis_g[0] = excess_g[0]/256;
        redis_r[0] = excess_r[0]/256;

        redis_b[1] = excess_b[1]/256;
        redis_g[1] = excess_g[1]/256;
        redis_r[1] = excess_r[1]/256;

        redis_b[2] = excess_b[2]/256;
        redis_g[2] = excess_g[2]/256;
        redis_r[2] = excess_r[2]/256;

        redis_b[3] = excess_b[3]/256;
        redis_g[3] = excess_g[3]/256;
        redis_r[3] = excess_r[3]/256;

        redis_b[4] = excess_b[4]/256;
        redis_g[4] = excess_g[4]/256;
        redis_r[4] = excess_r[4]/256;

        redis_b[5] = excess_b[5]/256;
        redis_g[5] = excess_g[5]/256;
        redis_r[5] = excess_r[5]/256;

        redis_b[6] = excess_b[6]/256;
        redis_g[6] = excess_g[6]/256;
        redis_r[6] = excess_r[6]/256;

        redis_b[7] = excess_b[7]/256;
        redis_g[7] = excess_g[7]/256;
        redis_r[7] = excess_r[7]/256;
        for(int m=0; m<256; m++)
        {
            hist_b[i][0][m] += redis_b[0];
            hist_g[i][0][m] += redis_g[0];
            hist_r[i][0][m] += redis_r[0];

            hist_b[i][1][m] += redis_b[1];
            hist_g[i][1][m] += redis_g[1];
            hist_r[i][1][m] += redis_r[1];

            hist_b[i][2][m] += redis_b[2];
            hist_g[i][2][m] += redis_g[2];
            hist_r[i][2][m] += redis_r[2];

            hist_b[i][3][m] += redis_b[3];
            hist_g[i][3][m] += redis_g[3];
            hist_r[i][3][m] += redis_r[3];

            hist_b[i][4][m] += redis_b[4];
            hist_g[i][4][m] += redis_g[4];
            hist_r[i][4][m] += redis_r[4];

            hist_b[i][5][m] += redis_b[5];
            hist_g[i][5][m] += redis_g[5];
            hist_r[i][5][m] += redis_r[5];

            hist_b[i][6][m] += redis_b[6];
            hist_g[i][6][m] += redis_g[6];
            hist_r[i][6][m] += redis_r[6];

            hist_b[i][7][m] += redis_b[7];
            hist_g[i][7][m] += redis_g[7];
            hist_r[i][7][m] += redis_r[7];
        }

//------------------CDF calculation--------------------------------

        cdf_b[0][0] = hist_b[i][0][0];
        cdf_g[0][0] = hist_g[i][0][0];
        cdf_r[0][0] = hist_r[i][0][0];

        cdf_b[1][0] = hist_b[i][1][0];
        cdf_g[1][0] = hist_g[i][1][0];
        cdf_r[1][0] = hist_r[i][1][0];

        cdf_b[2][0] = hist_b[i][2][0];
        cdf_g[2][0] = hist_g[i][2][0];
        cdf_r[2][0] = hist_r[i][2][0];

        cdf_b[3][0] = hist_b[i][3][0];
        cdf_g[3][0] = hist_g[i][3][0];
        cdf_r[3][0] = hist_r[i][3][0];

        cdf_b[4][0] = hist_b[i][4][0];
        cdf_g[4][0] = hist_g[i][4][0];
        cdf_r[4][0] = hist_r[i][4][0];

        cdf_b[5][0] = hist_b[i][5][0];
        cdf_g[5][0] = hist_g[i][5][0];
        cdf_r[5][0] = hist_r[i][5][0];

        cdf_b[6][0] = hist_b[i][6][0];
        cdf_g[6][0] = hist_g[i][6][0];
        cdf_r[6][0] = hist_r[i][6][0];

        cdf_b[7][0] = hist_b[i][7][0];
        cdf_g[7][0] = hist_g[i][7][0];
        cdf_r[7][0] = hist_r[i][7][0];
        for(int m =1; m<256; m++)
        {
            cdf_b[0][m] =cdf_b[0][m-1] +  hist_b[i][0][m];
            cdf_g[0][m] =cdf_g[0][m-1] +  hist_g[i][0][m];
            cdf_r[0][m] =cdf_r[0][m-1] +  hist_r[i][0][m];

            cdf_b[1][m] =cdf_b[1][m-1] +  hist_b[i][1][m];
            cdf_g[1][m] =cdf_g[1][m-1] +  hist_g[i][1][m];
            cdf_r[1][m] =cdf_r[1][m-1] +  hist_r[i][1][m];

            cdf_b[2][m] =cdf_b[2][m-1] +  hist_b[i][2][m];
            cdf_g[2][m] =cdf_g[2][m-1] +  hist_g[i][2][m];
            cdf_r[2][m] =cdf_r[2][m-1] +  hist_r[i][2][m];

            cdf_b[3][m] =cdf_b[3][m-1] +  hist_b[i][3][m];
            cdf_g[3][m] =cdf_g[3][m-1] +  hist_g[i][3][m];
            cdf_r[3][m] =cdf_r[3][m-1] +  hist_r[i][3][m];

            cdf_b[4][m] =cdf_b[4][m-1] +  hist_b[i][4][m];
            cdf_g[4][m] =cdf_g[4][m-1] +  hist_g[i][4][m];
            cdf_r[4][m] =cdf_r[4][m-1] +  hist_r[i][4][m];

            cdf_b[5][m] =cdf_b[5][m-1] +  hist_b[i][5][m];
            cdf_g[5][m] =cdf_g[5][m-1] +  hist_g[i][5][m];
            cdf_r[5][m] =cdf_r[5][m-1] +  hist_r[i][5][m];

            cdf_b[6][m] =cdf_b[6][m-1] +  hist_b[i][6][m];
            cdf_g[6][m] =cdf_g[6][m-1] +  hist_g[i][6][m];
            cdf_r[6][m] =cdf_r[6][m-1] +  hist_r[i][6][m];

            cdf_b[7][m] =cdf_b[7][m-1] +  hist_b[i][7][m];
            cdf_g[7][m] =cdf_g[7][m-1] +  hist_g[i][7][m];
            cdf_r[7][m] =cdf_r[7][m-1] +  hist_r[i][7][m];
        }
//----------------LUT calculatin-----------------------------------------
        for(int m=0; m<256; m++)
        {
            lut_b[i][0][m] = (cdf_b[0][m] * 255)/(tile_total);
            lut_g[i][0][m] = (cdf_g[0][m] * 255)/(tile_total);
            lut_r[i][0][m] = (cdf_r[0][m] * 255)/(tile_total);

            lut_b[i][1][m] = (cdf_b[1][m] * 255)/(tile_total);
            lut_g[i][1][m] = (cdf_g[1][m] * 255)/(tile_total);
            lut_r[i][1][m] = (cdf_r[1][m] * 255)/(tile_total);

            lut_b[i][2][m] = (cdf_b[2][m] * 255)/(tile_total);
            lut_g[i][2][m] = (cdf_g[2][m] * 255)/(tile_total);
            lut_r[i][2][m] = (cdf_r[2][m] * 255)/(tile_total);

            lut_b[i][3][m] = (cdf_b[3][m] * 255)/(tile_total);
            lut_g[i][3][m] = (cdf_g[3][m] * 255)/(tile_total);
            lut_r[i][3][m] = (cdf_r[3][m] * 255)/(tile_total);

            lut_b[i][4][m] = (cdf_b[4][m] * 255)/(tile_total);
            lut_g[i][4][m] = (cdf_g[4][m] * 255)/(tile_total);
            lut_r[i][4][m] = (cdf_r[4][m] * 255)/(tile_total);

            lut_b[i][5][m] = (cdf_b[5][m] * 255)/(tile_total);
            lut_g[i][5][m] = (cdf_g[5][m] * 255)/(tile_total);
            lut_r[i][5][m] = (cdf_r[5][m] * 255)/(tile_total);

            lut_b[i][6][m] = (cdf_b[6][m] * 255)/(tile_total);
            lut_g[i][6][m] = (cdf_g[6][m] * 255)/(tile_total);
            lut_r[i][6][m] = (cdf_r[6][m] * 255)/(tile_total);

            lut_b[i][7][m] = (cdf_b[7][m] * 255)/(tile_total);
            lut_g[i][7][m] = (cdf_g[7][m] * 255)/(tile_total);
            lut_r[i][7][m] = (cdf_r[7][m] * 255)/(tile_total);

        }

    }
    for(int y=0; y<image.rows; y++)
    {
        for(int x=0; x<image.cols; x++)
        {
            Vec3b pixels = image.at<Vec3b>(y,x);
            unsigned char B = pixels[0];
            unsigned char G = pixels[1];
            unsigned char R = pixels[2];

            int tx = x/tile_w;
            int ty = y/tile_h;

            int tx1 = (tx + 1 < 8) ? tx+1:tx;
            int ty1 = (ty + 1 < 8) ? ty+1:ty;

            int wx = ((x%tile_w)*255)/tile_w;
            int wy = ((y%tile_h)*255)/tile_h;

            int p1b = lut_b[ty][tx][B];
            int p2b = lut_b[ty][tx1][B];
            int p3b = lut_b[ty1][tx][B];
            int p4b = lut_b[ty1][tx1][B];

            int p1g = lut_g[ty][tx][G];
            int p2g = lut_g[ty][tx1][G];
            int p3g = lut_g[ty1][tx][G];
            int p4g = lut_g[ty1][tx1][G];

            int p1r = lut_r[ty][tx][R];
            int p2r = lut_r[ty][tx1][R];
            int p3r = lut_r[ty1][tx][R];
            int p4r = lut_r[ty1][tx1][R];

            int outb = ((255-wx)*(255-wy)*p1b + wx*(255-wy)*p2b + (255-wx)*wy*p3b + wx*wy*p4b)>>16;
            int outg = ((255-wx)*(255-wy)*p1g + wx*(255-wy)*p2g + (255-wx)*wy*p3g + wx*wy*p4g)>>16;
            int outr = ((255-wx)*(255-wy)*p1r + wx*(255-wy)*p2r + (255-wx)*wy*p3r + wx*wy*p4r)>>16;

            pixels[0] = outb;
            pixels[1] = outg;
            pixels[2] = outr;

            Clahe_out.at<Vec3b>(y,x) = pixels;
        }
    }
}

void onParameterchange(int,void* userdata)
{
    Parameters* data =(Parameters*)userdata;
    data->cliplimit = getTrackbarPos("Clip","Parameters");
    data->val = getTrackbarPos("Gamma","Parameters");
    data->st = getTrackbarPos("Strength","Parameters");
    data->c = getTrackbarPos("Center","Parameters");
    data->level = getTrackbarPos("Sharp","Parameters");
    int gam[256];
    for(int i=0; i<256; i++)
    {
        gam[i] = 255 * pow(i/255.0,(data->val)/10.0);
    }
    gamma_lut(gam,data->input,data->output);
    Clahe(data->cliplimit,data->output,data->Clahe_output);
    Sigmoid_TM(data->st,data->c,data->Clahe_output,data->Sig_output);
    unmask_sharp(data->level,data->Sig_output,data->final_output);
//    WBC(data->Sig_output,data->final_output);
    imshow("Output",data->final_output);

}
Mat frameprocess(const Mat& input,int gam[],int cliplimit,int st,int c,int level)
{
    Mat gammaout;
    Mat claheout;
    Mat sigout;
    Mat finalout;

    gamma_lut(gam,input,gammaout);
    Clahe(cliplimit,gammaout,claheout);
    Sigmoid_TM(st,c,claheout,sigout);
    unmask_sharp(level,sigout,finalout);

    return finalout;

}

// Using the OpenCV Trackbar to change the value of variables(cliplimit,gamma etc)
int main()
{
    Parameters data;
    int val=5,st=8,c=5,limit=50,level=50;
    int ID=0;
    int count_in=0;
    Mat frame;
    int count = 0;
    cout<< "Enter the video file: ";
//    string input_file = "C:\\Users\\Diwakar\\Documents\\New folder\\Image_Processing\\video\\Emotions-30Sec.mp4";
    string input_file;
    getline(cin,input_file);
    cout << "Enter the frame number: ";
    cin >> count_in;

    fs::path input_path(input_file);

    string output_file = input_path.stem().string() + "_HDR_Merged" + input_path.extension().string();
//    fs::path output_path = input_path.parent_path() / (output_file);
    fs::path output_path = "video/" + (output_file);

    // Reading the input video file to extract audio"
//    string command_in ="ffmpeg -y "
//                       "-i \"" + input_path.string() + "\" "
//                       "-vn "
//                       "-acodec copy "
//                       "audio.aac";

    string command_in ="ffmpeg -y "
                       "-i \"" + input_path.string() + "\" "
                       "-vn "
                       "-c:a aac "
                       "-b:a 192k "
                       "audio.aac";

    system(command_in.c_str());

    //Creating a folder name, if not there
    string folder="setting";
    if(!fs::exists(folder))
        fs::create_directory(folder);

    folder="video";
    if(!fs::exists(folder))
        fs::create_directory(folder);

    folder = "image";
    if(!fs::exists(folder))
        fs::create_directory(folder);
    //--------------------------------------------------

    // Reading the setting.yml file for default the parameters value
    // IF the program is run for first time there will be no setting.yml file ,but there will be no issue
    FileStorage fsRead("setting/setting.yml",FileStorage::READ);
    if(fsRead.isOpened())
    {
        fsRead["limit"]>>limit;
        fsRead["gamma"]>>val;
        fsRead["strength"]>>st;
        fsRead["center"]>>c;
        fsRead["sharp"]>>level;
        fsRead.release();
    }
    VideoCapture vid(input_file);
    if(!vid.isOpened())
    {
        cout<<"Video file not found"<<endl;
        return -1;
    }
    int frame_width = vid.get(CAP_PROP_FRAME_WIDTH);
    int frame_height = vid.get(CAP_PROP_FRAME_HEIGHT);
    double fps = vid.get(CAP_PROP_FPS);

    VideoWriter writer("video/output.mp4",VideoWriter::fourcc('a','v','c','1'),fps,Size(frame_width,frame_height));

    if(!writer.isOpened())
        return -1;

    while(vid.read(frame))
    {
        ID++;
        if(ID==count_in)
        {
            break;
        }
    }

    data.input = frame.clone();

    if(data.input.empty())
    {
        cout<<"Invalid Input Image";
        return -1;
    }
    //Creating a window for track bar
    namedWindow("Parameters",WINDOW_AUTOSIZE);
    // Creating a trackbar for each paramters in a single window
    createTrackbar("Clip","Parameters",&limit,250,onParameterchange,&data);
    createTrackbar("Gamma","Parameters",&val,20,onParameterchange,&data);
    createTrackbar("Strength","Parameters",&st,15,onParameterchange,&data);
    createTrackbar("Center","Parameters",&c,10,onParameterchange,&data);
    createTrackbar("Sharp","Parameters",&level,100,onParameterchange,&data);

    //Callback function that will run the code unless terminated
    onParameterchange(0,&data);
    waitKey(0);
    destroyAllWindows();

    imwrite("image/gamma_output.jpg",data.output);
    imwrite("image/clahe_output.jpg",data.Clahe_output);
    imwrite("image/Sig_output.jpg",data.Sig_output);
    imwrite("image/Final_output.jpg",data.final_output);

    // To restart the video frame reading from 0
    vid.set(CAP_PROP_POS_FRAMES,0);

    Mat frame1;
    Mat frame2;
    Mat frame3;

    int gam[256];
    for(int i=0; i<256; i++)
    {
        gam[i] = 255 * pow(i/255.0,data.val/10.0);
    }

    while(true)
    {
        if(!vid.read(frame1))
        {
            break;
        }
        bool hasframe2 = vid.read(frame2);
        bool hasframe3 = vid.read(frame3);
        future<Mat> future1 = async(launch::async,frameprocess,frame1.clone(),gam,data.cliplimit,data.st,data.c,data.level);
        future<Mat> future2;
        if(hasframe2)
        {
            future2 = async(launch::async,frameprocess,frame2.clone(),gam,data.cliplimit,data.st,data.c,data.level);
        }
        future<Mat> future3;
        if(hasframe3)
        {
            future3 = async(launch::async,frameprocess,frame3.clone(),gam,data.cliplimit,data.st,data.c,data.level);
        }
        Mat result1 = future1.get();
        writer.write(result1);
        if(hasframe2)
        {
            Mat result2 = future2.get();
            writer.write(result2);
        }
        if(hasframe3)
        {
            Mat result3 = future3.get();
            writer.write(result3);
        }
        count++;
        cout <<count<<endl;
        if(!hasframe2)
        {
            break;
        }
        if(!hasframe3)
        {
            break;
        }
    }
    // Releasing the input and output video to complete them
    vid.release();
    writer.release();
    cout << "Final" <<endl;

    // Command for reading the HDR output video and audio.acc and then merge them together
    string command =
        "ffmpeg -y "
        "-i video/output.mp4 "
        "-i audio.aac "
        "-c:v copy "
        "-c:a copy "
        "\"" + output_path.string() + "\"";
    system(command.c_str());

    //Write the new values in setting.yml file for next run default values
    FileStorage fsWrite("setting/setting.yml",FileStorage::WRITE);
    fsWrite << "limit" << data.cliplimit;
    fsWrite << "gamma" << data.val;
    fsWrite << "strength" << data.st;
    fsWrite << "center" << data.c;
    fsWrite << "sharp" << data.level;
    fsWrite.release();
    return 0;
}
