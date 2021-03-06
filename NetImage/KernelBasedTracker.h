#pragma once

#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

class KernelBasedTracker
{
public:
    KernelBasedTracker(int bin_size,const Mat &frame,Rect &box, int image_w, int image_h);
    KernelBasedTracker(KernelBasedTracker& tracker);
    void CalcTargetModel(const Mat &target,const Mat & kernel,Mat &target_model);
    Mat CalcKernel(int w,int h);
    void CalcMeanShift(const Mat &current_frame,bool multi_scale_box_flag=false);
    Rect getBoundingBox();
    float CalcBhattacharya(const Mat &candidate_model);
    void AdjustToEven(int &num);
    void AdjustToOdd(int &num);
    void UpdateBox(Rect &box);
    
    void AdjustBox(Rect &box);
    
    Rect box_;

private:
    int bin_num_;
    Mat kernel_;

    Mat target_model_;
    int MAXW, MAXH;
};
