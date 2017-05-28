#include <opencv/cv.hpp>
#include <opencv2/shape.hpp>
#include "ImageBasicOperations.h"
#include "ImageDB.h"
#include "models/PieceModel.h"

using namespace cv;
using namespace std;

/** GLOBAL ATTRIBUTES **/

const CvSize ImageDB::imageResizeSize = CvSize(imageResizeHeight, imageResizeWidth);


string pathRoot = "D:\\Facultate\\c++Project\\opencv_c-c-\\edgeDetection\\database";
string folderRoot = "\\attempt7";
string pionText = "pion";
string calText = "cal";
string turaText = "tura";
string nebunText = "nebun";
string reginaText = "regina";
string regeText = "rege";

string jpg_extension = ".jpg";
string piece_format = "\\piece_";
string no_piece_format = "\\piece_n_";
string result_format = "_result_";

vector<PieceModel> allPieces;

cv::Ptr<cv::ShapeContextDistanceExtractor> mysc;

/** HEADERS **/

void loadPiece(PieceType type, string pieceText);

void ImageDB::loadSampleImages() {
    mysc = cv::createShapeContextDistanceExtractor();
    loadPiece(PieceType::pion, pionText); //pt. pion
//    loadPiece(cal, calText); //pt. cal
//    loadPiece(tura, turaText); //pt. tura
//    loadPiece(nebun, nebunText); //pt. nebun
//    loadPiece(regina, reginaText); //pt. regina
//    loadPiece(rege, regeText); //pt. rege
}

void loadPieceContour(string pieceRoot, vector<vector<Point>> &pieceContours);

cv::Mat doSubtraction(cv::Mat &withoutImg, cv::Mat &withImg);

/** OPERATIONS FOR FINDING THE CONTOUR & LAYER **/

/**
 * We call this function in order to find in what layer the piece is part of
 *
 * ** The layers represent the angle/distance from the camera to the piece **
 *
 * When the piece is closer to the camera, the view is "from above angle" and its harder to identify that piece
 * so this should be called so we know at least that it is close and the camera and treat it differently
 *
 * @param basePoint The lowest point of the piece (Height max)
 * @return The layer from which the piece is part of
 */
int findOriginLayer(Point basePoint) {
    if (basePoint.y <= ImageDB::layer0) { //closest to the camera
        return 0;
    } else if (basePoint.y <= ImageDB::layer1) { // below the half of the chess table
        return 1;
    } else if (basePoint.y <= ImageDB::layer2) { // above the half of the chess table
        return 2;
    } else if (basePoint.y <= ImageDB::layer3) { // furthers to the camera
        return 3;
    }
}

/**
 *  Function used to extract the contour of chess piece from an image and
 *  return an object containing the contour and the layer of which the piece is part of
 *
 * @param mat The incoming matrice that should be binary and from which the piece contour will be extracted
 * @return The object containing the contour and the layer from which is a part of
 */
PieceContour ImageDB::getContourFromMat(const Mat &mat) {
    PieceContour pc = PieceContour();
    vector<vector<Point>> fullContour;

    // We do a canny in order to get the contours of all the scene objects
    Mat canny_output;
    Canny(mat.clone(), canny_output, 255, 255, 3);
    vector<Vec4i> hierarchy;

    findContours(canny_output, fullContour, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // Search for the object with the biggest contour (most likely that will be our piece if THE TABLE IS NOT MOVED)
    double maxContourArea = 0;
    int indexMaxContour = 0;

    int bigContourSize = fullContour.size();

    // Iterate through all the contours to find the one with the biggest area
    for (int i = 0; i < bigContourSize; i++) {
        double currentContourArea = contourArea(fullContour[i], false);
        if (maxContourArea < currentContourArea) {
            maxContourArea = currentContourArea;
            indexMaxContour = i;
        }
    }

    pc.contour = fullContour[indexMaxContour];
    pc.computeTheLowestPoint();
    pc.layer = findOriginLayer(pc.lowestPoint);
    vector<vector<Point> > fdsfas;
    fdsfas.push_back(pc.contour);


    Mat drawing = Mat::zeros(mat.size(), CV_8UC3);
    Scalar color = Scalar(RNG(12345).uniform(0, 255), RNG(12345).uniform(0, 255), RNG(12345).uniform(0, 255));
    drawContours(drawing, fdsfas, 0, color, 2, 8, hierarchy, 0, Point());

    /// Show in a window
    namedWindow("Contours", CV_WINDOW_AUTOSIZE);
    imshow("Contours", drawing);
    waitKey(600 * 10);

    return pc;
}

cv::Mat doSubtraction(cv::Mat &withoutImg, cv::Mat &withImg) {
    Ptr<BackgroundSubtractorMOG2> pMOG2 = createBackgroundSubtractorMOG2();
    pMOG2->setShadowValue(0);
//    pMOG2->setDetectShadows(false);
//    pMOG2->setShadowThreshold(0.9);
    Mat maskResult;
    withoutImg = imagePreparation::blurImage(withoutImg); // we blur the image , we don't care about the details
    for (int i = 0; i < 50; i++) {
        pMOG2->apply(withoutImg, maskResult, 0.5); // FEED the image so the bg-subtractor learn it
    }
    withImg = imagePreparation::blurImage(withImg);
    pMOG2->apply(withImg, maskResult,
                 0); //feed to the subtractor the image with the piece and with a learning factor of 0 TODO change to 0
//    maskResult = imagePreparation::erosionImage(maskResult, 2, 1); // make a erosion to eliminate the noise
    return maskResult;
}

/** OPERATIONS FOR MATCHING **/

/**
 * Function used to compare an contour to the sample from the database in order to identify it
 * by comparing it to all the samples we have and selecting the best match using a BEST AVERAGE
 *
 *
 * @param incomingContour The object containing the contour that should be identified
 * @return The piece type of the contour {@link PieceType}
 */
PieceType ImageDB::matchChessPieces(PieceContour incomingContour) {
    float bestDis = FLT_MAX;
    PieceType pieceType = PieceType::unknown;

    //iterate through all of the 6 pieces
    for (size_t i = 0; i < allPieces.size(); i++) {
        PieceModel currentPiece = allPieces[i];
        int pieceContoursSize = currentPiece.contours.size();
        float averageDistance = 0;
        // iterate through all the sample contours of one piece and compute the distances
        for (int j = 0; j < pieceContoursSize; j++) {
            // TODO should see the layer from which is a part of and handle if its a special case (too close to the camera)
            // TODO we could ask for contours that are part of that layer or donno, figure it out :)
            PieceContour pieceContour = currentPiece.contours[j];

            if (incomingContour.layer == 0) {
                if (pieceContour.layer == incomingContour.layer) {
                    float distanceFound = mysc->computeDistance(incomingContour.contour, pieceContour.contour);
                    averageDistance += distanceFound;
                }
            } else {
                float distanceFound = mysc->computeDistance(incomingContour.contour, pieceContour.contour);
                averageDistance += distanceFound;
            }
        }

        averageDistance /= pieceContoursSize; //get the average contour distance after we calculated for all the pieces

        if (averageDistance < bestDis) {
            bestDis = averageDistance;
            pieceType = currentPiece.pieceType;
        }
    }
    return pieceType;
}

/** Create DB samples **/

void createSamples(string pieceName) {
    string pieceRoot = piece_format;
    int maxTries = 3; // if we find a image that has cols/rows = 0 (doesn't exist) we exit
    int i = 0;
    while (i < 99 && maxTries != 0) {
        stringstream aux;
        if (i < 10) {
            aux << 0;
        }
        aux << i;
        // create path ("folderPath/pion_##.jpg") to the image WITH the piece
        string source_image = pathRoot + folderRoot + "\\" + pieceName + pieceRoot + aux.str() + jpg_extension;
        // create path ("folderPath/pion_n_##.jpg") to the image WITHOUT the piece
        string source_n_image = pathRoot + folderRoot + "\\" + pieceName + no_piece_format + aux.str() + jpg_extension;
        // create path ("folderPath/pion_result_##.jpg") to the result of the bg-subtraction
        string result_image =
                pathRoot + folderRoot + "\\" + pieceName + "\\" + pieceName + result_format + aux.str() + jpg_extension;
        Mat withoutPiece = imread(source_n_image);
        Mat withPiece = imread(source_image);
        i++;
        if (withoutPiece.cols == 0 || withoutPiece.rows == 0 || withPiece.cols == 0 || withPiece.rows == 0) {
            maxTries--;
        } else {
            resize(withoutPiece, withoutPiece, ImageDB::imageResizeSize);
            resize(withPiece, withPiece, ImageDB::imageResizeSize);
            Mat result = doSubtraction(withoutPiece, withPiece);
            imwrite(result_image, result);
        };
    }
    printf("\nFinished creating samples for %s", pieceName.c_str());
}

void ImageDB::createDatabase() {
    createSamples(pionText);
//    createSamples(calText);
//    createSamples(turaText);
//    createSamples(nebunText);
//    createSamples(regeText);
//    createSamples(reginaText);
}

/** Load DB samples **/


/**
 * This function loads all the sample images for a piece (tura,pion etc) and gets their contour
 *
 * @param pieceRoot The name of the piece we are trying to load
 * @return A object containing a list of contours
 */
PieceModel loadSampleImages(string pieceRoot) {
    PieceModel pieceModel = PieceModel(); // instantiate the object that will contain all the contours
    int maxTries = 3; // if we find a image that has cols/rows = 0 (doesn't exist) we exit
    int i = 0;

    // iterate through all the sample piece images and extract the contour
    while (i < 99 && maxTries != 0) {
        stringstream aux;
        if (i < 10) {
            aux << 0; //add 0 in front to have the _## format
        }
        aux << i;
        //created "folderPath/pion_result_##.jpg"
        string pieceFullPath =
                pathRoot + folderRoot + "\\" + pieceRoot + "\\" + pieceRoot + result_format + aux.str() + jpg_extension;
        Mat img = imread(pieceFullPath, 0);
        if (img.rows == 0 || img.cols == 0) { //if the image doesn't exist
            maxTries--;
        } else {
            resize(img, img, ImageDB::imageResizeSize);
            namedWindow("read", CV_WINDOW_AUTOSIZE);
            imshow("read", img);
            PieceContour pieceContour = ImageDB::getContourFromMat(img);
            pieceModel.contours.push_back(pieceContour);
        }
        i++;
    }
    return pieceModel;
}

/**
 * Load all the contours for the specified piece
 * @param type Piece type (eg: pion, tura etc...)
 * @param pieceText The piece name
 */
void loadPiece(PieceType type, string pieceText) {
    PieceModel model = loadSampleImages(pieceText);
    model.pieceType = type;
    allPieces.push_back(model);
    printf("\n\n Successfully loaded <%s>", pieceText.c_str());
}

