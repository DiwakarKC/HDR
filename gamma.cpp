#include "header.h"

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
