/* 
 * File:   main.cpp
 * Author: maltepagel
 */

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <time.h>

using namespace std;

#include "KnightPosition.h"
#include "PositionRepair.h"


#define ASK_USER 0

#define COL_MIN 5
#define COL_MAX 16
#define DEFAULT_COLS 8


void ask_user(void);
void show_solution(KnightPosition*, int, int);

char letters[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P'};

int cols;
int column;
int row;

void ask_user() {
    int i;
    char c;

    cout << endl;

    cout << "How many rows resp. columns (" << COL_MIN << " - " << COL_MAX << ")?" << endl;
    scanf("%d", &i);
    if (i < COL_MIN || i > COL_MAX) {
        cout << "Invalid value! Taking " << DEFAULT_COLS << endl;
        cols = DEFAULT_COLS;
    } else
        cols = i;


    cout << "Start in which column ('A' to '" << (letters[cols - 1]) << "')?" << endl;
    scanf("%s", &c);
    i = (int) c - 97;
    if (i >= 0 && i < cols)
        column = i;
    else {
        i = (int) c - 65;
        if (i >= 0 && i < cols)
            column = i;
        else {
            column = rand() % cols;
            cout << "Invalid value! Taking " << letters[column] << endl;
        }
    }


    cout << "Start in which row (1 to " << cols << ")?" << endl;
    scanf("%d", &i);
    if (i > 0 && i <= cols)
        row = i - 1;
    else {
        row = rand() % cols;
        cout << "Invalid value! Taking " << (row + 1) << endl;
    }
}

/**
 * Preparing show,
 * eventually cleaning memory
 */
void show_solution(KnightPosition * start_position, int delete_all, int closed_path) {

    int i, j;

    int ** show_collection = new int*[cols];
    for (i = 0; i < cols; i++) {
        show_collection[i] = new int[cols];
        for (j = 0; j < cols; j++)
            show_collection[i][j] = 0;
    }
    i = 0;
    KnightPosition * position = start_position;
    KnightPosition * position_to_delete;
    do {
        show_collection[cols - position->row - 1][position->column] = ++i;
        position_to_delete = position;
        if (position->next_position)
            position = position->next_position;
        if (delete_all)
            delete(position_to_delete);
    }    while (position->next_position);
    if (delete_all)
        delete(position);
    show_collection[cols - position->row - 1][position->column] = ++i;


    /**
     * Dont' blame me for not using 'cout' consequently -
     * formatted output is much more readable
     */
    if (closed_path)
        for (i = 0; i < cols; i++)
            cout << "******";
    cout << endl;
    for (i = 0; i < cols; i++) {
        if (closed_path)
            cout << "* ";
        printf("%2d", (cols - i));
        cout << " |";
        for (j = 0; j < cols; j++)
            printf("%5d", show_collection[i][j]);
        if (closed_path)
            cout << " *";
        cout << endl;
    }
    if (closed_path)
        cout << "* ";
    cout << "     ";
    for (i = 0; i < cols; i++)
        cout << "-----";
    if (closed_path)
        cout << "*";
    cout << endl;
    if (closed_path)
        cout << "* ";
    printf("%4s", " ");
    for (i = 0; i < cols; i++)
        printf("%5c", letters[i]);
    if (closed_path)
        cout << " *";
    cout << endl;
    if (closed_path) {
        for (i = 0; i < cols; i++)
            cout << "******";
        cout << endl;
    }
}

int main (int argc, char** argv) {

    KnightPosition * start_position;
    KnightPosition *** complete_collection;
    int i, j;

    time_t t;
    time(&t);
    srand((unsigned int) t);

    if (ASK_USER)
        ask_user();
    else {
        cols = DEFAULT_COLS;
        column = rand() % cols;
        row = rand() % cols;
    }

    /**
     * Matrix of pointers needed for a moment
     */
    complete_collection = new KnightPosition**[cols];
    for (i = 0; i < cols; i++)
        complete_collection[i] = new KnightPosition*[cols];
    for (i = 0; i < cols; i++)
        for (j = 0; j < cols; j++)
            complete_collection[i][j] = NULL;

    /**
     * 'start_position' is - no one would ever guess this - the field where to start,
     * temporarily stored at [column][row] in 'complete_collection'
     */
    start_position = new KnightPosition(column, row, cols, complete_collection);

    /**
     * All fields should be initialized; matrix not needed any longer
     */
    delete[](complete_collection);


    /**
     * Setting a knight on 'start_position' and invoking path search
     */
    start_position->set_knight_and_start_search(NULL);


    /**
     * If finding closed path is impossible: show result;
     * otherwise: search for closed path
     */
    if (KnightPosition::failed || ((cols * cols) % 2 != 0))
        show_solution(start_position, 1, 0);
    else
        start_position->search_last_position(start_position, &show_solution);


    //show_solution(start_position, 1, 0);


    /**
     * The result should be visible also on windows systems
     */
#ifdef _WIN32
    char c = 'n';
    while (c != 'y') {
        cout << "Quit application [y/n]?" << endl;
        scanf("%s", &c);
    }
#endif


    return 0;
}
