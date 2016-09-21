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
#include "Netflix.h"

using namespace std;

void build_actual_cache(map<int, map<int, int>> &actual_ratings_cache) {
    ifstream cache_file("gca386-ActualRatingsCache.txt");

    if (cache_file.is_open()) {
        int movie_id, user_id, rating;
        while (cache_file >> movie_id >> user_id >> rating)
        {
            actual_ratings_cache[movie_id][user_id] = rating;
        }
        cache_file.close();
    }
}

void build_user_averages_cache(map<int, double> &user_averages) {
    ifstream cache_file("gca386-AllUsersAveragesCache.txt");

    if (cache_file.is_open()) {
        int user_id;
        double avg;
        while (cache_file >> user_id >> avg)
        {
            user_averages[user_id] = avg;
        }
        cache_file.close();
    }
}

void build_user_sds_cache(map<int, double> &user_sds) {
    ifstream cache_file("gca386-AllUsersSDCache.txt");

    if (cache_file.is_open()) {
        int user_id;
        double sd;
        while (cache_file >> user_id >> sd)
        {
            user_sds[user_id] = sd;
        }
        cache_file.close();
    }
}

void build_movie_norm_ratings_cache(map<int, double> &movie_norm_ratings) {
    ifstream cache_file("gca386-MovieNormMean.txt");

    if (cache_file.is_open()) {
        int movie_id;
        double mean;
        while (cache_file >> movie_id >> mean)
        {
            movie_norm_ratings[movie_id] = mean;
        }
        cache_file.close();
    }
}

int predict(istream &r, ostream &w) {
    map<int, map<int, int>> actual_ratings;
    build_actual_cache(actual_ratings);

    map<int, double> user_averages;
    build_user_averages_cache(user_averages);

    map<int, double> user_sds;
    build_user_sds_cache(user_sds);

    map<int, double> movie_norm_ratings;
    build_movie_norm_ratings_cache(movie_norm_ratings);

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

            // double prediction = (0.4*averages_cache[movie_id] + 0.6*user_averages[user_id]);
            double prediction = movie_norm_ratings[movie_id] * user_sds[user_id] + user_averages[user_id];
            int actual = actual_ratings[movie_id][user_id];
            ++count;
            prediction = floor(prediction * 10) / 10;
            rmse += pow(prediction - actual, 2);

            w << prediction << endl;
        }
    }

    rmse = sqrt(rmse / count);
    rmse = floor(rmse * 100) / 100; // truncate to two decimal places
    w << "RMSE: " << rmse;
    return rmse;
}