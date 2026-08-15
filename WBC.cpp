#include "header.h"

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
