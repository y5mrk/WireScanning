//
//  main.cpp
//  wirescanning
//
//  Created by YoshimuraKazumi on 2016/08/22.
//  Copyright © 2016年 YoshimuraKazumi. All rights reserved.
//


#include <opencv2/opencv.hpp>
#include <vector>
#include "SvgDrawer.hpp"

bool CamRead(int, char**){
    cv::VideoCapture cap(0); // デフォルトカメラをオープン
    if(!cap.isOpened())  // 成功したかどうかをチェック
        return false; // プログラム終了
    
    cv::Mat edges;
    cv::namedWindow("frames",1);
    cv::Mat frame;
    for(;;)
    {
        cap >> frame; // カメラから新しいフレームを取得
        
        imshow("feames", frame);
        if(cv::waitKey(30) >= 0) break;
    }
    // VideoCapture デストラクタにより，カメラは自動的に終了処理されます
    
    cv::imwrite("img.png", frame);
    return true;
}


int main (int argc, char *argv[]){
    
    bool cam;
//    cam = CamRead(argc,argv);
    
//    if(cam){
    cv::Mat src_img = cv::imread("img.png");
    if(!src_img.data) return -1;
    
    int width = src_img.cols;
    int height = src_img.rows;
    
    cv::Mat gray_img, bin_img;
    cv::cvtColor(src_img, gray_img, CV_BGR2GRAY);
    cv::Mat line_img(cv::Size(width, height), CV_8UC3, cv::Scalar(255,255,255));
    
    std::vector<std::vector<cv::Point> > contours;
    // 画像の二値化
    cv::Mat nega_img = ~gray_img;
    cv::threshold(nega_img, bin_img, 0, 255, cv::THRESH_BINARY|cv::THRESH_OTSU);
    // 輪郭の検出
    cv::findContours(bin_img, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    
    int num = 0;
    std::vector< cv::Point > approx;
    
    double max_area = 0.0;
    int max_num = 0;
    
//    for(int i = 0; i < contours.size(); ++i) {
    for (auto contour = contours.begin(); contour != contours.end(); contour++){
    
        cv::approxPolyDP(cv::Mat(*contour), approx, 0.001 * cv::arcLength(*contour, true), true);
        double area = cv::contourArea(approx);
        if(area > max_area){
            max_area = area;
            max_num = num;
        }
        
        if (area > 1000.0){
            //青で囲む場合
            cv::polylines(src_img, approx, true, cv::Scalar(255, 0, 0), 2);
            cv::polylines(line_img, approx, true, cv::Scalar(255, 0, 0), 1);
            std::stringstream sst;
            sst << "area : " << area;
            cv::putText(src_img, sst.str(), approx[0], CV_FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(0, 128, 0));
            
            //輪郭に隣接する矩形の取得
//            cv::Rect brect = cv::boundingRect(cv::Mat(approx).reshape(2));
            
            
            
            //表示
//            cv::imshow("label" + std::to_string(roiCnt+1), roi[roiCnt]);
            
            
        }
        num++;
        
    }
    
    
//    std::cout << max_area<< ", " << max_num;
    
    for(int j = 0; j < contours.size(); ++j) {
        //入力画像に表示する場合
        cv::drawContours(line_img, contours, j, CV_RGB(0, 0, 0), 1);
    }
    
    // 特徴点検出の準備
    cv::Ptr<cv::AKAZE> detector = cv::AKAZE::create();
    
    // 特徴点の検出
    std::vector<cv::KeyPoint> keypoints;
    detector->detect(line_img, keypoints);
    
    cv::drawKeypoints(line_img, keypoints, line_img);
    
    cv::namedWindow("fit ellipse", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::namedWindow("bin image", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::namedWindow("outline image", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::imshow("fit ellipse", src_img);
    cv::imshow("bin image", bin_img);
    cv::imshow("outline image", line_img);
    
//    std::cout << approx;
//     svgファイルの作成
    mi::SvgDrawer drawer ( width, height, "test1.svg");
    drawer.setViewBox( 0, 0, 20, 20);
    
    for(int i=0; i<num; i++){
        drawer.PolyLine(contours[i]);
    }
        
    cv::waitKey(0);
    
    return 0;
//    }
}

