/* 
 * File:   KnightPosition.cpp
 * Author: maltepagel
 */

#include "KnightPosition.h"


int KnightPosition::knight_count = 0;
int KnightPosition::knight_number = 0;
int KnightPosition::cols = 0;
long KnightPosition::memory_count = 0;
char KnightPosition::letters[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P'};
int KnightPosition::reachable_positions_offset_set = 0;
int KnightPosition::failed = 0;

/**
 * Constructor.
 * Takes basic values,
 * invokes building of other knight positions,
 * memorizes reachable knight positions
 *
 * @param <int> column, row [where are we?]
 * @param <int> num_cols [number rows resp. columns of play table]
 * @param <KnightPosition***> collection [matrix to temporarily store knight positions]
 */
KnightPosition::KnightPosition(int column, int row, int num_cols, KnightPosition*** collection) {

    collection[column][row] = this;

    this->column = column;
    this->row = row;

    if (!cols)
        cols = num_cols;

    has_knight = 0;

    next_position = NULL;
    previous_position = NULL;

    first_collected_position = NULL;
    last_collected_position = NULL;

    repair_partner = NULL;
    has_repair_partner = 0;

    center_distance = abs((float) ((cols - 1) - (2 * column) - (2 * row)));

    if (!reachable_positions_offset_set) {
        if (column < 4) {
            if (row < 4)
                reachable_positions_offset = 4;
            else
                reachable_positions_offset = 6;
        } else {
            if (row < 4)
                reachable_positions_offset = 2;
            else
                reachable_positions_offset = 0;
        }
        reachable_positions_offset_set = 1;
    }

    fill_reachable_positions(collection);

    knight_count++;

    lower_or_equal = (rand() % (cols * cols + center_distance + 1 - knight_count) == 0) ? 1 : 0;

    //cout << "KnightPosition number " << knight_count << " constructed at " << letters[column] << (row + 1) << endl;
}

/**
 * Clears all allocated memory
 */
KnightPosition::~KnightPosition() {

    struct reachable_position_pointer * tmp_position, * next_tmp_position;

    if (repair_partner)
        delete(repair_partner);

    if (!first_collected_position)
        return;

    tmp_position = first_collected_position->basic_next;
    while (tmp_position) {
        next_tmp_position = first_collected_position->basic_next->basic_next;
        first_collected_position->basic_next = next_tmp_position;
        delete(tmp_position);
        tmp_position = next_tmp_position;
    }
    delete(first_collected_position->basic_next);
    delete(first_collected_position);
    first_collected_position = NULL;

    //cout << "KnightPosition " << (cols * cols - --knight_count) << " at " << letters[column] << (row + 1) << " destroyed" << endl;
}

void KnightPosition::search_last_position(KnightPosition* start_position, void (*show_solution) (KnightPosition*, int, int)) {

    if (next_position) {
        next_position->previous_position = this;
        next_position->search_last_position(start_position, show_solution);
        return;
    }

    has_repair_partner = 1;
    repair_partner = new PositionRepair(start_position, this, show_solution);
}

/**
 * Counts number of possible jumps to free field (i.e. without knight)
 *
 * @return <int> [number of possible jumps]
 */
int KnightPosition::get_num_possibilities() {
    int possibilities = 0;

    if (!first_collected_position)
        return 0;

    struct reachable_position_pointer * tmp_position = first_collected_position;
    while (tmp_position) {
        if (!tmp_position->possible_position->has_knight)
            possibilities++;
        tmp_position = tmp_position->basic_next;
    }

    return possibilities;
}

/**
 * Important heuristic: positions with more possible moves are worse
 * Less important heuristic: positions near the center are worse
 */
void KnightPosition::order_reachable_positions() {

    int possibilities, success;

    struct reachable_position_pointer * tmp_position_to_add;
    struct reachable_position_pointer * tmp_tmp_position;
    struct reachable_position_pointer * tmp_tmp_tmp_position;

    struct reachable_position_pointer * tmp_position = first_collected_position;

    while (tmp_position != NULL) {

        possibilities = tmp_position->possible_position->get_num_possibilities();

        if (!tmp_position->possible_position->has_knight) {

            if (!possibilities)
                possibilities = cols * 8;

            tmp_position_to_add = tmp_position;
            tmp_position_to_add->possible_position = tmp_position->possible_position;
            tmp_position_to_add->heuristic = cols * possibilities + tmp_position->possible_position->center_distance;
            tmp_position_to_add->next = NULL;

            if (!first_position_to_try) {
                first_position_to_try = tmp_position_to_add;
                last_position_to_try = first_position_to_try;
            } else {
                tmp_tmp_position = first_position_to_try;
                success = 0;
                while (tmp_tmp_position && !success) {
                    if (tmp_position_to_add->heuristic < tmp_tmp_position->heuristic || (lower_or_equal && tmp_position_to_add->heuristic == tmp_tmp_position->heuristic)) {
                        if (tmp_tmp_position == first_position_to_try) {
                            tmp_position_to_add->next = tmp_tmp_position;
                            first_position_to_try = tmp_position_to_add;
                            success = 1;
                        } else {
                            tmp_tmp_tmp_position = first_position_to_try;
                            while (tmp_tmp_tmp_position->next != tmp_tmp_position)
                                tmp_tmp_tmp_position = tmp_tmp_tmp_position->next;
                            tmp_position_to_add->next = tmp_tmp_tmp_position->next;
                            tmp_tmp_tmp_position->next = tmp_position_to_add;
                            success = 1;
                        }
                    }
                    tmp_tmp_position = tmp_tmp_position->next;
                }
                if (!success) {
                    last_position_to_try->next = tmp_position_to_add;
                    last_position_to_try = tmp_position_to_add;
                }
            }
        }

        tmp_position = tmp_position->basic_next;
    }
}

/**
 * Jumps to probably best position.
 * Clears this position from list.
 * Stops if play table is full
 */
void KnightPosition::do_search() {
    struct reachable_position_pointer * position_to_try = first_position_to_try;

    next_position = position_to_try->possible_position;

    first_position_to_try = (first_position_to_try->next) ? first_position_to_try->next : NULL;

    if (knight_number >= (cols * cols - 1)) {
        /*
cout << endl;
cout << "\t**********" << endl;
cout << "\t* SOLVED *" << endl;
cout << "\t**********" << endl;
cout << endl;
         */
        return;
    }

    next_position->set_knight_and_start_search(this);
}

/**
 * Last jump was bad - try the next best or, if there is none, go back
 */
void KnightPosition::another_try_from_above() {
    next_position->next_position = NULL;

    if (!first_position_to_try) {
        go_back();
        return;
    }

    if (memory_count > MEMORY_LIMIT || (memory_count && knight_number == 1)) {
        if (knight_number == 1)
            cout << "Does not seem to be solvable" << endl;
        else
            cout << "Too much recursion, I'm giving up; knights: " << knight_number << " / " << (cols * cols) << endl;
        failed = 1;
        return;
    }

    /*
cout << "I am at " << letters[column] << (row + 1) << " at step " << knight_number << " (" << memory_count << " tries)" << endl;
cout << "I came from " << letters[next_position->column] << (next_position->row + 1) << endl;
cout << "I want to try " << letters[first_position_to_try->possible_position->column] << (first_position_to_try->possible_position->row + 1) << endl;
cout << "---" << endl;
     */

    do_search();
}

/**
 * Delete this position from list of jumps and go back
 */
void KnightPosition::go_back() {
    memory_count++;
    knight_number--;
    has_knight = 0;
    previous_position->another_try_from_above();
}

/**
 * Remembers previous position,
 * estimates quality of possible jumps,
 * invokes jumping (if possible)
 *
 * @param <KnightPosition*> last_position [position where we came from,
 *                                      i.e. previous position in list of jumps]
 */
void KnightPosition::set_knight_and_start_search(KnightPosition* last_position) {
    knight_number++;
    has_knight = 1;
    previous_position = last_position;

    last_position_to_try = NULL;
    first_position_to_try = NULL;

    order_reachable_positions();

    if (!first_position_to_try) {
        go_back();
        return;
    }

    do_search();
}

/**
 * Creates a data collection for each reachable position
 *
 * @param <KnightPosition*> position [to add]
 */
void KnightPosition::add_to_reachable_positions(KnightPosition* position) {

    struct reachable_position_pointer * position_data = new reachable_position_pointer;

    position_data->possible_position = position;
    position_data->heuristic = 0;
    position_data->next = NULL;
    position_data->basic_next = NULL;

    if (!first_collected_position) {
        first_collected_position = position_data;
        last_collected_position = first_collected_position;
    } else {
        last_collected_position->basic_next = position_data;
        last_collected_position = position_data;
    }
}

/**
 * Evaluates possible positions for jumping.
 * Adds - if not already there - position to 'collection',
 * invokes putting it to list of reachable positions
 *
 * @param <KnightPosition***> collection [matrix to temporarily store knight positions]
 */
void KnightPosition::fill_reachable_positions(KnightPosition*** collection) {
    int x_move[] = {1, 2, 2, 1, -1, -2, -2, -1, 1, 2, 2, 1, -1, -2};
    int y_move[] = {-2, -1, 1, 2, 2, 1, -1, -2, -2, -1, 1, 2, 2, 1};
    int i, x, y;
    KnightPosition * tmp_position;

    for (i = reachable_positions_offset; i < (8 + reachable_positions_offset); i++) {
        x = column + x_move[i];
        y = row + y_move[i];

        if (x < 0 || x > (cols - 1) || y < 0 || y > (cols - 1))
            continue;
        if (!collection[x][y]) {
            tmp_position = new KnightPosition(x, y, cols, collection);
            add_to_reachable_positions(tmp_position);
        } else
            add_to_reachable_positions(collection[x][y]);
    }
}
