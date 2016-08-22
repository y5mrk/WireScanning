//
//  main.cpp
//  wirescanning
//
//  Created by YoshimuraKazumi on 2016/08/22.
//  Copyright © 2016年 YoshimuraKazumi. All rights reserved.
//

//#include <iostream>
//
//int main(int argc, const char * argv[]) {
//    // insert code here...
//    std::cout << "Hello, World!\n";
//    return 0;
//}

#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
    cv::Mat image = cv::imread("harigane.JPG"); // 画像の読み込み
    cv::namedWindow("input");                // ウィンドウの準備
    cv::imshow("input", image);              // 画像の表示
    cv::waitKey();                           // キー入力待ち
    cv::imwrite("output.jpg", image);        // 画像の保存
    
    return 0;
}