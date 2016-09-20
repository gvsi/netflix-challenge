//
// Created by Giovanni Alcantara on 9/19/16.
//

void movie_norm_mean_cache(istream &r, ostream &w) {
    map<int, double> norm_mean_cache;

    int movie_id = 0;

    map<int, double> user_averages;
    ifstream uac("AllUsersAveragesCache.bin");
    boost::archive::binary_iarchive ia3(uac);
    ia3 >> user_averages;

    map<int, double> user_sds;
    ifstream uac2("AllUsersSDCache.bin");
    boost::archive::binary_iarchive ia2(uac2);
    ia2 >> user_sds;

    double movie_normalised_avg = 0.0;

    string line;
    while(getline(r, line)) {
        if (line.back() == ':') {
            line.pop_back(); // remove : from the end
            movie_id = stoi(line);

            ostringstream movie_file_name;
            movie_file_name << internal << setfill('0') << setw(7) << movie_id;
            ifstream movie_file("netflix/training_set/mv_" + movie_file_name.str() + ".txt");

            if (movie_file.is_open()) {
                string line2;
                getline(movie_file,line2);
                line2.pop_back(); // remove : from the end
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

                w << movie_id << " " << movie_normalised_avg << endl;
                norm_mean_cache[movie_id] = movie_normalised_avg;
                movie_file.close();
            }
        }
    }

    ofstream ofs0("MovieNormMean.bin");
    boost::archive::binary_oarchive oarch0(ofs0);
    oarch0 << norm_mean_cache;
}

void sd_cache(istream &r, ostream &w) {
    map<int, double> user_sds;

    map<int, double> user_averages;
    ifstream uac("AllUsersAveragesCache.bin");
    boost::archive::binary_iarchive ia3(uac);
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
            if (count == 1) {
                count = 2;
            }
            user_sd = user_sd / (count - 1);
            user_sd = sqrt(user_sd);

            user_sds[user_id] = user_sd;

            w << user_id << " " << user_sd << endl;
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
