//
//  Created by Samy on 05/12/2022.
//  This file contains many functions used to manage audio for this project :
// Play,Pause,Stop a sound. Change the volume, get duration, current timecode ...

#define VOLUME_UP 1
#define VOLUME_DOWN 0

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "miniaudio.h"


int soundGetTimer(time_t start,time_t totalPause){
    time_t actualTime;
    time(&actualTime);

    return (int) (actualTime - start - totalPause);
}

void soundStart(ma_sound *sound, time_t *startTime){
    ma_sound_start(sound);
    time(startTime);
}

void soundPause(ma_sound *sound, time_t *pause){
    time(pause);
    ma_sound_stop(sound);
}

void soundPlay(ma_sound *sound,time_t *pauseTime,time_t *totalPause){
    ma_sound_start(sound);
    time_t tempTime;
    time(&tempTime);
    *totalPause += (tempTime - *pauseTime);

}

void soundSetVolume(ma_sound *sound,int volumeAction){
    if(volumeAction != 1 && volumeAction != 2){
        printf("ERROR, wrong volume parameter");
        return;
    }
    double soundVolume;
    soundVolume = ma_sound_get_volume(sound); // Give volume ( 0 - 1)
    switch (volumeAction) {
        case VOLUME_UP:
            soundVolume += 0.1;
            ma_sound_set_volume(sound, (float)soundVolume);
            break;
        case VOLUME_DOWN:
            soundVolume -= 0.1;
            ma_sound_set_volume(sound,(float)soundVolume);
            break;

    }
}

