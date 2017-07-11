//
// Created by artin on 06/7/2017.
//

#include <opencv2/imgcodecs.hpp>
#include <opencv/cv.hpp>
#include "ContourDatabase.h"
#include "../ImageBasicOperations.h"

using namespace cdb;

const CvSize ContourDatabase::imageResizeSize = CvSize(imageResizeHeight, imageResizeWidth);


/**
 * Function that will call the function that create a binary image containing the shape of the image an save it raw
 *
 * THIS SHOULD BE CALLED IF WE DONT HAVE ANY BINARY IMAGES (OR "SAMPLES")
 */
void ContourDatabase::createDatabase() {
    createSamples(pawnText);
    createSamples(knightText);
    createSamples(rookText);
    createSamples(bishopText);
    createSamples(kingText);
    createSamples(reginaText);
}

/**
* Function used to load 2 images from a path, these 2 images represent the imagine WITH the piece and the image
* WITHOUT the piece so in the end to do a background subtraction in order to remove the backgroun and highlight the
* pieces form
*
* In the end will produce a binari image of the piece's contour and will save it as a jpg file
*
* @param pieceName The name of the piece we want to create the binary images
*/
void ContourDatabase::createSamples(string pieceName) {
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
        string source_n_image =
                pathRoot + folderRoot + "\\" + pieceName + no_piece_format + aux.str() + jpg_extension;
        // create path ("folderPath/pion_result_##.jpg") to the result of the bg-subtraction
        string result_image =
                pathRoot + folderRoot + "\\" + pieceName + "\\" + pieceName + result_format + aux.str() +
                jpg_extension;
        Mat withoutPiece = imread(source_n_image);
        Mat withPiece = imread(source_image);
        i++;
        if (withoutPiece.cols == 0 || withoutPiece.rows == 0 || withPiece.cols == 0 || withPiece.rows == 0) {
            maxTries--;
        } else {
            resize(withoutPiece, withoutPiece, imageResizeSize);
            resize(withPiece, withPiece, imageResizeSize);
            Mat result = doSubtraction(withoutPiece, withPiece);
            imwrite(result_image, result);
        };
    }
    printf("\nFinished creating samples for %s", pieceName.c_str());
}

/**
* Function used to load all the pieces contour
*
* THIS SHOULD BE CALLED BEFORE MAKING A MATCHING
*/
void ContourDatabase::loadSampleImages() {
    loadPiece(pawn, PieceType::pion, pawnText); //pt. pion
    loadPiece(knight, PieceType::cal, knightText); //pt. cal
    loadPiece(rook, PieceType::tura, rookText); //pt. tura
    loadPiece(bishop, PieceType::nebun, bishopText); //pt. nebun
    loadPiece(queen, PieceType::regina, reginaText); //pt. regina
    loadPiece(king, PieceType::rege, kingText); //pt. rege
    areSamplesLoaded = true;
}

/**
* This function loads all the sample images for a piece (tura,pion etc) and gets their contour
*
* @param pieceRoot The name of the piece we are trying to load
* @return A object containing a list of contours
*/
PieceModel ContourDatabase::generatePieceModel(string pieceRoot) {
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
                pathRoot + folderRoot + "\\" + pieceRoot + "\\" + pieceRoot + result_format + aux.str() +
                jpg_extension;
        Mat img = imread(pieceFullPath, 0);
        if (img.rows == 0 || img.cols == 0) { //if the image doesn't exist
            maxTries--;
        } else {
            resize(img, img, imageResizeSize);
            namedWindow("read", CV_WINDOW_AUTOSIZE);
            imshow("read", img);
            PieceContour pieceContour = getContourFromMat(img);
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
void ContourDatabase::loadPiece(PieceModel &piece, PieceType type, string pieceText) {
    piece = generatePieceModel(pieceText);
    piece.pieceType = type;
    allPieces.push_back(piece);
    printf("\n\n Successfully loaded <%s>", pieceText.c_str());
}

/**
 * Function used to compare an contour to the sample from the database in order to identify it
 * by comparing it to all the samples we have and selecting the best match using a BEST AVERAGE
 *
 *
 * @param pieceIncoming The object containing the contour that should be identified
 * @return The piece type of the contour {@link PieceType}
 */
PieceType ContourDatabase::matchChessPieces(PieceContour pieceIncoming) {
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

            if (pieceIncoming.layer == 0) {
                if (pieceContour.layer == pieceIncoming.layer) {
                    float distanceFound = mysc->computeDistance(pieceIncoming.contour, pieceContour.contour);
                    averageDistance += distanceFound;
                }
            } else {
                float distanceFound = mysc->computeDistance(pieceIncoming.contour, pieceContour.contour);
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

/**
* This will do a background subtraction between two images and will return the result
*
* In this function, the input images will be blurred in order to remove the noise
*
*
* @param withoutImg The image with only the background
* @param withImg The image with the background and the piece
* @return A binary image highliting the contour of the piece
*/
cv::Mat ContourDatabase::doSubtraction(cv::Mat &withoutImg, cv::Mat &withImg) {
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
int ContourDatabase::findOriginLayer(Point basePoint) {
    if (basePoint.y <= layer0) { //closest to the camera
        return 0;
    } else if (basePoint.y <= layer1) { // below the half of the chess table
        return 1;
    } else if (basePoint.y <= layer2) { // above the half of the chess table
        return 2;
    } else if (basePoint.y <= layer3) { // furthers to the camera
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
PieceContour ContourDatabase::getContourFromMat(const Mat &mat) {
    PieceContour pc = PieceContour();
    vector<vector<Point>> fullContour;

    // We do a canny in order to get the contours of all the scene objects
    Mat canny_output, matInput = mat.clone();

    // We do the following operations in order to eliminate noise, but we will also remove the details
    matInput = imagePreparation::erosionImage(matInput, 2, 2);
    matInput = imagePreparation::dilationImage(matInput, 2, 2);

    Canny(matInput.clone(), canny_output, 255, 255, 3);

    findContours(canny_output, fullContour, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

//    TODO THIS IS USED TO PRINT THE Contour
//    vector<Vec4i> hierarchy;
//    namedWindow("Contours", CV_WINDOW_AUTOSIZE);
//    Mat drawing = Mat::zeros(matInput.size(), CV_8UC3);
//    for (int i = 0; i < fullContour.size(); i++) {
//        Scalar color = Scalar(RNG(12345).uniform(0, 255), RNG(12345).uniform(0, 255), RNG(12345).uniform(0, 255));
//        drawContours(drawing, fullContour, i, color, 2, 8, hierarchy, 0, Point());
//        printf("\n%d / %d", i,fullContour.size());
//        imshow("Contours", drawing);
//        waitKey(500);
//    }

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

    return pc;
}