#!/bin/sh
g++ -o out 12_lead_ECG_test_main.cpp generate_12_lead_ecg.cpp rr_interval_using_fftw.cpp math_random_number.cpp math_basic_statistics.cpp math_probability_distribution.cpp math_interpolation.cpp -lm -lfftw3
