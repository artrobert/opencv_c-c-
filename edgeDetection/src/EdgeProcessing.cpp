//
// Created by artin on 13/6/2017.
//

#include <opencv/cv.hpp>
#include "EdgeProcessing.h"

using namespace cv;


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
 * Given 2 segments, returns the point of intersection if they intersect, else returns point (0,0)
 *
 * @param line1 Segment 1
 * @param line2 Segment 2
 * @return return if true, the point of intersection , else point(0,0)
 */
Point2f getIntersection(Vec4i &line1, Vec4i &line2) {
    // Lines 1
    Point2f o1 = Point(line1[0], line1[1]);
    Point2f o2 = Point(line1[2], line1[3]);

    // Line 2
    Point2f p1 = Point(line2[0], line2[1]);
    Point2f p2 = Point(line2[2], line2[3]);

    // line2.xy1 - line1.xy1
    Point2f x = p1 - o1;

    // line1.xy2 - line1.xy1
    Point2f d1 = o2 - o1;
    // line2.xy2 - line2.xy2
    Point2f d2 = p2 - p1;

    float cross = d1.x * d2.y - d1.y * d2.x;

    if (abs(cross) < /*EPS*/1e-8) {
        return Point2f(0, 0);
    }
    double t1 = (x.x * d2.y - x.y * d2.x) / cross;
    return (o1 + d1 * t1);
}

/**
 * This method is used to extract lines with the HoughLines function from OPENCV
 *
 * @param cannyMat The BINARY Mat containing the edges after a canny(preferably) was applied
 * @param resultLines Vector containing extracted lines
 */
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
    }
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

        // This will get the angle that the line is making with the X axis
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

/**
 * Method used to filter the vertical and the horizontal lines by intersection.
 *
 * Select 2 distinct lines (from vertical for example) and see if they intersect
 * From tests we see that for : 1) verticals   : the intersect.y is >=0
 *                              2) horizontals : the intersect.x >=0 OR intersect.y >=0
 *
 * @param verticalLines Vector containing vertical lines
 * @param horizontalLines Vector containing horizontal lines
 */
void filterLinesByIntersection(vector<Vec4i> &verticalLines, vector<Vec4i> &horizontalLines) {

    for (int i = 0; i < verticalLines.size(); i++) {
        Vec4i l1 = verticalLines.at(i);
        for (int j = i; j < verticalLines.size(); j++) {
            if (i != j) {
                Vec4i l2 = verticalLines.at(j);
                Point2f intersectionPoint = getIntersection(l1, l2);
                if (intersectionPoint.y >= 0) {
                    verticalLines.erase(verticalLines.begin() + j);
                    j--;
                }
            }
        }
    }

    for (int i = 0; i < horizontalLines.size(); i++) {
        Vec4i l1 = horizontalLines.at(i);
        for (int j = i; j < horizontalLines.size(); j++) {
            if (i != j) {
                Vec4i l2 = horizontalLines.at(j);
                Point2f intersectionPoint = getIntersection(l1, l2);
                if (intersectionPoint.x >= 0 || intersectionPoint.y >= 0) {
                    horizontalLines.erase(horizontalLines.begin() + j);
                    j--;
                }
            }
        }
    }
}

/**
 * Method used find margin edges and calculate the table's corner points
 * !NOTE! Vec4 is like P1([0],[1]) P2([2],[3])
 *
 * @param verticalLines The vector where the vertical lines will be placed (have pozitive angle)
 * @param horizontalLines The vector where the horizontal lines will be placed (have negative angle)
 * @param tableIntersectionPoints The vector where the tables corner points will be placed
 * @param cdst Matrix used to show the points, ONLY FOR DEBUG NEEDED
 */
void findTableCornerPoints(vector<Vec4i> &verticalLines, vector<Vec4i> &horizontalLines,
                           vector<Point> &tableIntersectionPoints, Mat &cdst) {

    // These will hold the margin edges of the chess table
    Vec4i verticalRightLine = verticalLines[0], verticalLeftLine = verticalLines[0];
    Vec4i horizontalLowLine = horizontalLines[0], horizontalHighLine = horizontalLines[0];

    //Search for the low line points (aka the bottom-est and top-est edges)
    for (Vec4i l:verticalLines) {
        if (l[1] < verticalLeftLine[1]) {
            verticalLeftLine = l;
        }
        if (l[1] > verticalRightLine[1]) {
            verticalRightLine = l;
        }
//        line(cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 255, 0), 3, CV_AA);
    }

    //Search for the left line points (aka the left-est and right-est edges)
    for (Vec4i l:horizontalLines) {
        if (l[1] > horizontalLowLine[1]) {
            horizontalLowLine = l;
        }

        if (l[1] < horizontalHighLine[1]) {
            horizontalHighLine = l;
        }
        line(cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 0, 0), 3, CV_AA);
    }

    tableIntersectionPoints.push_back(getIntersection(verticalLeftLine, horizontalLowLine)); //left point
    tableIntersectionPoints.push_back(getIntersection(verticalLeftLine, horizontalHighLine)); //top top
    tableIntersectionPoints.push_back(getIntersection(verticalRightLine, horizontalLowLine)); //bottom
    tableIntersectionPoints.push_back(getIntersection(verticalRightLine, horizontalHighLine)); //right point


    //Debug purpose
    //bottom line
//    line(cdst, Point(verticalLeftLine[0], verticalLeftLine[1]), Point(verticalLeftLine[2], verticalLeftLine[3]), Scalar(0, 0, 255), 3, CV_AA);
//    //top line
//    line(cdst, Point(verticalRightLine[0], verticalRightLine[1]), Point(verticalRightLine[2], verticalRightLine[3]), Scalar(0, 0, 255), 3, CV_AA);
//    //left line
//    line(cdst, Point(horizontalLowLine[0], horizontalLowLine[1]), Point(horizontalLowLine[2], horizontalLowLine[3]), Scalar(0, 0, 255), 3, CV_AA);
//    //right line
//    line(cdst, Point(horizontalHighLine[0], horizontalHighLine[1]), Point(horizontalHighLine[2], horizontalHighLine[3]), Scalar(0, 0, 255), 3, CV_AA);
//    const char *show_table_corner_points = "Table margin lines";
//    namedWindow(show_table_corner_points, CV_WINDOW_AUTOSIZE);
//    imshow(show_table_corner_points, cdst);
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

/**
 * We extract a square from the image and determine its color by binarize it, counting the pixels
 * and determine who are predominant
 *
 * @param src The Mat containing the chessboard
 * @param squareMatrix A matrix of {@link ChessSquare} objects representing the chessboard table
 * @param pointMatrix A matrix of points representing all the chessboard square corners
 */
void determineSquareColors(cv::Mat &src, ChessSquareMatrix &squareMatrix, size_t squareRow, size_t squareCol) {

    cv::Mat maskRoi = cv::Mat::zeros(src.size(), src.type());
    // Mask used to extract a single square, make it red so we can later binarize the image
    cv::Mat maskRoiBlue = cv::Mat(src.size(), src.type(), Scalar(255, 0, 0)); //mask used to extract a single square

    cv::Mat binaryExtractedChessSquare;
    ChessSquare *currentSquare = addressof(squareMatrix.getSquare(squareRow, squareCol));

    Point rook_points[4];
    rook_points[0] = currentSquare->topLeft;
    rook_points[1] = currentSquare->bottomLeft;
    rook_points[2] = currentSquare->bottomRight;
    rook_points[3] = currentSquare->topRight;

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
    // See the percentage of the newly added pixels (what is more predominant whites/blacks)
    double perc_of_white_stay_white = (new_white_pixels * 100) / white_pixels;
    if (perc_of_white_stay_white < 50) {
        squareColor = 0; // the color is black
    } else {
        squareColor = 1; // the color is white
    }

    // !!!NOTE!! This is very dependent of the square we extracted (above code)
    // To remove the dependency we need to add(col+row)%2 if = 0 same color else the other one
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

void EdgeProcessing::startProcess(Mat &src, ChessSquareMatrix &squareMatrix) {

    Mat dst, cdst;
    int lowThreshold = 220; // TODO THIS SHOULD BE AUTOMATIZED, SEARCH INTERNET
    Canny(src, dst, lowThreshold, lowThreshold * 3, 3);
    cvtColor(dst, cdst, CV_GRAY2BGR);

    const char *show_canny_startup_image = "Canny of the start input image";
    namedWindow(show_canny_startup_image, CV_WINDOW_AUTOSIZE);
    imshow(show_canny_startup_image, cdst);

    // This vector will contain the output lines from the library's function to detect lines
    vector<Vec4i> vecHoughLines;
    // This will detect 2 set of lines ( horizontal and vertical) but with different slopes
    houghSimpleLines(dst, vecHoughLines); //This will get all lines in the image

    int angleSeparator = 2;
    vector<Vec4i> verticalPositiveAngleLines, horizontalNegativeAngleLines;
    // Separates the lines in 2 sets after an angle
    separateLinesByAngle(vecHoughLines, angleSeparator, verticalPositiveAngleLines, horizontalNegativeAngleLines);

    // Filter lines by their intersection
    filterLinesByIntersection(verticalPositiveAngleLines, horizontalNegativeAngleLines);

    // This variable will hold the chess table corner points
    vector<Point> tableIntersectionPoints;
    // Get the tables corner points
    findTableCornerPoints(verticalPositiveAngleLines, horizontalNegativeAngleLines, tableIntersectionPoints, cdst);

    // Search and remove edges that have close points
    searchCommonPointsAndRemove(verticalPositiveAngleLines, 20, 20);

    sort(verticalPositiveAngleLines.begin(), verticalPositiveAngleLines.end(), comparisonFirstPointXAsc);
    sort(horizontalNegativeAngleLines.begin(), horizontalNegativeAngleLines.end(), comparisonFirstPointXAsc);
    searchCloseDistanceAndRemove(verticalPositiveAngleLines, 10); //distance limit 30
    searchCloseDistanceAndRemove(horizontalNegativeAngleLines, 10); //distance limit 30

    for (Vec4i v:horizontalNegativeAngleLines) {
        line(cdst, Point(v[0], v[1]), Point(v[2], v[3]), Scalar(255, 0, 0), 3, CV_AA);
    }
    for (Vec4i v:verticalPositiveAngleLines) {
        line(cdst, Point(v[0], v[1]), Point(v[2], v[3]), Scalar(0, 0, 255), 3, CV_AA);
    }
    namedWindow("lines", CV_WINDOW_AUTOSIZE);
    imshow("lines", cdst);

    Point2f pointMatrix[9][9];
    //Creates a 9x9 matrix with all the line intersection points of the table
    createSquarePointMatrix(verticalPositiveAngleLines, horizontalNegativeAngleLines, pointMatrix);

    //Creates a 8x8 matrix with the squares(corner points, neighbors,color,if it has piece) of the table
    createSquareMatrix(squareMatrix, pointMatrix);

    determineSquareColors(src, squareMatrix, 0, 3);

    //Print the square colors
    printf("\nWhite = 1 ; Black = 0");
    for (int i = 7; i >= 0; i--) {
        printf("\n");
        for (int j = 0; j < 8; j++) {
            printf("%d  ", squareMatrix.getSquare(i, j).color);
        }
    }


    ChessSquare *currentSquare = addressof(squareMatrix.getSquare(0, 4));
    Point rook_points[4];
    rook_points[0] = currentSquare->topLeft;
    rook_points[1] = currentSquare->bottomLeft;
    rook_points[2] = currentSquare->bottomRight;
    rook_points[3] = currentSquare->topRight;

    Mat eqSq;
    extractSquare(rook_points, src, eqSq);
    imshow("dsd", eqSq);
}