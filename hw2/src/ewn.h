#ifndef EWN_H
#define EWN_H

#define ROW 6
#define COL 7
#define MAX_CUBES 6
#define RED 0
#define BLUE 1

#define PERIOD 21      // the period of the given dice sequence
#define MAX_PLIES 150  // the average ply of a game is far smaller than this number
#define MAX_MOVES 8

class EWN {
public:
    int board[ROW * COL];
    int pos[MAX_CUBES * 2];  // red cubes: 0~5, blue cubes: 6~11
    int num_cubes[2];
    int next;  // next player

    int dice_seq[PERIOD];
    int history[MAX_PLIES];
    int n_plies;

//public:
    void init_board();
    bool is_over();
    int move_gen_all(int *move_arr);
    void do_move(int move);
    void undo();

    int greedy();
    // int heuristic();
    friend int search(EWN &, int alpha, int beta, int depth);
};

int search_and_get_move(EWN &, int depth);
int get_random_move(EWN &);

#endif

class TMP_EWN{
public:
    int tmp_pos[MAX_CUBES*2];
    int tmp_num_cubes[2];
    int tmp_next;
    int tmp_board[ROW * COL];
    int tmp_dice_seq[PERIOD];
    int tmp_n_plies;

    TMP_EWN(){};
    void set_tmp_ewn(const EWN& game);
    int tmp_move_gen_all(int *move_arr);
    int tmp_move_gen(int *move_arr, int cube, int location);
    void do_tmp_move(int move);
    int tmp_is_over();
    void print_tmp_board(); // for debug

    void copy_from_tmp_ewn(const TMP_EWN& tmp_game);
    void undo_tmp_move(int move);
    int do_tmp_move_return_record(int move);

    void set_tmp_ewn_dice_seq(const EWN& game);
    void copy_from_tmp_ewn_without_diceseq(const TMP_EWN& tmp_game);
    void set_tmp_ewn_without_diceseq(const EWN& game);
};


int do_one_try_tmpewn_version(const TMP_EWN& source_tmp_game, TMP_EWN& use_tmp_game );
int do_one_try(const EWN& game, TMP_EWN& tmp_game);
int do_one_try_tmpewn_version_and_with_check(const TMP_EWN& source_tmp_game, TMP_EWN& use_tmp_game );

