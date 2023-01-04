/*
 * Author : Samy
 * Created : 06/12/2022
 *
 * This file provides custom functions to communicate with the database
 * Create the database, send data, retrieve data
 */


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <mysql/mysql.h>
#include "dbconf.h"
#include "db_func.h"
#include "miniaudio.h"
#include "radiofunc.h"

void dbAddError(MYSQL *mysql){

    FILE *error = NULL;
    error = fopen("../logs/mysql_errors.log","a+t");

    if(error == NULL){
        error = fopen("../logs/mysql_errors.log","w+t");
        if(error == NULL){
            printf("Erreur d\'ouverture du fichier mysql_errors...");
            return;
        }
    }

    time_t today;
    today = time(NULL);
    char dateError[50];
    struct tm *pTime = localtime(&today);
    strftime(dateError,50,"%d/%m/%Y-%H:%M:%S",pTime);

    fprintf(error,"%s -> %s\n",dateError,mysql_error(mysql));
    fprintf(stderr,"%s", mysql_error(mysql));

    mysql_close(mysql);


}

void dbCreate(MYSQL *mysql){

    printf("Cr\202ation de la BDD %s...\n",DB_NAME);

    // WE CREATE THE DATABASE / DELETE IT AND RECREATE IT IF EXISTS

    if(mysql_query(mysql,"CREATE DATABASE IF NOT EXISTS radioC"))
        dbAddError(mysql);
    if(mysql_query(mysql,"USE radioC"))
        dbAddError(mysql);
    if(mysql_query(mysql,"DROP TABLE IF EXISTS radio"))
        dbAddError(mysql);
    if(mysql_query(mysql,"CREATE TABLE radio"
                         "("
                         "    id INTEGER AUTO_INCREMENT PRIMARY KEY,"
                         "    name varchar(50),"
                         "    genre varchar(30)"
                         ")"))
        dbAddError(mysql);
    if(mysql_query(mysql,"DROP TABLE IF EXISTS music"))
        dbAddError(mysql);
    if(mysql_query(mysql,"CREATE TABLE music"
                         "("
                         "    id INTEGER AUTO_INCREMENT PRIMARY KEY,"
                         "    name varchar(100),"
                         "    genre varchar(30),"
                         "    duration INTEGER,"
                         "    path VARCHAR(500)"
                         ")"))
        dbAddError(mysql);
    if(mysql_query(mysql,"DROP TABLE IF EXISTS radio_music"))
        dbAddError(mysql);
    if(mysql_query(mysql,"CREATE TABLE radio_music"
                         "("
                         "    music INTEGER REFERENCES music(id),"
                         "    radio INTEGER REFERENCES radio(id),"
                         "    PRIMARY KEY(music,radio)"
                         ")"))
        dbAddError(mysql);
    printf("Base de donnee cree avec succes \n");
}

int dbNewRadio(MYSQL *mysql,char *name,char *genre){

    if(strlen(name) > 50 || strlen(name) < 3){
        printf("Le nom de la radio doit faire entre 3 et 50 caracteres !\n");
        return 0;
    }
    if(strlen(genre) > 30 || strlen(genre) < 3){
        printf("Le genre de la radio doit faire entre 3 et 30 caracteres !\n");
        return 0;
    }

    char buffer[150];

    snprintf(buffer,128,"INSERT INTO radio(name,genre) VALUES('%s','%s')",name,genre);
    if(mysql_query(mysql,buffer)){
        dbAddError(mysql);
        return 0;
    }
    return 1;
}

int getMusicId(MYSQL *mysql,char *music){
    int musicID;
    char buffer[150];

    snprintf(buffer,150,"SELECT id FROM music WHERE name = '%s'",music);
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
    mysql_free_result(res);
    if(row){
        musicID = atoi(row[0]);
    }else{
        fprintf(stderr,"NO MUSIC FOUND FOR getRadioID : %s\n",music);
        return 0;
    }


    return musicID;
}

int dbNewMusic(MYSQL *mysql,ma_engine *engine,char *path,char *name, char *genre,char *radio){
    char buffer[800];
    int radioID = 0;
    int musicID = 0;

    // Verify that the length of value match database limits
    if(strlen(name) > 100 || strlen(name) < 3){
        fprintf(stderr,"Le nom de la musique doit faire entre 3 et 100 caracteres !\n");
        return 0;
    }
    if(strlen(genre) > 30 || strlen(genre) < 3){
        fprintf(stderr,"Le genre de la musique doit faire entre 3 et 30 caracteres !\n");
        return 0;
    }

    float soundDuration;
    ma_sound sound;
    ma_result soundResult;
    soundResult = ma_sound_init_from_file(engine,path, 0, NULL, NULL, &sound);
    if (soundResult != MA_SUCCESS) {
        return 0;
    }

    ma_sound_get_length_in_seconds(&sound,&soundDuration);
    int soundDudu = (int) soundDuration;
    ma_sound_uninit(&sound);

   if(radio == NULL){
       snprintf(buffer,255,"INSERT INTO music(name,genre,duration,path) VALUES('%s','%s','%d','%s')",name,genre,soundDudu,path);
       if(mysql_query(mysql,buffer)){
           dbAddError(mysql);
           return 0;
       }
       return 1;
   }

   char temp[150];

   radioID = getRadioID(mysql,radio);
   if(radioID == 0) return 0;

   snprintf(buffer,255,"INSERT INTO music(name,genre,duration,path) VALUES('%s','%s',%d,'%s')",name,genre,soundDudu,path);

    if(mysql_query(mysql,buffer)){
        dbAddError(mysql);
        return 0;
    }
    musicID = getMusicId(mysql,name);
    if(musicID == 0) return 0;

    snprintf(buffer,255,"INSERT INTO radio_music(music,radio) VALUES(%d,%d)",musicID,radioID);
    if(mysql_query(mysql,buffer)){
        dbAddError(mysql);
        return 0;
    }

    return 1;
}

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
    mysql_free_result(res);
    if(row){
        radioID = atoi(row[0]);
    }else{
        fprintf(stderr,"NO RADIO FOUND FOR getRadioID : %s\n",radio);
        return 0;
    }
    return radioID;

}


