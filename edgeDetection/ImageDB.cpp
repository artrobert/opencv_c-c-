#include <opencv/cv.hpp>
#include <opencv2/shape.hpp>
#include "ImageBasicOperations.h"
#include "ImageDB.h"

using namespace cv;
using namespace std;

enum pieceType {
    pion = 1,
    cal = 2,
    tura = 3,
    nebun = 4,
    regina = 5,
    rege = 6,
    unknown = -1
};

typedef struct SampleContour {
    vector<Point> contour;
} SampleContour;

typedef struct ChessPiece {
    pieceType type;
    vector<SampleContour> pieceContours;
} ChessPiece;

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

Size imageResizeSize(800, 600);

vector<ChessPiece> allPieces;

cv::Ptr<cv::ShapeContextDistanceExtractor> mysc;

void loadPiece(pieceType type, string pieceText);

void loadPieceContour(string pieceRoot, vector<vector<Point>> &pieceContours);

void getContourFromMatrice(const Mat &mat, vector<Point> &c);

cv::Mat doSubtraction(cv::Mat &withoutImg, cv::Mat &withImg);

/** HEADERS **/

void ImageDB::loadSampleImages() {
    mysc = cv::createShapeContextDistanceExtractor();
    loadPiece(pion, pionText); //pt. pion
//    loadPiece(cal, calText); //pt. cal
//    loadPiece(tura, turaText); //pt. tura
//    loadPiece(nebun, nebunText); //pt. nebun
//    loadPiece(regina, reginaText); //pt. regina
//    loadPiece(rege, regeText); //pt. rege
}

void loadPieceContour(string pieceRoot, vector<SampleContour> &pieceContours) {
    int maxTries = 3; // if we find a image that has cols/rows = 0 (doesn't exist) we exit
    int i = 0;
    while (i < 99 && maxTries != 0) {
        stringstream aux;
        if (i < 10) {
            aux << 0; //add 0 in front to have the _## format
        }
        aux << i;
        //created "folderPath/pion_result_##.jpg"
        string pieceFullPath = pathRoot + folderRoot +"//"+ pieceRoot + result_format + aux.str() + jpg_extension;
        Mat img = imread(pieceFullPath, 0);
        if (img.rows == 0 || img.cols == 0) { //if the image doesn't exist
            maxTries--;
        } else {
            resize(img, img, imageResizeSize);
            SampleContour sampleContour;
            ImageDB::getContourFromMat(img, sampleContour.contour);
            pieceContours.push_back(sampleContour);
        }
        i++;
    }
}

void ImageDB::getContourFromMat(const Mat &mat, vector<Point> &c) {
    vector<vector<Point>> fullContour;

    findContours(mat, fullContour, CV_RETR_TREE, CHAIN_APPROX_SIMPLE);

    int bigContourSize = fullContour.size();

    for (size_t border = 0; border < bigContourSize; border++) {
        int pointSize = fullContour[border].size();
        for (size_t point = 0; point < pointSize; point++) {
            c.push_back(fullContour[border][point]);
        }
    }
}

void loadPiece(pieceType type, string pieceText) {
    ChessPiece piece;
    piece.type = type;
    loadPieceContour(pieceText, piece.pieceContours);
    allPieces.push_back(piece);
    printf("\n\n Successfully loaded <%s>", pieceText.c_str());
}

int ImageDB::matchChessPieces(vector<Point> incomingContour) {
    float bestDis = FLT_MAX;
    pieceType pieceType = unknown;

    //iterate through all of the 6 pieces
    for (size_t i = 0; i < allPieces.size(); i++) {
        ChessPiece currentPiece = allPieces[i];
        size_t pieceContoursSize = currentPiece.pieceContours.size();
        float averageDistance = 0;
        // iterate through all the sample contours of one piece and compute the distances
        for (int j = 0; j < pieceContoursSize; j++) {
            SampleContour sampleContour = currentPiece.pieceContours[j];
            float distanceFound = mysc->computeDistance(incomingContour, sampleContour.contour);
            averageDistance += distanceFound;
        }

        averageDistance /= pieceContoursSize; //get the average contour distance after we calculated for all the pieces

        if (averageDistance < bestDis) {
            bestDis = averageDistance;
            pieceType = currentPiece.type;
        }
    }

    return pieceType;
}

/** Create sample images **/

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
            resize(withoutPiece, withoutPiece, imageResizeSize);
            resize(withPiece, withPiece, imageResizeSize);
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
    maskResult = imagePreparation::erosionImage(maskResult, 2, 1); // make a erosion to eliminate the noise
    return maskResult;
}
