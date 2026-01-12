/* stats_tests.cpp
 *
 * Unit tests for the simple statistics library
 *
 * EECS 280 Statistics Project
 *
 * Protip #1: Write tests for the functions BEFORE you implement them!  For
 * example, write tests for median() first, and then write median().  It sounds
 * like a pain, but it helps make sure that you are never under the illusion
 * that your code works when it's actually full of bugs.
 *
 * Protip #2: Instead of putting all your tests in main(),  put each test case
 * in a function!
 */


#include "stats.hpp"
#include <iostream>
#include <cassert>
#include <ostream>
#include <vector>
#include <cmath>
using namespace std;

void test_count();

void test_sum();

void test_mean();

void test_median();

void test_min();

void test_max();

void test_stdev();

void test_percentile();

void test_filter();


// Add prototypes for you test functions here.

int main() {
    test_count();
    test_sum();
    test_mean();
    test_median();
    test_min();
    test_max();
    test_stdev();
    test_percentile();
    test_filter();
    cout << "All tests passed!" << endl;
    return 0;
}

void test_count() {
    cout << "test_count" << endl;
    vector<double> data = {1.0, 2.0, 3.0, 4.0};
    assert(count(data) == 4);
    assert(count({}) == 0);
    cout << "PASS!" << endl;
}

void test_sum() {
    cout << "test_sum" << endl;
    assert(sum({1.0, 2.0, 3.0}) == 6.0);
    assert(sum({-1.0, 1.0}) == 0.0);
    cout << "PASS!" << endl;
}

void test_mean() {
    cout << "test_mean" << endl;
    assert(mean({1.0, 2.0, 3.0}) == 2.0);
    assert(mean({-1.5, 2.7, 5.235}) == 2.145);
    assert(mean({5.0}) == 5.0);
    cout << "PASS!" << endl;
}

void test_median() {
    cout << "test_median" << endl;
    assert(median({1.0, 3.0, 2.0}) == 2.0);
    assert(median({1.0, 2.0, 3.0, 4.0}) == 2.5);
    assert(median({-1.0, -2.0, 3.0, 4.0}) == 1.0);
    assert(median({5.0}) == 5.0);
    cout << "PASS!" << endl;
}

void test_min() {
    cout << "test_min" << endl;
    assert(min({5.0, 1.0, 3.0}) == 1.0);
    assert(min({-1.0, -5.0, 0.0}) == -5.0);
    cout << "PASS!" << endl;
}

void test_max() {
    cout << "test_max" << endl;
    assert(max({5.0, 1.0, 3.0}) == 5.0);
    assert(max({-1.0, -5.0, 0.0}) == 0.0);
    assert(max({0.0, 0.0}) == 0.0);
    cout << "PASS!" << endl;
}

void test_stdev() {
    cout << "test_stdev" << endl;
    vector<double> data = {2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0};
    double s = stdev(data);
    cout << s << endl;
    assert(std::abs(s - 2.138) < 0.001);

    cout << "PASS!" << endl;
}

void test_percentile() {
    cout << "test_percentile" << endl;
    vector<double> data = {15, 20, 35, 40, 50};

    assert(std::abs(percentile(data, 0.0) - 15) < 0.00001);
    assert(std::abs(percentile(data, 0.25) - 20) < 0.00001);
    assert(std::abs(percentile(data, 0.5) - 35) < 0.00001);
    assert(std::abs(percentile(data, 0.75) - 40) < 0.00001);
    assert(std::abs(percentile(data, 1.0) - 50) < 0.00001);

    assert(std::abs(percentile(data, 0.4) - 29) < 0.00001);
    assert(std::abs(percentile(data, 0.8) - 42) < 0.00001);

    cout << "PASS!" << endl;
}

void test_filter() {
    cout << "test_filter" << endl;
    vector<double> values = {10.0, 20.0, 30.0, 40.0};
    vector<double> labels = {1.0, 2.0, 1.0, 2.0};

    vector<double> filtered = filter(values, labels, 1.0);
    assert(filtered.size() == 2);
    assert(filtered[0] == 10.0);
    assert(filtered[1] == 30.0);

    filtered = filter(values, labels, 3.0);
    assert(filtered.empty());

    cout << "PASS!" << endl;
}
