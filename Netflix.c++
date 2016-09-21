// ----------------------------
// Copyright (C) 2016
// Giovanni Alcantara
// ----------------------------

// --------
// includes
// --------

#include <cassert>  // assert
#include <iostream> // endl, istream, ostream
#include <fstream>
#include <math.h>
#include <map>
#include <boost/serialization/map.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include "Netflix.h"

using namespace std;

int predict(istream &r, ostream &w) {
    map<int, map<int, int>> new_all_movie_ratings;
//    ifstream mac("MovieArchiveCache");
//    boost::archive::text_iarchive ia(mac);
//    ia >> new_all_movie_ratings;
//
//    map<int, double> user_averages;
//    ifstream uac("UserAveragesCache");
//    boost::archive::text_iarchive ia3(uac);
//    ia3 >> user_averages;

    map<int, double> user_sds;
    ifstream uac2("AllUsersSDCache.bin");
    boost::archive::binary_iarchive ia2(uac2);
    ia2 >> user_sds;

    map<int, double> movie_norm_ratings;
    ifstream mns("MovieNormMean.bin");
    boost::archive::binary_iarchive ia4(mns);
    ia4 >> movie_norm_ratings;

    int movie_id = 0;
    int user_id = 0;

    double rmse = 0;
    int count = 0;

//    string line;
//    while(getline(r, line)) {
//        if (line.back() == ':') {
//            w << line << endl;
//            line.pop_back(); // remove : from the end
//            movie_id = stoi(line);
//
//        } else {
//            user_id = stoi(line);
//
//            // double prediction = (0.4*averages_cache[movie_id] + 0.6*user_averages[user_id]);
//            double prediction = movie_norm_ratings[movie_id] * user_sds[user_id] + user_averages[user_id];
//            int actual = new_all_movie_ratings[movie_id][user_id];
//            ++count;
//            prediction = floor(prediction * 10) / 10;
//            rmse += pow(prediction - actual, 2);
//
//            w << prediction << endl;
//        }
//    }
//
//    rmse = sqrt(rmse / count);
//    rmse = floor(rmse * 100) / 100; // truncate to two decimal places
    w << "RMSE: " << rmse;
    return rmse;
}
