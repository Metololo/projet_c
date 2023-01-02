//
// Created by Samy on 02/01/2023.
//

#ifndef PROJET_C_GTK_FUNCTIONS_H
#define PROJET_C_GTK_FUNCTIONS_H


GtkWidget *window;
GtkBuilder *builder;
GtkWidget *radio_name;
GtkWidget *next_radio;
GtkWidget *prev_radio;


void next_radio_clicked(GtkButton *b, gpointer user_data);

void prev_radio_clicked(GtkButton *b, gpointer user_data);

#endif //PROJET_C_GTK_FUNCTIONS_H
