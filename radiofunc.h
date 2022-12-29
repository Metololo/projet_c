//
// Created by Samy on 26/12/2022.
//

#ifndef PROJET_C_RADIOFUNC_H
#define PROJET_C_RADIOFUNC_H

#include "structures.h"
//Return the ID of a given radio name or 0 if not found or error

// Create a Queue with all musics related to the radio. It doesn't start playing it....
void radioInit(MYSQL *mysql,char *radio,Music **front,Music **rear);

// Free all nodes from the radio Queue..
void radioStop(Music **front,Music **rear);

// Play the first song in the QUEUE
void radioPlay(Music **front,Music **rear,ma_engine *engine,ma_sound *sound);

// free the Node of the finished song..
void radioNext(Music **front,Music **rear,ma_sound *sound);

// Create a new radio Node and initialize it
Radio *addRadio(int id,char *name,char *genre);

// Take all radios from database and create a doubly linked list
void radioListInit(MYSQL *mysql,Radio **head,Radio **tail);

#endif //PROJET_C_RADIOFUNC_H
