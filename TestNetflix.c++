// --------------------------------
// projects/collatz/TestCollatz.c++
// Copyright (C) 2016
// Glenn P. Downing
// --------------------------------

// https://code.google.com/p/googletest/wiki/V1_7_Primer#Basic_Assertions

// --------
// includes
// --------

#include <iostream> // cout, endl
#include <sstream>  // istringtstream, ostringstream
#include <string>   // string
#include <utility>  // pair
#include <map> // map

#include "gtest/gtest.h"

#include "Netflix.h"

using namespace std;

// ----
// build_actual_cache
// ----


TEST(NetflixFixture, read_actual_cache) {
    map<int, map<int, int>> actual_ratings;
    build_actual_cache(actual_ratings);
    actual_ratings[109516][3441] = 3;
    actual_ratings[10729][510123] = 5;
    actual_ratings[16265][265] = 5;
}
//
//// ----
//// eval
//// ----
//
//TEST(CollatzFixture, eval_2) {
//  const int v = collatz_eval(100, 200);
//  ASSERT_EQ(125, v);
//}
//
//TEST(CollatzFixture, eval_3) {
//  const int v = collatz_eval(201, 210);
//  ASSERT_EQ(89, v);
//}
//
//TEST(CollatzFixture, eval_4) {
//  const int v = collatz_eval(900, 1000);
//  ASSERT_EQ(174, v);
//}
//
//TEST(CollatzFixture, eval_5) {
//  const int v = collatz_eval(17717, 21404);
//  ASSERT_EQ(274, v);
//}
//
//TEST(CollatzFixture, eval_6) {
//  const int v = collatz_eval(28162, 81335);
//  ASSERT_EQ(351, v);
//}
//
//TEST(CollatzFixture, eval_7) {
//  const int v = collatz_eval(18028, 22718);
//  ASSERT_EQ(274, v);
//}
//
//TEST(CollatzFixture, eval_8) {
//  const int v = collatz_eval(20033, 40174);
//  ASSERT_EQ(324, v);
//}
//
//// -----
//// print
//// -----
//
//TEST(CollatzFixture, print) {
//  ostringstream w;
//  collatz_print(w, 1, 10, 20);
//  ASSERT_EQ("1 10 20\n", w.str());
//}
//
//// -----
//// solve
//// -----
//
//TEST(CollatzFixture, solve) {
//  istringstream r("1 10\n100 200\n201 210\n900 1000\n");
//  ostringstream w;
//  collatz_solve(r, w);
//  ASSERT_EQ("1 10 20\n100 200 125\n201 210 89\n900 1000 174\n", w.str());
//}
//
///**
// * verifies that max_cycle_length(a, b) == max_cycle_length(b, a)
// */
//TEST(CollatzFixture, solve_inverse) {
//  istringstream r("4944 48637\n48637 4944\n");
//  ostringstream w;
//  collatz_solve(r, w);
//  ASSERT_EQ("4944 48637 324\n48637 4944 324\n", w.str());
//}
//
///**
//* verifies that max_cycle_length(a, a) == cycle_length(a)
// */
//TEST(CollatzFixture, solve_single) {
//  istringstream r("247272 247272\n");
//  ostringstream w;
//  collatz_solve(r, w);
//  ASSERT_EQ("247272 247272 94\n", w.str());
//}
