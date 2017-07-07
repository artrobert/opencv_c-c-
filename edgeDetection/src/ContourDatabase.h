//
// Created by artin on 06/7/2017.
//

#ifndef EDGEDETECTION_CONTOURDATABASE_H
#define EDGEDETECTION_CONTOURDATABASE_H

#include <opencv2/core/types_c.h>
#include <opencv2/shape.hpp>
#include "../models/PieceModel.h"

namespace cdb {

    using namespace cv;
    using namespace std;


    class ContourDatabase {

    public:

        string pawnText = "pion";
        string knightText = "cal";
        string rookText = "tura";
        string bishopText = "nebun";
        string reginaText = "regina";
        string kingText = "rege";

        const string jpg_extension = ".jpg";
        const string piece_format = "\\piece_";
        const string no_piece_format = "\\piece_n_";
        const string result_format = "_result_";

        static const int maxLayers = 4;

        static const CvSize imageResizeSize;

        static const int imageResizeHeight = 800;
        static const int imageResizeWidth = 600;

        // The closest to the camera and the most harder to find
        static const int layer0 = imageResizeHeight / maxLayers * 1;
        static const int layer1 = imageResizeHeight / maxLayers * 2;
        static const int layer2 = imageResizeHeight / maxLayers * 3;
        static const int layer3 = imageResizeHeight / maxLayers * 4;

        // Boolean indicated that the samples are fully loaded
        bool areSamplesLoaded = false;

        // Object used to compare the contours
        cv::Ptr<ShapeContextDistanceExtractor> mysc;

        // The path to the location of the piece folders
        string pathRoot;
        // The folder of the piece
        string folderRoot;

        // These will contain info about the PIECE TYPE and POSSIBLE CONTOURS
        PieceModel pawn;
        PieceModel rook;
        PieceModel bishop;
        PieceModel knight;
        PieceModel queen;
        PieceModel king;

        vector<PieceModel> allPieces;

        // Constructor
        ContourDatabase();

        // If there is no database of images (binary mask images) CALL THIS FIRST
        void createDatabase();

        void createSamples(string pieceName);

        // Call this to load the contours
        void loadSampleImages();

        PieceModel generatePieceModel(string pieceRoot);

        void loadPiece(PieceModel &piece, PieceType type, string pieceText);

        PieceType matchChessPieces(PieceContour pieceIncoming);

        // This will do a bakcgroun subtraction between two images and will return the binary mask
        cv::Mat doSubtraction(cv::Mat &withoutImg, cv::Mat &withImg);

        // This will return the contour
        PieceContour getContourFromMat(const Mat &mat);

        // Given the base point of a piece, it will determine its layer
        int findOriginLayer(Point basePoint);
    };

    ContourDatabase::ContourDatabase() {
        mysc = createShapeContextDistanceExtractor();
    }
}
#endif //EDGEDETECTION_CONTOURDATABASE_H
