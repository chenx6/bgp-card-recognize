#include <cmath>
#include <string>
#include <vector>

#include "encode.h"

using namespace std;

const string dict =
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-_";

string v(int t, int e) {
  string o;
  for (int i = 0; i < e; i++) {
    int a = t % 64;
    t = floor(t / 64);
    o = dict[a] + o;
  }
  return o;
}

string encode(const Card &o) {
  string data;
  data += o.id >= 10000 ? v(o.id - 10000 + 3072, 2) : v(o.id, 2);
  data += v(o.level, 1);
  data += v(1 * (o.exclude ? 1 : 0) + 2 * o.art + 4 * o.train + 8 * o.ep +
                24 * o.skill,
            2);
  return data;
}

string encode_all(const vector<Card> &cards) {
  string data;
  for (auto &&o : cards) {
    data += encode(o);
  }
  return data;
}