/* 
 * File:   PositionRepair.cpp
 * Author: maltepagel
 */

#include "PositionRepair.h"
#include "KnightPosition.h"


char PositionRepair::letters[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P'};
int PositionRepair::cols = 0;

PositionRepair::PositionRepair(KnightPosition* start, KnightPosition* end_position, void (*solution_function) (KnightPosition*, int, int)) {

    this->repair_partner = end_position;
    this->show_solution = solution_function;
    this->start_position = start;

    if (!cols)
        cols = KnightPosition::cols;

    struct KnightPosition::reachable_position_pointer * tmp_position = repair_partner->first_collected_position;
    while (tmp_position != NULL) {
        if (tmp_position->possible_position == start_position) {
            show_solution(start_position, 1, 1);
            return;
        }
        tmp_position = tmp_position->basic_next;
    }

    show_solution(start_position, 0, 0);
    cout << endl;

    //cout << "PositionRepair at " << letters[repair_partner->column] << (repair_partner->row + 1) << endl;

    search_mirror();
}

PositionRepair::~PositionRepair() {
    //cout << "PositionRepair at " << letters[repair_partner->column] << (repair_partner->row + 1) << " destroyed" << endl;
}

void PositionRepair::pancake() {
    KnightPosition * tmp;

    mirror->next_position = repair_partner;
    repair_partner->next_position = repair_partner->previous_position;
    repair_partner->previous_position = mirror;

    tmp = repair_partner->next_position;
    while (tmp != after_mirror) {
        tmp->next_position = tmp->previous_position;
        tmp = tmp->next_position;
    }

    after_mirror->next_position = NULL;

    //cout << "mirror at " << letters[mirror->column] << (mirror->row + 1) << endl;

    start_position->search_last_position(start_position, show_solution);
}

int PositionRepair::num_next_possibilities(KnightPosition* position) {
    int count = 0;

    KnightPosition * tmp_mirror;
    KnightPosition * tmp_after_mirror;

    struct KnightPosition::reachable_position_pointer * tmp_position = position->first_collected_position;
    while (tmp_position != NULL) {
        tmp_mirror = tmp_position->possible_position;
        tmp_after_mirror = tmp_mirror->next_position;

        tmp_position = tmp_position->basic_next;

        if (tmp_after_mirror == position)
            continue;

        if (tmp_after_mirror->has_repair_partner)
            continue;

        count++;
    }

    return count;
}

void PositionRepair::search_mirror() {

    int tmp_heuristic, heu, next_possibilities, sub, total;
    KnightPosition * tmp_mirror;
    KnightPosition * tmp_after_mirror;

    mirror = NULL;
    after_mirror = NULL;
    tmp_heuristic = cols * cols * cols;

    struct KnightPosition::reachable_position_pointer * tmp_position = repair_partner->first_collected_position;
    while (tmp_position != NULL) {
        tmp_mirror = tmp_position->possible_position;
        tmp_after_mirror = tmp_mirror->next_position;

        tmp_position = tmp_position->basic_next;

        if (tmp_after_mirror == repair_partner)
            continue;

        if (tmp_after_mirror->has_repair_partner)
            continue;

        next_possibilities = num_next_possibilities(tmp_after_mirror);
        if (!next_possibilities)
            continue;

        heu = tmp_after_mirror->center_distance;
        sub = abs((float) (tmp_after_mirror->column - start_position->column)) + abs((float) (tmp_after_mirror->row - start_position->row));
        total = heu + sub;
        total *= (cols - next_possibilities);

        if (total < tmp_heuristic) {
            tmp_heuristic = total;
            mirror = tmp_mirror;
            after_mirror = tmp_after_mirror;
        }
    }

    if (mirror == NULL)
        return;

    pancake();
}
