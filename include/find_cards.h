#ifndef _FIND_CARDS_H_
#define _FIND_CARDS_H_

#include <cstdint>
#include <tuple>
#include <vector>

#include <opencv2/opencv.hpp>

int find_by_x_axis(cv::Mat &img, int start, int end, int step, int pos_y);
int find_by_y_axis(cv::Mat &img, int start, int end, int step, int card_bottom);
std::tuple<int, int, int, int> find_edge(cv::Mat &img, int x, int y);
std::vector<std::tuple<int, int>> find_blank(cv::Mat &img, int start_point,
                                             int end_point, int search_begin,
                                             int card_width, bool search_y);
std::vector<uint8_t> preprocess_img(cv::Mat &img, int x0, int y0, int height,
                                    int width);
std::vector<uint64_t> find_all_cards(cv::Mat &threshould_, cv::Mat &gray, int x,
                                int y);

#endif