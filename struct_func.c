//
// Created by Samy on 24/12/2022.
//

#include <stdlib.h>
#include <stdio.h>
#include "structures.h"
#include <string.h>


Music *newSong(int id,char *name,char *genre,int duration,char *path){

    Music *newMusic = malloc(sizeof(Music));
    if(newMusic == NULL) return NULL;
    snprintf(newMusic->name,MUSIC_NAME_SIZE,"%s",name);
    snprintf(newMusic->genre,MUSIC_GENRE_SIZE,"%s",genre);
    snprintf(newMusic->path,PATH_SIZE,"%s",path);
    newMusic->duration = duration;
    newMusic->next = NULL;
    newMusic->id = id;
    return newMusic;

}

void Enqueue(Music **front,Music **rear,int id,char *name,char *genre, int duration,char *path){

    Music *newMusic = newSong(id,name,genre,duration,path);
    if((*front) == NULL && (*rear) == NULL){
        (*front) = (*rear) = newMusic;
        return;
    }
    (*rear)->next = newMusic;
    (*rear) = newMusic;

}

void Dequeue(Music **front,Music **rear){
    Music *leaver = *front;
    if(*front == NULL) return;
    if(*front == *rear){
        *front = *rear = NULL;
    }else{
        *front = (*front)->next;
    }
    free(leaver);
}

int isEmpty(Music *front){
    return front == NULL ? 1 : 0;
}

Music *getFront(Music **front){
    if(isEmpty(*front)) return NULL;
    return *front;
}
