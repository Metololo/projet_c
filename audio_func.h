//
// Created by Samy on 05/12/2022.
//

#ifndef PROJET_C_AUDIO_FUNC_H
#define PROJET_C_AUDIO_FUNC_H

#endif //PROJET_C_AUDIO_FUNC_H


// -- soundTimer --
// This function return the time passed of a sound from the value start ( start should be the time in seconds where the sound started )
int soundGetTimer(time_t start,time_t pause);

// -- soundStart --
// Start playing a sound, and initializing the time in seconds when the sounds started
void soundStart(ma_sound *sound, time_t *startTime);

// -- soundStart --
// Pause a sound, and take pause time at parameter
void soundPause(ma_sound *sound, time_t *pause);

// -- soundStart --
// Pause a sound, and actualize the pause Time.
void soundPlay(ma_sound *sound,time_t *pause);

