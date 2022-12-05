#define MINIAUDIO_IMPLEMENTATION
#include <stdio.h>
#include <time.h>
#include "miniaudio.h"
#include "audio_func.h"



int main() {

    ma_result result;
    ma_engine engine; // Declare the engine used to play sound

    ma_result soundResult;
    ma_sound sound; // Use to play sounds and manage them.

    time_t startTime; // Store the time when the sound started
    time_t pauseTime = 0; // time when sound paused
    time_t totalPauseTime = 0; // total pause time

    int soundDuration = 0;

    result = ma_engine_init(NULL,&engine); // Start the engine
    if(result != MA_SUCCESS){
        return result;
    }


    //This function init a sound from pathfile ( doesn't play it ).
    soundResult = ma_sound_init_from_file(&engine, "music.mp3", 0, NULL, NULL, &sound);
    if (soundResult != MA_SUCCESS) {
        return soundResult;
    }
    printf("Press enter to start playing...");
    getchar();
    fflush(stdin);
    soundStart(&sound, &startTime);
    char c = 'a';
    getchar();
    printf("%lld", pauseTime);
    soundPause(&sound,&pauseTime);
    getchar();
    printf("%lld", pauseTime);
    soundStart(&sound,&startTime);
    getchar();
    fflush(stdin);
    printf("%lld", pauseTime);
    getchar();


    ma_engine_uninit(&engine);

    return 0;
}

