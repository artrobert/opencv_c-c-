#include <opencv/cv.hpp>

using namespace cv;
using namespace std;

string pathRoot = "D:\\Facultate\\c++Project\\opencv_c-c-\\edgeDetection\\database\\";
string folderRoot = "attempt7\\";
string pawnPiece = "pion";
string knightPiece = "cal";
string rookPiece = "tura";
string bishopPiece = "nebun";
string queenPiece = "regina";
string kingPiece = "rege";

string jpg_extension = ".jpg";
string result_format = "result_";
string piece_format = "piece_";
string no_piece_format = "piece_n_";

Size imageResizeSize(500, 500);

void loadPawns();

void loadKnights();

void loadBishops();

void loadRooks();

void loadQueen();

void loadKing();

void loadPiece(int maxPieceNumber, string pieceRoot);

void loadSampleImages() {
    loadPawns();
    loadKnights();
    loadBishops();
    loadRooks();
    loadQueen();
    loadKing();
}

void loadPiece(string pieceRoot,vector<vector<Point>> pieceContour) {
    stringstream aux;
    int maxTries = 3; // if we find a image that has cols/rows = 0 (doesn't exist) we exit
    int i = 0;
    while (i < 99 && maxTries != 0)
        aux << i;
    string pieceFullPath =
            pathRoot + folderRoot + pieceRoot + aux.str() + jpg_extension; //created "folderPath/pion_##.jpg"
    Mat img = imread(pieceFullPath, 0);
    if (img.rows == 0 || img.cols == 0) { //if the image doesn't exist
        maxTries--;
        i++;
    }else{
        resize(img,img,imageResizeSize);
        vector<Point> contour;
        getContour(img,contour);
    }
}

void simpleContour(const Mat &mat) {
    vector<vector<Point>> bigcontour;

    vector<Point> c;

    findContours(mat, bigcontour, CV_RETR_TREE, CHAIN_APPROX_SIMPLE);

    int bigContourSize = bigcontour.size();
    for (size_t border = 0; border < bigContourSize; border++) {
        int pointSize = bigcontour[border].size();
        for (size_t point = 0; point < pointSize; point++) {
            c.push_back(bigcontour[border][point]);
        }
    }

    int n = 300; // number of ok points
    // add dummy data
    int dummy = 0;
}
