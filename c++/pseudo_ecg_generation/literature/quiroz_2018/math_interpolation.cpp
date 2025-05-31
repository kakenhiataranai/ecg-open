
#include "math_interpolation.h"



double linear_interpolation::execute_linear_interpolation(double x, double x_1, double x_2, double y_1, double y_2) {
 double res = y_1;
 res += (y_2 - y_1) / (x_2 - x_1) * (x - x_1);
 return res;
}
