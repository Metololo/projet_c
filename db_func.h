//
// Created by Samy on 07/12/2022.
//

#ifndef PROJET_C_DB_FUNC_H
#define PROJET_C_DB_FUNC_H

#endif //PROJET_C_DB_FUNC_H

// When a mysql error occur, add the error in /log/mysql_errors.log
void dbAddError(MYSQL *mysql);

//Script for creating the project database.
// You can find the SQL scripts for the entire database in /SQL/script.txt
void dbCreate(MYSQL *mysql);

// To get a setting from /settings/config.txt
// Return the value for the setting as a String or a NULL pointer if the function doesn't find the
//setting
// Use "//" to comment a line
char *getSetting(char *buffer, char *setting);

