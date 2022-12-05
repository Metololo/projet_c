//
//  Created by Samy on 05/12/2022.
//  This file contains many functions used to manage audio for this project :
// Play,Pause,Stop a sound. Change the volume, get duration, current timecode ...

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "miniaudio.h"


int soundGetTimer(time_t start,time_t pause){
    time_t actualTime;
    time(&actualTime);

    return (int) (actualTime - start - pause);
}

void soundStart(ma_sound *sound, time_t *startTime){
    ma_sound_start(sound);
    time(startTime);
}

void soundPause(ma_sound *sound, time_t *pause){
    time(pause);
    ma_sound_stop(sound);
}

void soundPlay(ma_sound *sound,time_t *pauseTime,time_t *pause){
    time_t tempTime;
    time(&tempTime);
    ma_sound_start(sound);
    *pause = tempTime - *pause;


}

