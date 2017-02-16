#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

//Global variables

Mat src , src_gray;
Mat dst,detected_edges;

int edgeThresh=1;
int lowThreshold;
int const max_lowThreshold=100;
int ratio=3;
int kernel_size=3;
char* window_name="Edge Map";


/**
 * @function CannyThreshold
 * @brief Trackbar callback - Canny thresholds input with a ratio 1:3
 */
void CannyThreshold(int, void*)
{
    /// Reduce noise with a kernel 3x3
    blur( src_gray, detected_edges, Size(3,3) );

    /// Canny detector
    Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );

    /// Using Canny's output as a mask, we display our result
    dst = Scalar::all(0);

    src.copyTo( dst, detected_edges);
    imshow( window_name, dst );
}


int main(int argc, char** argv)
{

    String imageName("../chess.jpg");

    if(argc >1){
        imageName=argv[1];
    }

    src = imread(imageName); // Read the file

    if(!src.data) // Check for invalid input
    {
        std::cout << "Could not open or find the frame" << std::endl;
        return -1;
    }

    //Create a matrix of the same type and size as src (for dst)
    dst.create(src.size(),src.type());

    //Comvert the image to grayscale
    cv::cvtColor(src,src_gray,CV_BGR2GRAY);

    //Create a window
    namedWindow(window_name,CV_WINDOW_AUTOSIZE);

    //Create a Trackbar for user to enter threshold
    createTrackbar("Min Threshold:",window_name,&lowThreshold,max_lowThreshold,CannyThreshold);

    //Show the image
    CannyThreshold(0,0);

    //Wait until user exit program by pressing a key
    cv::waitKey(0); // Wait for a keystroke in the window
    return 0;
}