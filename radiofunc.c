//
// Created by Samy on 14/12/2022.
//

// This file contains all the fonctions related to manipulates radios (start radio, stop radio..)
#include <stdio.h>
#include <stdlib.h>
#include "struct_func.h"
#include "structures.h"
#include <mysql/mysql.h>
#include "db_func.h"
#include <string.h>
#include "miniaudio.h"
#include "audio_func.h"
#include <unistd.h>

int getRadioID(MYSQL *mysql,char *radio){
    int radioID;
    char buffer[150];

    snprintf(buffer,150,"SELECT id FROM radio WHERE name = '%s'",radio);
    if(mysql_query(mysql,buffer)){
        dbAddError(mysql);
        return 0;
    }
    MYSQL_RES *res = mysql_store_result(mysql);

    if(res == NULL){
        dbAddError(mysql);
        return 0;
    }
    MYSQL_ROW row;
    row = mysql_fetch_row(res);
    if(row){
        radioID = atoi(row[0]);
    }else{
        printf("NO RADIO FOUND !");
        return 0;
    }
    return radioID;

}

void radioInit(MYSQL *mysql,char *radio,Music **front,Music **rear){
    char buffer[150];
    MYSQL_ROW row;
    int radioID = getRadioID(mysql,radio);

    snprintf(buffer,150,"SELECT id,name,genre,duration,path FROM music WHERE radio = %d ORDER BY RAND()",radioID);

    if(mysql_query(mysql,buffer)){
        dbAddError(mysql);
        return;
    }

    MYSQL_RES *res = mysql_store_result(mysql);
    if(!res){
        fprintf(stderr,"AUCUNNN SONNN trouve !!!");
        return;
    }
    unsigned int field = mysql_num_fields(res);

    while((row = mysql_fetch_row(res))){
        int id = atoi(row[0]);
        int duration = atoi(row[3]);
        char name[MUSIC_NAME_SIZE];
        strcpy(name,row[1]);
        char genre[MUSIC_GENRE_SIZE];
        strcpy(genre,row[2]);
        char path[PATH_SIZE];
        strcpy(path,row[4]);
        Enqueue(front,rear,id,name,genre,duration,path);

    }
}


void radioNext(Music **front,Music **rear,ma_sound *sound){
    Dequeue(front,rear);
    ma_sound_uninit(sound);
}

void radioPlay(Music **front,Music **rear,ma_engine *engine,ma_sound *sound){
    if(isEmpty(*front)){
        fprintf(stderr,"Aucun son dans la radio");
        return;
    }
    float timer;
    time_t startTime = 0; // Store the time when the sound started
    time_t totalPauseTime = 0; // total pause time
    Music *currentSong = getFront(front);
    ma_result result;
    char duration[50];

   while(*front != NULL){
        result = ma_sound_init_from_file(engine,currentSong->path, 0, NULL, NULL, sound);
        if (result != MA_SUCCESS) {
            return;
        }
        ma_sound_start(sound);

        printf("----PLAYING----\n");
        printf("--NAME : %s --\n",currentSong->name);
        printf("-- GENRE : %s --\n",currentSong->genre);
        soundFormatTime(duration,50,(float) currentSong->duration);
        printf("-- Total duration : %s --\n", duration);

        while(!ma_sound_at_end(sound)){
            sleep(1);
        }

        radioNext(front,rear,sound);
        printf("\n\n NEXT SONG.....\n\n");

        currentSong = getFront(front);
    }

}



void radioStop(){

}

void radioFree(Music **front,Music **rear){
    while(!isEmpty(*front)){
        Dequeue(front,rear);
    }
}