#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace cv;
using namespace std;
using namespace chrono;

void convertToGrayscale(const Mat& colorImg, Mat& grayImg) {
    for (int i = 0; i < colorImg.rows; ++i) {
        for (int j = 0; j < colorImg.cols; ++j) {
            Vec3b pixel = colorImg.at<Vec3b>(i, j);
            grayImg.at<uchar>(i, j) = 0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0];
        }
    }
}

void crossFade(const Mat& colorImg, const Mat& gray3, Mat& result, double p) {
    result = Mat(colorImg.rows, colorImg.cols, CV_8UC3);
    for (int i = 0; i < colorImg.rows; ++i) {
        for (int j = 0; j < colorImg.cols; ++j) {
            Vec3b colorPixel = colorImg.at<Vec3b>(i, j);
            Vec3b grayPixel = gray3.at<Vec3b>(i, j);
            result.at<Vec3b>(i, j) = Vec3b(
                saturate_cast<uchar>(colorPixel[0] * p + grayPixel[0] * (1 - p)),
                saturate_cast<uchar>(colorPixel[1] * p + grayPixel[1] * (1 - p)),
                saturate_cast<uchar>(colorPixel[2] * p + grayPixel[2] * (1 - p))
            );
        }
    }
}

int main(int argc, char** argv) {
    cout << fixed << setprecision(3);

    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <image_path>" << endl;
        return -1;
    }

    Mat colorImg = imread(argv[1], IMREAD_COLOR);
    if (colorImg.empty()) {
        cout << "Could not open or find the image" << endl;
        return -1;
    }

    // Convertir a escala de grises
    Mat grayImg(colorImg.rows, colorImg.cols, CV_8UC1);
    convertToGrayscale(colorImg, grayImg);

    // convertir escala de grises a 3 canales para la mezcla
    Mat gray3;
    cvtColor(grayImg, gray3, COLOR_GRAY2BGR);

    int frames = 96; // 4 seconds * 24 fps
    Mat result(colorImg.rows, colorImg.cols, CV_8UC3); // Asignar memoria una sola vez
    auto start = high_resolution_clock::now();

    for (int i = 0; i < frames; ++i) {
        double p = 1.0 - (double)i / (frames - 1);

        // Llamar a la función secuencial para hacer el cross-fade
        crossFade(colorImg, gray3, result, p);

        stringstream ss;
        ss << "frame_sequential_" << setw(4) << setfill('0') << i << ".png";
        string filename = ss.str();
        imwrite(filename, result);

        // Debug: print progress every 10 frames
        if ((i + 1) % 10 == 0) {
            cout << "Generated frame " << i << " (p = " << fixed << setprecision(3) << p << ")" << endl;
        }
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    cout << "Tiempo de ejecucion secuencial: " << duration.count() << " ms" << endl;

    return 0;
}