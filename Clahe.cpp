#include "header.h"

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
