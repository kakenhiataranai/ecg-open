#!/bin/sh
g++ -o out case_1_ecg_12_lead_McSharry_Tabassum_main.cpp case_1_ecg_McSharry_model_using_fftw.cpp ecg_model_parameter.cpp -lm -O3
