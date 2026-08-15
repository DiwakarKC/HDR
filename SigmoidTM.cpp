#include "header.h"

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
