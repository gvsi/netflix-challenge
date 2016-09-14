// ----------------------------
// Copyright (C) 2016
// Giovanni Alcantara
// ----------------------------

// --------
// includes
// --------

#include <cassert>  // assert
#include <iostream> // endl, istream, ostream
#include <sstream>
#include <fstream>
#include <stdint.h>
#include <iomanip>
#include <map>
#include <boost/serialization/map.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "Netflix.h"

using namespace std;


void movie_lookup(int movie_id, map<int, int>& ratings) {
  // return rating
  ostringstream movie_file_name;
  movie_file_name << internal << setfill('0') << setw(7) << movie_id;
  // cout << "mv_" + movie_file_name.str() + ".txt" << endl;
  ifstream movie_file("netflix/training_set/mv_" + movie_file_name.str() + ".txt");

  if (movie_file.is_open()) {
    string line;
    getline(movie_file,line);
    line.pop_back(); // remove : from the end
    // assert(stoi(line), movie_id); // confirms that it's the right file

    while (getline(movie_file,line)) {
      istringstream ss(line);
      string token;
      getline(ss, token, ',');
      int user_id = stoi(token);
      if (ratings.count(user_id)) {
        getline(ss, token, ',');
        ratings[user_id] = stoi(token);
      }
    }
    movie_file.close();
  }
  // return 3;
}

void print_all_movie_ratings(ostream &w, map<int, map<int, int>> all_movie_ratings) {

  // Print movie rating
  for(map<int,map<int, int>>::iterator iter = all_movie_ratings.begin(); iter != all_movie_ratings.end(); ++iter) {
    int movieid = iter->first;
    map<int, int> ratings = iter->second;

    w << movieid << ":" << endl;

    for(map<int, int>::iterator iter2 = ratings.begin(); iter2 != ratings.end(); ++iter2) {
      int userid = iter2->first;
      int rating = iter2->second;
      w << userid << " " << rating << endl;
    }
  }
}

void store_movie_ratings(ostream &w, map<int,map<int, int>> &all_movie_ratings, map<int, int> &movie_ratings, int movie_id) {
  if (!movie_ratings.empty()) {
    movie_lookup(movie_id, movie_ratings);

    // Store in cache
    all_movie_ratings[movie_id] = movie_ratings;

    movie_ratings.clear();
  }
}

void archive_movie_ratings(map<int, map<int, int>> &all_movie_ratings) {
  ofstream ofs("MovieArchiveCache");
  boost::archive::text_oarchive oarch(ofs);
  oarch << all_movie_ratings;
  // std::map<int, int> new_map;
  // boost::archive::text_iarchive iarch(ss);
  // iarch >> new_map;
  // std::cout << (map == new_map) << std::endl;
}


void generate_ratings_cache(istream &r, ostream &w) {

  int movie_id = 0;
  int user_id = 0;
  map<int,int> movie_ratings;
  map<int,map<int, int>> all_movie_ratings;

  string line;
  while(getline(r, line)) {
    if (line.back() == ':') {

      store_movie_ratings(w, all_movie_ratings, movie_ratings, movie_id);

      line.pop_back(); // remove : from the end
      movie_id = stoi(line);
    } else {
      user_id = stoi(line);
      movie_ratings[user_id] = -1;
    }
  }

  // Store ratings for last movie
  store_movie_ratings(w, all_movie_ratings, movie_ratings, movie_id);

  archive_movie_ratings(all_movie_ratings);


  // Read cache
  map<int, map<int, int>> new_all_movie_ratings;
  ifstream ifs("MovieArchiveCache");
  boost::archive::text_iarchive iarch(ifs);
  iarch >> new_all_movie_ratings;

  print_all_movie_ratings(w, new_all_movie_ratings);

}
