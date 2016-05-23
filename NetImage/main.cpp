#include "mainwindow.h"
#include "KernelBasedTracker.h"
#include "Histogram.h"
#include "timer.h"
#include <QApplication>

using namespace cv;
using namespace std;


int flag = 0;
Timer timer;
//car: w, s, a, d, x

const char  * WINDOW_NAME  = "Face Tracker";
const int CASCADE_NAME_LEN = 2048;
char    CASCADE_NAME[CASCADE_NAME_LEN] = "haarcascade_frontalface_alt2.xml";

void send_command(QTcpSocket& socket, char ch)
{
    if (flag == 1) {
        flag = 0;
        socket.write((char*)&ch, sizeof(char));
        socket.waitForBytesWritten();
    }
}

IplImage QImage2IplImage(QImage image)
{
    Mat mat;
    //qDebug() << image.format();
    switch(image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    default:
        break;
    }
    return IplImage(mat);
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //w.show();

    //waited for connection
    qDebug() << "Client Side";
    QTcpSocket socket;
    socket.connectToHost("192.168.8.1", 2001);
    qDebug() << "Connecting to Remote...";
    socket.waitForConnected();
    qDebug() << "Connection success.";

    timer.start();
    /*
    while (1) {
        char ch = 'e';
        socket.write((char*)&ch, sizeof(char));
        socket.waitForBytesWritten();

        QThread::sleep(60);
    }*/
    //first we stop the car
    send_command(socket, 'x');



    //Classifier Initialize
    const int scale = 2;

    // create all necessary instances
    cvNamedWindow (WINDOW_NAME, CV_WINDOW_AUTOSIZE);
    //CvCapture * camera = cvCreateFileCapture("walking_man_fixed_scale.mpeg");//cvCreateCameraCapture (CV_CAP_ANY);
    CvHaarClassifierCascade* cascade = (CvHaarClassifierCascade*) cvLoad (CASCADE_NAME, 0, 0, 0);
    CvMemStorage* storage = cvCreateMemStorage(0);
    assert (storage);


    // did we load the cascade?!?
    if (! cascade)
        abort ();

    // get an initial frame and duplicate it for later work

    IplImage pImg = QImage2IplImage(w.getImage());
    IplImage *  current_frame = (IplImage*)&pImg;

    IplImage *  draw_image    = cvCreateImage(cvSize (current_frame->width, current_frame->height), IPL_DEPTH_8U, 4);
    IplImage *  gray_image    = cvCreateImage(cvSize (current_frame->width, current_frame->height), IPL_DEPTH_8U, 1);
    IplImage *  small_image   = cvCreateImage(cvSize (current_frame->width / scale, current_frame->height / scale), IPL_DEPTH_8U, 1);
    assert (current_frame && gray_image && draw_image && small_image);

    Mat orig_img = imread("psb.png");

    Mat hsv_img;
    cvtColor(orig_img, hsv_img, CV_BGR2HSV);

    Histogram1D h;
    MatND hist, old_hist;
    old_hist = h.getHistogram(hsv_img);
    normalize( old_hist, old_hist, 0, 1, NORM_MINMAX, -1, Mat() );

    int lost_cnt = 0;
    char ss = 'e';
    while (1)
    {
        //download image file
        pImg = QImage2IplImage(w.getImage());
        current_frame = (IplImage*)&pImg;

        // convert to gray and downsize
        cvCvtColor (current_frame, gray_image, CV_BGR2GRAY);
        cvResize (gray_image, small_image, CV_INTER_LINEAR);

        // detect faces
        CvSeq* faces = cvHaarDetectObjects (small_image, cascade, storage,
                                            1.1, 2, CV_HAAR_DO_CANNY_PRUNING,
                                            cvSize (30, 30));

        // draw faces
        cvFlip (current_frame, draw_image, 1);

        double minBha = 100.0;
        double best_r = 0;
        CvPoint best_p = cvPoint(0, 0);
        for (int i = 0; i < (faces ? faces->total : 0); i++)
        {
            CvRect* r = (CvRect*) cvGetSeqElem (faces, i);
            CvPoint center;
            int radius;
            center.x = cvRound((small_image->width - r->width*0.5 - r->x) *scale);
            center.y = cvRound((r->y + r->height*0.5)*scale);
            radius = cvRound((r->width + r->height)*0.25*scale);

            //filter other smaller circles
            Mat image(current_frame, 0);
            int min_x = (center.x - radius > 0) ? (center.x - radius) : 0;
            int min_y = (center.y - radius > 0) ? (center.y - radius) : 0;
            min_x = image.cols - min_x - radius * 2;

            Mat cur(image, Rect(min_x, min_y, radius * 2, radius * 2));

            cvtColor(cur, hsv_img, CV_BGR2HSV);
            hist = h.getHistogram(hsv_img);
            normalize( hist, hist, 0, 1, NORM_MINMAX, -1, Mat() );

            double bha = compareHist(hist, old_hist, CV_COMP_BHATTACHARYYA);
            if (bha < minBha) {
                minBha = bha;
                best_r = radius;
                best_p = center;
            }

        }

        //qDebug() << "BHA: " << minBha;

        //object lost ?
        if ((faces ? faces->total : 0) == 0 && minBha > 0.4) {
            ++lost_cnt;
            if (lost_cnt > 5) {
                send_command(socket, ss);
                lost_cnt = 0;
                qDebug() << "Object lost";
            }
        }
        else {
            lost_cnt = 0;

            cvCircle (draw_image, best_p, best_r, CV_RGB(0,255,0), 3, 8, 0 );

            if (2 * best_r > current_frame->height * 3 / 10) {
                send_command(socket, 'x');
                qDebug() << "Too Large";
            }
            else if (best_p.x < current_frame->width * 2 / 7) {
                send_command(socket, 'e');
                ss = 'e';
                qDebug() << "Right";
            }
            else if (best_p.x > current_frame->width * 5 / 7) {
                send_command(socket, 'q');
                ss = 'q';
                qDebug() << "Left";
            }
            else {
                send_command(socket, 'w');
                qDebug() << "Straight";
            }
        }

        // just show the image
        cvShowImage (WINDOW_NAME, draw_image);

        // wait a tenth of a second for keypress and window drawing
        int key = cvWaitKey (40);
        if (key == 'q' || key == 'Q')
            break;

    }

    socket.close();
    qDebug() << "Close Connection";

    return a.exec();
}



