//
// Created by samya on 08/12/2022.
//

#ifndef PROJET_C_STRUCTURES_H
#define PROJET_C_STRUCTURES_H
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

typedef struct Radio{
    unsigned int id;
    char name[51];
    char genre[31];
    int pause;
    Music *head;
}Radio;

#endif //PROJET_C_STRUCTURES_H





