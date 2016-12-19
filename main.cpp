
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <sstream>
#include <iomanip>

#include <getopt.h>
#include <time.h>

using namespace std;

string getTimeStamp(const std::string& space = "_") {

    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    ostringstream os;
    os << setw(2);
    os << setfill('0');
    os << timeinfo->tm_year + 1900 << "";
    os << timeinfo->tm_mon + 1 << "";
    os << timeinfo->tm_mday << space;
    os << timeinfo->tm_hour << "";
    os << timeinfo->tm_min << "";
    os << timeinfo->tm_sec;
    return os.str();
}

int markMotion(const cv::Mat& motion_frame, cv::Mat& org_frame) {
    int number_of_changes = 0;
    int y_start = 0, y_stop = motion_frame.rows;
    int x_start = 0, x_stop = motion_frame.cols;

    int min_x = motion_frame.cols, max_x = 0;
    int min_y = motion_frame.rows, max_y = 0;

    for(int j = y_start; j < y_stop; j+=2) {
        for(int i = x_start; i < x_stop; i+=2) {
            if(motion_frame.at<uchar>(j,i) == 255) {
                number_of_changes++;
                if(min_x>i) min_x = i;
                if(max_x<i) max_x = i;
                if(min_y>j) min_y = j;
                if(max_y<j) max_y = j;
            }
        }
    }

    if(number_of_changes) {
        min_x = std::max(min_x - 10, 0);
        min_y = std::max(min_y - 10, 0);
        max_x = std::min(max_x + 10, motion_frame.cols - 1);
        max_y = std::min(max_y + 10, motion_frame.rows - 1);

        cv::Rect rect(cv::Point(min_x, min_y), cv::Point(max_x, max_y));
        cv::rectangle(org_frame, rect, cv::Scalar(0, 255, 255), 1);
    }

    return number_of_changes;
}

int cameraMotion(const std::string& input,
                 double deviation,
                 const std::string& out_dir,
                 int timeout) {

    cv::VideoCapture camera;

    if(!camera.open(input)) {
        cerr << "Failed to open camera" << endl;
        return EXIT_FAILURE;
    }

    cv::Mat a_frame, b_frame, c_frame, d1_frame, d2_frame, motion_frame, b_org_frame;
    camera >> a_frame;
    camera >> b_frame;
    camera >> c_frame;

    b_org_frame = b_frame;

    cv::cvtColor(a_frame, a_frame, CV_RGB2GRAY);
    cv::cvtColor(b_frame, b_frame, CV_RGB2GRAY);
    cv::cvtColor(c_frame, c_frame, CV_RGB2GRAY);

    int frame_no = 0;

    int64 start_tm = cv::getTickCount(), now_tm;

    while(camera.grab()) {

        ++frame_no;

        a_frame = b_frame;
        b_frame = c_frame;

        cv::Mat tmp_frame;
        if(!camera.retrieve(tmp_frame)) {
            cerr << "#" << frame_no << " Failed to retrieve frame #" << frame_no << endl;
        }

        cout << getTimeStamp() << " recorded " << frame_no << " frames\r";

        tmp_frame.copyTo(c_frame);
        cv::cvtColor(c_frame, c_frame, CV_RGB2GRAY);

        cv::absdiff(c_frame, b_frame, d1_frame);
        cv::absdiff(b_frame, a_frame, d2_frame);
        cv::bitwise_and(d1_frame, d2_frame, motion_frame);

        cv::threshold(motion_frame, motion_frame, 35, 255, CV_THRESH_BINARY);

        cv::Mat kernel_ero = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2,2));
        cv::erode(motion_frame, motion_frame, kernel_ero);

        cv::Scalar mean, stddev;
        cv::meanStdDev(motion_frame, mean, stddev);

        if(stddev[0] >= deviation) {
            int n_changes = markMotion(motion_frame, b_org_frame);

            ostringstream os;
            os << out_dir << "/";
            os << "motion_" << getTimeStamp() << ".jpg";
            cv::imwrite(os.str(), b_org_frame);

            cout << endl;
            cout << "Detected motion - Deviation: " << stddev[0] << ". Stored " << os.str() << endl;
        }

        b_org_frame = tmp_frame;

        now_tm = cv::getTickCount();
        int64 duration = now_tm - start_tm;
        duration /= 1000000000;
        if(int(duration) >= timeout) {
            cout << endl;
            cout << "Recording ended after " << duration << " seconds." << endl;
            break;
        }
    }
}

int main(int argc, char** argv) {

    int c;
    int digit_optind = 0;

    string input, output_dir;
    int timeout = 0;
    double deviation = 3.0;

    while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"deviation", required_argument, 0, 'd' },
            {"input", required_argument, 0, 'i' },
            {"output", required_argument, 0, 'o' },
            {"timeout", required_argument, 0, 't' },
            {0, 0, 0, 0 }
        };

        c = getopt_long(argc, argv, "d:i:o:t:",
                        long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
        case 0 :
            cout << "0: " << optarg << endl;
            break;
        case 'd' :
            deviation = strtod(optarg, NULL);
            break;
        case 'i' :
            input = optarg;
            break;
        case 'o' :
            output_dir = optarg;
            break;
        case 't' :
            timeout = atoi(optarg);
            break;
        }
    }

    if(input.size()) {
        cameraMotion(input, deviation, output_dir, timeout);
    }
}
