#ifndef _ENCODE_H_
#define _ENCODE_H_

#include <string>
#include <vector>

class Card {
public:
  int id;
  int level;
  bool exclude;
  int art;
  int train;
  int ep;
  int skill;
  Card(int id, int level, bool exclude, int art, int train, int ep, int skill)
      : id(id), level(level), exclude(exclude), art(art), train(train), ep(ep),
        skill(skill) {}
};

std::string encode_all(const std::vector<Card> &cards);

#endif
