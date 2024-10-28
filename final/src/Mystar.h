#include <cstdio>
#include <time.h>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include "MyAI.h"

// no more then 31 or will have negative int bug
#define HASH_INDEX_BIT 23
#define MAX_KEY_NUM 2

#define MAX_BOARD_NUM (BOARD_SIZE * BOARD_SIZE)
#define DOUBLE_PIECE_NUM (PIECE_NUM *2 +1)

#define MAX_PLY 150
#define MAX_SCORE 6000
#define MAX_POSSIBLE_MOVE 6


#define EXACT_SCORE 0
#define FAIL_LOW 1
#define FAIL_HIGH 2

#define COSTANT_UNSIGNED_MAX 4294967295


class TMP_board{
public:
	int board_state[ MAX_BOARD_NUM ]; // 0 - 24
	int piece_pos[ DOUBLE_PIECE_NUM ]; // 1 - 12, red is 7-12 !!!!!

	int this_turn_role; // red or blue, unchange
	int next; // red or blue;
	//int dice; // 1-6

	int num_cubes[2]; // red or blue
	int alive[2]; // bit string for alive piece

	int record_ply[MAX_PLY];
	int record_index;


	// fast border and movable piece caculate
	int moveable_piece[ (2 << PIECE_NUM ) ][PIECE_NUM+2][2]; // max two piece is movable
	int moveable_piece_count[ (2 << PIECE_NUM ) ][PIECE_NUM+2]; // 0 or 1 or 2 

	int moveable_piece_determinacy[ (2 << PIECE_NUM ) ][ PIECE_NUM+2 ]; // dice independent

	int bolder[2][MAX_BOARD_NUM][MAX_POSSIBLE_MOVE]; // 0 for red 1 for blue
	int bolder_count[2][MAX_BOARD_NUM];

	// score related
	int alive_type_score[PIECE_NUM+2];
	int alive_count_score[PIECE_NUM+2];
	int position_score[2][MAX_BOARD_NUM]; // 0 for red 1 for blue



	TMP_board(void){};

	

	void set_board(const MyAI& game);
	int is_end(void);

	void do_move(const int piece, const int start_point, const int end_point);
	void undo_move(void);

	int heuristic();
	int little_heurtistic( const int& piece_bias, const int& cacu_color, int part1_array[] );
	//int get_distance(int piece);
	//int is_alive(int piece);

	void init_border(void);
	void init_score(void);


	int get_move_all( const int& now_dice ,int result[] ); // same format as sample code

	void print_status(void);


};

class KEY_DICT{
public:
	unsigned long long int key_pos[ DOUBLE_PIECE_NUM  ][ MAX_BOARD_NUM ][ MAX_KEY_NUM ];
	unsigned long long int key_dice[ PIECE_NUM +1 ][MAX_KEY_NUM];

	KEY_DICT(void){};
	void init_key(void);
	void init_a_key(unsigned long long int dst[]);
	void gen_hash_key(const TMP_board& tmp_game, const int& now_dice, unsigned long long int dst[]); // return hash, also write full key in array
	void move_hash_key(const TMP_board& tmp_game, const int& move_piece, const int& start_pos, const int& end_pos, const int& prev_dice, const int& now_dice ,unsigned long long int dst[]);
	void move_hash_key_only_pos(const TMP_board& tmp_game, const int& move_piece, const int& start_pos, const int& end_pos , unsigned long long int dst[]);
	void move_hash_key_only_dice( const int& now_dice , unsigned long long int dst[]);
	void unmove_hash_key_only_dice( const int& prev_dice , unsigned long long int dst[]);
	int return_hash_index(const unsigned long long int dst[]);

	void debug_func(void); //print key

};



// one for red and one for blue
class HASH_ENTRY{
public:
	unsigned long long int saved_key[MAX_KEY_NUM];
	//int dice; // easy check, may not needed
	int depth;

	// piece, start index, end index
	int move_direction[3];
	int score;
	int status; // 0 for true, 1 for fail low, 2 for fail high


	HASH_ENTRY(void){};

	//int match(const unsigned long long int tmp_key[]);
	//int register_entry(TMP_board& tmp_game);
	//void debug_func(void);


};


class HASH_TABLE{
public:
	HASH_ENTRY entry[ 2 << HASH_INDEX_BIT];
	int occupy[ 2 << HASH_INDEX_BIT];


	HASH_TABLE(void){};
	void init(void); // clear table
	
	int match(int index, int search_depth , const unsigned long long int tmp_key[]); // 0 if not match, 1 if search depth <= record depth, -1 if search depth > record depth

	void get_status(int index, int out_move[], int& out_status, int& out_score);
	void register_entry(int index, const int search_depth, const unsigned long long int tmp_key[], const int in_move[], const int in_status, const int in_score );


};
class DICE_SEQ{
public:
	int table[ 2 << PIECE_NUM ][PIECE_NUM+2]; // 64 * 6, index 0 is not use!!!

	DICE_SEQ(void){};
	void init(void);

};

class TIME_RECORD{
public:
	// use CLOCKS_PER_SEC to transform sec to ticks
	clock_t start_time;
	clock_t limit_time;

	double remain_time;

	int prev_status; // -1 if exceed two many time, 0 just, 1 if remain many time

	
	int now_turn; // initialize to 1
	int estimate_max_turn; // set to 15  

	double step_time; // 4 sec ? 
	double addition_time; // 5 dec, make time out

	int now_depth; // initial 7, maybe 6-9

	TIME_RECORD(void){};

	int set_depth(void); // return depth, also start timing

	int is_timeout(void); // return 1 if exceed step_time + addition time; 
	void turn_over(void); // end this turn time, modified parameter may be
	void init(int total_time); // set up all parameter


};

void print_ulli_bit(const unsigned long long int print_key, int print_end);
void print_int_bit_with_len(const int print_key, int print_len);

int STAR_FINAL(TMP_board& tmp_game, unsigned long long int game_key[], int alpha, int beta, int now_depth);
int NegaScout(TMP_board& tmp_game, const int this_dice, unsigned long long int game_key[], int alpha, int beta, int now_depth);
void NegaScout_return_move(TMP_board& tmp_game, int this_dice ,int set_depth, int out_move[]);


extern KEY_DICT Key_manager;
extern TIME_RECORD Time_manager;
extern HASH_TABLE Transposition_table[2]; // 0 for red, 1 for blue
extern DICE_SEQ Dice_manager;

