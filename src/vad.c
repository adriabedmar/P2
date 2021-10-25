#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "vad.h"
#include "pav_analysis.h"

const float FRAME_TIME = 10.0F; /* in ms. */
const double INIT_FRAMES = 12;         /* in frames */
const float A1 = 3;               /* threshold over noise for k1 */
const float A2 = 10;              /* threshold over noise for k2 */
const double MIN_FRAMES_SV = 7;       /* Minimun time in frames to change from silence to voice */
const double MIN_FRAMES_VS = 10;      /* Minimun time in frames to change from voice to silence */


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
  /* 
   * DELETE and include a call to your own functions
   *
   * For the moment, compute random value between 0 and 1 
   */
  Features feat;
  //feat.zcr = compute_zcr(x,N,fm);
  feat.p = compute_power(x,N);
  //feat.am = compute_am(x,N);

  return feat;
}

/* 
 * TODO: Init the values of vad_data
 */

VAD_DATA * vad_open(float rate) {
  VAD_DATA *vad_data = malloc(sizeof(VAD_DATA));
  vad_data->state = ST_INIT;
  vad_data->laststate = ST_INIT;
  vad_data->sampling_rate = rate;
  vad_data->frame_length = rate * FRAME_TIME * 1e-3;
  vad_data->frames_VS = MIN_FRAMES_VS;
  vad_data->frames_SV = MIN_FRAMES_SV;
  vad_data->initFrames = INIT_FRAMES;
  return vad_data;
}

VAD_STATE vad_close(VAD_DATA *vad_data) {
  /* 
   * TODO: decide what to do with the last undecided frames
   */
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
    //For the Init frames we calculate the median power level 
    vad_data->initFrames -= 1;
    vad_data->k0 += pow(10,(vad_data->last_feature)/10);
    if(vad_data->initFrames <= 0){
      vad_data->k0 = 10*log10((vad_data->k0)/INIT_FRAMES);
      vad_data->k1 = vad_data->k0 + A1;
      vad_data->k2 = vad_data->k0 + A2;
      vad_data->state = ST_SILENCE;
      vad_data->laststate = ST_SILENCE;
    }
    break;

  case ST_SILENCE:
    if (vad_data->last_feature >= vad_data->k1){
      vad_data->state = ST_UNDEF;
    }
    break;

  case ST_VOICE:
    if (vad_data->last_feature < vad_data->k2){
      vad_data->state = ST_UNDEF;
    }  
    break;

  case ST_UNDEF:
    if (vad_data->laststate == ST_SILENCE){
      if (vad_data->last_feature >= vad_data->k1){
        vad_data->frames_SV -=1;
        if((vad_data->frames_SV <= 0) && (vad_data->last_feature >= vad_data->k2)){
          vad_data->state = ST_VOICE;
          vad_data->laststate = ST_VOICE;
          vad_data->frames_SV = MIN_FRAMES_SV;
          break;
        }
        if(vad_data->frames_VS <= 0){
          vad_data->state = ST_SILENCE;
          vad_data->laststate = ST_SILENCE;
          vad_data->frames_SV = MIN_FRAMES_SV;
          break;
        }
        else{
          break;
        }

      }
      else{
        vad_data->state = ST_SILENCE;
        vad_data->laststate = ST_SILENCE;
        vad_data->frames_SV = MIN_FRAMES_SV;
        break;
      }
    if (vad_data->laststate == ST_VOICE){
      if (vad_data->last_feature <= vad_data->k2){
        vad_data->frames_VS -=1;
        if((vad_data->frames_VS <= 0) && (vad_data->last_feature <= vad_data->k1)){
          vad_data->state = ST_SILENCE;
          vad_data->laststate = ST_SILENCE;
          vad_data->frames_VS = MIN_FRAMES_VS;
          break;
        }
        if(vad_data->frames_VS <= 0){
          vad_data->state = ST_VOICE;
          vad_data->laststate = ST_VOICE;
          vad_data->frames_VS = MIN_FRAMES_VS;
          break;
        }
        else{
          break;
        }
      }
      else{
        vad_data->state = ST_VOICE;
        vad_data->laststate = ST_VOICE;
        vad_data->frames_VS = MIN_FRAMES_VS;
        break;
      }
    } 
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
