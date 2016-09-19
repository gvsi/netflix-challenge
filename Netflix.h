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
#include <string>   // string

using namespace std;

/**
 * @param r an istream
 * @param w an ostream
 */
void user_predict(int user_id, int movie_id);
void pearson_correlation(istream &r, ostream &w);
void sd_cache(istream &r, ostream &w);
void mean_cache(istream &r, ostream &w);

void generate_inverted_cache();

void generate_ratings_cache(istream &r, ostream &w);

void predict(istream &r, ostream &w);

void averages_cache(istream &r, ostream &w);

#endif
