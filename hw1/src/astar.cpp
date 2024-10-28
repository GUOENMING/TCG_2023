#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <math.h>
#include <unordered_map>
#include <queue>
#include <vector>
#include <ctime>

#include "ewn.h"


#define MAX_DISTANCE 999
#define MAX_STATE_RECORD 5000000
#define POSSIBLE_ALIVE ( 1 << MAX_PIECES ) // 2**6
#define MY_MAX(a,b) ( (a) > (b) ? (a) : (b) )

// TA code


// these variables are available after calling EWN::scan_board()
int ROW;
int COL;
static int dir_value[8];

EWN::EWN() {
    row = 0;
    col = 0;
    pos[0] = 999;
    pos[MAX_PIECES + 1] = 999;
    for (int i = 1; i <= MAX_PIECES; i++) {
        pos[i] = -1;
    }
    period = 0;
    goal_piece = 0;
    n_plies = 0;
}

void set_dir_value() {
    dir_value[0] = -COL - 1;
    dir_value[1] = -COL;
    dir_value[2] = -COL + 1;
    dir_value[3] = -1;
    dir_value[4] = 1;
    dir_value[5] = COL - 1;
    dir_value[6] = COL;
    dir_value[7] = COL + 1;
}

void EWN::scan_board() {
    scanf(" %d %d", &row, &col);
    for (int i = 0; i < row * col; i++) {
        scanf(" %d", &board[i]);
        if (board[i] > 0)
            pos[board[i]] = i;
    }
    scanf(" %d", &period);
    for (int i = 0; i < period; i++) {
        scanf(" %d", &dice_seq[i]);
    }
    scanf(" %d", &goal_piece);

    // initialize global variables
    ROW = row;
    COL = col;
    set_dir_value();
}

void EWN::print_board() {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            printf("%4d", board[i * col + j]);
        }
        printf("\n");
    }
}

bool EWN::is_goal() {
    if (goal_piece == 0) {
        if (board[row * col - 1] > 0) return true;
    }
    else {
        if (board[row * col - 1] == goal_piece) return true;
    }
    return false;
}

/*
move: an integer using only 12 bits
3~0: store the direction
7~4: store the piece number
11~8: store the eaten piece (used only in history)
*/

int move_gen2(int *moves, int piece, int location) {
    int count = 0;
    int row = location / COL;
    int col = location % COL;

    bool left_ok = col != 0;
    bool right_ok = col != COL - 1;
    bool up_ok = row != 0;
    bool down_ok = row != ROW - 1;

    if (up_ok) moves[count++] = piece << 4 | 1;
    if (left_ok) moves[count++] = piece << 4 | 3;
    if (right_ok) moves[count++] = piece << 4 | 4;
    if (down_ok) moves[count++] = piece << 4 | 6;

    if (up_ok && left_ok) moves[count++] = piece << 4 | 0;
    if (up_ok && right_ok) moves[count++] = piece << 4 | 2;
    if (down_ok && left_ok) moves[count++] = piece << 4 | 5;
    if (down_ok && right_ok) moves[count++] = piece << 4 | 7;

    return count;
}

int EWN::move_gen_all(int *moves) {
    int count = 0;
    int dice = dice_seq[n_plies % period];
    if (pos[dice] == -1) {
        int small = dice - 1;
        int large = dice + 1;

        while (pos[small] == -1) small--;
        while (pos[large] == -1) large++;

        if (small >= 1)
            count += move_gen2(moves, small, pos[small]);
        if (large <= MAX_PIECES)
            count += move_gen2(moves + count, large, pos[large]);
    }
    else {
        count = move_gen2(moves, dice, pos[dice]);
    }
    return count;
}

void EWN::do_move(int move) {
    int piece = move >> 4;
    int direction = move & 15;
    int dst = pos[piece] + dir_value[direction];

    if (n_plies == MAX_PLIES) {
        fprintf(stderr, "cannot do anymore moves\n");
        exit(1);
    }
    if (board[dst] > 0) {
        pos[board[dst]] = -1;
        move |= board[dst] << 8;
    }
    board[pos[piece]] = 0;
    board[dst] = piece;
    pos[piece] = dst;
    history[n_plies++] = move;
}

void EWN::undo() {
    if (n_plies == 0) {
        fprintf(stderr, "no history\n");
        exit(1);
    }

    int move = history[--n_plies];
    int eaten_piece = move >> 8;
    int piece = (move & 255) >> 4;
    int direction = move & 15;
    int dst = pos[piece] - dir_value[direction];

    if (eaten_piece > 0) {
        board[pos[piece]] = eaten_piece;
        pos[eaten_piece] = pos[piece];
    }
    else board[pos[piece]] = 0;
    board[dst] = piece;
    pos[piece] = dst;
}


// TA code end
//---------------------------------------------------------------
// I modify EWN func here

int EWN::get_period(){
	return period;

}

int EWN::get_pos(int i){
	return pos[i];
}

int EWN::get_dice_sequence(int index){
	return dice_seq[index];
}

int EWN::get_goal_piece(){
	return goal_piece;
}

void EWN::set_tmp_pos_and_plies(int *tmp_pos, int tmp_plies){
	for(int i = 1; i <= MAX_PIECES; i++){
		pos[i] = tmp_pos[i];
	}

	n_plies = tmp_plies;
	return;
}

void EWN::change_board_to_tmp_pos(){
	for(int i = 0; i < row; i++){
		for (int j = 0; j <col; j++){
			board[ i*col+j ]=0;
		}
	}

	for(int i =1; i <= MAX_PIECES; i++){
		if(pos[i]!=-1){
			board[ pos[i] ] =i;
		}
	}
	return;
}






//----------------------------------------------------------------------
// this part is my code

// First , preprocessing input





// for every piece , create one storage
// if 1,3,5 peice is alive, use 00...00010101 for POSSIBLE_ALIVE index
// suppose below every POSSIBLE_ALIVE index is use 00...00010101

class Dice_Storage{
public:

	// I describle detailly every attribute meaning in init_storage func
	int num_seq[POSSIBLE_ALIVE][ MAX_PLIES +2 ]; // a move seq for this piece, ex: 2,6,8,10 means period  2,6,8,10 can move, start from 1
	int at_least_index[POSSIBLE_ALIVE][ MAX_PLIES +2 ];

	// in MAX_PLIES, total number of move , in one period count => at_least_index[period]	
	int total_count[POSSIBLE_ALIVE]; 
	unsigned int bit_exp[POSSIBLE_ALIVE]; 


	Dice_Storage();
	
	void init_storage(unsigned int *bit_seq, int period, int now_piece); //  init everything

};


class Dice_Move{
public:

	/*
	below  only work when MAX_PERIOD <= 32, encode dice_seq to a bit string 
	for example dice seq = 1 2 2 3 4, period =5 ,then
	dic_seq_bit[1]= 0...00001
	dic_seq_bit[2]= 0...00110
	dic_seq_bit[3]= 0...01000
	*/
	unsigned int dic_seq_bit[ MAX_PIECES + 2]; 

	//int dice_seq[MAX_PERIOD]; 
	int period;
	int goal_piece; // new add this for not reference to EWN every time we call heristic

	Dice_Storage* Piece[ MAX_PIECES + 2 ]; // a pointer to Dice_storage, null for startup dead piece  


	

	Dice_Move();

	void init_move( EWN& game );

	/*
	for example, dice seq. is 1 2 2 3 4 , we are in step 12, every piece is alive, 
	we need to know piece 2 and 4 use how many step to meet with distane " 3  <- max (col,row) "

	then we fill
	alive = 63 (0...0111111)
	now_step = 12
	distance = 3
	first = 2 
	second = 4 
	( limit is when we want to early stop for very big step)
	*/ 

	int get_step_from_2p(int first, int second, int distance , int now_step, int alive, int limit);
	
	// get a piece 's  need step to final ( ROW * COL ) , use positon not distance here for simplicity
	int get_step_to_final(int first, int position , int now_step, int alive);

	// same format as above func, but use distance not position because we don't know where to go
	int get_need_move(int now_piece, int distance, int now_step, int alive);

	// maximum for loop go "limit" times
	int caculate_dice_bit( unsigned int cacul_bit, int now_step, int distance, int limit);

	void print_for_debug( EWN& game, int mode);


};





// class method write in here

inline Dice_Move::Dice_Move(){

	//std::memset(dice_seq, 0, sizeof(dice_seq));
	period = 0;
	goal_piece = 0;
	std::memset(dic_seq_bit, 0, sizeof(dic_seq_bit));

	
	for(int i=0; i< MAX_PIECES + 2; i++){
		Piece[i] = NULL;
	}



}


inline void Dice_Move::init_move(EWN& game){

	
	goal_piece = game.get_goal_piece();
	period = game.get_period();

	// init bit string for piece 1..6
	for(int i =0; i < period; i++){
		int now = game.get_dice_sequence(i);
		dic_seq_bit[ now ] |= (1 << i);
	}

	// init 
	for(int i = 1; i <= MAX_PIECES; i++){
		if (game.get_pos(i) >= 0){
			Piece[i] = new Dice_Storage();
			Piece[i]->init_storage(dic_seq_bit, period, i);
		}
	}



	return;
}



// max return MAX_PLIES (100)
inline int Dice_Move::get_need_move(int now_piece, int distance, int now_step, int alive){
	
	int result = MAX_PLIES; // 100
	int *this_num_seq = Piece[now_piece]->num_seq[alive];
	int *this_at_least_index = Piece[now_piece]->at_least_index[alive];
	int this_total_count = Piece[now_piece]->total_count[alive];
	
	if (this_total_count == 0){
		// dead piece or just can't move
		//std::printf("calling a dead piece?\n");
		return result;
	}

	int now_index = this_at_least_index[now_step];
	//first term is max step, second term is now step in piece "now piece", remain is the maximum this piece can move 
	int remain = this_total_count - now_index;
	if ( remain < distance ){
		// not enough in 100 step
		return result;
	}

	//the minimum dice step need to wait for "distance" times
	//result = this_num_seq[now_index + distance] - now_step;

	return this_num_seq[now_index + distance] - now_step;

}


inline int Dice_Move::get_step_to_final(int first, int position, int now_step, int alive){
	
	int tmp_row = position / COL;
	int tmp_col = position % COL;

	// final is point (ROW,COL)
	int distance = MY_MAX(ROW - tmp_row, COL - tmp_col);
	int result = get_need_move( first, distance, now_step, alive);

	return result; 

}

inline int Dice_Move::caculate_dice_bit( unsigned int cacul_bit, int now_step, int distance, int limit){
	
	int result = 0;

	for(int i=1 ; i<limit; i++){

		int mod_step = (now_step + i -1 ) % period;
		if( ( cacul_bit & (1 << mod_step) ) != 0 ){
			if(++result >= distance){
				return i;
			}
		}

	}

	//beyond limit so return limit
	return limit;


}

// max return limit
inline int Dice_Move::get_step_from_2p(int first, int second, int distance , int now_step, int alive, int limit){
	
	//tmp_bit is the bit when the step first or second can move 

	//std::printf("pieces: %d %d, alive: %d\n", first, second, alive);

	unsigned int tmp_bit = Piece[first] -> bit_exp[ alive ] | Piece[second] -> bit_exp[ alive ];
	
	// copy above func code for avoiding stack call
	
	int result = 0;

	for(int i=1 ; i< limit; i++){

		int mod_step = (now_step + i -1 ) % period;
		if( ( tmp_bit & ( 1 << mod_step ) ) != 0 ){
			if( ++result >= distance ){
				return i;
			}
		}

	}

	//beyond limit so return limit
	return limit;

}











inline Dice_Storage::Dice_Storage(){

	std::memset(num_seq, 0, sizeof(num_seq));
	std::memset(at_least_index, 0, sizeof(at_least_index));
	std::memset(total_count, 0, sizeof(total_count));
	std::memset(bit_exp, 0, sizeof(bit_exp));


}



inline void Dice_Storage::init_storage(unsigned int *bit_seq, int period, int now_piece){
	

	// we have POSSIBLE_ALIVE state, construct now_piece can move bit string
	for (int state = 0; state < POSSIBLE_ALIVE; state++ ){
		
		// init bit_exp

		unsigned int temp = 0;

		if ( ( state & (1 << (now_piece-1)) ) != 0){

			temp |= bit_seq[ now_piece ];

			for(int i = now_piece + 1 ; i <= MAX_PIECES; i++){ // forward
				if( ( state & (1<< (i-1)) ) != 0 )
					break;
				temp |= bit_seq[i];
			}
			for(int i = now_piece - 1; i >= 1; i--){ // backward
				if( ( state & (1<< (i-1)) ) != 0 )
					break;
				temp |= bit_seq[i];

			}
		}

		bit_exp[state] = temp;

		// init others

		/* for example consider peice 2
		   dice seq 1 2 2 3 4 have period = 5
		   every peice is alive
		   so peice 2's temp(or bit_exp) = 0....00110
		   we need num_seq[ 111111 (all_alive correspond num 63) ]={ x, 2, 3, x....} because step 2, 3 can move
		   and at_least_index[ 63 ]={x, 0, 1, 2, 2, 2, x... } means in step i , remain "total_count[63] - at_least_index[63][i]" steps can move

		
		int index = 1; 
		for( int i = 1; i <= period; i++){

			if ( (temp & 1 ) == 1 ){

				num_seq[ state ][ index ]= i;
				index++;
			
			}
			at_least_index[ state ][i] = index-1;

			temp >>=1;
		
		}
		total_count[ state ] =  index-1;
		
		update!!!!!!!!!!!!:

			change array size from MAX_PERIOD to MAX_PLIES
			that is , I duplicate the origin seq to { x, 2, 3 , (period)+2, (period)+3, 2(period) +2, ....}
			and {x, 0, 1, 2, 2, 2, ( new period ) 2, 3, 4, 4, 4, (new period again)....}

		*/

		int index = 1;

		for (int i = 1; i <= MAX_PLIES  ; i++){
			
			int mod_step = (i-1) % period ; 
			
			//printf("%d \n", mod_step);

			if ( ( temp &  (1 << mod_step ) ) != 0 ){

				num_seq[ state ][ index ] = i ;
				index++;

			}
			at_least_index[ state ][i] = index-1;

		}
		//std::printf("state: %d %d\n", state , index);
		total_count[ state ] = index-1 ;
		//std::printf( "piece: %d state: %d count: %d\n", now_piece ,state ,total_count[state]);


	}

	return;

}


// debug func
inline void Dice_Move::print_for_debug( EWN& game, int mode){
	
	int alive[ MAX_PIECES+2 ] = {0};
	int test_bit = 0;

	for( int i = 1; i <= MAX_PIECES; i++){
		if ( game.get_pos(i) != -1 ){
			std::printf("piece %d is alive!\n", i);

			alive[i] = 1;
			test_bit |= ( 1 << (i-1) ) ;
		}
	}
	std::printf("Device seq is: \n");
	for (int i =0; i < period; i++){
		std::printf("%d ", game.get_dice_sequence(i));
	}
	std::printf("\n----end dice-------\n");

	if( mode ==	1){

		std::printf("\ntest_bit: (in number) %d\n", test_bit);


		std::printf("-----------\n");
		for( int i = 1; i <= MAX_PIECES; i++){
			if( alive[i] ){
				if (Piece[i] == NULL){
					std::printf("No ptr err !!!!!!!!\n");
				}
				std::printf("piece %d 's bit_exp (in number) is %d\n", i, Piece[i]->bit_exp[test_bit] );
				
				/*
				for(int j= 1; j<= POSSIBLE_ALIVE; j++){
					std::printf("state: %d total count %d \n", j, Piece[i]->total_count[j]);
				}
				*/

				for (int j = 1; j <= Piece[i]->total_count[test_bit]; j++){
					std::printf("num_seq index: %d value is %d\n", j, Piece[i]->num_seq[test_bit][j]);
				}
				std::printf("---------------\n");

				for (int j = 1; j <= period; j++){
					std::printf("at_least_index index: %d value is %d\n", j, Piece[i]->at_least_index[test_bit][j]);
				}

				std::printf("---------------\n\n\n");


			}
		}
		std::printf("--------  end ---------\n");

	}



	else if(mode == 2){

		for(int i = 1; i <= MAX_PIECES; i++){
			// piece distance now_step alive
			int res = get_need_move(i, 5, 2, test_bit);
			printf("piece: %d, distance:5, now_step:2, result is %d\n", i, res);

		}

	}
	else if (mode == 3){

		int a_row[2]={0,0};
		int a_col[2]={0,0};
		int a_name[2]={0,0};
		int index =0;
		for(int i =1 ; i<= MAX_PIECES; i++){
			if (alive[i]){
				a_name[index] = i;
				a_col[index] = game.get_pos(i) % COL;
				a_row[index++] = game.get_pos(i) / COL;

				if(index>=2){
					break;
				}
			}
		}
		int row_positive = MY_MAX(a_row[0]-a_row[1], a_row[1]-a_row[0]);
		int col_positive = MY_MAX(a_col[0]-a_col[1], a_col[1]-a_col[0]);
		int outcome = get_step_from_2p( a_name[0], a_name[1], MY_MAX(row_positive, col_positive), 2, test_bit, 100);
		printf("piece: %d, piece: %d, have distance : %d, and from start with step 2 need %d step to meet\n", a_name[0], a_name[1], MY_MAX(row_positive, col_positive), outcome);
	}


	return;
}







// STATE_space recording here
//------------------------------------



class MY_STATE_STORAGE{
public:

	int pos[MAX_PIECES+2]; // use 1 to 6
	int farther; // father index
	int now_step; // g()
	//int visited; // why I add this ???
	unsigned int now_alive; // bit exp for alive
	
	// because priority queue have no decrease key, use this to check it is up_to_date , register score = h() + g() == priority queue used
	int register_score; 



	MY_STATE_STORAGE() : farther(-1), now_step(-1), now_alive(0), register_score(0) { 
		for(int i = 1; i <= MAX_PIECES; i++){
			pos[i]=0;
		}
	}

	void init_my_state_class( int* tmp_pos, int tmp_farther, int tmp_step );


	bool check_visit(int new_step){
		// I found maybe the "visited" parameter is useless ?

		/* new_step = this turn's g()
		if new_step < saved now_step it is better, so we update it 
		else it is meaningless so we just pass and pop(PQ)
		*/
		return (new_step < now_step )? true : false;
	}

	void change_farther(int farther_index ){
		farther = farther_index;
	}
	void change_now_step(int tmp_step){
		now_step= tmp_step;
	}

	void change_pos(int piece, int new_pos){
		for(int i=1; i<= MAX_PIECES; i++){
			if( pos[i] == new_pos ){
				pos[i] = -1;
				now_alive ^= ( ( (unsigned int) 1 ) << (i-1));
			}
		}
		pos[piece] = new_pos;
		

	}

	void change_register_score(int tmp_score){
		register_score = tmp_score;
	}

	int check_goal(int final_piece){
		if(!final_piece){
			for(int i=1; i <= MAX_PIECES; i++){
				if(pos[i] == (COL*ROW -1)){
					return true;
				}
			}
			return false;
		}
		else if ( pos[final_piece] == (COL*ROW -1)){
			return true;
		}
		return false;
	}

	void print_status(){
		std::printf("pos: 1 2 3 4 5 6\n     ");
		for(int i=1; i<= MAX_PIECES; i++){
			std::printf("%d ", pos[i]);
		}
		std::printf("\nnow_step: %d, farther: %d\n", now_step, farther);
		std::printf("now_alive: %d,register_score: %d\n",(int) now_alive, register_score);
		return;
	}	



	//implement hash
	struct Hash {
        std::size_t operator()(const MY_STATE_STORAGE& tmp) const {

        	std::size_t hash_value = 0;

        	for(int i = 1; i <= MAX_PIECES; i++){

        		hash_value ^= ( std::hash<int>()(tmp.pos[i]) << i );
        	
        	}
            
        	return hash_value;
        }
    };
	

	bool operator==(const MY_STATE_STORAGE& tmp) const {

		return ( pos[1] == tmp.pos[1] ) && ( pos[2] == tmp.pos[2] ) && ( pos[3] == tmp.pos[3] ) && ( pos[4] == tmp.pos[4] ) && ( pos[5] == tmp.pos[5] ) && ( pos[6] == tmp.pos[6] ) ;
	}
	
};

inline void MY_STATE_STORAGE::init_my_state_class( int* tmp_pos, int tmp_farther, int tmp_step ){
	farther = tmp_farther;
	now_step = tmp_step;
	//visited = -1;
	now_alive = 0;
	for(int i = 1; i <= MAX_PIECES; i++){
			pos[i] = tmp_pos[i];
			if( pos[i]!= -1){
				now_alive |= (1 << (i-1));
			}
	}
	register_score = 0;
	return;
}









MY_STATE_STORAGE State_space[ MAX_STATE_RECORD ];

int state_use_index = 0;




// for hash table


 // MY_STATESTORAGE is the key, so I need to implement " hash method " and " == operator" 
using MY_KV = std::unordered_map < MY_STATE_STORAGE, int , MY_STATE_STORAGE::Hash > ;

MY_KV  GLOBAL_TABLE = {} ;





class HASH_TABLE_METHOD{
public:

	// HASH_TABLE should be pointer 
	MY_KV* HASH_TABLE;

	HASH_TABLE_METHOD( MY_KV* tmp);

	int check_exist_and_return_index( MY_STATE_STORAGE& tmp ); // tmp for key, index for pre state, index=-1 for not exist

	void insert_table( MY_STATE_STORAGE& tmp , int index);

	// debug func
	void debug_hash_table( MY_STATE_STORAGE& tmp , int mode );

};


// make class varible reference to global table  ''' init method '''
inline HASH_TABLE_METHOD::HASH_TABLE_METHOD( MY_KV* tmp ){
	HASH_TABLE = tmp;

}

inline int HASH_TABLE_METHOD::check_exist_and_return_index( MY_STATE_STORAGE& tmp ){
	if ( HASH_TABLE->find( tmp ) != HASH_TABLE->end() ){
		return (*HASH_TABLE)[tmp];
	}
	return -1;
}

inline void HASH_TABLE_METHOD::insert_table(  MY_STATE_STORAGE& tmp , int index ){
	(*HASH_TABLE)[tmp]=index;
	return;
}


inline void HASH_TABLE_METHOD::debug_hash_table( MY_STATE_STORAGE& tmp , int mode ){
	if (mode == 1){

		int tmp_pos[ MAX_PIECES+2 ] = {1,2,3,4,5,6};

		MY_STATE_STORAGE& test1 = State_space[1000];

		test1.init_my_state_class(tmp_pos, -1, 0);

		MY_STATE_STORAGE& test2 = State_space[1001];

		test2.init_my_state_class(tmp_pos, 1000, 10);

		if( test1 == test2 ){
			std::printf("class == normal \n");
		}
		else{
			std::printf("class == abnormal !!!!!!!\n");
		}

	}



}



// for priority queue

using PQ_MEMBER_TYPE = std::pair< int , int > ; 

struct Custom_compare{
	bool operator()( const PQ_MEMBER_TYPE&  a, const PQ_MEMBER_TYPE&  b ) const {
			// second is a index so we only compare first term

		if ( a.first >= b.first ){
			return true;
		}
		return false;
	}
};


/*
	prioty queue use < prioty ( g()+h() , minimal for high priority ), index ( a index for State_space ) >
*/
std::priority_queue < PQ_MEMBER_TYPE , std::vector< PQ_MEMBER_TYPE> , Custom_compare > GLOBAL_PQ;

class PQ_METHOD{
public:
	// PQ should be pointer 
	std::priority_queue < PQ_MEMBER_TYPE , std::vector< PQ_MEMBER_TYPE> , Custom_compare >* PQ;	

	PQ_METHOD( std::priority_queue < PQ_MEMBER_TYPE , std::vector< PQ_MEMBER_TYPE > , Custom_compare >* tmp ){
		PQ = tmp;
	}
	

};


















//-------------------------------------

// A* and heriustic func here
//------------------------------




int RATIO = 10; // g() and h() all multiply 10 for not using float


// now state is the new tried state, so its now_step should be parent's now_step + 1
int my_heuristic( MY_STATE_STORAGE& now_state , Dice_Move& DM ){
	/*
	DM.goal_piece => 0 for all, else is the only one target piece
	now_state.now_alive => bit exp 0...010101 if 1,3,5 is alive
	now_state.now_step => 250 if g()=250   (multiply 10)
	DM.get_step_to_final( 1, now_state.pos[1], now_state.now_step , now_state.now_alive) for calculate steps piece 1 need to go to goal_pos
	DM.get_step_from_2p( 1, 2, 5, now_state.now_step, now_state.alive, 10 ) for calculate steps piece 1 meet piece 2 when they have distance 5 , return limit if steps bigger than limit

	*/

	// first check goal_piece is alive
	if ( DM.goal_piece  && !( now_state.now_alive & (1 << (DM.goal_piece-1)) ) )
		return 999;


	int alive_num = 0; // we can use it to predicted how unreliable our estimate is ( bigger => unreliable)
	
	int alive_piece[MAX_PIECES+2]; 
	int alive_piece_rowcol[MAX_PIECES+2][2]; // 0 for row 1 for column

	for( int i =1; i <= MAX_PIECES; i++){

		if( ( now_state.now_alive & (1 << (i-1) ) ) != 0 ) {
			alive_piece[alive_num] = i;
			alive_piece_rowcol[alive_num][0] = now_state.pos[i] / COL;
			alive_piece_rowcol[alive_num][1] = now_state.pos[i] % COL;

			alive_num++;
		}

	}
	//std::printf("alive piece number is %d \n", alive_num );
	
	int to_end_minimum = 999;
	int tmp1;

	if(!DM.goal_piece){
		for( int i=0; i< alive_num; i++){
			tmp1 = DM.get_step_to_final( alive_piece[i], now_state.pos[ alive_piece[i] ], now_state.now_step, now_state.now_alive );
			to_end_minimum = (tmp1 < to_end_minimum) ? tmp1 : to_end_minimum ; 
		}
	}
	else{
		to_end_minimum = DM.get_step_to_final( DM.goal_piece, now_state.pos[ DM.goal_piece ], now_state.now_step, now_state.now_alive );
	}

	// upper bound
	to_end_minimum = (to_end_minimum < 50) ? to_end_minimum : 50 ;

	//std::printf("to final minimum is %d \n", to_end_minimum );


	int to_dead_minimum = 25; // the first value is the initial limit
	

	for( int i = 0; i< alive_num; i++){
		for (int j = i+1; j < alive_num; j++){

			// distance = max row or col 
			int dis = MY_MAX(  MY_MAX( alive_piece_rowcol[i][0] - alive_piece_rowcol[j][0] , alive_piece_rowcol[j][0] - alive_piece_rowcol[i][0] ) , MY_MAX( alive_piece_rowcol[i][1] - alive_piece_rowcol[j][1] , alive_piece_rowcol[j][1] - alive_piece_rowcol[i][1] ) );    
			//std::printf("dis: %d\n", dis);

			tmp1 = DM.get_step_from_2p( alive_piece[i], alive_piece[j], dis, now_state.now_step, now_state.now_alive, to_dead_minimum );
			to_dead_minimum = (tmp1 < to_dead_minimum ) ? tmp1 : to_dead_minimum ;
			if(to_dead_minimum <= 1){
				break;
			}		
		}
	}

	to_dead_minimum = (to_dead_minimum < 25) ? to_dead_minimum : 25;
	
	int easy_distance = 999;
	if(!DM.goal_piece){
		for(int i=0; i< alive_num; i++){
			int r = alive_piece_rowcol[i][0];
			int c = alive_piece_rowcol[i][1];
			tmp1 = MY_MAX(COL-c, ROW-r);
			easy_distance = (tmp1 < easy_distance) ? tmp1 : easy_distance ;
		}
	}
	else{
		int r = now_state.pos[ DM.goal_piece ] / COL;
		int c = now_state.pos[ DM.goal_piece ] % COL;
		
		easy_distance = MY_MAX(COL-c, ROW-r);

	}


	//std::printf("to dead minimum is %d \n", to_dead_minimum );

	// int g_score = now_state.now_step * RATIO;
	// int h_score = (to_end_minimum * RATIO) / 2 + ( to_dead_minimum * RATIO ) / 3;
	// int h_score = (to_end_minimum * RATIO) / 5 ;

	int h_score =  easy_distance * RATIO + ( to_end_minimum * RATIO ) / 5 +  ( to_dead_minimum * RATIO ) / 5;
	// int h_score =  easy_distance * RATIO ;

	return h_score;

}



// record print for show the search in Astar func, remember mute when handup homework



int record_board[MAX_PIECES+2][MAX_ROW * MAX_COL]={{0}};
int record_pq_times = 0;

void print_search_board(){
	std::printf("\n");
	for(int k=1; k <= MAX_PIECES; k++){
		std::printf("piece: %d\n", k);
	    for (int i = 0; i < ROW; i++) {
	        for (int j = 0; j < COL; j++) {
	            std::printf("%6d", record_board[k][ i * COL + j]);
	        }
	        std::printf("\n");
	    }	
	}
}


// func for record and print the best result , and time record

class OAO{
public:
	int piece;
	int direction;
	OAO(){};
	void init_ans(int tmp_piece, int tmp_dir);

};


inline void OAO::init_ans(int tmp_piece, int tmp_dir){
	piece = tmp_piece;
	direction = tmp_dir;
	return;	
}

bool ans_is_found = false;
int best_find_step = 999; // == best step

OAO Now_ans_inverse[ MAX_PLIES ]; // index from 0 to best_find_step-1

clock_t start_time, limit_time;
int priority_queue_search_times;



void record_ans( MY_STATE_STORAGE* tail ){

	MY_STATE_STORAGE*  tail_parent;

	int tmp_index = 0;

	while( tail->farther != -1 ){
		tail_parent = &State_space[ tail->farther ];

		// get the different_piece  from two state space which pos is different but not dead piece 
		int different_piece = -1;
		int different_dir = -1; 
		for(int i =1; i <= MAX_PIECES; i++){
			if( (tail->pos[i] != -1 ) & ( tail_parent->pos[i] != tail->pos[i] ) ){
				different_piece = i;
				for( int j = 0 ; j < 8 ; j++){
					if( dir_value[j] == ( tail->pos[i] - tail_parent->pos[i]) ){
						different_dir = j ; 
						break;
					}

				}

				break;
			}
		}


		Now_ans_inverse [ tmp_index++ ].init_ans( different_piece , different_dir );
		tail = tail_parent;
	}


	best_find_step = tmp_index;
	// here is accurate
	// std::printf("goal need step: %d\n", best_find_step);

	ans_is_found = true;
	return;

}

void print_record_ans(){
	if(!ans_is_found){
		std::printf("I find no answer QQ\n");
	}
	else{
		std::printf("%d\n", best_find_step);
		for(int i = best_find_step-1; i>=0; i--){
			std::printf("%d %d\n", Now_ans_inverse[i].piece, Now_ans_inverse[i].direction );
		}
	}
}






bool Astar(Dice_Move& DM, EWN& game){
	// initialize PQ and HAST_TABLE, State_space[] need to initialize every time we use a new state
	PQ_METHOD local_pq = PQ_METHOD( &GLOBAL_PQ);
	HASH_TABLE_METHOD local_hash_table = HASH_TABLE_METHOD( &GLOBAL_TABLE );

	int my_moves[MAX_MOVES];



	while(!local_pq.PQ->empty()){

		if(priority_queue_search_times++ % 2000 == 0){
			// std::printf("now %f second pass\n", ( (double) ( std::clock() - start_time ) )/CLOCKS_PER_SEC );
			// 18 * 2000 = 36000 
			if ( ( std::clock() - start_time ) > limit_time  || (state_use_index > ( MAX_STATE_RECORD - 36000 ) ) ){
				return false;
			}
		}

		// tmp_pair .first => g()+h() , .second => index for State_space, So we use "State_space[tmp_pair.second]"
		PQ_MEMBER_TYPE tmp_pair = local_pq.PQ->top();

		local_pq.PQ->pop();




		// first , check this tmp_pair is up_to_date
		if( State_space[ tmp_pair.second ].register_score != tmp_pair.first ){
			continue;
		}



		// second , check is goal ?

		if(State_space[ tmp_pair.second ].check_goal( game.get_goal_piece() )){
			//std::printf("goal state index: %d, farther is %d\n", tmp_pair.second ,State_space[ tmp_pair.second ].farther);
			//std::printf("goal need step: %d\n", State_space[ tmp_pair.second ].now_step);

			//std::printf("%d %d should be same?\n", local_pq.PQ->size(), GLOBAL_PQ.size());

			if ( State_space[ tmp_pair.second ].now_step < best_find_step){
				
				//std::printf("goal need step: %d\n", State_space[ tmp_pair.second ].now_step);
				//std::printf("now %f second pass\n", ( (double) ( std::clock() - start_time ) )/CLOCKS_PER_SEC );
				record_ans( &State_space[ tmp_pair.second ] );
			}

			//return true;
			continue;
		}	

		// pruned all current cost >= best cost-1  if I find an answer
		if ( ans_is_found && (State_space[tmp_pair.second].now_step >= best_find_step-1) ){
			continue;
		}


		// get dice
		// int this_turn_dice = game.get_dice_sequence(  State_space[tmp_pair.second].now_step % DM.period  );
		

		// the board parameter in EWN not change yet, use change_board_to_tmp_pos(); if need
		game.set_tmp_pos_and_plies( State_space[tmp_pair.second].pos , State_space[tmp_pair.second].now_step );

		int probability = game.move_gen_all(my_moves); // TA code help us 


		for(int i = 0; i < probability; i++){

			int next_piece = my_moves[i] >> 4;

			// origin pos (flatten style ) + dir_value[]
			int next_postition = State_space[tmp_pair.second].pos[ next_piece ] + dir_value[ (my_moves[i] & 15) ] ;


			// set pos, set farther, set step count
			State_space[state_use_index].init_my_state_class( State_space[tmp_pair.second].pos , tmp_pair.second , State_space[tmp_pair.second].now_step + 1); 
			State_space[state_use_index].change_pos( next_piece, next_postition );

			if(( game.get_goal_piece() ) && ( State_space[ state_use_index ].pos[ game.get_goal_piece() ] == -1 ) ){
				continue;
			}


			//debug
			/*
			record_pq_times ++;
			record_board[ next_piece ][ next_postition ] ++;
			if (record_pq_times % 400000 ==0){
				std::printf("search times: %d\n", record_pq_times);
				print_search_board();
				std::memset(record_board, 0, sizeof(record_board));
				
			}*/

			// check hash, -1 for not exist and value for State_space index
			int tmp_state = local_hash_table.check_exist_and_return_index( State_space[state_use_index] );
			
			//std::printf("tmp_state is: %d\n", tmp_state);

			//debug
			//State_space[state_use_index].print_status();

			//if(tmp_state == -1){
			if( (tmp_state == -1) || (State_space[ state_use_index ].now_step < State_space[ tmp_state ].now_step) ){
				// add hash table
				local_hash_table.insert_table( State_space[ state_use_index ] , state_use_index);



				// caculate h()+g() , and save
				int h_score = my_heuristic( State_space[ state_use_index ], DM );
				

				int total_score = State_space[ state_use_index ].now_step * RATIO + h_score;
				State_space[ state_use_index ].change_register_score(total_score);


				// add priority queue
				if(State_space[ state_use_index ].now_step <= MAX_PLIES ){
					local_pq.PQ->push( { total_score, state_use_index } );
				}

				state_use_index++;

				// and if tmp_state != -1 , I need to pop the old state from priority queue if exist!!!!
				if (tmp_state != -1){
					State_space[ tmp_state ].change_register_score(2147483647);
				}



				//debug
				/*
				if(state_use_index % 5000 == 0){
					std::printf("state_use_index: %d\n", state_use_index);
				}
				*/
			}
			/*
			else{
				
				// if visited before, check now_step is less than before? 
				if( State_space[ state_use_index ].now_step < State_space[ tmp_state ].now_step ){
					
					//////// this will be a bug, so I modified the structure //////

					// change to the top() get index 
					 State_space[ tmp_state ].change_farther( State_space[ state_use_index ].farther );
					 State_space[ tmp_state ].change_now_step( State_space[ state_use_index ].now_step );

					int h_score = my_heuristic( State_space[ tmp_state ], DM );
					int total_score = State_space[ tmp_state ].now_step * RATIO + h_score;

					// save h() + g() to this state 
					
					State_space[ tmp_state ].change_register_score(total_score);

					if(State_space[ tmp_state  ].now_step <= MAX_PLIES ){
						local_pq.PQ->push( { total_score, tmp_state } );
					}
					

					//don't do state index++

				}
				// else do nothing
			}*/

		}



















		// initialize a new state_space (just temporarly use an un use space, if we need create then state_use_index ++ )
		//State_space[ state_use_index ].init_my_state_class()



	}
	

	return false;
}









// main func here
//----------------------------------

int main(){
	
	EWN game;
	Dice_Move DM;

	start_time = std::clock();
	limit_time = (int) ((double) 4.3 * CLOCKS_PER_SEC ) ; 

	game.scan_board();
	
	DM.init_move(game);

	// game.print_board();


	//first state create
	int tmp_pos[ MAX_PIECES+2 ];
	for(int i =1; i<= MAX_PIECES; i++){
		tmp_pos[i] = game.get_pos(i);
	}

	state_use_index = 0;

	// farther = -1 , now_step = 0
	State_space[ state_use_index ].init_my_state_class( tmp_pos, -1, 0);
	State_space[ state_use_index ].change_register_score(100);
	state_use_index++;

	GLOBAL_PQ.push( {100, 0} );


	priority_queue_search_times=0;

	// std::printf("start_time(clocks) is %lu, limit_time(clocks) is %lu\n\n", start_time, limit_time);

	// go start!!!
	Astar(DM, game);
	print_record_ans();

	

	//DM.print_for_debug(game, 1);
	//DM.print_for_debug(game, 2);
	//DM.print_for_debug(game, 3);

	//std::printf("MY_HASH_CLASS use %d, sizeof int is %d\n", (int)sizeof(MY_HASH_CLASS), (int) sizeof(int));
	//std::printf("State_space size: %d\n", (int) sizeof(State_space));
	//std::printf("test : %d\n", MY_MAX( MY_MAX(-2,2), MY_MAX(-3,3) ) );

	//HASH_TABLE_METHOD local_hash_table = HASH_TABLE_METHOD(GLOBAL_TABLE);
	//local_hash_table.debug_hash_table(State_space[0], 1);

	return 0;

}



