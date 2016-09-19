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
#include<queue>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include "Netflix.h"

using namespace std;

class CompareDist
{
public:
    bool operator()(pair<int,double> n1,pair<int,double> n2) {
        return n1.second>n2.second;
    }
};

template <class T, class S, class C>
    S& Container(priority_queue<T, S, C>& q) {
        struct HackedQueue : private priority_queue<T, S, C> {
            static S& Container(priority_queue<T, S, C>& q) {
                return q.*&HackedQueue::c;
            }
        };
    return HackedQueue::Container(q);
}

void generate_inverted_cache() {
  // user -> [(movie, rating)]

  map<int, map<int, int>> cache_ratings;


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
        getline(ss, token, ',');
        cache_ratings[user_id][movie_id] = stoi(token);
      }
      movie_file.close();
    }
  }

  ofstream ofs0("InvertedIndex.bin");
  boost::archive::binary_oarchive oarch0(ofs0);
  oarch0 << cache_ratings;
}

void pearson_correlation(istream &r, ostream &w) {
  map<int, vector<pair<int, double>>> pc_cache;

  map<int, double> user_averages;
  ifstream uac("AllUsersAveragesCache.bin");
  boost::archive::binary_iarchive ia(uac);
  ia >> user_averages;

  map<int, double> user_sds;
  ifstream uac2("AllUsersSDCache.bin");
  boost::archive::binary_iarchive ia2(uac2);
  ia2 >> user_sds;

  w << "Loading InvertedIndex.bin" << endl;
  map<int, map<int, int>> user_ratings;
  ifstream uac3("InvertedIndex.bin");
  boost::archive::binary_iarchive ia3(uac3);
  ia3 >> user_ratings;

  int movie_id = 0;
  int user1_id = 0;

  string line;
  while(getline(r, line)) {
    if (line.back() == ':') {
      // w << line << endl;
      line.pop_back(); // remove : from the end
      movie_id = stoi(line);

    } else {
      user1_id = stoi(line);
      w << user1_id << ":" << endl;
      double user1_average = user_averages[user1_id];
      double user1_sd = user_sds[user1_id];

      map<int, int> ratings1 = user_ratings[user1_id];

      // svector<pair<int, double>> v;
      priority_queue<pair<int,double>,vector<pair<int,double>>,CompareDist> pq;

      // iterate through all users AGAIN
      for(map<int, map<int, int>>::iterator iter_21 = user_ratings.begin(); iter_21 != user_ratings.end(); ++iter_21) {
        double pearson_c = 0.0;
        int count = 0;

        int user2_id = iter_21->first;
        double user2_average = user_averages[user2_id];
        double user2_sd = user_sds[user2_id];

        map<int, int> ratings2 = iter_21->second;

        if (user1_id != user2_id) {
          // iterate through all the movies of user1
          for(map<int, int>::iterator iter2 = ratings1.begin(); iter2 != ratings1.end(); ++iter2) {
            int movie_id = iter2->first;
            // only compute pearson if user2 has rated the movie
            if (ratings2.count(movie_id)) {
              int rating1 = iter2->second;
              int rating2 = ratings2[movie_id];
              count++;
              pearson_c += ((rating1 - user1_average) / user1_sd) * ((rating2 - user2_average) / user2_sd);
            }
          }
          pearson_c = pearson_c / (count - 1);
          if (count >= 50 && pearson_c >= 0.5) {
            // w << "\t Column user2_id: " << user2_id << ", PC: " << pearson_c << endl;
            pair<int, double> p = make_pair(user2_id, pearson_c);
            pq.push(p);
            if (pq.size() > 50)
              pq.pop();
          }
        }
      }
      vector<pair<int, double>> v = Container(pq);
      //
      // sort(v.begin(), v.end(), [](const std::pair<int,double> &left, const std::pair<int,double> &right) {
      //   return left.second > right.second;
      // });

      pc_cache[user1_id] = v;

      for(vector<pair<int, double>>::iterator iter2 = pc_cache[user1_id].begin(); iter2 != pc_cache[user1_id].end(); ++iter2) {
        w << "\t Column user2_id: " << iter2->first << ", PC: " << iter2->second << endl;
      }
      break;
    }
  }

  ofstream ofs0("PearsonCorrelationCache");
  boost::archive::text_oarchive oarch0(ofs0);
  oarch0 << pc_cache;

}

void sd_cache(istream &r, ostream &w) {
  map<int, double> user_sds;

  map<int, double> user_averages;
  ifstream uac("UserAveragesCache");
  boost::archive::text_iarchive ia3(uac);
  ia3 >> user_averages;

  for (int i = 0; i < 100; ++i) {
    w << "Loading invertedindex" << i << endl;
    map<int, map<int, int>> user_ratings;
    ifstream aci("InvertedIndexes/InvertedIndex" + to_string(i));
    boost::archive::text_iarchive iai(aci);
    iai >> user_ratings;

    for(map<int, map<int, int>>::iterator iter = user_ratings.begin(); iter != user_ratings.end(); ++iter) {
      double user_sd = 0.0;
      int count = 0;

      int user_id = iter->first;
      double user_average = user_averages[user_id];

      map<int, int> ratings = iter->second;

      for(map<int, int>::iterator iter2 = ratings.begin(); iter2 != ratings.end(); ++iter2) {
        int movie_id = iter2->first;
        int rating = iter2->second;
        user_sd += pow(rating - user_average, 2);
        count++;
      }

      user_sd = user_sd / (count - 1);
      user_sd = sqrt(user_sd);
      user_sds[user_id] = user_sd;

      w << "User " << user_id << ", st: " << user_sd << endl;
    }
  }

  ofstream ofs0("AllUsersSDCache.bin");
  boost::archive::binary_oarchive oarch0(ofs0);
  oarch0 << user_sds;
}

void mean_cache(istream &r, ostream &w) {
  map<int, double> user_averages;

  for (int i = 0; i < 100; ++i) {
    w << "Loading invertedindex" << i << endl;
    map<int, map<int, int>> user_ratings;
    ifstream aci("InvertedIndexes/InvertedIndex" + to_string(i));
    boost::archive::text_iarchive iai(aci);
    iai >> user_ratings;

    for(map<int, map<int, int>>::iterator iter = user_ratings.begin(); iter != user_ratings.end(); ++iter) {
      double user_average = 0.0;
      int count = 0;

      int user_id = iter->first;
      map<int, int> ratings = iter->second;

      for(map<int, int>::iterator iter2 = ratings.begin(); iter2 != ratings.end(); ++iter2) {
        int movie_id = iter2->first;
        int rating = iter2->second;
        user_average += rating;
        count++;
      }

      user_average = user_average / count;
      user_averages[user_id] = user_average;

      w << "User " << user_id << ", mean: " << user_average << endl;
    }
  }

  ofstream ofs0("AllUsersAveragesCache.bin");
  boost::archive::binary_oarchive oarch0(ofs0);
  oarch0 << user_averages;

}

void user_predict(int user_id, int movie_id) {
  map<int, map<int, int>> user_ratings;
  ifstream mac("InvertedIndex.bin");
  boost::archive::binary_iarchive ia(mac);
  ia >> user_ratings;

  double averages_cache [17771];
  ifstream ac("AveragesCache");
  boost::archive::text_iarchive ia2(ac);
  ia2 >> averages_cache;

  map<int, double> user_averages;
  ifstream uac("UserAveragesCache");
  boost::archive::text_iarchive ia3(uac);
  ia3 >> user_averages;

  cout << user_ratings[497196][12] << endl; // 3
  cout << user_ratings[924104][12] << endl; // 3
  cout << user_ratings[2238663][12] << endl; // 5
  //
  // int user1_id = user_id;
  //
  // w << "Row user1_id: " << user1_id << endl;
  // double user1_average = user_averages[user1_id];
  // double user1_sd = user_sds[user1_id];
  //
  // map<int, int> ratings1 = user_ratings[user_id];
  //
  // // iterate through all users AGAIN
  // for(map<int, map<int, int>>::iterator iter_21 = user_ratings.begin(); iter_21 != user_ratings.end(); ++iter_21) {
  //   double pearson_c = 0.0;
  //   int count = 0;
  //
  //   int user2_id = iter_21->first;
  //   double user2_average = user_averages[user2_id];
  //   double user2_sd = user_sds[user2_id];
  //
  //   map<int, int> ratings2 = iter_21->second;
  //
  //   if (user1_id <= user2_id) {
  //     // iterate through all the movies of user1
  //     for(map<int, int>::iterator iter2 = ratings1.begin(); iter2 != ratings1.end(); ++iter2) {
  //       int movie_id = iter2->first;
  //       // only compute pearson if user2 has rated the movie
  //       if (ratings2.count(movie_id)) {
  //         int rating1 = iter2->second;
  //         int rating2 = ratings2[movie_id];
  //         count++;
  //         pearson_c += ((rating1 - user1_average) / user1_sd) * ((rating2 - user2_average) / user2_sd);
  //       }
  //     }
  //     pearson_c = pearson_c / (count - 1);
  //
  //     vector<pair<int, double>> all_pearsons;
  //     if (count >= 50) {
  //       // w << "\t Column user2_id: " << user2_id << ", PC: " << pearson_c << endl;
  //       pair<int, double> p1 = make_pair(user2_sd, pearson_c);
  //       all_pearsons.push_back(p1);
  //     }
  //
  //     sort(all_pearsons.begin(), all_pearsons.end(), [](const std::pair<int,double> &left, const std::pair<int,double> &right) {
  //       return left.second < right.second;
  //     });
  //
  //     for(std::vector<pair<int, double>>::iterator it = v.begin(); it != v.end(); ++it) {
  //       int movie_id = it->first;
  //       int pear = it->second;
  //       cout << movie_id << ": " << pear << endl;
  //     }
  //
  //   }
  // }

}

void predict(istream &r, ostream &w) {
  map<int, map<int, int>> new_all_movie_ratings;
  ifstream mac("MovieArchiveCache");
  boost::archive::text_iarchive ia(mac);
  ia >> new_all_movie_ratings;

  // movie averages
  // double averages_cache [17771];
  // ifstream ac("AveragesCache");
  // boost::archive::text_iarchive ia2(ac);
  // ia2 >> averages_cache;

  map<int, double> user_averages;
  ifstream uac("UserAveragesCache");
  boost::archive::text_iarchive ia3(uac);
  ia3 >> user_averages;

  map<int, double> user_sds;
  ifstream uac2("AllUsersSDCache.bin");
  boost::archive::binary_iarchive ia2(uac2);
  ia2 >> user_sds;

  int movie_id = 0;
  int user_id = 0;
  double movie_normalised_avg = 0.0;

  double rmse = 0;
  int count = 0;

  string line;
  while(getline(r, line)) {
    if (line.back() == ':') {
      w << line << endl;
      line.pop_back(); // remove : from the end
      movie_id = stoi(line);

      ostringstream movie_file_name;
      movie_file_name << internal << setfill('0') << setw(7) << movie_id;
      ifstream movie_file("netflix/training_set/mv_" + movie_file_name.str() + ".txt");

      if (movie_file.is_open()) {
        string line2;
        getline(movie_file,line2);
        line2.pop_back(); // remove : from the end
        // assert(stoi(line), movie_id); // confirms that it's the right file
        int u_count = 0;
        while (getline(movie_file,line2)) {
          istringstream ss(line2);
          string token;
          getline(ss, token, ',');
          int u_id = stoi(token);
          getline(ss, token, ',');
          int u_rating = stoi(token);
          if (user_sds[u_id] != 0) {
            double user_normalised = (u_rating - user_averages[u_id]) / user_sds[u_id];
            movie_normalised_avg += user_normalised;
            ++u_count;
          }
        }
        movie_normalised_avg /= u_count;
        movie_file.close();
      }

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

      // double prediction = (0.4*averages_cache[movie_id] + 0.6*user_averages[user_id]);
      double prediction = movie_normalised_avg * user_sds[user_id] + user_averages[user_id];
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
