

#include <cv.h>
#include <highgui.h>
#include <math.h>

#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    //VideoCapture mycap;
    //if( argc != 2 && !mycap.open(argv[1]))
    //  return -1;

    //mycap >> img;
    try
    {
        Mat src, dst, color_dst;
        if( argc != 2 || !(src=imread(argv[1], 0)).data)
          return -1;

        Canny( src, dst, 50, 200, 3 );
        cvtColor( dst, color_dst, CV_GRAY2BGR );

        vector<Vec2f> lines;
        HoughLines( dst, lines, 2, CV_PI/90, 120 );

        cout << "Found " << lines.size() << " lines" << endl;

        for( size_t i = 0; i < lines.size()/5; i++ )
        {
          float rho = lines[i][0];
          float theta = lines[i][1];
          double a = cos(theta), b = sin(theta);
          double x0 = a*rho, y0 = b*rho;
          Point pt1(cvRound(x0 + 1000*(-b)),
          cvRound(y0 + 1000*(a)));
          Point pt2(cvRound(x0 - 1000*(-b)),
          cvRound(y0 - 1000*(a)));
          line( color_dst, pt1, pt2, Scalar(0,0,255), 3, 8 );
        }

        namedWindow( "lines", CV_WINDOW_AUTOSIZE );
        imshow( "lines", color_dst );
        waitKey(0);
        //imwrite(argv[2], img);
    }
    catch (const cv::Exception &e)
    {
        cerr << "Caught exception: " << e.what() << endl;
    }
    return 0;
}
