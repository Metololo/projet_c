//
// Created by Samy on 05/12/2022.
//

#ifndef PROJET_C_AUDIO_FUNC_H
#define PROJET_C_AUDIO_FUNC_H
#define VOLUME_UP 1
#define VOLUME_DOWN 0
#endif //PROJET_C_AUDIO_FUNC_H


// -- soundTimer --
// This function return the time passed of a sound from the value start ( start should be the time in seconds where the sound started )
float soundGetTimer(time_t start,time_t pause);

// -- soundStart --
// Start playing a sound, and initializing the time in seconds when the sounds started
void soundStart(ma_sound *sound, time_t *startTime);

// -- soundStart --
// Pause a sound, and take pause time at parameter
void soundPause(ma_sound *sound, time_t *pause);

// -- soundStart --
// play a sound, and actualize the pause Time.
void soundPlay(ma_sound *sound,time_t *pauseTime,time_t *totalPause);

// -- soundFormatTime
// -- convert a float time in seconds to a string with format : "MM:SS" ( ex 125 seconds -> 02:05 )
void soundFormatTime(char *string,int size,float time);