#include "first.h"
#include <opencv2/opencv.hpp>
#include <iomanip>
#include <sstream>

using namespace cv;
using namespace std;

void overlayImage(Mat & dst, const Mat & src, const Point& pos) {
    if (src.empty() || dst.empty()) return;

    Rect roiRect(pos, src.size());
    if (roiRect.x < 0 || roiRect.y < 0 ||
        roiRect.br().x > dst.cols || roiRect.br().y > dst.rows) return;

    Mat roi = dst(roiRect);

    if (src.channels() == 4) {
        vector<Mat> srcChannels(4), roiChannels(3);
        split(src, srcChannels);
        split(roi, roiChannels);

        Mat alpha;
        srcChannels[3].convertTo(alpha, CV_32F, 1.0/255.0);

        for (int c = 0; c < 3; ++c) {
            Mat bg, fg;
            roiChannels[c].convertTo(bg, CV_32F);
            srcChannels[c].convertTo(fg, CV_32F);

            Mat result = fg.mul(alpha) + bg.mul(1.0 - alpha);
            result.convertTo(roiChannels[c], CV_8U);
        }
        merge(roiChannels, roi);
    } else {
        src.copyTo(roi);
    }
}

static string getTimeStr() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    ostringstream oss;
    oss << setfill('0') << setw(2) << ltm->tm_hour << ":"
        << setw(2) << ltm->tm_min << ":"
        << setw(2) << ltm->tm_sec;
    return oss.str();
}

int main() {
    VideoCapture cap(0);
    if (!cap.isOpened()) return -1;

    Mat logo, caption;
    imread("logo.png", IMREAD_UNCHANGED).copyTo(logo);
    imread("caption.png", IMREAD_UNCHANGED).copyTo(caption);

    Mat logoSmall;
    if (!logo.empty()) {
        double scale = min(160.0/logo.cols, 160.0/logo.rows);
        resize(logo, logoSmall, Size(), scale, scale, INTER_AREA);
    }

    bool flags[3] = {false, false, true};
    const char keys[] = {'l', 'c', 't'};
    const int margin = 20;
    Mat frame;

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        if (flags[0] && !logoSmall.empty())
            overlayImage(frame, logoSmall, Point(frame.cols - logoSmall.cols - margin, margin));

        if (flags[1] && !caption.empty())
            overlayImage(frame, caption, Point(margin, frame.rows - caption.rows - margin));

        if (flags[2]) {
            string timeStr = getTimeStr();
            putText(frame, timeStr, Point(margin+2, margin+22), FONT_HERSHEY_COMPLEX, 0.7, Scalar(0,0,0), 2, LINE_AA);
            putText(frame, timeStr, Point(margin, margin+20), FONT_HERSHEY_COMPLEX, 0.7, Scalar(0,255,0), 1, LINE_AA);
        }

        imshow("Camera", frame);

        int key = waitKey(30);
        if (key == 27) break;
        for (int i = 0; i < 3; ++i)
            if (key == keys[i]) flags[i] = !flags[i];
    }

    return 0;
}