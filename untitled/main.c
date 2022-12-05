#include <stdio.h>
#include <winsock.h>
#include <mysql/mysql.h>
#include <stdlib.h>

int main() {


    MYSQL *mysql;
    mysql = mysql_init(NULL);
    mysql_options(mysql, MYSQL_READ_DEFAULT_GROUP, "option");

    if (mysql_real_connect(mysql, "localhost", "root", "", NULL, 0, NULL, 0)) {
        printf("OK");

        mysql_close(mysql);
    } else {
        printf("Erreur connexion à la BDD!");
    }
    return EXIT_SUCCESS;
}