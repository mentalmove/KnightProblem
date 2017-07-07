/* 
 * File:   PositionRepair.h
 * Author: maltepagel
 */

#ifndef POSITIONREPAIR_H
#define	POSITIONREPAIR_H


#include <iostream>
#include <cmath>

using namespace std;

class KnightPosition;

class PositionRepair {
public:
    PositionRepair(KnightPosition*, KnightPosition*, void (KnightPosition*, int, int));
    virtual ~PositionRepair();

private:
    static char letters[];
    static int cols;

    void (*show_solution) (KnightPosition*, int, int);

    KnightPosition * start_position;
    KnightPosition * repair_partner;

    KnightPosition * mirror;
    KnightPosition * after_mirror;

    void search_mirror(void);
    void pancake(void);
    int num_next_possibilities(KnightPosition*);
};


#endif	/* POSITIONREPAIR_H */
