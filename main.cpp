#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>

#include <iostream>
#include <sstream>
#include <iomanip>

#include <getopt.h>
#include <unistd.h>
#include <time.h>

using namespace std;

string getTimeStamp(const std::string& space = "_") {

    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    ostringstream os;
    os << timeinfo->tm_year + 1900 << "";
    os << setfill('0') << setw(2) << timeinfo->tm_mon + 1 << "";
    os << setfill('0') << setw(2) << timeinfo->tm_mday << space;
    os << setfill('0') << setw(2) << timeinfo->tm_hour << "";
    os << setfill('0') << setw(2) << timeinfo->tm_min << "";
    os << setfill('0') << setw(2) << timeinfo->tm_sec;
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
                 int timeout,
                 const std::string& prefix) {

    cv::VideoCapture camera;

    cv::Mat frame,
        fgMaskMOG2,
        motion_frame;

    cv::Ptr<cv::BackgroundSubtractorMOG2> pMOG2 = cv::createBackgroundSubtractorMOG2();
    
    pMOG2->setShadowValue(0);
    
    int frame_no = 0;

    int64 start_tm = cv::getTickCount();
    int64 duration = 0;

    while(int(duration) <= timeout) {

        if(!camera.isOpened()) {

            if(!camera.open(input)) {
                cerr << "Failed to open camera" << endl;
                sleep(1);
                continue;
            }
        }

        if(!camera.grab()) {
            camera.release();
            cerr << "Failed to get frame" << endl;
            sleep(1);
            continue;
        }

        ++frame_no;

        if(!camera.retrieve(frame)) {
            cerr << "#" << frame_no << " Failed to retrieve frame #" << frame_no << endl;
            camera.release();
            sleep(1);
            continue;
        }

        /* update the background model */
        pMOG2->apply(frame, fgMaskMOG2);

        ostringstream os1, os2, os3;
        os1 << out_dir << "/";
        os1 << frame_no << "_org_" << getTimeStamp() << ".jpg";
        cv::imwrite(os1.str(), frame);

        os2 << out_dir << "/";
        os2 << frame_no << "_fg_" << getTimeStamp() << ".jpg";
        cv::imwrite(os2.str(), fgMaskMOG2);

        motion_frame = fgMaskMOG2;
        
        // cv::threshold(motion_frame, motion_frame, 35, 255, CV_THRESH_BINARY);
        // cv::Mat kernel_ero = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2,2));
        // cv::erode(motion_frame, motion_frame, kernel_ero);

        os3 << out_dir << "/";
        os3 << frame_no << "_erode_" << getTimeStamp() << ".jpg";
        cv::imwrite(os3.str(), motion_frame);

        cv::Scalar mean, stddev;
        cv::meanStdDev(motion_frame, mean, stddev);

        cout << getTimeStamp() << " recorded " << frame_no << " frames. Deviation is " << stddev[0] << "." << endl;

        if(stddev[0] >= deviation) {
            int n_changes = markMotion(motion_frame, frame);

            ostringstream os;
            os << out_dir << "/";
            os << frame_no << "_" << prefix << getTimeStamp() << ".jpg";
            cv::imwrite(os.str(), frame);

            cout << "Detected motion - Deviation: " << stddev[0] << ". Stored " << os.str() << endl;
        }

        duration = cv::getTickCount() - start_tm;
        duration /= 1000000000;
    }

    cout << "Recording ended after " << duration << " seconds." << endl;

    return EXIT_FAILURE;
}

void printUsage() {
    cout << "-d, --deviation ARG" << endl;
    cout << "\t" << "Standard deviation" << endl;
    cout << "-i, --input ARG" << endl;
    cout << "\t" << "URL to read as input." << endl;
    cout << "-o, --output" << endl;
    cout << "\t" << "Output directory for motion pictures" << endl;
    cout << "-p, --prefix" << endl;
    cout << "\t" << "File name prefix" << endl;
    cout << "-t, --timeout" << endl;
    cout << "\t" << "Duration in seconds" << endl;
}

int main(int argc, char** argv) {

    int c;
    int digit_optind = 0;

    string input, output_dir = ".", prefix = "motion_";
    int timeout = 0;
    double deviation = 3.0;

    while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"deviation", required_argument, 0, 'd' },
            {"input", required_argument, 0, 'i' },
            {"output", required_argument, 0, 'o' },
            {"prefix", required_argument, 0, 'p' },
            {"timeout", required_argument, 0, 't' },
            {0, 0, 0, 0 }
        };

        c = getopt_long(argc, argv, "d:i:o:p:t:",
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
        case 'p' :
            prefix = optarg;
            break;
        case 't' :
            timeout = atoi(optarg);
            break;
        }
    }

    if(input.size()) {
        return cameraMotion(input, deviation, output_dir, timeout, prefix);
    }
    else {
        printUsage();
    }
    return 0;
}
