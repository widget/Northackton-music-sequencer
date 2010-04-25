

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

    Mat img, gray;
    //mycap >> img;
    if( argc != 2)
      return -1;
    try
    {
        img = imread(argv[1], 1);
        cout << "Loaded image" << endl;
        cvtColor(img, gray, CV_BGR2GRAY);
        cout << "Greyscaled" << endl;
        // smooth it, otherwise a lot of false circles may be detected
        GaussianBlur( gray, gray, Size(9, 9), 2, 2 );
        cout << "Blurred" << endl;
        vector<Vec3f> circles;
        HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 2, 40, 200, 100, 20, 28);
        cout << "Found " << circles.size() << " circles" << endl;
        for( size_t i = 0; i < circles.size(); i++ )
        {
            Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
            int radius = cvRound(circles[i][2]);
            cout << "Centre (" << circles[i][0] << ", " << circles[i][1] << ") radius " << circles[i][2] << endl;
            // draw the circle center
            circle( img, center, 3, Scalar(0,255,0), -1, 8, 0 );
            // draw the circle outline
            circle( img, center, radius, Scalar(0,0,255), 3, 8, 0 );
        }
        namedWindow( "circles", CV_WINDOW_AUTOSIZE );
        //moveWindow("circles", 100, 100);
        //resizeWindow("circles", 640, 480);
        imshow( "circles", img );
        waitKey(0);
        //imwrite(argv[2], img);
    }
    catch (const cv::Exception &e)
    {
        cerr << "Caught exception: " << e.what() << endl;
    }
    return 0;
}
