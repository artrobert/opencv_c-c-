#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{

    String imageName("../header.jpg");

//    if(argc != 2)
//    {
//        std::cout << "Usage: display_image ImageToLoadAndDisplay" << std::endl;
//        return -1;
//    }

    if(argc >1){
        imageName=argv[1];
    }

    Mat frame;
    frame = imread(imageName, CV_LOAD_IMAGE_COLOR); // Read the file

    if(!frame.data) // Check for invalid input
    {
        std::cout << "Could not open or find the frame" << std::endl;
        return -1;
    }

    namedWindow("Window", WINDOW_AUTOSIZE); // Create a window for display.
    imshow("Window", frame); // Show our image inside it.

    cv::waitKey(0); // Wait for a keystroke in the window
    return 0;
}