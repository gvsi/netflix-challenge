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

/**
 * @param actual_ratings a map of maps where to write each movie's actual user ratings.
 */

void build_actual_cache(map<int, map<int, int>> &actual_ratings);

/**
 * @param actual_ratings a map where to write each user's average rating.
 */
void build_user_averages_cache(map<int, double> &user_averages);

/**
 * @param actual_ratings a map where to write each user's standard deviation.
 */
void build_user_sds_cache(map<int, double> &user_sds);

/**
 * @param actual_ratings a map where to write each movie's normalised average rating.
 */
void build_movie_norm_ratings_cache(map<int, double> &movie_norm_ratings);


/**
 * @param r an istream
 * @param w an ostream
 * @return the RMSE of the prediction
 */

double predict(istream &r, ostream &w);

#endif
