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


map<int, map<int, int>> build_actual_cache();

/**
 * @param r an istream
 * @param w an ostream
 */

int predict(istream &r, ostream &w);

#endif
