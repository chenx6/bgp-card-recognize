#ifndef _HASH_H_
#define _HASH_H_

#include <cstdint>
#include <vector>

#include "jsonxx/json.hpp"
#include <opencv2/opencv.hpp>

/// @todo Direct pass this struct from js to C++
struct HashCardId {
  uint64_t hash_value;
  int card_id;
  HashCardId(uint64_t hash_value, int card_id)
      : hash_value(hash_value), card_id(card_id) {}
};

cv::Mat cut_image(cv::Mat &input);
uint64_t hash_(std::vector<uint8_t> &array);
int cmp_vec(uint64_t v1, uint64_t v2);
int get_nearest_id(uint64_t vec, jsonxx::json &hash_data);
int get_nearest_id(uint64_t vec, std::vector<HashCardId> &hash_data);

#endif