#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// function prototyping
Mat to442_grayscale(cv::Mat frame);
Mat to442_sobel(Mat gray_frame);

// main function
int main() {

    // finding file path and reading it
    string vid_path = "mondkey_vid2.mp4";
    
    VideoCapture videoCapture(vid_path);

    if(!videoCapture.isOpened()) {

        cout << "Error: Unable to open vid file" << endl;
        return -1;

    } 

    int framecount = 0;
    double startTime = double(getTickCount());

    while (true) {

        Mat frame;
        bool image_read = videoCapture.read(frame);

        if(!image_read) {
            break;
        }

        Mat gray_frame = to442_grayscale(frame);
        Mat sobel_frame = to442_sobel(gray_frame);

        framecount++;
        double currentTime = double(getTickCount());
        double elapsedTime = (currentTime - startTime) / getTickFrequency();
        double fps = framecount / elapsedTime;

        //cout << to_string(fps) << endl;

        putText(sobel_frame, "FPS: " + to_string(fps), Point(10, 30), QT_FONT_NORMAL, 1, Scalar(255, 0, 0), 2);

        imshow("Frame", sobel_frame);

        if (waitKey(25) == 'q'){
            break;
        } 
    }
}

// applying grayscale manually to one frame
Mat to442_grayscale(Mat frame) {
    
    int rows = frame.rows;
    int cols = frame.cols;

    // BGR 
    Mat gray_scale_im(rows, cols, CV_8UC1);

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            // blue pixel value
            uint8_t pixel_value_blue = frame.at<Vec3b>(i,j)[0];
            // green pixel value
            uint8_t pixel_value_green = frame.at<Vec3b>(i,j)[1];
            // red pixel value
            uint8_t pixel_value_red = frame.at<Vec3b>(i,j)[2];
            // finding gray pixel value
            uint8_t gray_pix = (0.212 * pixel_value_red + 0.7152 * pixel_value_green + 0.0722 * pixel_value_blue);
            // creating gray scale image
            gray_scale_im.at<uchar>(i,j) = static_cast<uchar>(gray_pix);
        }
    }

    return gray_scale_im;
}

// applying sobel filter manually to one grayscaled frame
Mat to442_sobel(Mat gray_frame) {

    int rows = gray_frame.rows;
    int cols = gray_frame.cols;
    
    // creating sobel kernel
    int sobel_kernel[6] = {-1, -2, -1, 1, 2, 1};

    Mat sobel_im(rows - 2, cols - 2, CV_8UC1);

    for(int i = 1; i < rows - 1; i++) {
        for(int j = 1; j < cols - 1; j++) {
            // initializing Gx and Gy to 0 for each pixel 
            int Gx = 0;
            int Gy = 0;

            // iterating through rows in kernel
            int kernel_index = 0;
            
            // finding Gx kernel (x direction, left to right)
            for(int k = -1; k < 2; k += 2) {
                // iterating through columns in kernel
                for(int l = -1; l < 2; l++) {
                    // adding value to kernel
                    Gx += gray_frame.at<uchar>(i + k,j + l) * sobel_kernel[kernel_index];
                    kernel_index++;
                }
            }
            // resetting kernel index
            kernel_index = 0;

            // finding Gy kernel
            // iterating through columns in Gy kernel
            for(int k = -1; k < 2; k+=2) {
                // iterating through rows in Gy kernel
                for(int l = -1; l < 2; l ++) {
                    Gy += gray_frame.at<uchar>(i + l, j + k) * sobel_kernel[kernel_index];
                    kernel_index++;
                }
            }
            // taking abs and finding Gx + Gy
            uchar pixel_value = abs(Gy) + abs(Gx);

            // writing value to pixel
            sobel_im.at<uchar>(i - 1, j - 1) = pixel_value;
        }
    }

    return sobel_im;
}

