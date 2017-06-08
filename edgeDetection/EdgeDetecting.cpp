//
// Created by rartin on 01/3/2017.
//

#include "EdgeDetecting.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <io.h>
#include "ImageBasicOperations.h"


using namespace cv;
using namespace std;

const char *window_name = "Edge Map";
Mat detected_edges, src_gray;

/**
 * This function is used in the sort() call so we can sort ASCENDING the vector containing the lines after the X of the first point
 * @param v1 The first line
 * @param v2 The second line
 * @return returns if the X of the first point is smaller than the second lines first point X
 */
bool comparisonFirstPointXAsc(Vec4i v1, Vec4i v2) {
    return (v1.val[0] <= v2.val[0]);
}

/**
 * This function is used in the sort() call so we can sort DESCENDING the vector containing the lines after the X of the first point
 * @param v1 The first line
 * @param v2 The second line
 * @return returns if the X of the first point is smaller than the second lines first point X
 */
bool comparisonFirstPointXDesc(Vec4i v1, Vec4i v2) {
    return (v1.val[0] >= v2.val[0]);
}

/**
 * This function is used in the sort() call so we can sort ASCENDING the vector containing the lines after the Y of the first point
 * @param v1 The first line
 * @param v2 The second line
 * @return returns if the X of the first point is smaller than the second lines first point X
 */
bool comparisonFirstPointYAsc(Vec4i v1, Vec4i v2) {
    return (v1.val[1] <= v2.val[1]);
}

/**
 * This function is used to sort the elements in ASCENDING order after Y and length (the line with the highest Y and the biggest length
 * is the last in the vector )
 * @param v1
 * @param v2
 * @return
 */
bool comparisonYmaxDMAx(Vec4i v1, Vec4i v2) {
    double d1 = norm(Point(v1[2], v1[3]) - Point(v1[0], v1[1]));
    double d2 = norm(Point(v2[2], v2[3]) - Point(v2[0], v2[1]));
    return (v1.val[1] <= v2.val[1] && d1 < d2);
}

/**
 * This function is used to sort the elements in DESCENDING order after X and length (the line with the lowest X and the biggest length
 * is the last in the vector )
 * @param v1
 * @param v2
 * @return
 */
bool comparisonXminDMAx(Vec4i v1, Vec4i v2) {
    double d1 = norm(Point(v1[2], v1[3]) - Point(v1[0], v1[1]));
    double d2 = norm(Point(v2[2], v2[3]) - Point(v2[0], v2[1]));
    return (v1.val[0] >= v2.val[0] && d1 < d2);
}

/**
 * This function is used in the sort() call so we can sort DESCENDING the vector containing the lines after the Y of the first point
 * @param v1 The first line
 * @param v2 The second line
 * @return returns if the X of the first point is smaller than the second lines first point X
 */
bool comparisonFirstPointYDesc(Vec4i v1, Vec4i v2) {
    return (v1.val[1] >= v2.val[1]);
}

/**
 * This function returns true if P1's X is smaller & Y is greater than P2's X and Y
 * @param v1 Point line 1
 * @param v2 Point line 2
 * @return true if P1's X is smaller & Y is greater than P2's X and Y false otherwise
 */
bool comparisonVectorsXLowYHigh(Vec4i v1, Vec4i v2) {
    return (v1.val[0] <= v2.val[0] && v1.val[1] >= v2.val[1]);
}

bool comparisonPointsYHigh(Point v1, Point v2) {
    return (v1.y > v2.y);
}

bool comparisonPointsYLow(Point v1, Point v2) {
    return (v1.y < v2.y);
}

bool comparisonPointsXLow(Point v1, Point v2) {
    return (v1.x > v2.x);
}

bool comparisonPointsXHigh(Point v1, Point v2) {
    return (v1.x < v2.x);
}

bool comparisonPointsYHighXLow(Point v1, Point v2) {
    return (v1.y > v2.y && v1.x < v2.x);
}

/**
 * This function returns true if P1's X & Y is greater than P2's X and Y
 * @param v1 Point line 1
 * @param v2 Point line 2
 * @return true if P1's X is smaller & Y is greater than P2's X and Y false otherwise
 */
bool comparisonFirstPointXHighYHigh(Vec4i v1, Vec4i v2) {
    return (v1.val[0] <= v2.val[0] && v1.val[1] <= v2.val[1]);
}

/**
 * Function used to compare the X and Y coordinates of 2 points to see if they are equal
 * @param v1 A vector containing 4 points of a line, we take only the first point of the line
 * @param v2 A vector containing 4 points of a line, we take only the first point of the line
 * @return true if the X and Y of the points are equal
 */
bool uniquePointComparison(Vec4i v1, Vec4i v2) {
    return v1.val[0] == v2.val[0] && v1.val[1] == v2.val[1];
}

/**
 * This function will be used to write the input mat along with one of its line and the point coordinates into a jpg file so it will be easier to view
 * @param src
 * @param pointNr
 * @param pointX
 * @param pointY
 * @param iteration
 */
void putTextIntoImage(const char *pathdir, cv::Mat &src, int pointNr, int pointX, int pointY, int iteration) {
    mkdir(pathdir);
    int fontFace = FONT_HERSHEY_SIMPLEX;
    double fontScale = 2;
    int thickness = 3;
    char s[30];
    sprintf(s, "(%d) %d %d", pointNr, pointX, pointY);
    putText(src, s, Point(pointX, pointY), fontFace, fontScale, Scalar(0, 0, 255), thickness, 8, false);
    char path[50];
    sprintf(path, "%s/grey_iteration_%d.jpg", pathdir, iteration);
    imwrite(path, src);
}

void searchCommonSlopeAndRemove(vector<Vec4i> &lines) {
    for (size_t i = 0; i < lines.size() - 1; i++) {
        Vec4i l = lines[i];
        double line1Size = sqrt((l[3] - l[1]) * (l[3] - l[1]) + (l[2] - l[1]) * (l[2] - l[1]));
        double lYDifference = l[3] - l[1];
        double lXDifference = l[2] - l[0];
        double lineOriginalSlope = lYDifference / lXDifference; //slope of the first line (the reference one)
        for (size_t j = 1; j < lines.size(); j++) {
            Vec4i l2 = lines[j];
            if (i != j) { //to be sure we don't compare the same line
                // calculate the slope that the first point of the i line make with the second point of the line j
                double l2l1YDiff = l2[3] - l[1];
                double l2l1XDiff = l2[2] - l[0];
                double distinctSlope = l2l1YDiff / l2l1XDiff;
                double l1l2YDiff = l[3] - l2[1];
                double l1l2XDiff = (l[2] - l2[0]);
                double distinctSlope2 = l1l2YDiff / l1l2XDiff;
                // ~error between the slopes must be around +-.30
                bool closeSlopes = (abs(lineOriginalSlope - distinctSlope) <= 0.20) ||
                                   (abs(lineOriginalSlope - distinctSlope2) <= 0.20);
                if (closeSlopes) {
                    double line2Size = sqrt((l2[3] - l2[1]) * (l2[3] - l2[1]) + (l2[2] - l2[1]) * (l2[2] -
                                                                                                   l2[1])); //calculate the length of the second line
                    // if the lines have almost the same coordinates we keep the one with the biggest length
                    if (line1Size < line2Size) {
                        lines[i] = lines[j];
                    }
                    lines.erase(lines.begin() +
                                j); //erase the J line that was already copied over the one with the smaller length
                    if (j < i) {
                        i--;
                        l = lines[i];
                    }
                    j--;
                }
            }
        }
    }
}

/**
 * This will take each line and compare it with the others and if the coordinates of its P1.X/P1.Y and P2.X/P2.Y are like +/- 15&10 pixels around, they will
 * select the line with the biggest length and override the smaller one so we can easily remove after
 *
 * Bug , some of the values are not erased !!!Strange!!
 *
 * @param src The vectorContaining the lines
 * @param comparisonType Type of comparison TODO:
 */
void searchCommonPointsAndRemove(vector<Vec4i> &lines, int errorMarginX, int errorMarginY) {
    for (size_t i = 0; i < lines.size() - 1; i++) {
        Vec4i l = lines[i];
        double line1Size = sqrt(
                (l[3] - l[1]) * (l[3] - l[1]) + (l[2] - l[1]) * (l[2] - l[1])); //length of the first line
        for (size_t j = 1; j < lines.size(); j++) {
            Vec4i l2 = lines[j];
            //if the point is around the other point
            if (i != j) { //to be sure we don't compare the same line
                bool closeLinePoints = (abs(l[0] - l2[0]) <= errorMarginX &&
                                        abs(l[1] - l2[1]) <= errorMarginY)//comparison between first point's coordinates
                                       || (abs(l[2] - l2[2]) <= errorMarginX &&
                                           abs(l[3] - l2[3]) <=
                                           errorMarginY);//comparison between second point's coordinates
                if (closeLinePoints) { // if the points are close enough (e.g +-10px for coordinates) or then is the same line
                    double line2Size = sqrt((l2[3] - l2[1]) * (l2[3] - l2[1]) + (l2[2] - l2[1]) * (l2[2] -
                                                                                                   l2[1])); //calculate the length of the second line
                    // if the lines have almost the same coordinates we keep the one with the biggest length
                    if (line1Size < line2Size) {
                        lines[i] = lines[j];
                    }
                    lines.erase(lines.begin() +
                                j); //erase the J line that was already copied over the one with the smaller length
                    if (j < i) {
                        i--;
                        l = lines[i];
                    }
                    j--;
                }
            }
        }
    }
}

/**
 * This will receive a vector of lines(represented as a vector of 4 points) and will calculate the distance between the start points
 * If they are closer than the distance limit, the second line it is compared to is erased
 *
 * @param $lines The vector containing the lines
 * @param distanceLimit The max distance limit
 */
void searchCloseDistanceAndRemove(vector<Vec4i> &lines, int distanceLimit) {
    for (int i = 0; i < lines.size() - 1; i++) {
        Vec4i l = lines[i];
        double lastDistance = 0;
        for (int j = i + 1; j < lines.size(); j++) {
            Vec4i l2 = lines[j];
            double distance = sqrt(pow(double(l[3] - l2[3]), 2.0) + pow(double(l[2] - l2[2]), 2.0));
//            printf("\n distance (%d)->(%d) from (%d,%d) to (%d,%d) is : %lf", i + 1, j + 1, l[2], l[3], l2[2], l2[3], distance);
            if (abs(distance - lastDistance < distanceLimit)) {
                lines.erase(lines.begin() + j);
                j--;
            } else {
                lastDistance = distance;
            }
        }
    }
}

void findSquareCoordinates(vector<Vec4i> &v1, vector<Vec4i> &v2) {
    vector<Vec4i> positiveAngleLines(v1);
    vector<Vec4i> negativeAngleLines(v2);

    //We sorted after the Y value ASCENDING for the / lines
    sort(positiveAngleLines.begin(), positiveAngleLines.end(), comparisonFirstPointYAsc);

    //We sorted after the X value DESCENDING for the \ lines in order to take the first value out (There are others ways but lets stick to this one)
    sort(negativeAngleLines.begin(), negativeAngleLines.end(), comparisonFirstPointXDesc);

    Vec4i fistLinePositive, secondLinePositive, firstLineNegative, secondLineNegative;

    //Get the bottom margin of the table and remove it from the vector
    fistLinePositive = positiveAngleLines[positiveAngleLines.size() - 1];
    positiveAngleLines.pop_back();

    //Get the left margin of the table and remove it from the vector
    firstLineNegative = negativeAngleLines[negativeAngleLines.size() - 1];
    negativeAngleLines.pop_back();

    //We sorted after the Y and X with the highest value ASCENDING for the / lines to get the one before the last line
    sort(positiveAngleLines.begin(), positiveAngleLines.end(), comparisonFirstPointXHighYHigh);

    //We sorted after the Y highest and X lowest value ASCENDING for the \ lines to get the one before the last line
    sort(negativeAngleLines.begin(), negativeAngleLines.end(), comparisonVectorsXLowYHigh);

    secondLinePositive = positiveAngleLines[positiveAngleLines.size() - 1];
    positiveAngleLines.pop_back();

    secondLineNegative = negativeAngleLines[negativeAngleLines.size() - 1];
    negativeAngleLines.pop_back();

}

/**
 * This function will separate the angle into 2 groups using an angle to differentiate between them.
 *
 * @param lines The vector of lines, found by Hough function, that will be separated into 2 vectors based on an angles positive and negative value.
 * @param angle The angle used to separate the lines.
 * @param angleGroup1 The list containing the lines that have a positive positive value.
 * @param angleGroup2 The list containing the lines that have a negative angle value.
 */
void separateLinesByAngle(vector<Vec4i> &lines, int angle, vector<Vec4i> &angleGroup1, vector<Vec4i> &angleGroup2) {
    int lineSize = lines.size();
    for (size_t i = 0; i < lineSize; i++) {
        Vec4i l = lines[i];

        //This will get the angle that the line is making with the X axis
        // Using this angle we can separate the lines into + and -
        Point p1, p2;
        p1 = Point(l[0], l[1]);
        p2 = Point(l[2], l[3]);

        double angleMade = atan2((double) (p1.y - p2.y), (double) (p1.x - p2.x));

        if (angle == (int) angleMade) {
            angleGroup1.push_back(l);
        } else if (-angle == (int) angleMade) {
            angleGroup2.push_back(l);
        }
    }
}

void
printToSeparateFiles(vector<Vec4i> linesAngle, const char *pathdir, const char *windowName, cv::Mat &cloneForOutput) {
    Mat matForOutput = cloneForOutput.clone();
    for (size_t i = 0; i < linesAngle.size(); i++) {
        Vec4i l = linesAngle[i];
        line(matForOutput, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
        Mat externalMat;
        line(externalMat, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
        //Create jpg files with the current line and the coordinates of its extremities
        putTextIntoImage(pathdir, externalMat, 1, l[0], l[1], i);
        putTextIntoImage(pathdir, externalMat, 2, l[2], l[3], i);
        printf("%d) Line P(%d,%d) P(%d,%d) slope %lf length %lf \n", i, l[0], l[1], l[2], l[3],
               (l[3] - l[1] / (double) (l[2] - l[0])), norm(Point(l[2], l[3]) - Point(l[0], l[1])));
    }
    namedWindow(windowName, CV_WINDOW_AUTOSIZE);
    imshow(windowName, matForOutput);
}

void getPointNewLocation(Vec4i &edge, int sqrtNr) {
    Point starting = Point(edge[0], edge[1]);
    Point ending = Point(edge[2], edge[3]);
    double d = sqrt(
            (ending.x - starting.x) * (ending.x - starting.x) + (ending.y - starting.y) * (ending.y - starting.y));
    double density = d / 8;
    double r = density / d * sqrtNr;

    double x3 = r * ending.x + (1 - r) * starting.x;
    double y3 = r * ending.y + (1 - r) * starting.y;

    edge[2] = (int) x3;
    edge[3] = (int) y3;
}

Point getPointNewLocation(Point starting, Point ending, int sqrtNr) {
    double d = sqrt(
            (ending.x - starting.x) * (ending.x - starting.x) + (ending.y - starting.y) * (ending.y - starting.y));
    double density = d / 8;
    double r = density / d * sqrtNr;

    double x3 = r * ending.x + (1 - r) * starting.x;
    double y3 = r * ending.y + (1 - r) * starting.y;

    return Point(x3, y3);

}

/**
 *
 * This function uses probabilistic Hough lines to identify all the lines in the image
 *
 * @param src The mat from where we will get the lines
 * @param gr The mat where we will write the lines for viewing
 */

void houghLines(cv::Mat &src, cv::Mat &gr) {

    // Apply a dilation to identify more lines
    src = imagePreparation::dilationImage(src, 2, 3);
    imshow(window_name, src);

    // The vector where we will keep the lines , they come in a pair of 4 coordinates 0-1 for the first point and 2-3 for the other , together they make the line
    vector<Vec4i> lines;

    // This will detect the lines: rho : The resolution of the parameter r in pixels. We use 1 pixel.
    //                             theta :The resolution of the parameter \theta in radians. We use 1 degree (CV_PI/180) , formule: 1) radiani=grade*pi/180  2) grade=radiani*180/pi
    //                             threshold : Only those liens are returned that get enough votes , current is 30
    //                             minLineLength : Minimum line length. Line segments shorter than that are rejected , current is 60
    //                             maxLineGap : Maximum allowed gap between points on the same line to link them , current is 3
    HoughLinesP(src, lines, 1, CV_PI / 180, 30, 120, 3);

    // These vectors will be used to separate the lines into 2 groups depending on their line angle using the ATAN2 function
    // The results should be a VALUE1 and -VALUE1 and maybe some VALUE2,3,4 that are not important

    // From test we saw that the prominent values are  2.XX and -2.XX and some 1.XX , -1.XX
    // TODO we could hard code this or make an algorithm to search for the most prominent V/-V value
    vector<Vec4i> linesAngle1, linesAngle2;

    // Separate the lines into 2 groups after an angle (the positive and negative value)
    // Angle used is 2
    separateLinesByAngle(lines, 2, linesAngle1, linesAngle2);

    printf("Lines found with angle 2:%d and with -2 : %d \n", linesAngle1.size(), linesAngle2.size());

//        std::sort(linesAngle1.begin(), linesAngle1.end(), comparisonFirstPointXAsc);
//        std::sort(linesAngle2.begin(), linesAngle2.end(), comparisonFirstPointYAsc);

    double squareMulti = 2;

//    sort(linesAngle2.begin(), linesAngle2.end(), comparisonXminDMAx);

//    findSquareCoordinates(linesAngle1,linesAngle2);

    searchCommonPointsAndRemove(linesAngle1, 20, 20);
//    searchCommonSlopeAndRemove(linesAngle1);
//    sort(linesAngle1.begin(), linesAngle1.end(), comparisonYmaxDMAx);


//    searchCommonPointsAndRemove(linesAngle2);
//    searchCommonSlopeAndRemove(linesAngle2);


    printToSeparateFiles(linesAngle1, "../images/result/angle2", "linesAngle2", gr);
    printToSeparateFiles(linesAngle2, "../images/result/anglen2", "linesAngle2", gr);
//    printToSeparateFiles(lines,"../images/result/all","all",gr);

}

void tryPerspectiveCorrection(cv::Mat &src, Point &pXH, Point &pXL, Point &pYH, Point &pYL) {
    //Compute quad point for edge
    Point Q1 = pYL;
    Point Q2 = pXL;
    Point Q3 = pYH;
    Point Q4 = pXH;

    // compute the size of the card by keeping aspect ratio.
    double ratio = 1;
    double cardH = sqrt((Q3.x - Q2.x) * (Q3.x - Q2.x) + (Q3.y - Q2.y) * (Q3.y - Q2.y));//Or you can give your own height
    double cardW = ratio * cardH;
    Rect R(Q1.x, Q1.y, cardW, cardH);

    Point R1 = Point2f(R.x, R.y);
    Point R2 = Point2f(R.x + R.width, R.y);
    Point R3 = Point2f(Point2f(R.x + R.width, R.y + R.height));
    Point R4 = Point2f(Point2f(R.x, R.y + R.height));

    std::vector<Point2f> quad_pts;
    std::vector<Point2f> squre_pts;

    quad_pts.push_back(Q1);
    quad_pts.push_back(Q2);
    quad_pts.push_back(Q3);
    quad_pts.push_back(Q4);

    squre_pts.push_back(R1);
    squre_pts.push_back(R2);
    squre_pts.push_back(R3);
    squre_pts.push_back(R4);


    Mat transmtx = getPerspectiveTransform(quad_pts, squre_pts);
    int offsetSize = 500;
    Mat transformed = Mat::zeros(src.rows, src.cols, CV_8UC3);
    warpPerspective(src, transformed, transmtx, transformed.size());

    //rectangle(src, R, Scalar(0,255,0),1,8,0);

    line(src, Q1, Q2, Scalar(0, 0, 255), 1, CV_AA, 0);
    line(src, Q2, Q3, Scalar(0, 0, 255), 1, CV_AA, 0);
    line(src, Q3, Q4, Scalar(0, 0, 255), 1, CV_AA, 0);
    line(src, Q4, Q1, Scalar(0, 0, 255), 1, CV_AA, 0);

    Mat maskForCorners = Mat::zeros(src.size(), CV_8UC1);
    R1.x -= 20;
    R1.y -= 20;
    R3.x += 20;
    R3.y += 20;
    rectangle(maskForCorners, R1, R3, Scalar(255, 255, 255), -1, 8);

    transformed = imagePreparation::convertImageGreyscale(transformed);
//    transformed = imagePreparation::blurImage(transformed);
    vector<Point> corners;
    goodFeaturesToTrack(transformed, corners, 200, 0.05, 20, maskForCorners, 3, false, 0.04);
    for (int i = 0; i < corners.size(); i++) {
        circle(transformed, Point(corners[i].x, corners[i].y), 3, 255, 1, 8, 0);
        char s[30];
        sprintf(s, "(%d)", i);
        putText(transformed, s, Point(corners[i].x, corners[i].y), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255),
                1, 8, false);
    }
    imshow("quadrilateral", transformed);
    imshow("src", src);
    imshow("fdsfds", maskForCorners);
    waitKey();
}

void getCornerList(cv::Mat &originalImage, vector<Point> &searchBottom, vector<Point> &searchLeft,
                   vector<double> &bottomEdgeSquareDimensions, vector<double> &leftEdgeSquareDimensions) {
    Mat im, imGrey;
    imGrey = imagePreparation::convertImageGreyscale(originalImage);
    imGrey = imagePreparation::blurImage(imGrey);
    vector<Point> corners;
    goodFeaturesToTrack(imGrey, corners, 100, 0.01, 20, noArray(), 3, false, 0.04);
    Point lowX = corners[0];
    Point highX = corners[0];
    Point highY = corners[0];
    Point lowY = corners[0];
    for (Point p:corners) {
        if (p.x > highX.x) {
            highX = p;
        }
        if (p.x < lowX.x) {
            lowX = p;
        }
        if (p.y > highY.y) {
            highY = p;
        }
        if (p.y < lowY.y) {
            lowY = p;
        }
    }

//    tryPerspectiveCorrection(imGrey,highX,lowX,highY,lowY);

    Mat mask = Mat::zeros(imGrey.size(), imGrey.type());
    line(mask, highY, highX, Scalar(255, 255, 255), 10, CV_AA);
//    namedWindow("showMask1", CV_WINDOW_AUTOSIZE);
//    imshow("showMask1", mask);
    goodFeaturesToTrack(imGrey, corners, 100, 0.2, 20, mask, 3, false, 0.04);
    for (Point p:corners) {
        if (p.y >= highX.y && p.x >= highY.x) {
            searchBottom.push_back(p);
        }
    }


    mask = Mat::zeros(imGrey.size(), imGrey.type());
    line(mask, highY, lowX, Scalar(255, 255, 255), 10, CV_AA);
//    namedWindow("showMask2", CV_WINDOW_AUTOSIZE);
//    imshow("showMask2", mask);
    goodFeaturesToTrack(imGrey, corners, 100, 0.2, 20, mask, 3, false, 0.04);

    for (Point p:corners) {
        if (p.y >= lowX.y && p.x <= highY.x) {
            searchLeft.push_back(p);

        }
    }
    sort(searchBottom.begin(), searchBottom.end(), comparisonPointsXHigh);
    sort(searchLeft.begin(), searchLeft.end(), comparisonPointsXHigh);
    Mat imgrey2, imgrey3;
    imgrey2 = imGrey.clone();
    imgrey3 = imGrey.clone();


    int fontFace = FONT_HERSHEY_SIMPLEX;
    double fontScale = 1;
    int thickness = 1;

    double pastDimBottom = 0, pastDimLeft = 0;
    for (int i = 1; i < searchBottom.size(); i++) {
        double length = sqrt(pow((double) (searchBottom[0].y - searchBottom[i].y), 2.0) +
                             pow((double) (searchBottom[0].x - searchBottom[i].x), 2.0));
        bottomEdgeSquareDimensions.push_back(length);
        length = sqrt(pow((double) (searchLeft[i].y - searchLeft[i + 1].y), 2.0) +
                      pow((double) (searchLeft[i].x - searchLeft[i + 1].x), 2.0)) + pastDimLeft;
        pastDimLeft = length;
        leftEdgeSquareDimensions.push_back(length);
    }


    //this is for the bottom edge
    for (int i = 0; i < searchBottom.size(); i++) {
        double distance = abs((highX.y - highY.y) * searchBottom[i].x - (highX.x - highY.x) * searchBottom[i].y +
                              highX.x * highY.y - highX.y * highY.x) /
                          sqrt((highX.y - highY.y) * (highX.y - highY.y) + (highX.x - highY.x) * (highX.x - highY.x));
        printf("\n distance from %d) is : %lf", i, distance);
        if (i + 1 != searchBottom.size()) {
            printf("\n distance between point %d and %d is %lf)",
                   i, i + 1,
                   sqrt((searchBottom[i + 1].y - searchBottom[i].y) * (searchBottom[i + 1].y - searchBottom[i].y) +
                        (searchBottom[i + 1].x - searchBottom[i].x) * (searchBottom[i + 1].x - searchBottom[i].x)));
        }
        circle(imgrey3, Point(searchBottom[i].x, searchBottom[i].y), 3, 255, 1, 8, 0);
        char s[30];
        sprintf(s, "(%d)", i);
        putText(imgrey3, s, Point(searchBottom[i].x, searchBottom[i].y), fontFace, fontScale, Scalar(0, 0, 255),
                thickness, 8, false);
    }
    // SHOW HORIZONTAL LINES CORNER POINTS
    namedWindow("test2", CV_WINDOW_AUTOSIZE);
    imshow("test2", imgrey3);

    //this is for the left edge
    for (int i = 0; i < searchLeft.size(); i++) {
        if (i + 1 != searchBottom.size()) {
            printf("\n distance between point %d and %d is %lf)",
                   i, i + 1,
                   sqrt((searchLeft[i + 1].y - searchLeft[i].y) * (searchLeft[i + 1].y - searchLeft[i].y) +
                        (searchLeft[i + 1].x - searchLeft[i].x) * (searchLeft[i + 1].x - searchLeft[i].x)));
        }
        circle(imgrey2, Point(searchLeft[i].x, searchLeft[i].y), 3, 255, 1, 8, 0);
        char s[30];
        sprintf(s, "(%d)", i);
        putText(imgrey2, s, Point(searchLeft[i].x, searchLeft[i].y), fontFace, fontScale, Scalar(0, 0, 255), thickness,
                8, false);
    }
    // SHOW VERTICAL LINE CORNER POINTS
    namedWindow("test", CV_WINDOW_AUTOSIZE);
    imshow("test", imgrey2);

    printf("line length %lf", sqrt(
            (highX.x - highY.x) * (highX.x - highY.x) + (highX.y - highY.y) * (highX.y - highY.y)));

}

Point2f getIntersection(Vec4i &line1, Vec4i &line2) {
    Point2f o1 = Point(line1[0], line1[1]);
    Point2f o2 = Point(line1[2], line1[3]);
    Point2f p1 = Point(line2[0], line2[1]);
    Point2f p2 = Point(line2[2], line2[3]);

    Point2f x = p1 - o1;
    Point2f d1 = o2 - o1;
    Point2f d2 = p2 - p1;

    float cross = d1.x * d2.y - d1.y * d2.x;
    if (abs(cross) < /*EPS*/1e-8) {
        return Point2f(0, 0);
    }
    double t1 = (x.x * d2.y - x.y * d2.x) / cross;
    return (o1 + d1 * t1);
}

void houghSimpleLines(cv::Mat &cannyMat, vector<Vec4i> &resultLines) {

    vector<Vec2f> houghResultLines;

    HoughLines(cannyMat, houghResultLines, 1, CV_PI / 180, 90, 0, 0);

    for (size_t i = 0; i < houghResultLines.size(); i++) {
        float rho = houghResultLines[i][0], theta = houghResultLines[i][1];
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a * rho, y0 = b * rho;
        double size = 1000;
        pt1.x = cvRound(x0 + size * (-b));
        pt1.y = cvRound(y0 + size * (a));
        pt2.x = cvRound(x0 - size * (-b));
        pt2.y = cvRound(y0 - size * (a));
        Vec4i v = Vec4i(pt1.x, pt1.y, pt2.x, pt2.y);
        resultLines.push_back(v);
//        line( cdst, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
    }
}

void filterAndRemoveLines(vector<Vec4i> houghLines, vector<Vec4i> &pozAngleLines, vector<Vec4i> &negAngleLines,
                          vector<Point> &tableIntersectionPoints) {

    separateLinesByAngle(houghLines, 2, pozAngleLines, negAngleLines);

    Vec4i pozHighLine = pozAngleLines[0], pozLowLine = pozAngleLines[0], negLowLine = negAngleLines[0], negHighLine = negAngleLines[0];
    //Search for the low line points
    for (Vec4i l:pozAngleLines) {
        if (l[1] < pozLowLine[1]) {
            pozLowLine = l;
        }
        if (l[1] > pozHighLine[1]) {
            pozHighLine = l;
        }
    }
    //Search for the left line points
    for (Vec4i l:negAngleLines) {
        if (l[0] < negLowLine[0]) {
            negLowLine = l;
        }
        if (l[0] > negHighLine[0]) {
            negHighLine = l;
        }
    }

    tableIntersectionPoints.push_back(getIntersection(pozLowLine, negLowLine)); //left point
    tableIntersectionPoints.push_back(getIntersection(pozLowLine, negHighLine)); //top top
    tableIntersectionPoints.push_back(getIntersection(pozHighLine, negLowLine)); //bottom
    tableIntersectionPoints.push_back(getIntersection(pozHighLine, negHighLine)); //right point
//    line(cdst, Point(pozLowLine[0], pozLowLine[1]), Point(pozLowLine[2], pozLowLine[3]), Scalar(0, 0, 255), 3, CV_AA); //bottom line
//    line( cdst, Point(pozHighLine[0],pozHighLine[1]), Point(pozHighLine[2],pozHighLine[3]), Scalar(0,0,255), 3, CV_AA); //top line
//    line(cdst, Point(negLowLine[0], negLowLine[1]), Point(negLowLine[2], negLowLine[3]), Scalar(0, 0, 255), 3, CV_AA); //left line
//    line( cdst, Point(negHighLine[0],negHighLine[1]), Point(negHighLine[2],negHighLine[3]), Scalar(0,0,255), 3, CV_AA); //right line
}

/**
 * We extract a square from the image and determine its color by binarize it, counting the pixels
 * and determine who are predominant
 *
 * @param src The Mat containing the chessboard
 * @param squareMatrix A matrix of {@link ChessSquare} objects representing the chessboard table
 * @param pointMatrix A matrix of points representing all the chessboard square corners
 */
void determineSquareColors(cv::Mat &src, ChessSquareMatrix &squareMatrix, int squareRow, int squareCol) {

    cv::Mat maskRoi = cv::Mat::zeros(src.size(), src.type());
    // Mask used to extract a single square, make it red so we can later binarize the image
    cv::Mat maskRoiBlue = cv::Mat(src.size(), src.type(), Scalar(255, 0, 0)); //mask used to extract a single square

    cv::Mat binaryExtractedChessSquare;
    ChessSquare *currentSquare = addressof(squareMatrix.getSquare(squareRow, squareCol));

    // TODO, HERE WE CHOOSE THE TOP LEFT SQUARE, NEED TO SEE WHAT IS THE BEST ONE TO CHOOSE
    Point rook_points[4];
    rook_points[0] = currentSquare->topLeft;
    rook_points[1] = currentSquare->bottomLeft;
    rook_points[2] = currentSquare->bottomRight;
    rook_points[3] = currentSquare->topRight;

//    rook_points[0] = Point((int) pointMatrix[0][1].x, (int) pointMatrix[0][1].y);
//    rook_points[1] = Point((int) pointMatrix[1][1].x, (int) pointMatrix[1][1].y);
//    rook_points[2] = Point((int) pointMatrix[2][2].x, (int) pointMatrix[2][2].y);
//    rook_points[3] = Point((int) pointMatrix[0][2].x, (int) pointMatrix[0][2].y);
    //
    // http://study.marearts.com/2016/07/opencv-drawing-example-line-circle.html
    // Fill the mask Mat with white pixels in order to extract the chess board square
    cv::fillConvexPoly(maskRoi, rook_points, 4, cv::Scalar(255, 255, 255));

    src.copyTo(binaryExtractedChessSquare, maskRoi);
    binaryExtractedChessSquare.copyTo(binaryExtractedChessSquare, maskRoiBlue);

//    imshow("before binarization", binaryExtractedChessSquare);

    // Binarize the extracted image so we can count the white/black pixels
    cvtColor(binaryExtractedChessSquare, binaryExtractedChessSquare, CV_BGR2GRAY);
    threshold(binaryExtractedChessSquare, binaryExtractedChessSquare, 100, 255, 0);

    int black_pixels = 0, white_pixels = 0;
    int new_black_pixels = 0, new_white_pixels = 0;

    int im_width = maskRoi.cols, im_height = maskRoi.rows;
    for (int i = 0; i < im_height; i++) {
        for (int j = 0; j < im_width; j++) {
            Vec3b bgrPixel = maskRoi.at<Vec3b>(i, j);
            int val = bgrPixel[0] + bgrPixel[1] + bgrPixel[2];
            if (val > 10) {
                white_pixels++;
            } else {
                black_pixels++;
            }

            Vec3b bgrPixelBinaryImage = binaryExtractedChessSquare.at<Vec3b>(i, j);
            int valBinary = bgrPixelBinaryImage[0] + bgrPixelBinaryImage[1] + bgrPixelBinaryImage[2];
            if (valBinary > 10) {
                new_white_pixels++;
            } else {
                new_black_pixels++;
            }
        }
    }

    int squareColor = 0;
    // See the precentage of the newly added pixels (what is more predominant whites/blacks)
    double perc_of_white_stay_white = (new_white_pixels * 100) / white_pixels;
    if (perc_of_white_stay_white < 50) {
        squareColor = 0; // the color is black
    } else {
        squareColor = 1; // the color is white
    }

    // !!!NOTE!! This is very dependent of the square we extracted (above code)
    // With the color we founded previously, we iterate top - bottom (2 by 2) and assign that color
    // While iterating, we also iterate left - right and assign the opposite color to the right and bottom squares
    for (int i = 7; i >= 1; i -= 2) {
        ChessSquare *sq = addressof(squareMatrix.getSquare(i, 0));
        sq->color = squareColor;
        while (sq->rightSquare != NULL) {
            sq->rightSquare->color = abs(sq->color - 1);
            sq->bottomSquare->color = abs(sq->color - 1);
            sq = sq->rightSquare;
        }
    }
}

/**
 * Function will receive 2 vectors of lines (horizontally and vertically), will get the intersections
 * and will form a matrix of 9x9 represeting the chess table square's corners
 *
 * @param pozAngleLines The vertical lines
 * @param negativeAngleLines The horizontal lines
 * @param pointMatrix The output matrix
 */
void createSquarePointMatrix(vector<Vec4i> pozAngleLines, vector<Vec4i> negativeAngleLines, Point2f pointMatrix[9][9]) {
    // This is commented because it set the top left corner as (0,0)
//    for (int i = 0; i < 9; i++) {
//        for (int j = 0; j < 9; j++) {
//            Point2f point = getIntersection(pozAngleLines[i], negativeAngleLines[j]);
//            pointMatrix[i][j] = point;
//        }
//    }

    // To keep consistency of the chess table (1-8 rows / a-h cols) and because the points start from the right corner
    // We will change their position in the matrix, this way (0,0) will start at the bottom left
    int maxLimit = 8;
    for (int i = 0; i <= maxLimit; i++) {
        for (int j = 0; j <= maxLimit; j++) {
            Point2f point = getIntersection(pozAngleLines[i], negativeAngleLines[j]);
            pointMatrix[maxLimit - i][j] = point;
        }
    }
}

/**
 * Creates the logic connections between the chess table squares and indexis them
 *
 * @param squareMatrix Object containing the {@link ChessSquare}s
 * @param pointMatrix Matrix containing the square corner points that will be assigned to them
 */
void createSquareMatrix(ChessSquareMatrix &squareMatrix, Point2f pointMatrix[9][9]) {

    int k = 1;

    char colChar = 'a';
    char rowChar = '1';

    for (size_t i = 0; i < 8; i++) {
        for (size_t j = 0; j < 8; j++) {
            ChessSquare *currentSquare = addressof(squareMatrix.getSquare(i, j));

            //Assign the index
            currentSquare->index = k;
            currentSquare->indexColRow[0] = rowChar;
            currentSquare->indexColRow[1] = colChar;
            currentSquare->row = i;
            currentSquare->col = j;

            // Set the corner points
            currentSquare->topLeft = pointMatrix[i + 1][j];
            currentSquare->bottomLeft = pointMatrix[i][j];
            currentSquare->bottomRight = pointMatrix[i][j + 1];
            currentSquare->topRight = pointMatrix[i + 1][j + 1];

            // If not in the bottom row set the bottom squares (left , middle or right)
            if (i != 0) {
                currentSquare->bottomSquare = addressof(squareMatrix.getSquare(i - 1, j));

                // If not in the first col, set the left square
                if (j != 0) {
                    currentSquare->bottomLeftSquare = addressof(squareMatrix.getSquare(i - 1, j - 1));
                }

                // If not in the last col, set the right square
                if (j != 7) {
                    currentSquare->bottomRightSquare = addressof(squareMatrix.getSquare(i - 1, j + 1));
                }
            }

            // If not in the first row, set the top squares (left, middle , right)
            if (i != 7) {
                currentSquare->topSquare = addressof(squareMatrix.getSquare(i + 1, j));

                // If not in the first col, set the top left square
                if (j != 0) {
                    currentSquare->topLeftSquare = addressof(squareMatrix.getSquare(i + 1, j - 1));
                }

                // If not in the last col, set the top right square
                if (j != 7) {
                    currentSquare->topRightSquare = addressof(squareMatrix.getSquare(i + 1, j + 1));
                }
            }

            // Set the left square if not in the first col
            if (j != 0) {
                currentSquare->leftSquare = addressof(squareMatrix.getSquare(i, j - 1));
            }

            // Set the right square if not in the last col
            if (j != 7) {
                currentSquare->rightSquare = addressof(squareMatrix.getSquare(i, j + 1));
            }

            k++;
            colChar++;
        }
        colChar = 'a';
        rowChar++;
    }
}

void extractSquare(Point corners[4], cv::Mat &src, cv::Mat &extractedSquare) {
    cv::Mat maskRoi = cv::Mat::zeros(src.size(), src.type());
    // http://study.marearts.com/2016/07/opencv-drawing-example-line-circle.html
    cv::fillConvexPoly(maskRoi, corners, 4, cv::Scalar(255, 255, 255));
    src.copyTo(extractedSquare, maskRoi);
}

void EdgeDetecting::startProcess(Mat &src, ChessSquareMatrix &squareMatrix) {
    /// Reduce noise with a kernel 3x3
//    blur( src_gray, detected_edges, Size(3,3) );
    cvtColor(detected_edges, src_gray, CV_GRAY2BGR);
    vector<Point> bottomEdgePoints, leftEdgePoints;
    vector<double> bottomEdgeSquareDims, leftEdgeSquareDims;
    getCornerList(src, bottomEdgePoints, leftEdgePoints, bottomEdgeSquareDims, leftEdgeSquareDims);

//    houghLines(detected_edges, src_gray);

    Mat dst, cdst;
    int lowThreshold = 220;
    Canny(src, dst, lowThreshold, lowThreshold * 3, 3);
    cvtColor(dst, cdst, CV_GRAY2BGR);

    namedWindow("cann", CV_WINDOW_AUTOSIZE);
    imshow("cann", cdst);

    vector<Vec4i> vecHoughLines, pozAngleLines, negativeAngleLines;
    vector<Point> tableIntersectionPoints;

    houghSimpleLines(dst, vecHoughLines); //This will get all lines in the image

    filterAndRemoveLines(vecHoughLines, pozAngleLines, negativeAngleLines, tableIntersectionPoints);

    // Print lines that were found , USEFUL FOR DEBUG
//    for (Vec4i v:negativeAngleLines) {
//        line(cdst, Point(v[0], v[1]), Point(v[2], v[3]), Scalar(0, 0, 255), 3, CV_AA);
//    }
//    namedWindow("lines", CV_WINDOW_AUTOSIZE);
//    imshow("lines", cdst);


    //Change perspective
//    for (Point2f r:tableIntersectionPoints) {
//        circle(cdst, tableIntersectionPoints[2], 3, 255, 1, 8, 0);
//    }
//    tryPerspectiveCorrection(src,tableIntersectionPoints[0],tableIntersectionPoints[3],tableIntersectionPoints[2],tableIntersectionPoints[1]);

    searchCommonPointsAndRemove(pozAngleLines, 20, 20);
//    searchCommonPointsAndRemove(negativeAngleLines, 5, 5);
//    searchCommonSlopeAndRemove(pozAngleLines);
    sort(pozAngleLines.begin(), pozAngleLines.end(), comparisonFirstPointXAsc);
    sort(negativeAngleLines.begin(), negativeAngleLines.end(), comparisonFirstPointXAsc);
    searchCloseDistanceAndRemove(pozAngleLines, 10); //distance limit 30
    searchCloseDistanceAndRemove(negativeAngleLines, 10); //distance limit 30

    for (Vec4i v:negativeAngleLines) {
        line(cdst, Point(v[0], v[1]), Point(v[2], v[3]), Scalar(255, 0, 0), 3, CV_AA);
    }
    for (Vec4i v:pozAngleLines) {
        line(cdst, Point(v[0], v[1]), Point(v[2], v[3]), Scalar(0, 0, 255), 3, CV_AA);
    }
    namedWindow("lines", CV_WINDOW_AUTOSIZE);
    imshow("lines", cdst);

    Point2f pointMatrix[9][9];
    //Creates a 9x9 matrix with all the line intersection points of the table
    createSquarePointMatrix(pozAngleLines, negativeAngleLines, pointMatrix);

    //Creates a 8x8 matrix with the squares(corner points, neighbors,color,if it has piece) of the table
    createSquareMatrix(squareMatrix, pointMatrix);

    determineSquareColors(src, squareMatrix,0,3);

    //Print the square colors
    printf("\nWhite = 1 ; Black = 0");
    for (int i = 7; i >= 0; i--) {
        printf("\n");
        for (int j = 0; j < 8; j++) {
            printf("%d  ", squareMatrix.getSquare(i, j).color);
        }
    }


    ChessSquare *currentSquare = addressof(squareMatrix.getSquare(0,4));
    Point rook_points[4];
    rook_points[0] = currentSquare->topLeft;
    rook_points[1] = currentSquare->bottomLeft;
    rook_points[2] = currentSquare->bottomRight;
    rook_points[3] = currentSquare->topRight;

    Mat eqSq;
    extractSquare(rook_points, src, eqSq);
    imshow("dsd", eqSq);
}