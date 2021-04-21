#include <tuple>
#include <vector>
#ifdef DEBUG
#include <stdio.h>
#endif

#include <opencv2/opencv.hpp>

#include "hash.h"

using namespace cv;
using namespace std;

/// TODO: These value are hard-encoded, and wait for optimize
constexpr int Y_AXIS_WHITEDOT_COUNT = 10;
constexpr double CARD_WIDTH_THRESHOULD = 0.3;
constexpr int SEARCH_OFFSET = 15;

/// @brief Find the non-white point in x axis
/// 顺着 x 轴寻找第一个非白色点的坐标
int find_by_x_axis(Mat &img, int start, int end, int step, int pos_y) {
  int target = 0;
  for (int pos_x = start; pos_x != end; pos_x += step) {
    if (img.at<uchar>(pos_y, pos_x) != 255) {
      target = pos_x;
      break;
    }
  }
  return target;
}

/// @brief Find white point in y axis by follow x axis
/// 顺着 x 轴寻找 y 轴上白色点大于阈值的点
int find_by_y_axis(Mat &img, int start, int end, int step, int card_bottom) {
  int target = 0;
  for (int pos_x = start; pos_x != end; pos_x += step) {
    int cnt = 0;
    for (int pos_y = card_bottom; pos_y > 0; pos_y--) {
      if (img.at<uchar>(pos_y, pos_x) == 255) {
        cnt++;
      } else if (cnt > Y_AXIS_WHITEDOT_COUNT) {
        break;
      } else {
        break;
      }
    }
    if (cnt >= Y_AXIS_WHITEDOT_COUNT) {
      target = pos_x;
      break;
    }
  }
  return target;
}

/// @brief Find card edge
/// @return card_left, card_right, card_top, card_bottom
tuple<int, int, int, int> find_edge(Mat &img, int x, int y) {
  int height = img.size().height, width = img.size().width;
  // Iterator the y axis
  for (int pos_y = y; pos_y < height; pos_y++) {
    int right = 0, left = width;
    if (img.at<uchar>(pos_y, x) != 255) {
      continue;
    }
    // Look for non-white dot on the x axis, and get the position of the dot
    right = find_by_x_axis(img, x, width, 1, pos_y);
    left = find_by_x_axis(img, x, 0, -1, pos_y);
    // If the distance between the non-white dot is longer than screenshot's
    // width, it must be the bottom of the card
    if (right - left < width / 4) {
      continue;
    }
    int card_bottom = pos_y;
    // Find the border of the card by finding white dot in y axis
    int card_right = find_by_y_axis(img, x, width, 1, card_bottom);
    int card_left = find_by_y_axis(img, x, 0, -1, card_bottom);
    if (card_left != 0 && card_right != 0) {
      int card_width = card_right - card_left;
      int card_top = card_bottom - card_width;
      return make_tuple(card_left, card_right, card_top, card_bottom);
    }
  }
  return make_tuple(0, 0, 0, 0);
}

/// @brief Find the blank line between cards
/// @param search_y search by y axis or not
vector<tuple<int, int>> find_blank(Mat &img, int start_point, int end_point,
                                   int search_begin, int card_width,
                                   bool search_y) {
  int threshhold = card_width * CARD_WIDTH_THRESHOULD;
  vector<int> lst;
  for (int pos_a = start_point; pos_a != end_point; pos_a++) {
    int cnt = 0;
    for (int pos_b = search_begin; pos_b != 0; pos_b--) {
      int dot_color =
          search_y ? img.at<uchar>(pos_b, pos_a) : img.at<uchar>(pos_a, pos_b);
      if (dot_color == 255) {
        cnt++;
      } else if (cnt > threshhold) {
        break;
      } else {
        break;
      }
    }
    if (cnt > threshhold) {
      lst.push_back(pos_a);
    }
  }
  vector<tuple<int, int>> result;
  for (size_t i = 1; i < lst.size(); i++) {
    int before_dot = lst[i - 1];
    int dot = lst[i];
    if (dot - before_dot >= threshhold) {
      result.push_back(make_tuple(before_dot, dot));
    }
  }
  return result;
}

/// @brief Preprocess image by cutting edge, resizing image, flatting matrix
/// @param y0 bottom of the image
vector<uint8_t> preprocess_img(Mat &img, int x0, int y0, int height,
                               int width) {
  auto selected_image = img(Range(y0 - height, y0), Range(x0, x0 + width));
  auto cutted = cut_image(selected_image);
  Mat resized;
  resize(cutted, resized, Size(32, 32), 0, 0, INTER_LINEAR);
#ifdef DEBUG
  // char name[64] = {0};
  // snprintf(name, 64, "resized_%d_%d.png", x0, y0);
  // imwrite(name, resized);
  imshow("selected", selected_image);
  imshow("resized", resized);
  waitKey(0);
#endif
  return vector<uint8_t>(resized.datastart, resized.dataend);
}

/// @brief Find all cards in image
/// @param threshould_ threshoulded Screenshot
/// @param gray gray Screenshot
/// @return card's vector
vector<uint64_t> find_all_cards(Mat &threshould_, Mat &gray, int x, int y) {
  vector<uint64_t> card_vecs;
  int height = threshould_.size().height, width = threshould_.size().width;
  // Find card that in left top position
  auto [card_left, card_right, card_top, card_bottom] =
      find_edge(threshould_, x, y);
#ifdef DEBUG
  Mat first_selected =
      gray(Range(card_top, card_bottom), Range(card_left, card_right));
  imshow("first", first_selected);
  waitKey(0);
#endif
  // Calculate card width and height by minus
  int card_width = card_right - card_left;
  int card_height = card_bottom - card_top;
  // Find the "blank" line between cards
  int start_point_x = card_left - SEARCH_OFFSET,
      start_point_y = card_top - SEARCH_OFFSET;
  auto dots_x = find_blank(threshould_, start_point_x, width, card_bottom,
                           card_width, true);
  auto dots_y = find_blank(threshould_, start_point_y, height, card_right,
                           card_height, false);
  if (dots_x.size() > 20 || dots_y.size() > 20) {
    return card_vecs;
  }
  // Iterator blank line's position and hash card
  for (auto &&xs : dots_x) {
    for (auto &&ys : dots_y) {
      auto [x0, x1] = xs;
      auto [y0, y1] = ys;
      auto processed_img =
          preprocess_img(gray, x0, y1, card_height, card_width);
      auto vec = hash_(processed_img);
#ifdef DEBUG
      printf("%llu\n", vec);
#endif
      card_vecs.push_back(vec);
    }
  }
  return card_vecs;
}
