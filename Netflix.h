// --------------------------
// Copyright (C) 2016
// Giovanni Alcantara
// --------------------------

#ifndef Netflix_h
#define Netflix_h

// --------
// includes
// --------

#include <iostream> // istream, ostream
#include <map>

using namespace std;


void build_actual_cache(map<int, map<int, int>> actual_ratings);

/**
 * @param r an istream
 * @param w an ostream
 */

int predict(istream &r, ostream &w);

#endif
