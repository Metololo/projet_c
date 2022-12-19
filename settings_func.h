//
// Created by samya on 08/12/2022.
//

#ifndef PROJET_C_SETTINGS_FUNC_H
#define PROJET_C_SETTINGS_FUNC_H

#endif //PROJET_C_SETTINGS_FUNC_H
#define LINE_MAX_LENGTH 128

typedef struct SETTING{
    char param[30];
    char value[30];
    int line;
}SETTING;

//Return 0 if error or 0 lines found,
//Or return the number of parameters in settings file.
int settingsGetNumber();

// Allocate an array of SETTING taking the size returned by settingGetNumber.
// Then store all the parameters and their value in the arrayZ with the line number
// ( doesn't take commentaries and empty lines in count ).
/* EXEMPLE :
 *
 * array[0].param = "volume" | array[0].value = "10" | array[0].line = 4;
 */
// DONT FORGET TO free(settings) when finish !
SETTING *settingsInit();

// Change the parameter with the new value
int settingsSet(SETTING *settings,char *param,char *value);

// Return the line of a specific parameter or -1 if not found
int settingsGetParamLine(SETTING *settings,char *param);
