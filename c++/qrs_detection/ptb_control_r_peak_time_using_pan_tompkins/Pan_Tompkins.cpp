

#include "Pan_Tompkins.h"

#include <iostream>
#include <fstream>
#include <sstream>



Pantompkins::Pantompkins(){
 WINDOWSIZE=20;
 FS=360;
 BUFFSIZE=600;
 NOSAMPLE=-32000;
 DELAY=22;
 FILEorVECTOR=true;
 c_internal_vector=0;
}

Pantompkins::Pantompkins(int W, int F, int B, int N, int D, bool FV){
 WINDOWSIZE=W;
 FS=F;
 BUFFSIZE=B;
 NOSAMPLE=N;
 DELAY=D;
 FILEorVECTOR=FV;//True:File False:vector
 c_internal_vector=0;
}

void Pantompkins::set_vector(std::vector<dataType> &in){
 internal_vector=in;
 FILEorVECTOR=false;
 c_internal_vector=0;

}


//void Pantompkins::open_file(const char file_in[], const char file_out[]){
// fin = fopen(file_in, "r");
// fout = fopen(file_out, "w+");
//}
//
//void Pantompkins::close_file(){
// fclose(fin);
// fclose(fout);
//}
//
//dataType Pantompkins::input_file(){
// double num = NOSAMPLE;
// if (!feof(fin))
//  fscanf(fin, "%lf", &num);
// return num;
//}


dataType Pantompkins::input_vector(){
 if(c_internal_vector<internal_vector.size()){
  c_internal_vector++;
  return internal_vector[c_internal_vector-1];
 }
 else{
  return NOSAMPLE;
 }
}


//void Pantompkins::output_file(int out){
// fprintf(fout, "%d\n", out);
// results_bool.push_back(static_cast<bool>(out));
//}


void Pantompkins::output_vector(int out){
 results_bool.push_back(static_cast<bool>(out));
}


std::vector<bool> Pantompkins::get_results_bool(){
 return results_bool;
};


std::vector<double> Pantompkins::get_results_peak_plot() {
 std::vector<double> out;
 std::vector<bool> flag = get_results_bool();
 for (int i = 0; i < results_bool.size(); i++) {
  if (flag[i]) {
   //out.push_back(internal_vector[i]);
   out.push_back(1);
  } 
  else {
   out.push_back(0);
  }
 }
 return out;
};


std::vector<int> Pantompkins::get_results_index(){
 std::vector<int> out;
 for(int i=0;i<results_bool.size();i++){
  if(results_bool[i]){
   out.push_back(i);
  }
 }
 return out;
};


void Pantompkins::exec(){

 //ここをmallocにしないとだめ
 //dataType signal[BUFFSIZE], dcblock[BUFFSIZE], lowpass[BUFFSIZE], highpass[BUFFSIZE], derivative[BUFFSIZE], squared[BUFFSIZE], integral[BUFFSIZE], outputSignal[BUFFSIZE];


 dataType* signal;
 dataType* dcblock;
 dataType* lowpass;
 dataType* highpass;
 dataType* derivative;
 dataType* squared;
 dataType* integral;
 dataType* outputSignal;
 signal = new dataType[BUFFSIZE];
 dcblock = new dataType[BUFFSIZE];
 lowpass = new dataType[BUFFSIZE];
 highpass = new dataType[BUFFSIZE];
 derivative = new dataType[BUFFSIZE];
 squared = new dataType[BUFFSIZE];
 integral = new dataType[BUFFSIZE];
 outputSignal = new dataType[BUFFSIZE];

 

 int rr1[8], rr2[8], rravg1, rravg2, rrlow = 0, rrhigh = 0, rrmiss = 0;
 long unsigned int i, j, sample = 0, lastQRS = 0, lastSlope = 0, currentSlope = 0;
 int current;
 dataType peak_i = 0, peak_f = 0, threshold_i1 = 0, threshold_i2 = 0, threshold_f1 = 0, threshold_f2 = 0, spk_i = 0, spk_f = 0, npk_i = 0, npk_f = 0;
 bool qrs, regular = true, prevRegular;

 // Initializing the RR averages
 for (i = 0; i < 8; i++){
        rr1[i] = 0;
        rr2[i] = 0;
    }

    // The main loop
    do{
  if (sample >= BUFFSIZE)
  {
   for (i = 0; i < BUFFSIZE - 1; i++)
   {
    signal[i] = signal[i+1];
    dcblock[i] = dcblock[i+1];
    lowpass[i] = lowpass[i+1];
    highpass[i] = highpass[i+1];
    derivative[i] = derivative[i+1];
    squared[i] = squared[i+1];
    integral[i] = integral[i+1];
    outputSignal[i] = outputSignal[i+1];
   }
   current = BUFFSIZE - 1;
  }
  else
  {
   current = sample;
  }
  if(FILEorVECTOR){
   //signal[current] = input_file();
  }
  else{
   signal[current] = input_vector();
  }

  // If no sample was read, stop processing!
  if (signal[current] == NOSAMPLE)
   break;
  sample++; // Update sample counter

  // DC Block filter
  // This was not proposed on the original paper.
  // It is not necessary and can be removed if your sensor or database has no DC noise.
  if (current >= 1)
   dcblock[current] = signal[current] - signal[current-1] + 0.995*dcblock[current-1];
  else
   dcblock[current] = 0;

  // Low Pass filter
  // Implemented as proposed by the original paper.
  // y(nT) = 2y(nT - T) - y(nT - 2T) + x(nT) - 2x(nT - 6T) + x(nT - 12T)
  // Can be removed if your signal was previously filtered, or replaced by a different filter.
  lowpass[current] = dcblock[current];
  if (current >= 1)
   lowpass[current] += 2*lowpass[current-1];
  if (current >= 2)
   lowpass[current] -= lowpass[current-2];
  if (current >= 6)
   lowpass[current] -= 2*dcblock[current-6];
  if (current >= 12)
   lowpass[current] += dcblock[current-12];

  // High Pass filter
  // Implemented as proposed by the original paper.
  // y(nT) = 32x(nT - 16T) - [y(nT - T) + x(nT) - x(nT - 32T)]
  // Can be removed if your signal was previously filtered, or replaced by a different filter.
  highpass[current] = -lowpass[current];
  if (current >= 1)
   highpass[current] -= highpass[current-1];
  if (current >= 16)
   highpass[current] += 32*lowpass[current-16];
  if (current >= 32)
   highpass[current] += lowpass[current-32];

  // Derivative filter
  // This is an alternative implementation, the central difference method.
  // f'(a) = [f(a+h) - f(a-h)]/2h
  // The original formula used by Pan-Tompkins was:
  // y(nT) = (1/8T)[-x(nT - 2T) - 2x(nT - T) + 2x(nT + T) + x(nT + 2T)]
        derivative[current] = highpass[current];
  if (current > 0)
   derivative[current] -= highpass[current-1];

  // This just squares the derivative, to get rid of negative values and emphasize high frequencies.
  // y(nT) = [x(nT)]^2.
  squared[current] = derivative[current]*derivative[current];

  // Moving-Window Integration
  // Implemented as proposed by the original paper.
  // y(nT) = (1/N)[x(nT - (N - 1)T) + x(nT - (N - 2)T) + ... x(nT)]
  // WINDOWSIZE, in samples, must be defined so that the window is ~150ms.

  integral[current] = 0;
  for (i = 0; i < WINDOWSIZE; i++)
  {
   if (current >= (dataType)i)
    integral[current] += squared[current - i];
   else
    break;
  }
  integral[current] /= (dataType)i;

  qrs = false;

  // If the current signal is above one of the thresholds (integral or filtered signal), it's a peak candidate.
        if (integral[current] >= threshold_i1 || highpass[current] >= threshold_f1)
        {
            peak_i = integral[current];
            peak_f = highpass[current];
        }

  // If both the integral and the signal are above their thresholds, they're probably signal peaks.
  if ((integral[current] >= threshold_i1) && (highpass[current] >= threshold_f1))
  {
   // There's a 200ms latency. If the new peak respects this condition, we can keep testing.
   if (sample > lastQRS + FS/5)
   {
       // If it respects the 200ms latency, but it doesn't respect the 360ms latency, we check the slope.
    if (sample <= lastQRS + (long unsigned int)(0.36*FS))
    {
        // The squared slope is "M" shaped. So we have to check nearby samples to make sure we're really looking
        // at its peak value, rather than a low one.
        currentSlope = 0;
        for (j = current - 10; j <= current; j++)
                        if (squared[j] > currentSlope)
                            currentSlope = squared[j];

        if (currentSlope <= (dataType)(lastSlope/2))
                    {
                        qrs = false;
                    }

                    else
                    {
                        spk_i = 0.125*peak_i + 0.875*spk_i;
                        threshold_i1 = npk_i + 0.25*(spk_i - npk_i);
                        threshold_i2 = 0.5*threshold_i1;

                        spk_f = 0.125*peak_f + 0.875*spk_f;
                        threshold_f1 = npk_f + 0.25*(spk_f - npk_f);
                        threshold_f2 = 0.5*threshold_f1;

                        lastSlope = currentSlope;
                        qrs = true;
                    }
    }
    // If it was above both thresholds and respects both latency periods, it certainly is a R peak.
    else
    {
        currentSlope = 0;
                    for (j = current - 10; j <= current; j++)
                        if (squared[j] > currentSlope)
                            currentSlope = squared[j];

                    spk_i = 0.125*peak_i + 0.875*spk_i;
                    threshold_i1 = npk_i + 0.25*(spk_i - npk_i);
                    threshold_i2 = 0.5*threshold_i1;

                    spk_f = 0.125*peak_f + 0.875*spk_f;
                    threshold_f1 = npk_f + 0.25*(spk_f - npk_f);
                    threshold_f2 = 0.5*threshold_f1;

                    lastSlope = currentSlope;
                    qrs = true;
    }
   }
   // If the new peak doesn't respect the 200ms latency, it's noise. Update thresholds and move on to the next sample.
   else
            {
                peak_i = integral[current];
    npk_i = 0.125*peak_i + 0.875*npk_i;
    threshold_i1 = npk_i + 0.25*(spk_i - npk_i);
    threshold_i2 = 0.5*threshold_i1;
    peak_f = highpass[current];
    npk_f = 0.125*peak_f + 0.875*npk_f;
    threshold_f1 = npk_f + 0.25*(spk_f - npk_f);
    threshold_f2 = 0.5*threshold_f1;
    qrs = false;
    outputSignal[current] = qrs;
    if (sample > DELAY + BUFFSIZE){
     if(FILEorVECTOR){
      //output_file(outputSignal[0]);
     }
     else{
      output_vector(outputSignal[0]);
     }
    }
    continue;
   }
  }

  // If a R-peak was detected, the RR-averages must be updated.
  if (qrs)
  {
   // Add the newest RR-interval to the buffer and get the new average.
   rravg1 = 0;
   for (i = 0; i < 7; i++)
   {
    rr1[i] = rr1[i+1];
    rravg1 += rr1[i];
   }
   rr1[7] = sample - lastQRS;
   lastQRS = sample;
   rravg1 += rr1[7];
   rravg1 *= 0.125;

   // If the newly-discovered RR-average is normal, add it to the "normal" buffer and get the new "normal" average.
   // Update the "normal" beat parameters.
   if ( (rr1[7] >= rrlow) && (rr1[7] <= rrhigh) )
   {
    rravg2 = 0;
    for (i = 0; i < 7; i++)
    {
     rr2[i] = rr2[i+1];
     rravg2 += rr2[i];
    }
    rr2[7] = rr1[7];
    rravg2 += rr2[7];
    rravg2 *= 0.125;
    rrlow = 0.92*rravg2;
    rrhigh = 1.16*rravg2;
    rrmiss = 1.66*rravg2;
   }

   prevRegular = regular;
   if (rravg1 == rravg2)
   {
    regular = true;
   }
   // If the beat had been normal but turned odd, change the thresholds.
   else
   {
    regular = false;
    if (prevRegular)
    {
     threshold_i1 /= 2;
     threshold_f1 /= 2;
    }
   }
  }
  // If no R-peak was detected, it's important to check how long it's been since the last detection.
  else
  {
      // If no R-peak was detected for too long, use the lighter thresholds and do a back search.
   // However, the back search must respect the 200ms limit and the 360ms one (check the slope).
   if ((sample - lastQRS > (long unsigned int)rrmiss) && (sample > lastQRS + FS/5))
   {
    for (i = current - (sample - lastQRS) + FS/5; i < (long unsigned int)current; i++)
    {
     if ( (integral[i] > threshold_i2) && (highpass[i] > threshold_f2))
     {
         currentSlope = 0;
                        for (j = i - 10; j <= i; j++)
                            if (squared[j] > currentSlope)
                                currentSlope = squared[j];

                        if ((currentSlope < (dataType)(lastSlope/2)) && (i + sample) < lastQRS + 0.36*lastQRS)
                        {
                            qrs = false;
                        }
                        else
                        {
                            peak_i = integral[i];
                            peak_f = highpass[i];
                            spk_i = 0.25*peak_i+ 0.75*spk_i;
                            spk_f = 0.25*peak_f + 0.75*spk_f;
                            threshold_i1 = npk_i + 0.25*(spk_i - npk_i);
                            threshold_i2 = 0.5*threshold_i1;
                            lastSlope = currentSlope;
                            threshold_f1 = npk_f + 0.25*(spk_f - npk_f);
                            threshold_f2 = 0.5*threshold_f1;
                            // If a signal peak was detected on the back search, the RR attributes must be updated.
                            // This is the same thing done when a peak is detected on the first try.
                            //RR Average 1
                            rravg1 = 0;
                            for (j = 0; j < 7; j++)
                            {
                                rr1[j] = rr1[j+1];
                                rravg1 += rr1[j];
                            }
                            rr1[7] = sample - (current - i) - lastQRS;
                            qrs = true;
                            lastQRS = sample - (current - i);
                            rravg1 += rr1[7];
                            rravg1 *= 0.125;

                            //RR Average 2
                            if ( (rr1[7] >= rrlow) && (rr1[7] <= rrhigh) )
                            {
                                rravg2 = 0;
                                for (i = 0; i < 7; i++)
                                {
                                    rr2[i] = rr2[i+1];
                                    rravg2 += rr2[i];
                                }
                                rr2[7] = rr1[7];
                                rravg2 += rr2[7];
                                rravg2 *= 0.125;
                                rrlow = 0.92*rravg2;
                                rrhigh = 1.16*rravg2;
                                rrmiss = 1.66*rravg2;
                            }

                            prevRegular = regular;
                            if (rravg1 == rravg2)
                            {
                                regular = true;
                            }
                            else
                            {
                                regular = false;
                                if (prevRegular)
                                {
                                    threshold_i1 /= 2;
                                    threshold_f1 /= 2;
                                }
                            }

                            break;
                        }
                    }
    }

    if (qrs)
          {
                    outputSignal[current] = false;
                    outputSignal[i] = true;
                    if (sample > DELAY + BUFFSIZE){
                     if(FILEorVECTOR){
                      //output_file(outputSignal[0]);
                     }
                     else{output_vector(outputSignal[0]);}
                    }
                    continue;
                }
   }

   // Definitely no signal peak was detected.
   if (!qrs)
   {
    // If some kind of peak had been detected, then it's certainly a noise peak. Thresholds must be updated accordinly.
    if ((integral[current] >= threshold_i1) || (highpass[current] >= threshold_f1))
    {
     peak_i = integral[current];
     npk_i = 0.125*peak_i + 0.875*npk_i;
     threshold_i1 = npk_i + 0.25*(spk_i - npk_i);
     threshold_i2 = 0.5*threshold_i1;
     peak_f = highpass[current];
     npk_f = 0.125*peak_f + 0.875*npk_f;
     threshold_f1 = npk_f + 0.25*(spk_f - npk_f);
     threshold_f2 = 0.5*threshold_f1;
    }
   }
  }

  outputSignal[current] = qrs;
  if (sample > DELAY + BUFFSIZE){
   if(FILEorVECTOR){
    //output_file(outputSignal[0]);
   }
   else{output_vector(outputSignal[0]);}
  }
 } while (signal[current] != NOSAMPLE);


 // Output the last remaining samples on the buffer
 for (i = 1; i < BUFFSIZE; i++){
  if(FILEorVECTOR){
   //output_file(outputSignal[0]);
  }
  else{output_vector(outputSignal[0]);}
 }


 delete[] signal;
 delete[] dcblock;
 delete[] lowpass;
 delete[] highpass;
 delete[] derivative;
 delete[] squared;
 delete[] integral;
 delete[] outputSignal;

}