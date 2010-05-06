
#include <unistd.h>
#include <stdint.h>

#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>

#include <cv.h>
#include <highgui.h>
#include <math.h>


using namespace cv;
using namespace std;

/** Program entry point.
 *  This is the only function in the programme at this time,
 *  and takes options -i \<filename\> for the filename to be loaded
 *  and -v to indicate the file is video.
 */
int main(int argc, char** argv)
{
    int video = -1;
    int c;
    string filename;
    string serdevice;

    Point poi[16][4];
    uint8_t outmsg[16];

    // Create an array of points that are hardcoded, to be checked for
    // circles indicating a token at that point
    for (unsigned int i = 0; i < 16; ++i)
    {
        for (unsigned int j = 0; j < 4; ++j)
        {
            poi[i][j].x = 50 + (i*36);
            poi[i][j].y = 200 + (j*36);
        }
    }

    // Use getopt to sort out options
    while ((c = getopt(argc, argv, "d:vi:")) != -1)
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

            case 'd':
                serdevice = optarg;
                cout << "Sending serial strings to device " << serdevice << endl;
                cout << "Ensure it is set to the correct baudrate with stty or similar" << endl;
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

    ofstream serport(serdevice.c_str());
    Mat img, gray;
    int delay;
    try
    {
        VideoCapture viddevice(0);
        if(!viddevice.isOpened())  // check if we succeeded
        {
          cout << "Couldn't open device" << endl;
          return 1;
        }

        if (video > -1)
        {
            // Couldn't read the framerate
            delay = 100;

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
                //img = cvQueryFrame(viddevice);
                viddevice >> img;
            }
            cvtColor(img, gray, CV_BGR2GRAY);
            //cout << "Greyscaled" << endl;
            // smooth it, otherwise a lot of false circles may be detected,
            // but it affects the circle boundaries being detected
            //GaussianBlur( gray, gray, Size(5, 5), 2, 2 );
            //cout << "Blurred" << endl;
            vector<Vec3f> circles;
            HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 2, 40, 200, 35, 5, 28);
            //cout << "Found " << circles.size() << " circles" << endl;
            IplImage ipimg = img;

            // Draw the circles
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
            //const unsigned int MIN_DIST = 20;

            memset(outmsg, 0, 16);

            for (unsigned int i = 0; i < 16; ++i)
            {
                for (unsigned int j = 0; j < 4; ++j)
                {
                    bool match = false;
                    for (vector<Vec3f>::const_iterator iter = circles.begin();
                         (iter != circles.end()) && !match;
                         ++iter)
                    {
                        // Calculate distance to each point of interest
                        float dist = poi[i][j].x - (*iter)[0];
                        dist *= dist;
                        dist += pow(poi[i][j].y - (*iter)[1], 2);
                        dist = sqrt(dist);

                        // Halt if the distance is less than the circle radius
                        if (dist <= (*iter)[2])
                        {
                            match = true;
                        }
                    }

                    unsigned int idx = (j*4)+i;
                    unsigned int charidx = idx % 4;
                    unsigned int stridx = (idx - charidx) /4;

                    // Draw all the points, changing the colour if it's a match
                    if (match)
                    {
                        circle( img, poi[i][j], 3, on, -1, 8, 0 );
                        outmsg[i] += (1 << j);
                        //cout << dec << "Dot " << i << ", " << j << " set at " << stridx << ", " << charidx << endl;
                    }
                    else
                    {
                        circle( img, poi[i][j], 3, off, -1, 8, 0 );
                    }
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

#ifdef DEBUG
            // Display the command string that's due to be sent
            cout << "$" << hex;
            for (unsigned int n = 0; n < 16; ++n)
            {
                cout << int(outmsg[n]);
            }
            cout << endl;
#endif

            if (serport)
            {
                // Send data to the serial port
                serport<< "$" << hex; // Packet start char
                for (unsigned int n = 0; n < 16; ++n)
                {
                    // payload
                    serport << int(outmsg[n]);
                }
                serport << endl;
            }
        } while (again);
    }
    catch (const cv::Exception &e)
    {
        cerr << "Caught exception: " << endl;
    }
    return 0;
}
