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
#include "struct_func.h"
#include <gtk/gtk.h>
#include "gtk_functions.h"
#include <ctype.h>

GtkWidget *window;
GtkBuilder *builder;
GtkWidget *radio_name;
GtkWidget *next_radio;
GtkWidget *prev_radio;
GtkWidget *musicName;
GtkWidget *musicGenre;
GtkWidget *widgetTimer;
GtkWidget *widgetDuration;
GtkWidget *timerBar;
GtkWidget *pauseButton;
GtkWidget *volumeButton;
GtkWidget *volumeUp;
GtkWidget *volumeDown;
GtkWidget *pauseImage;

G_MODULE_EXPORT void next_radio_clicked(GtkButton *b, gpointer user_data);
G_MODULE_EXPORT void prev_radio_clicked(GtkButton *b, gpointer user_data);
G_MODULE_EXPORT gboolean test(gpointer data);
G_MODULE_EXPORT void pauseSound(GtkButton *b, gpointer user_data);

int main(int argc,char **argv) {


    RadioListInfo radiosInfos; // Store the position of the current radio playing, the head of the radio list, and

    DataPLAY radioData; // Structure used to be passed to callbacks functions for gtk ( many infos )

    Radio *radioListHead = NULL; // Head of the radio list
    Radio *radioListTail = NULL; // Tail of the radio list

    Radio *currentRadio; // This is a pointer to the current Radio playing

    Music *radioFront = NULL; // The front of the music Queue ( ex :  I'm in radio "rock1", this is the first music that should play
    Music *radioRear = NULL; // The tail of the music Queue

    ma_result result;
    ma_engine engine; // Declare the engine used to play sound

    ma_result soundResult;
    ma_sound sound; // Use to play sounds and manage them.

    time_t startTime = 0; // Store the time when the sound started in seconds
    time_t pauseTime = 0; // time when sound paused in seconds
    time_t totalPauseTime = 0; // total pause time in seconds ( ex : I paused my song 1sec then 3sec -> 4sec )

    float soundDuration = 0; // The duration of the sound in seconds

    int choice;
    char timeNow[50]; // Use to store the timer string ( ex : 0:20 -> song is playing since 20 sec)
    char duration[50]; // Use to store the song duration string (ex : 3:20 -> the song length is 3min20)

    float timer; // The timer in seconds ( we convert it in a string and pass it to timeNow )

    MYSQL *mysql;

    SETTING *settings; // This will store an array of all settings ( check struct SETTING )
    int settingsPos; // Store the position of a setting
    double volume;// store the initial volume

    settings = settingsInit(); // DONT FORGET TO freeSettings(settings) at the end
    //settingsSet(settings,"blabla","bonjourbonuour");


   // -----------------------------
   // START MYSQL
    // -----------------------------


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

    radiosInfos.pos = 0;

    radioListInit(mysql,&radioListHead,&radioListTail);

    radiosInfos.radioListHead = radioListHead;
    radiosInfos.radioListTail = radioListTail;

    //------------------
    // Then we INIT the music list of the first radio
    //------------------

    radioInit(mysql,radioListHead->name,&radioFront,&radioRear);

    //------------------
    // Initialize the radioData structure.
    //------------------

    radioData.sound = &sound;
    radioData.engine = &engine;
    radioData.rear = &radioRear;
    radioData.front = &radioFront;
    radioData.isPlaying = 0;
    radioData.pauseTime = 0;
    radioData.totalPauseTime = 0;
    radioData.startTime = 0;
    radioData.isPaused = 1;
    radioData.wantToPause = 0;

    // We get the default volume
    settingsPos = settingsGetParamLine(settings,"defaultVolume");
    volume = atof(settings[settingsPos].value)/10;

    radioData.volume = volume;

    // -----------------------------
    // START GTK
    // -----------------------------

    gtk_init(&argc, &argv);

    // We connect the theme.css style to gtk

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
    musicName = GTK_WIDGET(gtk_builder_get_object(builder, "music-name"));
    musicGenre = GTK_WIDGET(gtk_builder_get_object(builder, "music-genre"));
    widgetTimer = GTK_WIDGET(gtk_builder_get_object(builder, "sound-timer"));
    widgetDuration = GTK_WIDGET(gtk_builder_get_object(builder, "sound-duration"));
    timerBar = GTK_WIDGET(gtk_builder_get_object(builder, "timer-bar"));
    pauseButton = GTK_WIDGET(gtk_builder_get_object(builder, "pause-button"));
    volumeButton = GTK_WIDGET(gtk_builder_get_object(builder, "volume-button"));
    volumeUp = GTK_WIDGET(gtk_builder_get_object(builder, "volume-up"));
    volumeDown = GTK_WIDGET(gtk_builder_get_object(builder, "volume-down"));
    pauseImage = GTK_WIDGET(gtk_builder_get_object(builder, "pause-image"));


    g_signal_connect(next_radio, "clicked", G_CALLBACK(next_radio_clicked), &radiosInfos);
    g_signal_connect(prev_radio, "clicked", G_CALLBACK(prev_radio_clicked), &radiosInfos);
    g_signal_connect(pauseButton, "clicked", G_CALLBACK(pauseSound), &radioData);
    g_signal_connect(pauseButton, "clicked", G_CALLBACK(pauseSound), &radioData);

    gtk_scale_button_set_value(GTK_SCALE_BUTTON(volumeButton),radioData.volume);

    g_object_unref(builder);


    if(!radioIsEmpty(radioListHead)){
        gtk_label_set_text(GTK_LABEL(radio_name),(const gchar*) radioListHead->name);
    }



    g_timeout_add(200, test, &radioData);


    gtk_widget_show(window);
    gtk_main();


    mysql_close(mysql);
    ma_engine_uninit(&engine);
    free(settings);



    /*radioInit(mysql,"rap1",&radioFront,&radioRear);

    Music *temp = radioFront;
   while(temp != NULL){

        printf("%d %s %s %d %s\n",temp->id,temp->name,temp->genre,temp->duration,temp->path);
        temp = temp->next;
    }

    radioPlay(&radioFront,&radioRear,&engine,&sound);

    radioStop(&radioFront,&radioRear);*/






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

    if(list->radioListHead == NULL){
        gtk_label_set_text(GTK_LABEL(radio_name),(const gchar*) "No radio created");
        return;
    }

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

    if(list->radioListHead == NULL){
        gtk_label_set_text(GTK_LABEL(radio_name),(const gchar*) "No radio created");
        return;
    }

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

void pauseSound(GtkButton *b, gpointer user_data){

    DataPLAY *infos = (DataPLAY *) user_data;
    infos->wantToPause = 1;

}

gboolean test(gpointer data){

    DataPLAY *infos = (DataPLAY *) data;


    Music **front = infos->front;
    Music **rear = infos->rear;
    ma_engine *engine = infos->engine;
    ma_sound *sound = infos->sound;

    float timer = 0;

    Music *currentSong = getFront(front);
    ma_result result;
    char duration[50];
    char timeNow[50];

    gdouble pulsation = 1.0/currentSong->duration;

    //if no sound is playing we start it...

    if(!infos->isPlaying && infos->isPaused && infos->wantToPause){
        result = ma_sound_init_from_file(engine,currentSong->path, 0, NULL, NULL, sound);
        if (result != MA_SUCCESS) {
            return FALSE;
        }
        soundStart(sound,&infos->startTime);

        infos->isPlaying = 1;
        infos->isPaused = 0;
        infos->wantToPause = 0;

        gtk_label_set_text(GTK_LABEL(musicName),(const gchar*) currentSong->name);
        gtk_label_set_text(GTK_LABEL(musicGenre),(const gchar*) currentSong->genre);
        gtk_image_set_from_file((GtkImage *) pauseImage, "../images/pause.png");

        soundFormatTime(duration,50,(float) currentSong->duration);
        gtk_label_set_text(GTK_LABEL(widgetDuration),(const gchar*) duration);

        timer = soundGetTimer(infos->startTime,infos->totalPauseTime);
        soundFormatTime(timeNow,50,timer);
        gtk_label_set_text(GTK_LABEL(widgetTimer),(const gchar*) timeNow);
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(timerBar),(gdouble)pulsation*timer);

        return TRUE;

    }

    if(infos->isPlaying && infos->isPaused && infos->wantToPause){

        printf("%lld - %lld",infos->pauseTime,infos->totalPauseTime);
        soundPlay(sound,&infos->pauseTime,&infos->totalPauseTime);
        infos->isPaused = 0;
        infos->wantToPause = 0;
        gtk_image_set_from_file((GtkImage *) pauseImage, "../images/pause.png");
    }

    if(infos->isPlaying && !infos->isPaused && infos->wantToPause){

       soundPause(sound,&infos->pauseTime);
        infos->isPaused = 1;
        infos->wantToPause = 0;
        gtk_image_set_from_file((GtkImage *) pauseImage, "../images/play.png");
    }

    if(infos->isPlaying){

        infos->volume = gtk_scale_button_get_value(GTK_SCALE_BUTTON(volumeButton));
        ma_sound_set_volume(infos->sound,(float)infos->volume);

        if(ma_sound_at_end(sound)){
            printf("FIN DU SON !! ");

            infos->isPlaying = 0;
            infos->pauseTime = 0;
            infos->totalPauseTime = 0;
            infos->startTime = 0;
            infos->isPaused = 1;
            infos->wantToPause = 1;

            radioNext(infos->front,infos->rear,sound);

            if(*(infos->front) == NULL){
                return FALSE;
            }
            return TRUE;
        }
    }

    if(!infos->isPaused){
        timer = soundGetTimer(infos->startTime,infos->totalPauseTime);
        soundFormatTime(timeNow,50,timer);
        gtk_label_set_text(GTK_LABEL(widgetTimer),(const gchar*) timeNow);
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(timerBar),(gdouble)pulsation*timer);
    }

    return TRUE;
}