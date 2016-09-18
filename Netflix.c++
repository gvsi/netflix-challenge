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
#include <math.h>
#include <iomanip>
#include <map>
#include <boost/serialization/map.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "Netflix.h"

using namespace std;

void generate_inverted_cache() {
  // user -> [(movie, rating)]

  vector<map<int, map<int, int>>> cache_ratings;

  for (int i = 0; i < 100; ++i) {
    map<int, map<int, int>> m;
    cache_ratings.push_back(m);
  }


  for (int movie_id = 1; movie_id <= 17771; ++movie_id) {
    ostringstream movie_file_name;
    movie_file_name << internal << setfill('0') << setw(7) << movie_id;
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
        cache_ratings[user_id % 100][user_id][movie_id] = stoi(token);
      }
      movie_file.close();
    }
  }

  for(int i = 0; i < 100; ++i) {
    ofstream ofs0("InvertedIndex" + to_string(i));
    boost::archive::text_oarchive oarch0(ofs0);
    oarch0 << cache_ratings[i];
  }
}


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

void generate_averages_cache(int movie_id, double averages[]) {
  // return rating
  ostringstream movie_file_name;
  movie_file_name << internal << setfill('0') << setw(7) << movie_id;
  // cout << "mv_" + movie_file_name.str() + ".txt" << endl;
  ifstream movie_file("netflix/training_set/mv_" + movie_file_name.str() + ".txt");

  double average = 0.0;
  int count = 0;


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

      getline(ss, token, ',');
      average += stoi(token);
      ++count;
    }

    averages[movie_id] = average / count;
    // cout << movie_id << " " << fixed << setprecision(2) << averages[movie_id] << endl;
    movie_file.close();
  }
}

/*
void predict(istream &r, ostream &w) {
  vector<map<int, map<int,int>>> inverted_caches(100);
  w << "Loading invertedindex" << 0 << endl;
  map<int, map<int, int>> user_ratings;
  ifstream aci("InvertedIndexes/InvertedIndex" + to_string(0));
  boost::archive::text_iarchive iai(aci);
  iai >> user_ratings;
  inverted_caches[0] = user_ratings;

  for(map<int, map<int, int>>::iterator iter = inverted_caches[0].begin(); iter != inverted_caches[0].end(); ++iter) {
    int user_id = iter->first;
    map<int, int> ratings = iter->second;
    w << "user_id:" << user_id << ":" << endl;
    int count2 = 0;
    double user_average = 0.0;
    for(map<int, int>::iterator iter2 = ratings.begin(); iter2 != ratings.end(); ++iter2) {
      int movie_id = iter2->first;
      int rating = iter2->second;
      w << movie_id << ": " << rating << endl;
      user_average += rating;
      count2++;
    }
    w << "user_average: " << user_average;
    w << "count2: " << count2;
    user_average = user_average / count2;
    w << "user_average: " << user_average;
  }


}
*/

 void predict(istream &r, ostream &w) {
  map<int, map<int, int>> new_all_movie_ratings;
  ifstream mac("MovieArchiveCache");
  boost::archive::text_iarchive ia(mac);
  ia >> new_all_movie_ratings;

  double averages_cache [17771];
  ifstream ac("AveragesCache");
  boost::archive::text_iarchive ia2(ac);
  ia2 >> averages_cache;

  map<int, double> user_averages;
  ifstream uac("UserAveragesCache");
  boost::archive::text_iarchive ia3(uac);
  ia3 >> user_averages;

  // vector<map<int, map<int,int>>> inverted_caches(100);
  //
  // for (int i = 0; i < 100; ++i) {
  //   w << "Loading invertedindex" << i << endl;
  //   map<int, map<int, int>> user_ratings;
  //   ifstream aci("InvertedIndexes/InvertedIndex" + to_string(i));
  //   boost::archive::text_iarchive iai(aci);
  //   iai >> user_ratings;
  //   inverted_caches[i] = user_ratings;
  // }

  // map<int, double> user_averages;

  int movie_id = 0;
  int user_id = 0;

  double rmse = 0;
  int count = 0;

  string line;
  while(getline(r, line)) {
    if (line.back() == ':') {
      w << line << endl;
      line.pop_back(); // remove : from the end
      movie_id = stoi(line);

    } else {
      user_id = stoi(line);


      // double user_average = 0.0;
      // int count2 = 0;
      // for(map<int, int>::iterator iter = inverted_caches[user_id % 100][user_id].begin(); iter != inverted_caches[user_id % 100][user_id].end(); ++iter) {
      //   int rating = iter->second;
      //   user_average += rating;
      //   count2++;
      // }
      // user_average = user_average / count2;
      // user_averages[user_id] = user_average;

      double prediction = (0.4*averages_cache[movie_id] + 0.6*user_averages[user_id]);
      int actual = new_all_movie_ratings[movie_id][user_id];
      ++count;
      prediction = floor(prediction * 100) / 100;
      rmse += pow(prediction - actual, 2);

      w << prediction << endl;
    }
  }

  // ofstream ofs0("UserAveragesCache");
  // boost::archive::text_oarchive oarch0(ofs0);
  // oarch0 << user_averages;

  rmse = sqrt(rmse / count);
  w << "RMSE: " << rmse;
}


void movie_user_lookup(int movie_id, map<int, map<int, int>>& ratings) {
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

      getline(ss, token, ',');
      ratings[user_id][movie_id] = stoi(token);
    }
    movie_file.close();
  }
  // return 3;
}

void averages_cache(istream &r, ostream &w) {
  double averages_cache [17771];
  averages_cache[0] = -1;

  for (int movie_id = 1; movie_id <= 17770; ++movie_id) {
    generate_averages_cache(movie_id, averages_cache);
  }

  ofstream ofs("AveragesCache");
  boost::archive::text_oarchive oarch(ofs);
  oarch << averages_cache;
}
