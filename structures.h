//
// Created by samya on 08/12/2022.
//

#ifndef PROJET_C_STRUCTURES_H
#define PROJET_C_STRUCTURES_H

#include "miniaudio.h"
#include <mysql/mysql.h>

#define MUSIC_NAME_SIZE 101
#define MUSIC_GENRE_SIZE 31
#define PATH_SIZE 501



typedef struct Music Music;

struct Music{
    int id;
    char name[MUSIC_NAME_SIZE];
    char genre[MUSIC_GENRE_SIZE];
    char path[PATH_SIZE];
    int duration;
    Music *next;
};

typedef struct Radio Radio;

struct Radio{
    unsigned int id;
    char name[51];
    char genre[31];
    Radio *next;
    Radio *prev;
};


typedef struct RadioListInfo RadioListInfo;


typedef struct DataPLAY{

    Radio *current;
    Radio *radioListHead;
    Radio *radioListTail;

    int isSwitching;

    MYSQL *mysql;
    Music **front;
    Music **rear;
    ma_engine *engine;
    ma_sound *sound;

    int soundInitialized;
    int changeRadio;
    int isPlaying;
    int isPaused;
    int wantToPause;
    double volume;
    time_t startTime;
    time_t totalPauseTime;
    time_t pauseTime;

}DataPLAY;

#endif //PROJET_C_STRUCTURES_H





