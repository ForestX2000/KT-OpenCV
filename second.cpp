    //
    // Created by Acer on 12.11.2026.
    //

    #include "second.h"

    using namespace cv;
    using namespace std;

    #include <iostream>
    #include <ctime>


    void image(Mat *src, Mat *overlay, const Point &location) {
        for (int y = max(location.y, 0); y < src->rows; ++y) {
            int fY = y - location.y;
            if (fY >= overlay->rows) break;
            for (int x = max(location.x, 0); x < src->cols; ++x) {
                int fX = x - location.x;
                if (fX >= overlay->cols) break;
                double opacity = ((double)overlay->at<Vec4b>(fY, fX)[3]) / 255.0;
                if (opacity > 0) {
                    for (int c = 0; c < 3; ++c) {
                        src->at<Vec3b>(y, x)[c] = (uchar)(overlay->at<Vec4b>(fY, fX)[c] * opacity + src->at<Vec3b>(y, x)[c] * (1.0 - opacity));
                    }
                }
            }
        }
    }

    void addClock(Mat &img, int margin) {
        time_t now = time(0);
        tm *ltm = localtime(&now);
        char timeStr[100];
        sprintf(timeStr, "%02d:%02d:%02d", ltm->tm_hour, ltm->tm_min, ltm->tm_sec);

        putText(img, timeStr, Point(margin + 2, margin + 22), FONT_HERSHEY_COMPLEX, 0.7, Scalar(0, 0, 0), 2, LINE_AA);
        putText(img, timeStr, Point(margin, margin + 20), FONT_HERSHEY_COMPLEX, 0.7, Scalar(0, 255, 0), 1, LINE_AA);
    }

    int main() {
        VideoCapture cap(0);

        Mat logo = imread("logo.png", IMREAD_UNCHANGED);
        Mat caption = imread("caption.png", IMREAD_UNCHANGED);



        bool showLogo = false, showCaption = false, showClock = true;
        Mat frame;

        int margin = 20;

        Mat logoSmall;
        double scale = min(160.0 / logo.cols, 160.0 / logo.rows);
        resize(logo, logoSmall, Size(), scale, scale, INTER_AREA);

        while (true) {
            cap >> frame;
            if (frame.empty()) break;

            if (showLogo && !logoSmall.empty()) {
                int logoX = frame.cols - logoSmall.cols - margin;
                int logoY = margin;
                image(&frame, &logoSmall, Point(logoX, logoY));
            }


            if (showCaption && !caption.empty()) {
                int capX = margin;
                int capY = frame.rows - caption.rows - margin;
                image(&frame, &caption, Point(capX, capY));
            }

            if (showClock) {
                addClock(frame, margin);
            }

            imshow("Camera", frame);
            int key = waitKey(30);

            if (key == 27) break;
            if (key == 'l') showLogo = !showLogo;
            if (key == 'c') showCaption = !showCaption;
            if (key == 't') showClock = !showClock;
        }

        cap.release();
        destroyAllWindows();
        return 0;
    }