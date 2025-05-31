
#include "math_basic_statistics.h"


double basic_statistics::mean(std::vector<double> data) {
 double res = 0;
 int num = data.size();
 for (int i = 0; i < num; i++) {
  res += data[i];
 }
 res /= num;
 return res;
}

double basic_statistics::variance(std::vector<double> data) {
 double res = 0;
 double m = this->mean(data);
 int num = data.size();
 for (int i = 0; i < num; i++) {
  res += data[i] * data[i];
 }
 res /= num;
 res -= m * m;
 return res;
}

double basic_statistics::standard_deviation(std::vector<double> data) {
 double res = sqrt(this->variance(data));
 return res;
}