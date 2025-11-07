#include <opencv2/opencv.hpp>
#include <omp.h>
#include <iostream>
#include <string>
#include <chrono>

using namespace cv;
using namespace std;
using namespace chrono;

void convertToGrayscale(const Mat& colorImg, Mat& grayImg) {
#pragma omp parallel for
    for (int i = 0; i < colorImg.rows; ++i) {
        for (int j = 0; j < colorImg.cols; ++j) {
            Vec3b pixel = colorImg.at<Vec3b>(i, j);
            grayImg.at<uchar>(i, j) = 0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0];
        }
    }
}

void crossFade(const Mat& colorImg, const Mat& gray3, Mat& result, double p) {
#pragma omp parallel for
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
    if (argc != 3) {
        cout << "Uso: " << argv[0] << " <ruta_imagen> <num_hilos>" << endl;
        return -1;
    }

    int num_threads = atoi(argv[2]);
    omp_set_num_threads(num_threads);

    Mat colorImg = imread(argv[1], IMREAD_COLOR);
    if (colorImg.empty()) {
        cout << "No se pudo abrir o encontrar la imagen" << endl;
        return -1;
    }

    Mat grayImg(colorImg.rows, colorImg.cols, CV_8UC1);
    Mat gray3;
    Mat result(colorImg.rows, colorImg.cols, CV_8UC3);

    auto start = high_resolution_clock::now();

    // Convertir a escala de grises
    convertToGrayscale(colorImg, grayImg);

    // convertir escala de grises a 3 canales para la mezcla
    cvtColor(grayImg, gray3, COLOR_GRAY2BGR);

    int frames = 96; // 4 segundos * 24 fps
    std::vector<Mat> results;
    results.reserve(frames);

    for (int i = 0; i < frames; ++i) {
        double p = 1.0 - (double)i / (frames - 1);
        Mat result(colorImg.rows, colorImg.cols, CV_8UC3);
        crossFade(colorImg, gray3, result, p);
        results.push_back(result);
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    cout << "Tiempo de computo de OpenMP (" << num_threads << " hilos): " << duration.count() << " ms" << endl;

    // Guardar frames (sin medir tiempo)
    for (int i = 0; i < frames; ++i) {
        string filename = "frame_openmp_" + to_string(num_threads) + "_" + to_string(i) + ".png";
        imwrite(filename, results[i]);
    }

    return 0;
}