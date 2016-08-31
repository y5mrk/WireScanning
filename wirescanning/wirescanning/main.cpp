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

int main (int argc, char *argv[]){
    
    cv::Mat src_img = cv::imread("harigane.JPG");
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
    
    int i = 0;
    std::vector< cv::Point > approx;
    
//    for(int i = 0; i < contours.size(); ++i) {
    for (auto contour = contours.begin(); contour != contours.end(); contour++){
    
        cv::approxPolyDP(cv::Mat(*contour), approx, 0.001 * cv::arcLength(*contour, true), true);
        double area = cv::contourArea(approx);
        
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

        
        size_t count = contours[i].size();
        if(count < 150 || count > 1000) continue; // （小さすぎる|大きすぎる）輪郭を除外
        
        cv::Mat pointsf;
        cv::Mat(contours[i]).convertTo(pointsf, CV_32F);
        // 楕円フィッティング
        cv::RotatedRect box = cv::fitEllipse(pointsf);
        // 楕円の描画
        cv::ellipse(src_img, box, cv::Scalar(0,0,255), 2, CV_AA);
        
        i++;
    }
    
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
    
    // svgファイルの作成
    mi::SvgDrawer drawer ( width, height, "test1.svg");
    drawer.setViewBox( -2, -2, 2, 2);
    
//    for(int p = 0; p < approx.size()-1; ++p) {
//        drawer.drawLine(approx[p].x * 0.01, approx[p].y * 0.01, approx[p+1].x * 0.01, approx[p+1].y * 0.01);
//    }
    
    cv::waitKey(0);
    
    return 0;
}

