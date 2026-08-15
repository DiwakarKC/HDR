#include "header.h"

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
