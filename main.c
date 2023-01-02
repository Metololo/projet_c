#define MINIAUDIO_IMPLEMENTATION

#include <stdio.h>
#include <mysql/mysql.h>
#include "miniaudio.h"
#include "audio_func.h"
#include "dbconf.h"
#include "db_func.h"
#include "structures.h"
#include "settings_func.h"
#include "radiofunc.h"
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

GtkWidget *window;
GtkBuilder *builder;
GtkWidget *radio_name;
GtkWidget *next_radio;
GtkWidget *prev_radio;

G_MODULE_EXPORT void next_radio_clicked(GtkButton *b, gpointer user_data);
G_MODULE_EXPORT void prev_radio_clicked(GtkButton *b, gpointer user_data);
G_MODULE_EXPORT gboolean gboolean test(int (*a)(int));


int main(int argc,char **argv) {


    ma_result result;
    ma_engine engine; // Declare the engine used to play sound

    ma_result soundResult;
    ma_sound sound; // Use to play sounds and manage them.

    time_t startTime = 0; // Store the time when the sound started
    time_t pauseTime = 0; // time when sound paused
    time_t totalPauseTime = 0; // total pause time

    float soundDuration = 0;

    int choice;
    char timeNow[50];
    char duration[50];

    float timer;

   /* SETTING *settings;
    settings = settingsInit(); // DONT FORGET TO freeSettings(settings) at the end
    settingsSet(settings,"blabla","bonjourbonuour");*/


   // -----------------------------
   // START MYSQL
    // -----------------------------

    MYSQL *mysql;
    mysql = mysql_init(NULL);
    mysql_options(mysql, MYSQL_READ_DEFAULT_GROUP, "option");

    if (mysql_real_connect(mysql, DB_HOST, DB_USER, DB_PASSWORD, NULL, DB_PORT, NULL, 0) == NULL){
        dbAddError(mysql);
        fprintf(stderr, mysql_error(mysql));
    }

    if(mysql_query(mysql,"USE radioC")){
        dbCreate(mysql);
    }

    // -----------------------------
    // START MINIAUDIO
    // -----------------------------


    result = ma_engine_init(NULL,&engine); // Start the engine
    if(result != MA_SUCCESS){
        return result;
    }

    // -----------------------------
    // INIT RADIO LIST ( start at radioListHead ) with name,genre,id.
    // -----------------------------

    Radio *radioListHead = NULL;
    Radio *radioListTail = NULL;

    RadioListInfo radioList;
    radioList.pos = 0;

    radioListInit(mysql,&radioListHead,&radioListTail);

    // -----------------------------
    // START GTK
    // -----------------------------

    gtk_init(&argc, &argv);

    GtkCssProvider *cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, "../style/theme.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                              GTK_STYLE_PROVIDER(cssProvider),
                                              GTK_STYLE_PROVIDER_PRIORITY_USER);

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "../app.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "windowApp"));
    gtk_builder_connect_signals(builder, NULL);

    radio_name = GTK_WIDGET(gtk_builder_get_object(builder,"radio"));
    next_radio = GTK_WIDGET(gtk_builder_get_object(builder, "next-radio"));
    prev_radio = GTK_WIDGET(gtk_builder_get_object(builder, "prev-radio"));

    g_signal_connect(next_radio, "clicked", G_CALLBACK(next_radio_clicked), &radioList);
    g_signal_connect(prev_radio, "clicked", G_CALLBACK(prev_radio_clicked), &radioList);

    g_object_unref(builder);

    if(!radioIsEmpty(radioListHead)){
        gtk_label_set_text(GTK_LABEL(radio_name),(const gchar*) radioListHead->name);
    }

    radioList.radioListHead = radioListHead;
    radioList.radioListTail = radioListTail;

    Radio *currentRadio = radioGetCurrent(radioList);

    Music *radioFront = NULL; // AVANT DE LA FILE
    Music *radioRear = NULL; // ARRIERE DE LA FILE

    radioInit(mysql, currentRadio->name, &radioFront, &radioRear);

    Music *temp = radioFront;

    int i = 0;
    g_timeout_add(1000, test, &i);


    gtk_widget_show(window);
    gtk_main();

    radioStop(&radioFront, &radioRear);



    /*radioInit(mysql,"rap1",&radioFront,&radioRear);

    Music *temp = radioFront;
   while(temp != NULL){

        printf("%d %s %s %d %s\n",temp->id,temp->name,temp->genre,temp->duration,temp->path);
        temp = temp->next;
    }

    radioPlay(&radioFront,&radioRear,&engine,&sound);

    radioStop(&radioFront,&radioRear);*/





    mysql_close(mysql);
    ma_engine_uninit(&engine);
    //free(settings);

  /*

    //This function init a sound from pathfile ( doesn't play it ).
    soundResult = ma_sound_init_from_file(&engine, "music.mp3", 0, NULL, NULL, &sound);
    if (soundResult != MA_SUCCESS) {
        return soundResult;
    }


    do {
        printf("Bonjour !\n");
        printf("Que souhaitez-vous faire ?\n");
        printf("1. Lancer le son\n");
        printf("2. Mettre en pause le son\n");
        printf("3. Mettre en lecture le son\n");
        printf("Volume actuel : %d\n", (int) (ma_sound_get_volume(&sound)*10));
        printf("4. Monter le volume\n");
        printf("5. Baisser le volume\n");
        printf("6. Stopper le son\n");

        timer = soundGetTimer(startTime,totalPauseTime);
        soundFormatTime(timeNow,50,timer);
        printf("Timecode : %s\n",timeNow);

        ma_sound_get_length_in_seconds(&sound,&soundDuration);
        soundFormatTime(duration,50,soundDuration);
        printf("Duree total du son : %s\n",duration);

        scanf("%d", &choice);

        switch (choice) {
            case 1:
                soundStart(&sound, &startTime);
                break;

            case 2:
                soundPause(&sound, &pauseTime);
                break;

            case 3:
                soundPlay(&sound, &pauseTime, &totalPauseTime);
                break;

            case 4:
                soundSetVolume(&sound, VOLUME_UP);
                break;

            case 5:
                soundSetVolume(&sound, VOLUME_DOWN);
                break;

            case 6:
                totalPauseTime = 0;
                ma_sound_uninit(&sound);
                break;
        }
    } while (choice != 0);

    */

    return 0;
}


void next_radio_clicked(GtkButton *b, gpointer user_data){
    struct RadioListInfo *list = (struct RadioListInfo *) user_data;

    if(list->pos+1 < radioListGetSize(list->radioListHead))  list->pos++;
    int i = list->pos;
    Radio *temp = list->radioListHead;

    for(i= list->pos;i>0;--i){
        if(temp == NULL) return;
        temp = temp->next;
    }
    if(temp == NULL) return;



    gtk_label_set_text(GTK_LABEL(radio_name),(const gchar*) temp->name);
}

void prev_radio_clicked(GtkButton *b, gpointer user_data){
    struct RadioListInfo *list = (struct RadioListInfo *) user_data;

    if(list->pos > 0) list->pos--;
    int i = list->pos;
    Radio *temp = list->radioListHead;

    for(i=list->pos;i>0;--i){
        if(temp == NULL) return;
        temp = temp->next;
    }
    if(temp == NULL) return;



    gtk_label_set_text(GTK_LABEL(radio_name),(const gchar*) temp->name);
}

gboolean test(int (*a)(int)){
    int *i = (int *) data;
    if(*i<5) {
        ++*i;
        printf("%d ",*i);
        return TRUE;
    }
    else{
        printf("FINIS");
        return FALSE;
    }

}



