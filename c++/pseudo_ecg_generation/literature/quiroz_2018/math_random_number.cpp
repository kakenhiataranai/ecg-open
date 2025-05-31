
#include "math_random_number.h"



uniform_dist_random_number::uniform_dist_random_number(int mt_seed, double lower_bound, double upper_bound) : uniform_dist(lower_bound, upper_bound) {
 mt_engine.seed(mt_seed);
}

double uniform_dist_random_number::generate_random_number() {
 double res = uniform_dist(mt_engine);
 return res;
}



normal_dist_random_number::normal_dist_random_number(int mt_seed, double mean, double standard_dev) : normal_dist(mean, standard_dev) {
 mt_engine.seed(mt_seed);
}

double normal_dist_random_number::generate_random_number() {
 double res = normal_dist(mt_engine);
 return res;
}
