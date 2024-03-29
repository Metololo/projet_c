//
// Created by Samy on 07/12/2022.
//

#include "miniaudio.h"
#ifndef PROJET_C_DB_FUNC_H
#define PROJET_C_DB_FUNC_H

// When a mysql error occur, add the error in /log/mysql_errors.log
void dbAddError(MYSQL *mysql);

//Script for creating the project database.
// You can find the SQL scripts for the entire database in /SQL/script.txt
void dbCreate(MYSQL *mysql);

int dbDeleteRadio(MYSQL *mysql,int id);
// create new radio, return 1 if sucess or 0 if fail.
int dbNewRadio(MYSQL *mysql,char *name,char *genre);
// Add music to database, 1 if sucess or 0 if fail.
int dbNewMusic(MYSQL *mysql,ma_engine *engine,char *path,char *name, char *genre,char *radio);

// return a musicID or 0 if error
int getMusicId(MYSQL *mysql,char *music);

//return a radioID or 0 if error
int getRadioID(MYSQL *mysql,char *radio);

#endif //PROJET_C_DB_FUNC_H

