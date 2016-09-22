// ----------------------------
// Copyright (C) 2016
// Giovanni Alcantara
// ----------------------------

// --------
// includes
// --------

#include "Netflix.h"
#include <cassert> // assert
#include <fstream>
#include <iostream> // endl, istream, ostream
#include <map>
#include <math.h>

using namespace std;

/// Uses user averages, normalised movie averages, user standard deviation
/// caches
#define CACHE_OPT

// --------
// Builds the user average rating cache from the cache file
// --------
void build_user_averages_cache(map<int, double> &user_averages) {
  ifstream cache_file("gca386-AllUsersAveragesCache.txt");

  if (!cache_file) {
    cache_file.open(
        "/u/downing/cs/netflix-cs371p/gca386-AllUsersAveragesCache.txt");
  }

  if (cache_file.is_open()) {
    int user_id;
    double avg;
    while (cache_file >> user_id >> avg) {
      user_averages[user_id] = avg;
    }
    cache_file.close();
  }
}

// --------
// Builds the user standard deviation rating cache from the cache file
// --------
void build_user_sds_cache(map<int, double> &user_sds) {
  ifstream cache_file("gca386-AllUsersSDCache.txt");

  if (!cache_file) {
    cache_file.open("/u/downing/cs/netflix-cs371p/gca386-AllUsersSDCache.txt");
  }

  if (cache_file.is_open()) {
    int user_id;
    double sd;
    while (cache_file >> user_id >> sd) {
      user_sds[user_id] = sd;
    }
    cache_file.close();
  }
}

// --------
// Builds the movie normalized rating cache from the cache file
// --------
void build_movie_norm_ratings_cache(map<int, double> &movie_norm_ratings) {
  ifstream cache_file("gca386-MovieNormMean.txt");

  if (!cache_file) {
    cache_file.open("/u/downing/cs/netflix-cs371p/gca386-MovieNormMean.txt");
  }

  if (cache_file.is_open()) {
    int movie_id;
    double mean;
    while (cache_file >> movie_id >> mean) {
      movie_norm_ratings[movie_id] = mean;
    }
    cache_file.close();
  }
}

// --------
// Predicts ratings in stdin and prints to stdout
// --------
double predict(istream &r, ostream &w) {
#ifdef CACHE_OPT

  // Build actual rating map first
  map<int, map<int, int>> actual_ratings;
  ifstream cache_file("gca386-ActualRatingsCache.txt");

  if (!cache_file) {
    cache_file.open(
        "/u/downing/cs/netflix-cs371p/gca386-ActualRatingsCache.txt");
  }

  if (cache_file.is_open()) {
    int movie_id, user_id, rating;
    while (cache_file >> movie_id >> user_id >> rating) {
      actual_ratings[movie_id][user_id] = rating;
    }
    cache_file.close();
  }

  map<int, double> user_averages;
  build_user_averages_cache(user_averages);

  map<int, double> user_sds;
  build_user_sds_cache(user_sds);

  map<int, double> movie_norm_ratings;
  build_movie_norm_ratings_cache(movie_norm_ratings);
#endif

  int movie_id = 0;
  int user_id = 0;

  double rmse = 0;
  int count = 0;

  string line;
  while (getline(r, line)) {
    if (line.back() == ':') {
      w << line << endl;
      line.pop_back(); // remove : from the end
      movie_id = stoi(line);

    } else {
      user_id = stoi(line);

      double prediction;

#ifdef CACHE_OPT
      // double prediction = (0.4*averages_cache[movie_id] +
      // 0.6*user_averages[user_id]);
      prediction = movie_norm_ratings[movie_id] * user_sds[user_id] +
                   user_averages[user_id];
      prediction = floor(prediction * 10) / 10;

#else

      prediction = 3.7; // return absolute mean

#endif

      int actual = actual_ratings[movie_id][user_id];
      rmse += pow(prediction - actual, 2);

      ++count;
      w << prediction << endl;
    }
  }

  rmse = sqrt(rmse / count);
  rmse = floor(rmse * 100) / 100; // truncate to two decimal places
  w << "RMSE: " << rmse;
  return rmse;
}