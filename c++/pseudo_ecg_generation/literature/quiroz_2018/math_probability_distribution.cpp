
#include "math_probability_distribution.h"


#define PI 3.141592653589793



double normal_distribution::probability_density(double x, double meam, double standard_deviation) {
 double res;
 res = exp(-0.5 * (x - meam) * (x - meam) / (standard_deviation * standard_deviation)) / (standard_deviation * sqrt(2 * PI));
 return res;
}
