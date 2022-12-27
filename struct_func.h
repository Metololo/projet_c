//
// Created by Samy on 26/12/2022.
//

#ifndef PROJET_C_STRUCT_FUNC_H
#define PROJET_C_STRUCT_FUNC_H

#endif //PROJET_C_STRUCT_FUNC_H
#include "structures.h"

 // The file references all functions for Queues, linked lists...

// Create a new Music Node and return NULL if didn't work or the adress of the Node
Music *newSong(int id,char *name,char *genre,int duration,char *path);
// Enqueue a music into a list..
void Enqueue(Music **front,Music **rear,int id,char *name,char *genre, int duration,char *path);
//Dequeue the music from the list
void Dequeue(Music **front,Music **rear);
//Return 1 if a Queue is empty, or 0 if not.
int isEmpty(Music *front);
// Return the front of the Queue, or NULL if the Queue is empty.
Music *getFront(Music **front);