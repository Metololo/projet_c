//
// Created by samya on 08/12/2022.
//
#include "stdio.h"
#include "stdlib.h"
#include "settings_func.h"
#include "structures.h"
#include <string.h>

int settingsGetNumber(){

    FILE *conf;
    conf = fopen("../settings/config.txt","rt");
    if(conf == NULL){
        // TODO ? Crée une fonction qui recreer le fichier option en cas de perte
        return 0;
    }
    char buffer[LINE_MAX_LENGTH];
    int counter = 0;

    // READ ALL LINE FROM FILE config.txt
    while(!feof(conf)){
        fgets(buffer,LINE_MAX_LENGTH,conf);
        if (buffer[0] != '#' && buffer[0] != '\n') {
            if(strchr(buffer,':') != NULL)
                ++counter;
            }
        }
    fclose(conf);
    return counter;
}

SETTING *settingsInit(){
    int paramNumbers = settingsGetNumber();
    if(paramNumbers == 0) return NULL;
    char temp[LINE_MAX_LENGTH];
    SETTING *settings = malloc(sizeof(SETTING) * paramNumbers);
    if(settings == NULL) return NULL;
    char buffer[LINE_MAX_LENGTH];
    char *position;
    FILE *conf = NULL;
    int i = 0;
    int counter=0;

    conf = fopen("../settings/config.txt","rt");
    if(conf == NULL){
        free(settings);
        return NULL;
    }
    // READ ALL LINE FROM FILE config.txt
    while(!feof(conf)){
        fgets(buffer,LINE_MAX_LENGTH,conf);
        settings[i].line = counter;

        if(buffer[0] != '#' && buffer[0] != '\n'){
            if(buffer[strlen(buffer)-1] == '\n'){
                buffer[strlen(buffer)-1] = '\0';
            }
            //If we find the settings, we add it to settings
                position = strchr(buffer,':');
                if(position != NULL){


                    int namePose = (int) (position - buffer);
                    int valueLength = (int) strlen(position+1);

                    strncpy(settings[i].param, buffer, namePose);
                    settings[i].param[namePose] = '\0';
                    strncpy(settings[i].value,position+1,valueLength+1);
                    settings[i].value[valueLength] = '\0';
                    ++i;
                }

        }
        ++counter;

    }
    fclose(conf);
    return settings;

}

int settingsSet(SETTING *settings,char *param,char *value){
    FILE *conf;
    FILE *tempConf;
    char newLine[LINE_MAX_LENGTH];
    conf = fopen("../settings/config.txt","rt");
    tempConf = fopen("../settings/temp_config.txt","wt");
    if(conf == NULL || tempConf == NULL){
        return 0;
    }
    char buffer[LINE_MAX_LENGTH];
    int counter = 0;
    int line = settingsGetParamLine(settings,param);
    if(line == -1) return 0;

    // READ ALL LINE FROM FILE config.txt
    while(!feof(conf)){
        fgets(buffer,LINE_MAX_LENGTH,conf);

        //printf("%s",buffer);
                if(counter == line){
                    sprintf(newLine,"%s:%s\n",param,value);
                    fputs(newLine,tempConf);
                }else{
                    fputs(buffer,tempConf);
                }
        ++counter;
    }
    fclose(conf);
    remove("../settings/config.txt");
    fclose(tempConf);
    rename("../settings/temp_config.txt","../settings/config.txt");
    return counter;


}

int settingsGetParamLine(SETTING *settings,char *param){
    int size = settingsGetNumber();
    for(int i = 0;i<size;++i){
        if(!strcmp(settings[i].param,param)){
            return settings[i].line;
        }

    }
    return -1;
}

