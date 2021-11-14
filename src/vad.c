#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "vad.h"
#include "pav_analysis.h"

const float FRAME_TIME = 10.0F; /* in ms. */
const double INIT_FRAMES = 12;         /* in frames */
// const float A1 = 5;               /* threshold over noise for k1 */
// const float A2 = 4;              /* threshold over noise for k2 */
// const double MIN_FRAMES_SV = 2;       /* Time in frames to change from silence to voice */
// const double MIN_FRAMES_VS = 11;      /* Time in frames to change from voice to silence */


/* 
 * As the output state is only ST_VOICE, ST_SILENCE, or ST_UNDEF,
 * only this labels are needed. You need to add all labels, in case
 * you want to print the internal state in string format
 */

const char *state_str[] = {
  "UNDEF", "S", "V", "INIT"
};

const char *state2str(VAD_STATE st) {
  return state_str[st];
}

/* Define a datatype with interesting features */
typedef struct {
  //float zcr;
  float p;
  //float am;

} Features;

/* 
 * TODO: Delete and use your own features!
 */

Features compute_features(const float *x, int N) {
  /*
   * Input: x[i] : i=0 .... N-1 
   * Ouput: computed features
   */
  Features feat;
  //feat.zcr = compute_zcr(x,N,sampling_rate);
  feat.p = compute_power(x,N);
  //feat.am = compute_am(x,N);

  return feat;
}

/* 
 * TODO: Init the values of vad_data
 */

VAD_DATA * vad_open(float rate,  float a1, float a2, int sv, int vs) {
  VAD_DATA *vad_data = malloc(sizeof(VAD_DATA));
  vad_data->state = ST_INIT;
  vad_data->laststate = ST_INIT;

  vad_data->frame_length = rate * FRAME_TIME * 1e-3;
  vad_data->frames_VS = vs;
  vad_data->frames_SV = sv;
  vad_data->max_VS = vs;
  vad_data->max_SV = sv;
  vad_data->a1 = a1;
  vad_data->a2 = a2;
  vad_data->initFrames = INIT_FRAMES;
  return vad_data;
}

VAD_STATE vad_close(VAD_DATA *vad_data) {
  /* 
   * TODO: decide what to do with the last undecided frames
   */

  if (vad_data->state == ST_UNDEF){
    if(vad_data->last_feature < vad_data->k1){
      vad_data->state = ST_SILENCE; 
      vad_data->laststate = ST_SILENCE;
    }else{
      vad_data->state = ST_VOICE;
      vad_data->laststate = ST_VOICE;
    }
  }

  VAD_STATE state = vad_data->state;

  free(vad_data);
  return state;
}

unsigned int vad_frame_size(VAD_DATA *vad_data) {
  return vad_data->frame_length;
}

/* 
 * TODO: Implement the Voice Activity Detection 
 * using a Finite State Automata
 */

VAD_STATE vad(VAD_DATA *vad_data, float *x) {

  /* 
   * TODO: You can change this, using your own features,
   * program finite state automaton, define conditions, etc.
   */

  Features f = compute_features(x, vad_data->frame_length);
  vad_data->last_feature = f.p; /* save feature, in case you want to show */

  switch (vad_data->state) {
  case ST_INIT:
    //For the Init frames we calculate the median power level and median Zero crossing rate
    vad_data->initFrames -= 1;
    vad_data->k0 += pow(10,(f.p)/10);

    if(vad_data->initFrames <= 0){
      vad_data->k0 = 10*log10((vad_data->k0)/INIT_FRAMES);
      vad_data->k1 = (vad_data->k0) + (vad_data->a1);
      vad_data->k2 = (vad_data->k1) + (vad_data->a2);


      vad_data->state = ST_SILENCE;
      vad_data->laststate = ST_SILENCE;
    }
    break;

  case ST_SILENCE:
    if (f.p >= vad_data->k1){
      vad_data->state = ST_UNDEF;
      vad_data->laststate = ST_SILENCE;
      vad_data->frames_SV -=1;
    }
    break;

  case ST_VOICE:
    if (f.p < vad_data->k2){
      vad_data->state = ST_UNDEF;
      vad_data->laststate = ST_VOICE;
      vad_data->frames_VS -=1;
    }  
    break;

  case ST_UNDEF:
    if (vad_data->laststate == ST_SILENCE){
      if (f.p >= vad_data->k1){
        vad_data->frames_SV -=1;
        if((vad_data->frames_SV <= 0) && (f.p >= vad_data->k2)){
          vad_data->state = ST_VOICE;
          vad_data->laststate = ST_VOICE;
          vad_data->frames_SV = vad_data->max_SV;
          break;
        }
        else if(vad_data->frames_SV <= 0){
          vad_data->state = ST_SILENCE;
          vad_data->laststate = ST_SILENCE;
          vad_data->frames_SV = vad_data->max_SV;
          break;
        }
        else{
          break;
        }

      }
      else{
        vad_data->state = ST_SILENCE;
        vad_data->laststate = ST_SILENCE;
        vad_data->frames_SV = vad_data->max_SV;
        break;
      }
    }
    if (vad_data->laststate == ST_VOICE){
      if (f.p <= vad_data->k2){
        vad_data->frames_VS -=1;
        if((vad_data->frames_VS <= 0) && (f.p <= vad_data->k1)){
          vad_data->state = ST_SILENCE;
          vad_data->laststate = ST_SILENCE;
          vad_data->frames_VS = vad_data->max_VS;
          break;
        }
        if(vad_data->frames_VS <= 0){
          vad_data->state = ST_VOICE;
          vad_data->laststate = ST_VOICE;
          vad_data->frames_VS = vad_data->max_VS;
          break;
        }
        else{
          break;
        }
      }
      else{
        vad_data->state = ST_VOICE;
        vad_data->laststate = ST_VOICE;
        vad_data->frames_VS = vad_data->max_VS;
        break;
      }
    }
    else{
      printf("Error, laststate not valid.");
    } 
    break;

  }

  if (vad_data->state == ST_SILENCE ||
      vad_data->state == ST_VOICE)
    return vad_data->state;
  else
    return ST_UNDEF;
}

void vad_show_state(const VAD_DATA *vad_data, FILE *out) {
  fprintf(out, "%d\t%f\n", vad_data->state, vad_data->last_feature);
}
