/* 
 * File:   KnightPosition.h
 * Author: maltepagel
 */

#ifndef KNIGHTPOSITION_H
#define	KNIGHTPOSITION_H

#include <iostream>
#include <cmath>
#include <cstdlib>

using namespace std;

#include "PositionRepair.h"


/*
 IMPORTANT: HIGHER VALUES MIGHT BREAK THE STACK
 */
#define MEMORY_LIMIT 4096

class KnightPosition {
public:
    KnightPosition(int, int, int, KnightPosition***);
    virtual ~KnightPosition();


    static int cols;


    static int knight_count;
    static int knight_number;
    static long memory_count;

    static int failed;                                                          // for repair only
    PositionRepair * repair_partner;
    int has_repair_partner;


    int has_knight;
    int column, row;
    int center_distance;


    KnightPosition * next_position;
    KnightPosition * previous_position;

    struct reachable_position_pointer {
        KnightPosition * possible_position;
        int heuristic;
        struct reachable_position_pointer * next;
        struct reachable_position_pointer * basic_next;
    };
    struct reachable_position_pointer * first_collected_position;


    void set_knight_and_start_search(KnightPosition*);
    int get_num_possibilities(void);
    void another_try_from_above(void);

    void search_last_position(KnightPosition*, void (KnightPosition*, int, int));

private:
    static char letters[];


    static int reachable_positions_offset_set;


    struct reachable_position_pointer * last_collected_position;

    struct reachable_position_pointer * first_position_to_try;
    struct reachable_position_pointer * last_position_to_try;


    int lower_or_equal, reachable_positions_offset;


    void fill_reachable_positions(KnightPosition***);
    void add_to_reachable_positions(KnightPosition*);
    void order_reachable_positions(void);
    void do_search(void);
    void go_back(void);
};

#endif	/* KNIGHTPOSITION_H */

