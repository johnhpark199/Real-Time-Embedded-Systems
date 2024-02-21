#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <pthread.h>

using namespace cv;
using namespace std;

// function prototyping
Mat to442_grayscale(cv::Mat frame);
Mat to442_sobel(Mat gray_frame);
void *filter_runner(void* struct_in);
void createQuadrants(const Mat videoFrame, Mat frameQuadrant[4]);

// multithreading struct
struct quad_mat {
    // input image quadrant
    Mat quad_in;
    // output mat quadrant
    Mat quad_out;
};

// main function
int main() {

    // finding file path and reading it
    string vid_path = "big-buck-bunny-480p-30sec.mp4";
    
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

        // creating list of structs where image quarters will be stored
        // setting up pthread args
        struct quad_mat im_quarters[4];
        pthread_t tids[4];
        Mat subImages[4];
        // vector of subimages 
        // vector<Mat> subImages;
        
        createQuadrants(frame, subImages);
        for(int i = 0; i < 4; i++) {
            im_quarters[i].quad_in = subImages[i];
        }
        
        // creating threads
        pthread_create(&tids[0], NULL, filter_runner, &im_quarters[0]);
        pthread_create(&tids[1], NULL, filter_runner, &im_quarters[1]);    
        pthread_create(&tids[2], NULL, filter_runner, &im_quarters[2]); 
        pthread_create(&tids[3], NULL, filter_runner, &im_quarters[3]); 

        // waiting for first two threads to end and begin joining
        for (int i = 0; i < 4; i++) {
            pthread_join(tids[i], NULL);
        }
        Mat vcat;
        vconcat(im_quarters[0].quad_out, im_quarters[1].quad_out, vcat);
        vconcat(vcat, im_quarters[2].quad_out, vcat);
        vconcat(vcat, im_quarters[3].quad_out, vcat);

        // showing image
        imshow("normal_im", vcat);
        
        // increment framecount
        framecount++;
        

        // "q to exit"
        if (waitKey(25) == 'q'){
            // dipslaying average fps
            double currentTime = double(getTickCount());
            double elapsedTime = (currentTime - startTime) / getTickFrequency();
            double fps = framecount / elapsedTime;
            cout << to_string(fps) << endl;

            break;
        } 
    }
}

// creating quadrants
void createQuadrants(const Mat videoFrame, Mat frameQuadrant[4]) {
    int heightFirstFourth = videoFrame.rows / 4;
    int heightSecondFourth = heightFirstFourth * 2;
    int heightThirdFourth = heightFirstFourth * 3;
    int heightEnd = videoFrame.rows;
    int width = videoFrame.cols;

    int overlapAmount = 1;

    // Range(height, width) 
    // Top left quadrant, w/o middle height pxls and w/o width pxls
    frameQuadrant[0] = videoFrame(Range(0, heightFirstFourth+overlapAmount), Range(0, width));
    // Top right quadrant, w/o middle height pxls
    frameQuadrant[1] = videoFrame(Range(heightFirstFourth-overlapAmount, heightSecondFourth+overlapAmount), Range(0, width));
    // Bottom left quadrant, w/ middle height pxls and w/o width pxls
    frameQuadrant[2] = videoFrame(Range(heightSecondFourth-overlapAmount, heightThirdFourth+overlapAmount), Range(0, width));
    // Bottom right quadrant, w/ middle height pxls 
    frameQuadrant[3] = videoFrame(Range(heightThirdFourth-overlapAmount, heightEnd), Range(0, width));
}

// multithreading fucntion
void *filter_runner(void* struct_in) {
    
    // ACCESSING QUADRANT VIA POINTER
    struct quad_mat *quads = (struct quad_mat*) struct_in;

    // APPLYING GRAYSCALE TO QUADRANT 
    quads->quad_out = to442_grayscale(quads->quad_in);

    // APPLYING SOBEL TO QUADRANT
    quads->quad_out = to442_sobel(quads->quad_out);

    // end thread
    pthread_exit(NULL);
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
            // change pixel_value to int after multithreading working
            // taking abs and finding Gx + Gy
            uchar pixel_value = abs(Gy) + abs(Gx);

            // saturate_cast<uchar>(pixel_value) on left side of equals sine
            // writing value to pixel
            sobel_im.at<uchar>(i - 1, j - 1) = pixel_value;
        }
    }
    return sobel_im;
}