#include <math.h>
#include "pav_analysis.h"


float compute_power(const float *x, unsigned int N){
    float pow = 0;
    
    for (int i = 0; i < N; i++)
    {
        pow += x[i]*x[i];
    }
    return 10*log10(pow/N);
    
}

float compute_am(const float *x, unsigned int N){
    float amp = 0;
    for (int i = 0; i < N; i++)
    {
        amp += fabs(x[i]);
    }

    return amp/N;
    
}

float compute_zcr(const float *x, unsigned int N, float fm){
   float zcr = 0;
   for (int i = 1; i < N-1; i++)
   {
       if((x[i]>0 && x[i-1]<0) || (x[i]<0 && x[i-1]>0)){
           zcr+=1;
       }
   }
   return (fm*zcr)/(2*(N-1));
   
}

