#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <typeinfo>
#include <vector>
#include<fstream>
#include <windows.h>

using namespace cv;
using namespace std;

vector <int>label_list;
vector <int>final_label_list = { 0 };
int label_pic[1025][1025] = { 0 };//圖片輸入大小
float morphology[2560][2560];
fstream label_file, final_label_file, label_list_file;

int find_root(int x) {
    int x_root = x;
    while (label_list[x_root] != -1) {
        x_root = label_list[x_root];
        //cout << x_root << " ";
    }
    return x_root;
}
void binpic_write_to_label_pic(Mat pic) {
    for (int i = 0; i < pic.rows; i++) {
        uchar* pic_pointer = pic.ptr<uchar>(i);

        for (int j = 0; j < pic.cols; j++) {
            //if (pic_pointer[j] > meanval[0]) {
            if (pic_pointer[j] > 100) {
                pic_pointer[j] = 255;
                label_pic[i][j] = -1;
            }
            else {
                pic_pointer[j] = 0;
                label_pic[i][j] = 0;
            }

        }
    }
}
void resolve(int* n1, int* n2) {
    int temp, temp1, temp2;
    if (*n1 != *n2) {
        if (label_list[*n1] == -1 && label_list[*n2] == -1) {
            if (*n1 > *n2) {
                label_list[*n1] = *n2;
            }
            else {
                label_list[*n2] = *n1;
            }
        }

        else if (label_list[*n1] != -1 && label_list[*n2] != -1) {
            if (label_list[*n1] != label_list[*n2]) {
                temp1 = *n1;
                temp1 = find_root(temp1);
                temp2 = *n2;
                temp2 = find_root(temp2);
                if (temp1 != temp2) {
                    if (temp1 > temp2) {
                        label_list[temp1] = temp2;
                    }
                    else {
                        label_list[temp2] = temp1;
                    }
                }
            }

            temp = label_list[*n1] > label_list[*n2] ? label_list[*n2] : label_list[*n1];

            label_list[*n1] = temp;
            label_list[*n2] = temp;
        }
        else if (label_list[*n1] != -1 && label_list[*n2] == -1) {
            if (label_list[*n1] != *n2) {
                if (label_list[*n1] > *n2) {
                    temp1 = *n1;
                    temp1 = find_root(temp1);
                    if (temp1 > *n2) {
                        label_list[temp1] = *n2;
                    }
                    label_list[*n1] = *n2;

                }
                else {
                    label_list[*n2] = label_list[*n1];
                }
            }

        }
        else {
            if (label_list[*n2] != *n1) {
                if (label_list[*n2] > *n1) {
                    temp2 = *n2;
                    temp2 = find_root(temp2);
                    if (temp2 > *n1) {
                        label_list[temp2] = *n1;
                    }
                    label_list[*n2] = *n1;

                }
                else {
                    label_list[*n1] = label_list[*n2];
                }
            }


        }
    }

}

void second_pass(int pic_row, int pic_col) {
    int n = 0, temp = 0, now = 1, start = 1, cnt = 0;
    bool add = false;
    for (int i = 1; i < label_list.size(); i++) {
        final_label_list.push_back(find_root(i));
    }
    for (int i = 0; i < pic_row; i++) {
        for (int j = 0; j < pic_col; j++) {
            label_pic[i][j] = final_label_list[label_pic[i][j]];
        }
    }
    std::cout << "Finish print ";
}

void dilation_kernal_3(int rows, int cols) {
    int outpic_row, outpic_col;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            outpic_row = i + 1;
            outpic_col = j + 1;
            if (label_pic[i][j] == -1) {
                morphology[outpic_row - 1][outpic_col - 1] = 1;
                morphology[outpic_row - 1][outpic_col] = 1;
                morphology[outpic_row - 1][outpic_col + 1] = 1;
                morphology[outpic_row][outpic_col - 1] = 1;
                morphology[outpic_row][outpic_col] = 1;
                morphology[outpic_row][outpic_col + 1] = 1;
                morphology[outpic_row + 1][outpic_col - 1] = 1;
                morphology[outpic_row + 1][outpic_col] = 1;
                morphology[outpic_row + 1][outpic_col + 1] = 1;
                morphology[outpic_row + 1][outpic_col + 1] = 1;
            }
        }
    }
}
void erosion_kernal_3(int rows,int cols) {
    for (int i = 0; i < rows; i++) {
        morphology[i][0] = 0;
    }
    for (int j = 0; j < cols; j++) {
        morphology[0][j] = 0;
    }
    for (int i = 1; i < rows-1; i++) {
        for (int j = 1; j < cols-1; j++) {
            int outpic_row, outpic_col;
            outpic_row = i + 1;
            outpic_col = j + 1;
            if (label_pic[outpic_row -1][outpic_col -1] == -1 && label_pic[outpic_row -1][outpic_col] == -1 && label_pic[outpic_row -1][outpic_col +1] == -1) {
                if (label_pic[outpic_row][outpic_col - 1] == -1 && label_pic[outpic_row][outpic_col] == -1 && label_pic[outpic_row][outpic_col + 1] == -1) {
                    if (label_pic[outpic_row + 1][outpic_col - 1] == -1 && label_pic[outpic_row + 1][outpic_col] == -1 && label_pic[outpic_row + 1][outpic_col + 1] == -1) {
                        morphology[i+1][j+1] = 1;
                    }
                }
            }
        }
    }
}

void average(int rows, int cols) {
    int outpic_row, outpic_col;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            outpic_row = i + 1;
            outpic_col = j + 1;
            morphology[i][j] = (label_pic[outpic_row - 1][outpic_col - 1] + label_pic[outpic_row - 1][outpic_col] + label_pic[outpic_row - 1][outpic_col + 1] + label_pic[outpic_row][outpic_col - 1] + label_pic[outpic_row][outpic_col] + label_pic[outpic_row - 1][outpic_col + 1] + label_pic[outpic_row + 1][outpic_col - 1] + label_pic[outpic_row + 1][outpic_col] + label_pic[outpic_row + 1][outpic_col + 1])/8;
            }
    }
}
int main()
{
    
    Mat img = cv::imread("D:/graduate_project/learning_opencv/practice/morphology_test.jpg");

    Mat grayimg;
    cv::cvtColor(img, grayimg, COLOR_RGB2GRAY);
    binpic_write_to_label_pic(grayimg);
    dilation_kernal_3(grayimg.rows, grayimg.cols);
    for (int i = 0; i < grayimg.rows+2; i++) {
        for (int j = 0; j < grayimg.cols+2; j++) {
            cout << morphology[i][j] << ",";
        }
        cout << "\n";
    }
    
    return 0;
}