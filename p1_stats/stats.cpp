// stats.cpp
#include "stats.hpp"
#include <cassert>
#include <cstddef>
#include <vector>
#include <algorithm> // sort
#include <cmath> // sqrt, modf

using namespace std;

int count(vector<double> v) {
    return v.size();
}

double sum(vector<double> v) {
    assert(v.size() > 0);
    double sum = 0.0;
    for (size_t i = 0; i < v.size(); i += 1) {
        sum += v[i];
    }
    return sum;
}

double mean(vector<double> v) {
    assert(v.size() > 0);
    return sum(v) / count(v);
}

double median(vector<double> v) {
    assert(v.size() > 0);
    std::sort(v.begin(), v.end());
    if (v.size() % 2 == 0) {
        return (v[v.size() / 2 - 1] + v[v.size() / 2]) / 2;
    } else {
        return v[v.size() / 2];
    }
}

double min(vector<double> v) {
    assert(v.size() > 0);
    std::sort(v.begin(), v.end());
    return v[0];
}

double max(vector<double> v) {
    assert(v.size() > 0);
    std::sort(v.begin(), v.end());
    return v[v.size() - 1];
}

double stdev(vector<double> v) {
    assert(v.size() >= 2);
    double m = mean(v);
    double var = 0.0;
    for (size_t i = 0; i < v.size(); i += 1) {
        var += (v[i] - m) * (v[i] - m);
    }
    return sqrt(var / (v.size() - 1));
}

double percentile(vector<double> v, double p) {
    assert(v.size() > 0);
    assert(p <= 1);
    assert(p >= 0);
    std::sort(v.begin(), v.end());
    double rank = p * (v.size() - 1) + 1;
    double int_part = 0.0;
    double frac_part = 0.0;
    frac_part = modf(rank, &int_part);

    if (p == 1.0) return v[int_part - 1];

    return v[int_part - 1] + frac_part * (v[int_part] - v[int_part - 1]);
}

vector<double> filter(vector<double> v, vector<double> criteria, double target) {
    assert(v.size() == criteria.size());
    vector<double> filtered;
    for (size_t i = 0; i < v.size(); i += 1) {
        if (criteria[i] == target) {
            filtered.push_back(v[i]);
        }
    }
    return filtered;
}
