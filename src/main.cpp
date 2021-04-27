#include <vector>

#include "jsonxx/json.hpp"
#ifndef DEBUG
#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/emscripten.h>
#include <emscripten/val.h>
#include <stdio.h>
#else
#include <fstream>
const int DISTRICT_ID = 3;
#endif
#include <opencv2/opencv.hpp>

#include "encode.h"
#include "find_cards.h"
#include "hash.h"

using namespace cv;
using namespace std;
using namespace jsonxx;

#ifndef DEBUG
/// @brief process the screenshot and return the result
/// @param offset a pointer points to raw image data
/// @param width screenshot width
/// @param height screenshot height
/// @param hash_data_s bestdori's card hash data
/// @param first_x a dot in left-top card
/// @param first_y a dot in left-top card
/// @return recognized card id that store in vector
vector<int> process(int offset, int width, int height, string hash_data_s,
                    int first_x, int first_y) {
  uint8_t *buf = reinterpret_cast<uint8_t *>(offset);
  auto mat = Mat(height, width, CV_8UC4, buf);
  Mat gray, thresh;
  cv::cvtColor(mat, gray, cv::COLOR_RGB2GRAY);
  threshold(gray, thresh, 200, 255, 0);
  vector<uint64_t> card_vecs = find_all_cards(thresh, gray, first_x, first_y);

  json raw_hash_data = json::parse(hash_data_s);
  vector<HashCardId> hash_data;
  // TODO: Direct pass HashCardId to C++
  for (auto iter = raw_hash_data.begin(); iter != raw_hash_data.end(); iter++) {
    // webassembly need stoull to convert 64bit number...
    uint64_t k = stoull(iter.key(), nullptr, 16);
    int v = iter.value().as_int();
    hash_data.emplace_back(k, v);
  }
  vector<int> card_ids;
  for (uint64_t &vec : card_vecs) {
    card_ids.push_back(get_nearest_id(vec, hash_data));
  }
  return card_ids;
}

EMSCRIPTEN_BINDINGS(wasmdori) {
  emscripten::function("process", &process);
  emscripten::function("encode_all",
                       &encode_all); // TODO: Use this function to encode
  emscripten::register_vector<int>("vector<int>");
}
#else
int main(int argc, char **argv) {
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " [screenshot_path]" << endl;
    return -1;
  }
  // JSON data
  json character_data, cards_data, hash_data;
  ifstream character_ifs("./data/characters.json");
  ifstream card_ifs("./data/cards.json");
  ifstream hash_ifs("./data/hashes.json");
  character_ifs >> character_data;
  card_ifs >> cards_data;
  hash_ifs >> hash_data;

  Mat img = imread(argv[1]);
  Mat gray, gauss, thresh;
  cvtColor(img, gray, COLOR_BGR2GRAY);
  // Test threshould
  // BGR2GRAY -> GaussianBlur(?) -> threshold
  // bilateralFilter(gray, gauss, 7, 150, 150);
  // blur(gray, gauss, Size(7, 7));
  // GaussianBlur(gray, gauss, Size(7, 7), 0);
  // threshold(gauss, thresh, 210, 255, 0);

  // Bestdori threshould(paritcally)
  // threshold(gray, thresh, 200, 255, 0);

  threshold(gray, thresh, 200, 255, 0);
#ifdef DEBUG
  // imshow("gauss", gauss);
  imshow("threshhold", thresh);
  waitKey(0);
#endif
  // TODO: remove hard-encoded position
  auto card_vecs = find_all_cards(thresh, gray, 559, 407);
  vector<int> card_ids;
  for (auto &&vec : card_vecs) {
    card_ids.push_back(get_nearest_id(vec, hash_data));
  }
  vector<Card> cards;
  for (auto &&i : card_ids) {
    int character_id;
    auto card = cards_data[to_string(i)];
    if (!card["characterId"].get_value(character_id)) {
      cout << i << " Not exist" << endl;
      continue;
    }
    string card_name;
    if (!card["prefix"][DISTRICT_ID].get_value(card_name)) {
      cout << "Name not exist" << endl;
      continue;
    }
    auto card_max_level = card["levelLimit"].as_int();
    auto character_name =
        character_data[to_string(character_id)]["characterName"][DISTRICT_ID]
            .as_string();
    cout << i << " " << card_name << " " << character_name << "(y/n)";
    string answer;
    cin >> answer;
    if (answer[0] == 'n') {
      continue;
    }
    cards.push_back(Card(i, card_max_level, false, 1, 1, 2, 0));
  }
  auto res = encode_all(cards);
  cout << res << endl;
}
#endif