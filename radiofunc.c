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
#include "radiofunc.h"
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

    unsigned int field = mysql_num_fields(res);

    if(mysql_num_rows(res) == 0){
        printf("NO SOUND");
        mysql_free_result(res);
        return;
    }

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
    mysql_free_result(res);
}


void radioNext(Music **front,Music **rear,ma_sound *sound){
    Dequeue(front,rear);
    ma_sound_uninit(sound);
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

    res = mysql_store_result(mysql);
    while((row = mysql_fetch_row(res))){
        int id = atoi(row[0]);
        Radio *newRadio = addRadio(id,row[1],row[2]);
        radioListInsert(head,tail,newRadio);
    }
    mysql_free_result(res);
}

void radioListInsert(Radio **head,Radio **tail,Radio *radio){

    if(*tail == NULL) {
        *tail = radio;
        *head = radio;
        radio->next = radio->prev = *head;
        return;
    }

    (*head)->prev = radio;
    radio->next = *head;
    radio->prev = *tail;
    *head = radio;
    (*tail)->next = radio;

}

int radioListDelete(Radio **head,Radio **tail,int id){
    Radio *temp = NULL;
    Radio *temp2 = NULL;

    if(temp->id == id){
        *head = temp->next;
        free(temp);
        return 1;
    }

    for(temp = *head;temp != *tail;temp=temp->next){
        if(temp->id == id){
            if(*head == NULL){
                return 1;
            }
            if(temp == *head && temp == *tail){
                *head = NULL;
                *tail = NULL;
            }else{
                *head = temp->next;
                (*tail)->next = *head;
            }
            free(temp);
            return 1;
        }
    }
    return 0;
}

void radioListDeleteAll(Radio **head,Radio **tail){
    Radio *temp = NULL;
    (*tail)->next = NULL;
    *tail = NULL;


    while (*head != NULL){
         temp = *head;
         *head = temp->next;
        free(temp);
    }
}

int radioIsEmpty(Radio *head){
    return head == NULL ? 1 : 0;
}

