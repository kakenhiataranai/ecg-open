/* math_basic_statistics.h */


#ifndef MATH_BASIC_STATISTICS_H
#define MATH_BASIC_STATISTICS_H


#include <vector>
#include <cmath>


class basic_statistics {

public:

 double mean(std::vector<double> data);
 double variance(std::vector<double> data);
 double standard_deviation(std::vector<double> data);

};


#endif