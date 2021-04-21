#include <cmath>
#include <cstdint>
#include <memory>
#include <vector>

#ifndef DEBUG
#include "emscripten.h"
#endif
#include "jsonxx/json.hpp"
#include <opencv2/opencv.hpp>

#include "hash.h"

using namespace cv;
using namespace std;
using namespace jsonxx;

const double one_array[2] = {1 / sqrt(2.0), 1};
const double COS[] = {1.0,
                      0.9987954562051724,
                      0.9951847266721969,
                      0.989176509964781,
                      0.9807852804032304,
                      0.970031253194544,
                      0.9569403357322088,
                      0.9415440651830208,
                      0.9238795325112867,
                      0.9039892931234433,
                      0.881921264348355,
                      0.8577286100002721,
                      0.8314696123025452,
                      0.8032075314806449,
                      0.773010453362737,
                      0.7409511253549591,
                      0.7071067811865476,
                      0.6715589548470183,
                      0.6343932841636455,
                      0.5956993044924335,
                      0.5555702330196023,
                      0.5141027441932217,
                      0.4713967368259978,
                      0.4275550934302822,
                      0.38268343236508984,
                      0.33688985339222005,
                      0.29028467725446233,
                      0.24298017990326398,
                      0.19509032201612833,
                      0.14673047445536175,
                      0.09801714032956077,
                      0.049067674327418126,
                      6.123233995736766e-17,
                      -0.04906767432741801,
                      -0.09801714032956065,
                      -0.14673047445536164,
                      -0.1950903220161282,
                      -0.24298017990326387,
                      -0.29028467725446216,
                      -0.33688985339221994,
                      -0.3826834323650897,
                      -0.42755509343028186,
                      -0.4713967368259977,
                      -0.5141027441932217,
                      -0.555570233019602,
                      -0.5956993044924334,
                      -0.6343932841636454,
                      -0.6715589548470184,
                      -0.7071067811865475,
                      -0.7409511253549589,
                      -0.773010453362737,
                      -0.8032075314806448,
                      -0.8314696123025453,
                      -0.857728610000272,
                      -0.8819212643483549,
                      -0.9039892931234433,
                      -0.9238795325112867,
                      -0.9415440651830207,
                      -0.9569403357322088,
                      -0.970031253194544,
                      -0.9807852804032304,
                      -0.989176509964781,
                      -0.9951847266721968,
                      -0.9987954562051724,
                      -1.0,
                      -0.9987954562051724,
                      -0.9951847266721969,
                      -0.989176509964781,
                      -0.9807852804032304,
                      -0.970031253194544,
                      -0.9569403357322089,
                      -0.9415440651830208,
                      -0.9238795325112868,
                      -0.9039892931234434,
                      -0.881921264348355,
                      -0.8577286100002721,
                      -0.8314696123025455,
                      -0.8032075314806449,
                      -0.7730104533627371,
                      -0.7409511253549591,
                      -0.7071067811865477,
                      -0.6715589548470187,
                      -0.6343932841636459,
                      -0.5956993044924331,
                      -0.5555702330196022,
                      -0.5141027441932218,
                      -0.47139673682599786,
                      -0.4275550934302825,
                      -0.38268343236509034,
                      -0.33688985339221994,
                      -0.29028467725446244,
                      -0.24298017990326412,
                      -0.19509032201612866,
                      -0.1467304744553623,
                      -0.09801714032956045,
                      -0.04906767432741803,
                      -1.8369701987210297e-16,
                      0.04906767432741766,
                      0.09801714032956009,
                      0.14673047445536194,
                      0.1950903220161283,
                      0.24298017990326376,
                      0.29028467725446205,
                      0.3368898533922196,
                      0.38268343236509,
                      0.42755509343028214,
                      0.4713967368259976,
                      0.5141027441932216,
                      0.5555702330196018,
                      0.5956993044924329,
                      0.6343932841636456,
                      0.6715589548470183,
                      0.7071067811865474,
                      0.7409511253549589,
                      0.7730104533627367,
                      0.803207531480645,
                      0.8314696123025452,
                      0.857728610000272,
                      0.8819212643483548,
                      0.9039892931234431,
                      0.9238795325112865,
                      0.9415440651830208,
                      0.9569403357322088,
                      0.970031253194544,
                      0.9807852804032303,
                      0.9891765099647809,
                      0.9951847266721969,
                      0.9987954562051724,
                      1.0};

/// @brief Cut the edge of the card
Mat cut_image(Mat &input) {
  size_t width = input.size().width;
  size_t height = input.size().height;
  size_t sx = int(0 + 1 + width / 4);
  size_t sy = int(0 + 1 + height / 4);
  size_t sWidth = int(width / 2);
  size_t sHeight = int(height / 2);
  return input(Range(sy, sy + sHeight), Range(sx, sx + sWidth));
}

/// @brief Imitate Bestdori's hash function
unique_ptr<double[]> r(size_t length, vector<uint8_t> &gray) {
  unique_ptr<double[]> ret = make_unique<double[]>(length * length);
  // double *ret = new double[length * length];
  // Seems like FFT
  for (size_t y1 = 0; y1 < length; y1++)
    for (size_t x1 = 0; x1 < length; x1++) {
      double l = 0;
      for (size_t y2 = 0; y2 < length; y2++)
        for (size_t x2 = 0; x2 < length; x2++)
          l += COS[((2 * y2 + 1) * y1) % 128] * COS[((2 * x2 + 1) * x1) % 128] *
               gray[length * y2 + x2];
      l *= one_array[y1 > 0 ? 1 : 0] * one_array[x1 > 0 ? 1 : 0] / 4;
      ret[length * y1 + x1] = l;
    }
  return ret;
}

/// @brief Hash image function
/// @todo Replace this to more effient version
/// @note This algorithm is not licensed by the origin author
uint64_t hash_(vector<uint8_t> &array) {
  // Process data
  unique_ptr<double[]> res = r(32, array);
  vector<double> res1;
  for (int c = 1; c <= 8; c++)
    for (int o = 1; o <= 8; o++)
      res1.push_back(res[32 * c + o]);

  // Find the middle number
  vector<double> middle_res1 = res1;
  sort(middle_res1.begin(), middle_res1.end());
  double middle = middle_res1[(int)(middle_res1.size() / 2)];

  // Threshould data
  uint64_t vec = 0;
  for (auto &&i : res1) {
    vec <<= 1;
    vec += i > middle ? 1 : 0;
  }
  return vec;
}

/// @brief Hash image function (C array version)
uint64_t hash_(uint8_t array[], int length) {
  vector<uint8_t> vec(array, array + length);
  return hash_(vec);
}

/// @brief Compare two vector
/// @return Matching bit count
int cmp_vec(uint64_t v1, uint64_t v2) {
  int count = 0;
  uint64_t n = v1 ^ v2;
  for (int i = 0; i < 64; i++) {
    if ((n & 1) == 0) {
      count += 1;
    }
    n >>= 1;
  }
  return count;
}

/// @brief Get nearest vector and return its id
/// @param hash_data the card hash json from bestdori
int get_nearest_id(uint64_t vec, json &hash_data) {
  int max_diff = 0, nearest_id = -1;
  for (auto iter = hash_data.begin(); iter != hash_data.end(); iter++) {
    // webassembly need stoull to convert 64bit number...
    uint64_t k = stoull(iter.key(), nullptr, 16);
    int diff = cmp_vec(k, vec);
    if (diff > max_diff) {
      max_diff = diff;
      nearest_id = iter.value().as_int();
    }
  }
  return nearest_id;
}

int get_nearest_id(uint64_t vec, vector<HashCardId> &hash_data) {
  int max_diff = 0, nearest_id = -1;
  for (auto &&i : hash_data) {
    // webassembly need stoull to convert 64bit number...
    int diff = cmp_vec(i.hash_value, vec);
    if (diff > max_diff) {
      max_diff = diff;
      nearest_id = i.card_id;
    }
  }
  return nearest_id;
}