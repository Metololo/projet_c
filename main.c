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

    time_t startTime = 0; // Store the time when the sound started
    time_t pauseTime = 0; // time when sound paused
    time_t totalPauseTime = 0; // total pause time

    float soundDuration = 0;

    int choice;
    char timeNow[50];
    char duration[50];

    float timer;

    result = ma_engine_init(NULL,&engine); // Start the engine
    if(result != MA_SUCCESS){
        return result;
    }

    //This function init a sound from pathfile ( doesn't play it ).
    soundResult = ma_sound_init_from_file(&engine, "music.mp3", 0, NULL, NULL, &sound);
    if (soundResult != MA_SUCCESS) {
        return soundResult;
    }


    do {
        printf("Bonjour !\n");
        printf("Que souhaitez-vous faire ?\n");
        printf("1. Lancer le son\n");
        printf("2. Mettre en pause le son\n");
        printf("3. Mettre en lecture le son\n");
        printf("Volume actuel : %d\n", (int) (ma_sound_get_volume(&sound)*10));
        printf("4. Monter le volume\n");
        printf("5. Baisser le volume\n");
        printf("6. Stopper le son\n");

        timer = soundGetTimer(startTime,totalPauseTime);
        soundFormatTime(timeNow,50,timer);
        printf("Timecode : %s\n",timeNow);

        ma_sound_get_length_in_seconds(&sound,&soundDuration);
        soundFormatTime(duration,50,soundDuration);
        printf("Duree total du son : %s\n",duration);

        scanf("%d", &choice);

        switch (choice) {
            case 1:
                soundStart(&sound, &startTime);
                break;

            case 2:
                soundPause(&sound, &pauseTime);
                break;

            case 3:
                soundPlay(&sound, &pauseTime, &totalPauseTime);
                break;

            case 4:
                soundSetVolume(&sound, VOLUME_UP);
                break;

            case 5:
                soundSetVolume(&sound, VOLUME_DOWN);
                break;

            case 6:
                totalPauseTime = 0;
                ma_sound_uninit(&sound);
                break;
        }
    } while (choice != 0);

    ma_engine_uninit(&engine);

    return 0;
}

