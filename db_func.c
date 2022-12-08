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
#include <winsock.h>
#include "dbconf.h"

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

    mysql = mysql_init(NULL);
    mysql_options(mysql, MYSQL_READ_DEFAULT_GROUP, "option");

    if (mysql_real_connect(mysql, DB_HOST, DB_USER, DB_PASSWORD, NULL, DB_PORT, NULL, 0) == NULL){
        dbAddError(mysql);
        return;
    }
    printf("Cr\202ation de la BDD %s...",DB_NAME);

    // WE CREATE THE DATABASE / DELETE IT AND RECREATE IT IF EXISTS

    if(mysql_query(mysql,"CREATE DATABASE IF NOT EXISTS radioC"))
        dbAddError(mysql);
    if(mysql_query(mysql,"USE radioC"))
        dbAddError(mysql);
    if(mysql_query(mysql,"DROP TABLE IF EXISTS radio"))
        dbAddError(mysql);
    if(mysql_query(mysql,"CREATE TABLE radio"
                         "("
                         "    id int AUTO_INCREMENT PRIMARY KEY,"
                         "    name varchar(50),"
                         "    genre varchar(30)"
                         ")"))
        dbAddError(mysql);
    if(mysql_query(mysql,"DROP TABLE IF EXISTS music"))
        dbAddError(mysql);
    if(mysql_query(mysql,"CREATE TABLE music"
                         "("
                         "    id int AUTO_INCREMENT PRIMARY KEY,"
                         "    name varchar(100),"
                         "    genre varchar(30),"
                         "    radio int REFERENCES radio(id)"
                         ")"))
        dbAddError(mysql);



    mysql_close(mysql);

}






