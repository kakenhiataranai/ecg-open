/* math_random_number.h */


#ifndef MATH_RANDOM_NUMBER_H
#define MATH_RANDOM_NUMBER_H


#include <random>


class random_number {

public:

 virtual double generate_random_number() = 0;

protected:

 std::mt19937_64 mt_engine;

};


class uniform_dist_random_number : public random_number {

public:

 uniform_dist_random_number(int mt_seed, double lower_bound, double upper_bound);  
 double generate_random_number();   /* [ lower_bound , upper_bound ) の一様乱数を生成  */

private:

 std::uniform_real_distribution<> uniform_dist;

};


class normal_dist_random_number : public random_number {

public:

 normal_dist_random_number(int mt_seed, double mean, double standard_deviation);
 double generate_random_number();

private:

 std::normal_distribution<> normal_dist;

};




#endif