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


void radioInit(MYSQL *mysql,char *radio,Music **front,Music **rear){
    char buffer[150];
    MYSQL_ROW row;
    int radioID = getRadioID(mysql,radio);

    snprintf(buffer,150,"SELECT id,name,genre,duration,path FROM music WHERE id IN (SELECT music FROM radio_music WHERE radio = %d) ORDER BY RAND();",radioID);

    if(mysql_query(mysql,buffer)){
        dbAddError(mysql);
        return;
    }

    MYSQL_RES *res = mysql_store_result(mysql);
    if(!res){
        fprintf(stderr,"AUCUNNN SONNN trouve !!!\n");
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
        fprintf(stderr,"Aucun son dans la radio\n");
        return;
    }
    float timer;
    time_t startTime = 0; // Store the time when the sound started
    time_t totalPauseTime = 0; // total pause time
    Music *currentSong = getFront(front);
    ma_result result;
    time_t pauseTime = 0;
    char duration[50];
    char timeNow[50];

   while(*front != NULL){
        result = ma_sound_init_from_file(engine,currentSong->path, 0, NULL, NULL, sound);
        if (result != MA_SUCCESS) {
            return;
        }
       soundStart(sound,&startTime);

       printf("----PLAYING----\n");
       printf("--NAME : %s --\n",currentSong->name);
       printf("-- GENRE : %s --\n",currentSong->genre);
       soundFormatTime(duration,50,(float) currentSong->duration);

       printf("-- %s : %s --\n",timeNow,duration);
       int choice;


        while(!ma_sound_at_end(sound)){
            sleep(1);
        }

        radioNext(front,rear,sound);
        printf("\n\n NEXT SONG.....\n\n");

        currentSong = getFront(front);
    }

}

void radioStop(Music **front,Music **rear){
    while(!isEmpty(*front)){
        Dequeue(front,rear);
    }
}

Radio *addRadio(int id,char *name,char *genre){
    Radio *new = malloc(sizeof(Radio));
    snprintf(new->name,51,"%s",name);
    snprintf(new->genre,31,"%s",genre);
    new->id = id;
    new->prev = NULL;
    new->next = NULL;
    return new;
}


void radioListInit(MYSQL *mysql,Radio **head,Radio **tail){
    MYSQL_RES *res;
    MYSQL_ROW row;

    if(mysql_query(mysql,"SELECT * FROM radio")){
        dbAddError(mysql);
        return;
    }

    *tail = NULL;
    *head = NULL;

    res = mysql_store_result(mysql);
    while((row = mysql_fetch_row(res))){
        int id = atoi(row[0]);
        Radio *newRadio = addRadio(id,row[1],row[2]);
        if((*head) == NULL){
           (*head) = newRadio;
            (*tail) = newRadio;
        }else{
            (*head)->prev = newRadio;
            newRadio->next = (*head);
            (*head) = newRadio;
        }

    }
}
