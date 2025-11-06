#include <opencv2/opencv.hpp>
#include <mpi.h>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>

using namespace cv;
using namespace std;
using namespace chrono;

void convertToGrayscale(const Mat& colorImg, Mat& grayImg, int startRow, int endRow) {
    for (int i = startRow; i < endRow; ++i) {
        for (int j = 0; j < colorImg.cols; ++j) {
            Vec3b pixel = colorImg.at<Vec3b>(i, j);
            grayImg.at<uchar>(i, j) = 0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0];
        }
    }
}

void crossFade(const Mat& colorImg, const Mat& gray3, Mat& result, int startRow, int endRow, double p) {
    for (int i = startRow; i < endRow; ++i) {
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
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if (rank == 0) cout << "Usage: mpirun -np <num_processes> " << argv[0] << " <image_path>" << endl;
        MPI_Finalize();
        return -1;
    }

    Mat colorImg, grayImg, gray3;
    int rows, cols;

    if (rank == 0) {
        colorImg = imread(argv[1], IMREAD_COLOR);
        if (colorImg.empty()) {
            cout << "Could not open or find the image" << endl;
            MPI_Finalize();
            return -1;
        }
        rows = colorImg.rows;
        cols = colorImg.cols;
        grayImg = Mat(rows, cols, CV_8UC1);
        gray3 = Mat(rows, cols, CV_8UC3);
    }

    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Allocate space for images in non-root processes
    if (rank != 0) {
        colorImg = Mat(rows, cols, CV_8UC3);
        grayImg = Mat(rows, cols, CV_8UC1);
        gray3 = Mat(rows, cols, CV_8UC3);
    }

    // Broadcast color image
    MPI_Bcast(colorImg.data, rows * cols * 3, MPI_BYTE, 0, MPI_COMM_WORLD);

    // Parallel grayscale conversion
    int rowsPerProcess = rows / size;
    int startRow = rank * rowsPerProcess;
    int endRow = (rank == size - 1) ? rows : startRow + rowsPerProcess;

    convertToGrayscale(colorImg, grayImg, startRow, endRow);

    // Gather grayscale results
    if (rank == 0) {
        MPI_Gather(MPI_IN_PLACE, (endRow - startRow) * cols, MPI_BYTE,
                   grayImg.data, (endRow - startRow) * cols, MPI_BYTE, 0, MPI_COMM_WORLD);
    } else {
        MPI_Gather(grayImg.data + startRow * cols, (endRow - startRow) * cols, MPI_BYTE,
                   nullptr, 0, MPI_BYTE, 0, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        cvtColor(grayImg, gray3, COLOR_GRAY2BGR);
    }

    // Broadcast the 3-channel grayscale image
    MPI_Bcast(gray3.data, rows * cols * 3, MPI_BYTE, 0, MPI_COMM_WORLD);

    int frames = 96;
    auto start = high_resolution_clock::now();

    for (int i = 0; i < frames; ++i) {
        double p = 1.0 - (double)i / (frames - 1);
        Mat result(rows, cols, CV_8UC3);

        crossFade(colorImg, gray3, result, startRow, endRow, p);

        if (rank == 0) {
            MPI_Gather(MPI_IN_PLACE, (endRow - startRow) * cols * 3, MPI_BYTE,
                       result.data, (endRow - startRow) * cols * 3, MPI_BYTE, 0, MPI_COMM_WORLD);
        } else {
            MPI_Gather(result.data + startRow * cols * 3, (endRow - startRow) * cols * 3, MPI_BYTE,
                       nullptr, 0, MPI_BYTE, 0, MPI_COMM_WORLD);
        }

        if (rank == 0) {
            string filename = "frame_mpi_" + to_string(i) + ".png";
            imwrite(filename, result);
        }
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    if (rank == 0) cout << "Tiempo de ejecucion de MPI: " << duration.count() << " ms" << endl;

    MPI_Finalize();
    return 0;
}