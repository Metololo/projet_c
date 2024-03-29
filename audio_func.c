//
//  Created by Samy on 05/12/2022.
//  This file contains many functions used to manage audio for this project :
// Play,Pause,Stop a sound. Change the volume, get duration, current timecode ...
// You can check more infos about functions in audio_func.h


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "miniaudio.h"
#include "audio_func.h"


float soundGetTimer(time_t start,time_t totalPause){
    time_t actualTime;
    time(&actualTime);

    return (float) (actualTime - start - totalPause);
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

void soundFormatTime(char *string,int size,float time){
    time_t timeTemp = (time_t) time;
    struct tm *pTime = localtime(&timeTemp);
    strftime(string,size,"%M:%S",pTime);
}


