//
// Created by samya on 08/12/2022.
//

#ifndef PROJET_C_STRUCTURES_H
#define PROJET_C_STRUCTURES_H

#endif //PROJET_C_STRUCTURES_H

typedef struct Radio{
    unsigned int id;
    char name[51];
    char genre[31];
    int currentSong;
    int pause;
}Radio;

typedef struct Music{
    unsigned int id;
    char name[100];
    char genre[30];
    unsigned int duration;
    unsigned int radio;
}Music;

typedef struct MusicNode MusicNode;

struct MusicNode{
    Music *radio;
    MusicNode *next;
};