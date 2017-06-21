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
bool compYDesc(Vec4i v1, Vec4i v2) {
    return v1.val[1] >= v2.val[1];
}

/**
 * Function used to sort() the lines in ASCENDING order after the Y value of the first point
 * @param v1 The first line
 * @param v2 The second line
 * @return True if the Y of the first point of the first line is smaller than the Y of the second line of the first point
 */
bool compXAscYAsc(Vec4i v1, Vec4i v2) {
    return v1.val[0] <= v2.val[0] && v1.val[1] <= v2.val[1];
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
 * @param chessTableEdges Object that will contain the edges
 */
void separateLinesByAngle(vector<Vec4i> &lines, int angle, ChessTableEdges &chessTableEdges) {
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
            chessTableEdges.verticalPositiveAngleLines.push_back(l);
        } else if (-angle == (int) angleMade) {
            chessTableEdges.horizontalNegativeAngleLines.push_back(l);
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
void filterLinesByIntersection(ChessTableEdges &chessTableEdges) {

    vector<Vec4i> *verticalLines = &chessTableEdges.verticalPositiveAngleLines;

    for (int i = 0; i < verticalLines->size(); i++) {
        Vec4i l1 = verticalLines->at(i);
        for (int j = i; j < verticalLines->size(); j++) {
            if (i != j) {
                Vec4i l2 = verticalLines->at(j);
                Point2f intersectionPoint = getIntersection(l1, l2);
                if (intersectionPoint.y >= 0) {
                    verticalLines->erase(verticalLines->begin() + j);
                    j--;
                }
            }
        }
    }

    vector<Vec4i> *horizontalLines = &chessTableEdges.horizontalNegativeAngleLines;
    for (int i = 0; i < horizontalLines->size(); i++) {
        Vec4i l1 = horizontalLines->at(i);
        for (int j = i; j < horizontalLines->size(); j++) {
            if (i != j) {
                Vec4i l2 = horizontalLines->at(j);
                Point2f intersectionPoint = getIntersection(l1, l2);
                if (intersectionPoint.x >= 0 || intersectionPoint.y >= 0) {
                    horizontalLines->erase(horizontalLines->begin() + j);
                    j--;
                }
            }
        }
    }
}

/**
 * Function used to find the margin edges of the chess table
 *
 * @param chessTableEdges The object containing the chess table edges
 * @param cdst Matrix for debug
 */
void findMarginLines(ChessTableEdges &chessTableEdges, Mat &cdst) {

    // These will hold the margin edges of the chess table
    Vec4i *verticalRight = NULL, *verticalLeft = NULL, *horizontalLow = NULL, *horizontalHigh = NULL;
    verticalLeft = &chessTableEdges.verticalPositiveAngleLines[0];
    verticalRight = &chessTableEdges.verticalPositiveAngleLines[0];
    horizontalLow = &chessTableEdges.horizontalNegativeAngleLines[0];
    horizontalHigh = &chessTableEdges.horizontalNegativeAngleLines[0];

    //Search for the low line points (aka the bottom-est and top-est edges)
    for (int i = 0; i < chessTableEdges.verticalPositiveAngleLines.size(); i++) {
        Vec4i *l = &chessTableEdges.verticalPositiveAngleLines[i];
        if (l->val[1] < verticalLeft->val[1]) {
            verticalLeft = l;
        }
        if (l->val[1] > verticalRight->val[1]) {
            verticalRight = l;
        }
//        line(cdst, Point(l->val[0], l->val[1]), Point(l->val[2], l->val[3]), Scalar(0, 255, 0), 3, CV_AA);
    }

//    line(cdst, Point(verticalRight->val[0], verticalRight->val[1]), Point(verticalRight->val[2], verticalRight->val[3]), Scalar(0, 255, 0), 3, CV_AA);
//    line(cdst, Point(verticalLeft->val[0], verticalLeft->val[1]), Point(verticalLeft->val[2], verticalLeft->val[3]), Scalar(0, 255, 0), 3, CV_AA);

    chessTableEdges.verticalRight = verticalRight;
    chessTableEdges.verticalLeft = verticalLeft;

    //Search for the left line points (aka the left-est and right-est edges)
    for (int i = 0; i < chessTableEdges.horizontalNegativeAngleLines.size(); i++) {
        Vec4i *l = &chessTableEdges.horizontalNegativeAngleLines[i];
        if (l->val[1] > horizontalLow->val[1]) {
            horizontalLow = l;
        }

        if (l->val[1] < horizontalHigh->val[1]) {
            horizontalHigh = l;
        }
//        line(cdst, Point(l->val[0], l->val[1]), Point(l->val[2], l->val[3]), Scalar(255, 0, 0), 3, CV_AA);
    }

//    line(cdst, Point(horizontalHigh->val[0], horizontalHigh->val[1]), Point(horizontalHigh->val[2], horizontalHigh->val[3]), Scalar(0, 255, 0), 3, CV_AA);
//    line(cdst, Point(horizontalLow->val[0], horizontalLow->val[1]), Point(horizontalLow->val[2], horizontalLow->val[3]), Scalar(0, 255, 0), 3, CV_AA);

    chessTableEdges.horizontalHigh = horizontalHigh;
    chessTableEdges.horizontalLow = horizontalLow;
}

/**
 * Function used to shorten the lines. It takes all the lines (horizontal and vertical), calculate the intersection with
 * the margin edges and set the intersection points to the lines as the line's vertexes
 *
 * @param chessTableEdges Object that hold all the lines (horizontals, verticals, left, right, top and bottom)
 */
void shortenEdges(ChessTableEdges &chessTableEdges) {

    for (unsigned i = 0; i < chessTableEdges.horizontalNegativeAngleLines.size(); i++) {
        Vec4i *l = &chessTableEdges.horizontalNegativeAngleLines[i];
        Point2f intersectionLeft = getIntersection(*l, *chessTableEdges.verticalLeft);
        l->val[0] = (int) intersectionLeft.x;
        l->val[1] = (int) intersectionLeft.y;
        Point2f intersectionRight = getIntersection(*l, *chessTableEdges.verticalRight);
        l->val[2] = (int) intersectionRight.x;
        l->val[3] = (int) intersectionRight.y;
    }

    for (unsigned i = 0; i < chessTableEdges.verticalPositiveAngleLines.size(); i++) {
        Vec4i *l = &chessTableEdges.verticalPositiveAngleLines[i];
        Point2f intersectionTop = getIntersection(*l, *chessTableEdges.horizontalLow);
        l->val[0] = (int) intersectionTop.x;
        l->val[1] = (int) intersectionTop.y;
        Point2f intersectionBottom = getIntersection(*l, *chessTableEdges.horizontalHigh);
        l->val[2] = (int) intersectionBottom.x;
        l->val[3] = (int) intersectionBottom.y;
    }
}

/**
 * Function used to calculate the corner points of the table
 *
 * @param chessTableEdges The object containing the chess table edges
 * @param tableIntersectionPoints Vector to store the corner points
 * @param cdst Matrix to display (FOR DEBUG)
 */
void findTableCornerPoints(ChessTableEdges &chessTableEdges, vector<Point> &tableIntersectionPoints, Mat &cdst) {
    tableIntersectionPoints.push_back(
            getIntersection(*chessTableEdges.verticalLeft, *chessTableEdges.horizontalLow)); //left point
    tableIntersectionPoints.push_back(
            getIntersection(*chessTableEdges.verticalLeft, *chessTableEdges.horizontalHigh)); //top top
    tableIntersectionPoints.push_back(
            getIntersection(*chessTableEdges.verticalRight, *chessTableEdges.horizontalLow)); //bottom
    tableIntersectionPoints.push_back(
            getIntersection(*chessTableEdges.verticalRight, *chessTableEdges.horizontalHigh)); //right point

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
 * Function will receive 2 vectors of lines (horizontally and vertically) which are sorted by their X and will calculate
 * their intersections and will form a matrix of 9x9 representing the chess table square's corners
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

    // The (0,0) is the bottom left corner
    int maxLimit = 8;
    for (int i = 0; i <= maxLimit; i++) {
        for (int j = 0; j <= maxLimit; j++) {
            Point2f point = getIntersection(pozAngleLines[i], negativeAngleLines[j]);
            pointMatrix[i][j] = point;
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

    // For a matrix starting (0,0) to (7,7), if the cols+rows is an even number then point (0,0) is that color
    if ((squareRow + squareCol) % 2 == 0) {
        squareMatrix.getSquare(0, 0).color = squareColor;
    } else {
        squareMatrix.getSquare(0, 0).color = abs(squareColor - 1);
    }

    // Iterate bottom-top, left - right and assign the opposite color to the right and top squares
    for (int i = 0; i < 8; i++) {
        ChessSquare *sq = addressof(squareMatrix.getSquare(i, 0));
        while (sq != NULL) {
            if (sq->rightSquare != NULL) {
                sq->rightSquare->color = abs(sq->color - 1);
            }
            if (sq->topSquare != NULL) {
                sq->topSquare->color = abs(sq->color - 1);
            }
            sq = sq->rightSquare;
        }
    }
}

/**
 * Method used to extract a square from the Mat given the corner points
 *
 * @param corners A vector of the corner points
 * @param src The source Mat
 * @param extractedSquare The Mat containing the extracted square
 */
void extractSquare(Point corners[4], cv::Mat &src, cv::Mat &extractedSquare) {
    cv::Mat maskRoi = cv::Mat::zeros(src.size(), src.type());
    // http://study.marearts.com/2016/07/opencv-drawing-example-line-circle.html
    cv::fillConvexPoly(maskRoi, corners, 4, cv::Scalar(255, 255, 255));
    src.copyTo(extractedSquare, maskRoi);
}

void EdgeProcessing::startProcess(Mat &src, ChessSquareMatrix &squareMatrix) {

    Mat dst, cdst;
    int lowThreshold = 220; // TODO THIS SHOULD BE AUTOMATIZED, SEARCH INTERNET
    Canny(src, dst, lowThreshold, lowThreshold * 3, 3);
    cvtColor(dst, cdst, CV_GRAY2BGR);

//    const char *show_canny_startup_image = "Canny of the start input image";
//    namedWindow(show_canny_startup_image, CV_WINDOW_AUTOSIZE);
//    imshow(show_canny_startup_image, src);

    // This vector will contain the output lines from the library's function to detect lines
    vector<Vec4i> vecHoughLines;
    // This will detect 2 set of lines ( horizontal and vertical) but with different slopes
    houghSimpleLines(dst, vecHoughLines); //This will get all lines in the image

    int angleSeparator = 2;
    ChessTableEdges chessTableEdges;
    // Separates the lines in 2 sets after an angle
    separateLinesByAngle(vecHoughLines, angleSeparator, chessTableEdges);

    // Filter lines by their intersection
    filterLinesByIntersection(chessTableEdges);

    // This variable will hold the chess table corner points
    vector<Point> tableIntersectionPoints;

    // Find the margin edges
    findMarginLines(chessTableEdges, cdst);

    // Shorten the edges
    shortenEdges(chessTableEdges);

    // Get the tables corner points
    findTableCornerPoints(chessTableEdges, tableIntersectionPoints, cdst);

    // TODO here needs work because it depends on the image and how the table is positioned
    sort(chessTableEdges.verticalPositiveAngleLines.begin(), chessTableEdges.verticalPositiveAngleLines.end(),
         compYDesc);
    sort(chessTableEdges.horizontalNegativeAngleLines.begin(), chessTableEdges.horizontalNegativeAngleLines.end(),
         compYDesc);

    searchCloseDistanceAndRemove(chessTableEdges.verticalPositiveAngleLines, 10);
    searchCloseDistanceAndRemove(chessTableEdges.horizontalNegativeAngleLines, 10);

//    int i = 0;
//    for (int i = 0; i < chessTableEdges.horizontalNegativeAngleLines.size(); i++) {
//        Vec4i v = chessTableEdges.horizontalNegativeAngleLines[i];
//        line(cdst, Point(v[0], v[1]), Point(v[2], v[3]), Scalar(255, 0, 0), 3, CV_AA);
//
//        Vec4i v2 = chessTableEdges.verticalPositiveAngleLines[i];
//        line(cdst, Point(v2[0], v2[1]), Point(v2[2], v2[3]), Scalar(0, 255, 0), 3, CV_AA);
//    }
//    namedWindow("lines", CV_WINDOW_AUTOSIZE);
//    imshow("lines", cdst);

    Point2f pointMatrix[9][9];
    //Creates a 9x9 matrix with all the line intersection points of the table
    createSquarePointMatrix(chessTableEdges.verticalPositiveAngleLines, chessTableEdges.horizontalNegativeAngleLines,
                            pointMatrix);

    //Creates a 8x8 matrix with the squares(corner points, neighbors,color,if it has piece) of the table
    createSquareMatrix(squareMatrix, pointMatrix);

    determineSquareColors(src, squareMatrix, 3, 0);

    //Print the square colors
    printf("\nWhite = 1 ; Black = 0");
    for (int i = 7; i >= 0; i--) {
        printf("\n");
        for (int j = 0; j < 8; j++) {
            printf("%d  ", squareMatrix.getSquare(i, j).color);
        }
    }


//    ChessSquare *currentSquare = addressof(squareMatrix.getSquare(0, 4));
//    Point rook_points[4];
//    rook_points[0] = currentSquare->topLeft;
//    rook_points[1] = currentSquare->bottomLeft;
//    rook_points[2] = currentSquare->bottomRight;
//    rook_points[3] = currentSquare->topRight;

//    Mat eqSq;
//    extractSquare(rook_points, src, eqSq);
//    imshow("dsd", eqSq);
}