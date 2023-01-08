#define MINIAUDIO_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
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
GtkWidget *buttonSettings;
GtkWidget *stack;
GtkWidget *settingsMenu;
GtkWidget *leaveSettings;
GtkWidget *mainMenu;
GtkWidget *radioMode;
GtkWidget *radioFinish;
GtkWidget *defaultVolume;
GtkWidget *likeButton;
GtkWidget *likeImage;

G_MODULE_EXPORT void next_radio_clicked(GtkButton *b, gpointer user_data);
G_MODULE_EXPORT void prev_radio_clicked(GtkButton *b, gpointer user_data);
G_MODULE_EXPORT gboolean test(gpointer data);
G_MODULE_EXPORT void pauseSound(GtkButton *b, gpointer user_data);
void setRadioNoSound(DataPLAY *infos);
void goSettings();
void exitSettings(GtkButton *b, gpointer user_data);
void likeApp();

int radioChange(DataPLAY *list,int move);

int main(int argc,char **argv) {

    DataPLAY radioData; // Structure used to be passed to callbacks functions for gtk ( many infos )

    Radio *radioListHead = NULL; // Head of the radio list
    Radio *radioListTail = NULL; // Tail of the radio list
    Radio *currentRadio = NULL; // This is a pointer to the current Radio playing

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

    char radioModeActive[50]; // Use to set the radioMode setting.
    char radioFinishActive[50]; // Use to set the radioFinish setting.

    char volumeValue[3];
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

    radioListInit(mysql,&radioListHead,&radioListTail);


    radioData.current = radioListHead;
    radioData.radioListHead = radioListHead;
    radioData.radioListTail = radioListTail;

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
    radioData.isSwitching = 0;
    radioData.soundInitialized = 0;
    radioData.isPlaying = 0;
    radioData.pauseTime = 0;
    radioData.totalPauseTime = 0;
    radioData.startTime = 0;
    radioData.isPaused = 1;
    radioData.wantToPause = 0;
    radioData.mysql = mysql;
    radioData.changeRadio = 0;

    // We get the default volume
    volume = atof(settingsGetValue(settings,"defaultVolume"))/10;

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
    buttonSettings = GTK_WIDGET(gtk_builder_get_object(builder, "button-settings"));
    stack = GTK_WIDGET(gtk_builder_get_object(builder, "mainStack"));
    settingsMenu = GTK_WIDGET(gtk_builder_get_object(builder, "page1"));
    leaveSettings = GTK_WIDGET(gtk_builder_get_object(builder, "leave-settings"));
    mainMenu = GTK_WIDGET(gtk_builder_get_object(builder, "page0"));
    radioMode = GTK_WIDGET(gtk_builder_get_object(builder, "radio-mode"));
    radioFinish = GTK_WIDGET(gtk_builder_get_object(builder, "radio-finish"));
    defaultVolume = GTK_WIDGET(gtk_builder_get_object(builder, "default-volume"));
    likeButton = GTK_WIDGET(gtk_builder_get_object(builder, "like-button"));
    likeImage = GTK_WIDGET(gtk_builder_get_object(builder, "like-image"));

    g_object_unref(builder);

    g_signal_connect(next_radio, "clicked", G_CALLBACK(next_radio_clicked), &radioData);
    g_signal_connect(prev_radio, "clicked", G_CALLBACK(prev_radio_clicked), &radioData);
    g_signal_connect(pauseButton, "clicked", G_CALLBACK(pauseSound), &radioData);
    g_signal_connect(pauseButton, "clicked", G_CALLBACK(pauseSound), &radioData);
    g_signal_connect(buttonSettings, "clicked", G_CALLBACK(goSettings), NULL);
    g_signal_connect(leaveSettings, "clicked", G_CALLBACK(exitSettings),settings);
    g_signal_connect(likeButton, "clicked", G_CALLBACK(likeApp), NULL);

    gtk_scale_button_set_value(GTK_SCALE_BUTTON(volumeButton),radioData.volume);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(defaultVolume),volume*10);

    gtk_combo_box_set_active_id(GTK_COMBO_BOX(radioMode),settingsGetValue(settings,"radioMode"));
    gtk_combo_box_set_active_id(GTK_COMBO_BOX(radioFinish),settingsGetValue(settings,"radioFinish"));

    if(!radioIsEmpty(radioListHead)){
        gtk_label_set_text(GTK_LABEL(radio_name),(const gchar*) radioListHead->name);
    }

    g_timeout_add(200, test, &radioData);

    gtk_widget_show(window);
    gtk_main();


    mysql_close(mysql);
    ma_engine_uninit(&engine);
    free(settings);
    radioStop(&radioFront,&radioRear);
    radioListDeleteAll(&radioListHead,&radioListTail);

    return 0;
}

/* -------------------------------------------
 *   END OF MAIN HERE
 * -------------------------------------------
 */

/* -------------------------------------------
 *   GTK CALLBACKS FUNCTIONS DOWN THERE
 * -------------------------------------------
 */



void likeApp(){
    printf("Like\n");
}

void goSettings(){
    gtk_stack_set_visible_child(GTK_STACK(stack),settingsMenu);
}

void exitSettings(GtkButton *b, gpointer user_data){
    SETTING *settings = (SETTING *) user_data;

    char radioModeValue[50];
    char radioFinishValue[50];
    char defaultVolumeValue[50];

    snprintf(defaultVolumeValue,50,"%.0lf",(gtk_spin_button_get_value(GTK_SPIN_BUTTON(defaultVolume))));
    strncpy(radioModeValue, gtk_combo_box_get_active_id(GTK_COMBO_BOX(radioMode)),50);
    strncpy(radioFinishValue, gtk_combo_box_get_active_id(GTK_COMBO_BOX(radioFinish)),50);

    settingsSet(settings,"defaultVolume",defaultVolumeValue);
    settingsSet(settings,"radioMode",radioModeValue);
    settingsSet(settings,"radioFinish",radioFinishValue);

    gtk_stack_set_visible_child(GTK_STACK(stack),mainMenu);

}

void next_radio_clicked(GtkButton *b, gpointer user_data){
    DataPLAY *list = (DataPLAY *) user_data;

    if(list->radioListHead == NULL){
        gtk_label_set_text(GTK_LABEL(radio_name),(const gchar*) "No radio created");
        return;
    }
    if(list->isSwitching) return;
    list->isSwitching = 1;


    int radioRes = radioChange(list,1);
    switch(radioRes){
        case 0:
        case 2:
            list->soundInitialized = 0;
            setRadioNoSound(list);
            break;
    }
    list->isSwitching = 0;
}



void prev_radio_clicked(GtkButton *b, gpointer user_data){
    DataPLAY *list = (DataPLAY *) user_data;

    if(list->radioListHead == NULL){
        gtk_label_set_text(GTK_LABEL(radio_name),(const gchar*) "No radio created");
        return;
    }

    if(list->isSwitching) return;
    list->isSwitching = 1;

    int radioRes = radioChange(list,-1);

    switch(radioRes){
        case 0:
        case 2:
            list->soundInitialized = 0;
            setRadioNoSound(list);
            break;
    }

    list->isSwitching = 0;

}

int radioChange(DataPLAY *list,int move){
    if(move == 1) list->current = list->current->next;
    if(move == -1) list->current = list->current->prev;

    radioStop(list->front,list->rear);

    if(list->soundInitialized == 1){
        ma_sound_uninit(list->sound);
    }
    int radioRes = radioInit(list->mysql,list->current->name,list->front,list->rear);

    if(radioRes == 0 || radioRes == 2){
        return radioRes;
    }
    list->isPlaying = 0;
    list->pauseTime = 0;
    list->totalPauseTime = 0;
    list->startTime = 0;
    list->isPaused = 1;
    list->wantToPause = 1;

    gtk_label_set_text(GTK_LABEL(radio_name),(const gchar*) list->current->name);
    return 1;

}

void pauseSound(GtkButton *b, gpointer user_data){

    DataPLAY *infos = (DataPLAY *) user_data;
    infos->wantToPause = 1;

}

void setRadioNoSound(DataPLAY *infos){
    gtk_label_set_text(GTK_LABEL(radio_name),(const gchar*) infos->current->name);
    gtk_label_set_text(GTK_LABEL(musicName),(const gchar*) "No sound");
    gtk_label_set_text(GTK_LABEL(musicGenre),(const gchar*) "");
    gtk_label_set_text(GTK_LABEL(widgetDuration),(const gchar*) "0:00");
    gtk_label_set_text(GTK_LABEL(widgetTimer),(const gchar*) "0:00");
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(timerBar),0.0);
}

void setRadioSound(DataPLAY *infos){

    char duration[50];
    char timeNow[50];
    float timer = 0;

    Music *currentSong = getFront(infos->front);
    gdouble pulsation = 1.0/currentSong->duration;
    gtk_label_set_text(GTK_LABEL(musicName),(const gchar*) currentSong->name);
    gtk_label_set_text(GTK_LABEL(musicGenre),(const gchar*) currentSong->genre);
    gtk_image_set_from_file((GtkImage *) pauseImage, "../images/play.png");

    soundFormatTime(duration,50,(float) currentSong->duration);
    gtk_label_set_text(GTK_LABEL(widgetDuration),(const gchar*) duration);

    gtk_label_set_text(GTK_LABEL(widgetTimer),(const gchar*) "0:00");
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(timerBar),(gdouble)pulsation*timer);
}

gboolean test(gpointer data){

    DataPLAY *infos = (DataPLAY *) data;

    if(infos->isSwitching == 1){
        return TRUE;
    }

    Music **front = infos->front;
    Music *currentSong = getFront(front);

    if(currentSong == NULL){
        setRadioNoSound(infos);
        return TRUE;
    }
    if(!infos->isPlaying && infos->isPaused && !infos->wantToPause) {
        setRadioSound(infos);
    }

    Music **rear = infos->rear;
    ma_engine *engine = infos->engine;
    ma_sound *sound = infos->sound;
    MYSQL *mysql = infos->mysql;

    float timer = 0;


    ma_result result;
    char duration[50];
    char timeNow[50];

    gdouble pulsation = 1.0/currentSong->duration;

    //if no sound is playing we start it...

    if(!infos->isPlaying && infos->isPaused && infos->wantToPause){
        result = ma_sound_init_from_file(engine,currentSong->path, 0, NULL, NULL, sound);
        infos->soundInitialized = 1;
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

            if(*(infos->front) == NULL && *(infos->rear) == NULL){
                int test = radioInit(mysql,infos->current->name,infos->front,infos->rear);
                if(!test){
                    fprintf(stderr,"impossible de relancer la radio : erreur");
                    return FALSE;
                }
            }
            return TRUE;
        }
    if(!infos->isPaused){
        timer = soundGetTimer(infos->startTime,infos->totalPauseTime);
        soundFormatTime(timeNow,50,timer);
        gtk_label_set_text(GTK_LABEL(widgetTimer),(const gchar*) timeNow);
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(timerBar),(gdouble)pulsation*timer);
    }


    }

    return TRUE;


}