
#include <unistd.h>
#include <stdint.h>

#include <iostream>
#include <string>

#include <cv.h>
#include <highgui.h>
#include <math.h>


using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    int video = -1;
    int c;
    string filename;

    Point poi[16][4];

    for (unsigned int i = 0; i < 16; ++i)
    {
        for (unsigned int j = 0; j < 4; ++j)
        {
            poi[i][j].x = 50 + (i*36);
            poi[i][j].y = 200 + (j*36);
        }
    }
    
    while ((c = getopt(argc, argv, "vi:")) != -1)
    {
        switch (c)
        {
            case 'v':
                video = 1;//atoi(optarg);
                cout << "Using video " << endl;
                break;

            case 'i':
                filename = optarg;
                cout << "Loading file " << filename << endl;
                break;

            case '?':
                cerr << "Could not parse option " << optopt << endl;
                break;
                
            default:
                cerr << "Failed to process cmdline args. Aborted." << endl;
                return 1;
        }
    }

    if ((video > 1) && !filename.empty())
    {
        cerr << "Choose video input OR filename" << endl;
    }
    if ((video < 0) && filename.empty())
    {
        cout << "Select video input with -v (0-n) or filename -i <name>" << endl;
        return 0;
    }
    
    Mat img, gray;
    int delay;
    try
    {
        VideoCapture viddevice(filename);
        //CvCapture *viddevice;
        if (video > -1)
        {
            //viddevice = cvCreateFileCapture(filename.c_str());
            //delay = (int) 1000/ cvGetCaptureProperty(viddevice,CV_CAP_PROP_FPS);
            delay = (int) 1000/ viddevice.get(CV_CAP_PROP_FPS);
            
            cout << "Loaded video, framerate " << delay << endl;
        }
        namedWindow( "circles", CV_WINDOW_AUTOSIZE );

        bool again = false;
        
        do
        {
            if (video < 0)
            {
                img = imread(filename.c_str(), 1);
                cout << "Loaded image" << endl;
            }
            else
            {
                viddevice >> img;
                //img = cvQueryFrame(viddevice);
            }
            cvtColor(img, gray, CV_BGR2GRAY);
            //cout << "Greyscaled" << endl;
            // smooth it, otherwise a lot of false circles may be detected
            GaussianBlur( gray, gray, Size(9, 9), 2, 2 );
            //cout << "Blurred" << endl;
            vector<Vec3f> circles;
            HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 2, 40, 200, 35, 20, 28);
            //cout << "Found " << circles.size() << " circles" << endl;
            IplImage ipimg = img;
            for( size_t i = 0; i < circles.size(); i++ )
            {
                Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
                int radius = cvRound(circles[i][2]);
                //cout << "Centre (" << circles[i][0] << ", " << circles[i][1] << ") radius " << circles[i][2];
                //cout << ", colour (";
                CvScalar s;
                s = cvGet2D(&ipimg, circles[i][1], circles[i][0]);
                //cout << s.val[0] <<  ", " << s.val[1] << ", " << s.val[2] << ")" <<endl;
                // draw the circle center
                circle( img, center, 3, Scalar(0,255,0), -1, 8, 0 );
                // draw the circle outline
                circle( img, center, radius, Scalar(0,0,255), 3, 8, 0 );
            }

            Scalar off(255,0,0);
            Scalar on(0,0,255);
            const unsigned int MIN_DIST = 15;
            
            for (unsigned int i = 0; i < 16; ++i)
            {
                for (unsigned int j = 0; j < 4; ++j)
                {
                    bool match = false;
                    for (vector<Vec3f>::const_iterator iter = circles.begin();
                         (iter != circles.end()) && !match;
                         ++iter)
                    {
                        float dist = poi[i][j].x - (*iter)[0];
                        dist *= dist;
                        dist += pow(poi[i][j].y - (*iter)[1], 2);
                        dist = sqrt(dist);

                        if (dist < MIN_DIST)
                        {
                            match = true;
                        }
                    }

                    if (match)
                        circle( img, poi[i][j], 3, on, -1, 8, 0 );
                    else
                        circle( img, poi[i][j], 3, off, -1, 8, 0 );
                }
            }
            //moveWindow("circles", 100, 100);
            //resizeWindow("circles", 640, 480);
            imshow( "circles", img );
            if (video < 0)
            {
                // Wait once
                waitKey(0);
            }
            else
            {
                again = (waitKey(delay) == -1);
            }
        } while (again);
    }
    catch (const cv::Exception &e)
    {
        cerr << "Caught exception: " << e.what() << endl;
    }
    return 0;
}
