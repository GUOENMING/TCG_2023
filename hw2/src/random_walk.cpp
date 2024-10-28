#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "ewn.h"

// RED is 0 , BLUE is 1

#define is_red_cube(x) ((x) >= 0 && (x) < (MAX_CUBES))
#define is_blue_cube(x) ((x) >= (MAX_CUBES) && (x) < (MAX_CUBES) << 1)
#define is_red_cube_fast(x) ((x) < (MAX_CUBES))
#define is_blue_cube_fast(x) ((x) >= (MAX_CUBES))
#define is_empty_cube(x) ((x) == 0xf)
#define change_player(x) ((x) ^= 1)

static const int tmp_dir_val[2][4] = {{1, COL, COL+1, -COL+1}, {-1, -COL, -COL-1, COL-1}};



void TMP_EWN::print_tmp_board(){
	fprintf(stderr, "---------- next dice is %d --------\n", tmp_dice_seq[ tmp_n_plies % PERIOD]);
	for( int i=0; i<ROW; i++){
		for( int j=0; j<COL; j++){
			if(tmp_board[ i*COL + j] != -1){
				fprintf(stderr, "%4d", tmp_board[ i*COL + j]);
			}
			else{
				fprintf(stderr, "    ");
			}
		}
		fprintf(stderr,"\n");
	}
	char role_RED[6]="RED";
	char role_BLUE[6]="BLUE";
	if( tmp_next == RED ){
		fprintf(stderr, "--------- next is %s -------\n\n", role_RED);
	}
	else
		fprintf(stderr, "--------- next is %s -------\n\n", role_BLUE);
	return;
	
}

void TMP_EWN::copy_from_tmp_ewn(const TMP_EWN& tmp_game){
	memcpy(tmp_pos, tmp_game.tmp_pos, sizeof(tmp_game.tmp_pos));
	tmp_num_cubes[0]= tmp_game.tmp_num_cubes[0];
	tmp_num_cubes[1]= tmp_game.tmp_num_cubes[1];
	tmp_next= tmp_game.tmp_next;
	memcpy(tmp_dice_seq, tmp_game.tmp_dice_seq, sizeof(tmp_game.tmp_dice_seq));
	tmp_n_plies = tmp_game.tmp_n_plies;
	memcpy(tmp_board, tmp_game.tmp_board, sizeof(tmp_game.tmp_board));

	return;
}

void TMP_EWN::copy_from_tmp_ewn_without_diceseq(const TMP_EWN& tmp_game){
	memcpy(tmp_pos, tmp_game.tmp_pos, sizeof(tmp_game.tmp_pos));
	tmp_num_cubes[0]= tmp_game.tmp_num_cubes[0];
	tmp_num_cubes[1]= tmp_game.tmp_num_cubes[1];
	tmp_next= tmp_game.tmp_next;
	/////memcpy(tmp_dice_seq, tmp_game.tmp_dice_seq, sizeof(tmp_game.tmp_dice_seq));
	tmp_n_plies = tmp_game.tmp_n_plies;
	memcpy(tmp_board, tmp_game.tmp_board, sizeof(tmp_game.tmp_board));

	return;
}



void TMP_EWN::set_tmp_ewn_dice_seq(const EWN& game){
	memcpy(tmp_dice_seq, game.dice_seq, sizeof(game.dice_seq));
	return;
}

void TMP_EWN::set_tmp_ewn_without_diceseq(const EWN& game){
	/*for(int i =0; i<MAX_CUBES*2; i++){
		fprintf(stderr, "%d ", game.pos[i]);
	}
	fprintf(stderr, "\nsize is %d \n", (int)sizeof(game.pos));
	*/
	memcpy(tmp_pos, game.pos, sizeof(game.pos));
	/*
	for(int i =0; i<MAX_CUBES*2; i++){
		fprintf(stderr, "%d ", tmp_pos[i]);
	}
	fprintf(stderr, "\nsize is %d \n", (int)sizeof(tmp_pos));
	*/

	tmp_num_cubes[0] = game.num_cubes[0];
	tmp_num_cubes[1] = game.num_cubes[1];
	tmp_next=game.next;
	//////memcpy(tmp_dice_seq, game.dice_seq, sizeof(game.dice_seq));
	tmp_n_plies = game.n_plies;
	memcpy(tmp_board, game.board, sizeof(game.board));
}




void TMP_EWN::set_tmp_ewn(const EWN& game){

	/*for(int i =0; i<MAX_CUBES*2; i++){
		fprintf(stderr, "%d ", game.pos[i]);
	}
	fprintf(stderr, "\nsize is %d \n", (int)sizeof(game.pos));
	*/
	memcpy(tmp_pos, game.pos, sizeof(game.pos));
	/*
	for(int i =0; i<MAX_CUBES*2; i++){
		fprintf(stderr, "%d ", tmp_pos[i]);
	}
	fprintf(stderr, "\nsize is %d \n", (int)sizeof(tmp_pos));
	*/

	tmp_num_cubes[0] = game.num_cubes[0];
	tmp_num_cubes[1] = game.num_cubes[1];
	tmp_next=game.next;
	memcpy(tmp_dice_seq, game.dice_seq, sizeof(game.dice_seq));
	tmp_n_plies = game.n_plies;
	memcpy(tmp_board, game.board, sizeof(game.board));
}

int TMP_EWN::tmp_move_gen_all(int *move_arr) {
    int count = 0;
    const int dice = tmp_dice_seq[ tmp_n_plies % PERIOD];
    const int offset = tmp_next == BLUE ? MAX_CUBES : 0;
    int * const self_pos = tmp_pos + offset;

    if (self_pos[dice] == -1) {
        int small = dice - 1;
        int large = dice + 1;

        while (small >= 0 && self_pos[small] == -1) small--;
        while (large < MAX_CUBES && self_pos[large] == -1) large++;

        if (small >= 0)
            count += tmp_move_gen(move_arr, small + offset, self_pos[small]);
        if (large < MAX_CUBES)
            count += tmp_move_gen(move_arr + count, large + offset, self_pos[large]);
    }
    else {
        count = tmp_move_gen(move_arr, dice + offset, self_pos[dice]);
    }

    return count;
}
/*
move: an integer using only 12 bits
3~0: store the direction
7~4: store the cube number
11~8: store the eaten cube (used only in history)
*/

int TMP_EWN::tmp_move_gen(int *move_arr, int cube, int location) {
    int count = 0;
    const int row = location / COL;
    const int col = location % COL;
    bool h_ok;  // horizontal
    bool v_ok;  // vertical
    bool rev_v_ok;  // reverse, vertical

    if (is_red_cube_fast(cube)) {
        h_ok = col != COL - 1;
        v_ok = row != ROW - 1;
        rev_v_ok = row != 0;
    }
    else {
        h_ok = col != 0;
        v_ok = row != 0;
        rev_v_ok = row != ROW - 1;
    }
    if (h_ok) move_arr[count++] = cube << 4;
    if (v_ok) move_arr[count++] = cube << 4 | 1;
    if (h_ok && v_ok) move_arr[count++] = cube << 4 | 2;
    if (h_ok && rev_v_ok) move_arr[count++] = cube << 4 | 3;

    return count;
}

// because I don't save previous move in TMP_EWN, need send it in carefully
void TMP_EWN::undo_tmp_move(int move){
	change_player(tmp_next);

	tmp_n_plies--;
	int eaten_cube = move >> 8;
    int cube = (move & 0xff) >> 4;
    int direction = move & 0xf;
    int src = tmp_pos[cube] - tmp_dir_val[tmp_next][direction];

    if (!is_empty_cube(eaten_cube)) {
        if (is_red_cube_fast(eaten_cube)) tmp_num_cubes[RED]++;
        else tmp_num_cubes[BLUE]++;
        tmp_board[tmp_pos[cube]] = eaten_cube;
        tmp_pos[eaten_cube] = tmp_pos[cube];
    }
    else tmp_board[tmp_pos[cube]] = -1;
    tmp_board[src] = cube;
    tmp_pos[cube] = src;

    return;
}



void TMP_EWN::do_tmp_move(int move) {
    int cube = move >> 4;
    int direction = move & 0xf;
    int dst = tmp_pos[cube] + tmp_dir_val[tmp_next][direction];

    if (tmp_board[dst] >= 0) {
        if (is_red_cube_fast(tmp_board[dst])) tmp_num_cubes[RED]--;
        else tmp_num_cubes[BLUE]--;
        tmp_pos[tmp_board[dst]] = -1;
    }
    tmp_board[tmp_pos[cube]] = -1;
    tmp_board[dst] = cube;
    tmp_pos[cube] = dst;
    tmp_n_plies++;
    change_player(tmp_next);
    return;
}

// return record move that can be used in undo
int TMP_EWN::do_tmp_move_return_record(int move){
    int cube = move >> 4;
    int direction = move & 0xf;
    int dst = tmp_pos[cube] + tmp_dir_val[tmp_next][direction];

    if (tmp_board[dst] >= 0) {
        if (is_red_cube_fast(tmp_board[dst])) tmp_num_cubes[RED]--;
        else tmp_num_cubes[BLUE]--;
        tmp_pos[tmp_board[dst]] = -1;
        move |= tmp_board[dst] << 8;
    }
    else move |= 0xf00;
    tmp_board[tmp_pos[cube]] = -1;
    tmp_board[dst] = cube;
    tmp_pos[cube] = dst;
    tmp_n_plies++;
    change_player(tmp_next);
    return move;

}


// BLUE win return 1, RED win return -1
int TMP_EWN::tmp_is_over() {
    if (tmp_num_cubes[RED] == 0){
    	// print_tmp_board() //debug
    	return 1;
    }
    if (tmp_num_cubes[BLUE] == 0){
    	// print_tmp_board() //debug
    	return -1;
    }
    if (is_blue_cube(tmp_board[0])){
    	// print_tmp_board() //debug
    	return 1;
    }
    if (is_red_cube(tmp_board[ROW * COL - 1])){
    	// print_tmp_board() //debug
    	return -1;
    }
    return 0;
}



int reuse_move[MAX_MOVES];

// input this turn's board, so it's needed to check win or not before random walk
// return -1 for red win , 1 for blue win 
int do_one_try(const EWN& game, TMP_EWN& tmp_game){
	// duplicate for not corrupt origin one 
	tmp_game.set_tmp_ewn(game);

	int limit = 1000;
	int end = 0;


	//while(True)
	while(limit--){
		//tmp_game.print_tmp_board(); // debug

		// check win or not 
		end = tmp_game.tmp_is_over();
		if(end){
			return end;
		}
		
		// get next move 
		int available = tmp_game.tmp_move_gen_all(reuse_move);

		// do random move
		tmp_game.do_tmp_move(reuse_move[arc4random_uniform(available)]);

		
	}


	// this should not be happened ,if happened return RED win :(
	fprintf(stderr, "why don't end???\n");
	return -1;
}

int do_one_try_tmpewn_version(const TMP_EWN& source_tmp_game, TMP_EWN& use_tmp_game ){

	// use_tmp_game.copy_from_tmp_ewn( source_tmp_game );
	use_tmp_game.copy_from_tmp_ewn_without_diceseq( source_tmp_game );

	int limit = 1000;
	int end = 0;


	//while(True)
	while(limit--){
		//use_tmp_game.print_tmp_board(); // debug

		// check win or not 
		end = use_tmp_game.tmp_is_over();
		if(end){
			return end;
		}
		
		// get next move 
		int available = use_tmp_game.tmp_move_gen_all(reuse_move);

		// do random move
		use_tmp_game.do_tmp_move(reuse_move[arc4random_uniform(available)]);

		
	}

	// this should not be happened ,if happened return RED win :(
	fprintf(stderr, "why don't end???\n");
	return -1;

}


int do_one_try_tmpewn_version_and_with_check(const TMP_EWN& source_tmp_game, TMP_EWN& use_tmp_game ){

	// use_tmp_game.copy_from_tmp_ewn( source_tmp_game );
	use_tmp_game.copy_from_tmp_ewn_without_diceseq( source_tmp_game );

	int limit = 1000;
	int end = 0;


	//while(True)
	while(limit--){
		//use_tmp_game.print_tmp_board(); // debug

		// check win or not 
		end = use_tmp_game.tmp_is_over();
		if(end){
			return end;
		}
		
		// get next move 
		int available = use_tmp_game.tmp_move_gen_all(reuse_move);
		//int color = is_red_cube_fast(reuse_move[0] >> 4); // next color is RED then 1 else 0

		int check_cube, check_dst, check_direction;


		if( is_red_cube_fast( reuse_move[0] >> 4 ) ){
			// check win here
			for(int i=0; i<available; i++){
				check_cube = reuse_move[i] >> 4; // cube
				check_direction = reuse_move[i] & 0xf; //direction
				check_dst = use_tmp_game.tmp_pos[check_cube] + tmp_dir_val[ use_tmp_game.tmp_next ][check_direction]; // next pos
				
				// eat check
				if( use_tmp_game.tmp_pos[ check_dst ] >= 0 ){
					//if( is_red_cube_fast(check_cube) ){
						if( use_tmp_game.tmp_num_cubes[BLUE]==1 && is_blue_cube_fast( use_tmp_game.tmp_pos[ check_dst ]) ){
							return -1; // red win
						}
					/*}
					else{ 
						if( use_tmp_game.tmp_num_cubes[RED]==1 && is_red_cube_fast( use_tmp_game.tmp_pos[ check_dst ]) ){
							return 1; // blue win
						}
					}*/
				}

				// goal check

				//if( is_red_cube_fast(check_cube)){
					if( check_dst == (ROW * COL - 1) ){
						return -1; //red win
					}
				/*}
				else{
					if( check_dst == 0 ){
						return 1; //blue win
					}
				}*/
			}
		}
		else{

			for(int i=0; i<available; i++){
				check_cube = reuse_move[i] >> 4; // cube
				check_direction = reuse_move[i] & 0xf; //direction
				check_dst = use_tmp_game.tmp_pos[check_cube] + tmp_dir_val[ use_tmp_game.tmp_next ][check_direction]; // next pos
				
				// eat check
				if( use_tmp_game.tmp_pos[ check_dst ] >= 0 ){
					/*if( is_red_cube_fast(check_cube) ){
						if( use_tmp_game.tmp_num_cubes[BLUE]==1 && is_blue_cube_fast( use_tmp_game.tmp_pos[ check_dst ]) ){
							return -1; // red win
						}
					}
					else{*/ 
						if( use_tmp_game.tmp_num_cubes[RED]==1 && is_red_cube_fast( use_tmp_game.tmp_pos[ check_dst ]) ){
							return 1; // blue win
						}
					//}
				}

				// goal check

				/*if( is_red_cube_fast(check_cube)){
					if( check_dst == (ROW * COL - 1) ){
						return -1; //red win
					}
				}
				else{*/
					if( check_dst == 0 ){
						return 1; //blue win
					}
				//}
			}





		}

		// do random move
		use_tmp_game.do_tmp_move(reuse_move[arc4random_uniform(available)]);
		
	}

	// this should not be happened ,if happened return RED win :(
	fprintf(stderr, "why don't end???\n");
	return -1;

}





void board_function_test(){
	EWN game;
	TMP_EWN OAO_game;

	game.init_board();
	
	//red test
	fprintf( stderr, "\n\n winner is %d ( -1 is RED 1 is BLUE ) \n\n\n", do_one_try(game, OAO_game));


	//blue test
	int num_moves = game.move_gen_all(reuse_move);
	game.do_move( reuse_move[arc4random_uniform(num_moves)] );
	fprintf( stderr, "\n\n winner is %d ( -1 is RED 1 is BLUE ) \n\n\n", do_one_try(game, OAO_game));


	TMP_EWN OAO_game_2;
	// tmp_ewn version blue test
	OAO_game_2.set_tmp_ewn(game);
	fprintf( stderr, "\n\n winner is %d ( -1 is RED 1 is BLUE ) \n\n\n", do_one_try_tmpewn_version(OAO_game_2, OAO_game));


	fprintf( stderr,"----------------------------\n\n\n\n-------- new test --------\n\n\n\n\n\n");
	// red test
	num_moves = OAO_game_2.tmp_move_gen_all(reuse_move);
	int this_move = reuse_move[arc4random_uniform(num_moves)];
	int saved_move = OAO_game_2.do_tmp_move_return_record(  this_move );
	fprintf( stderr, "\n\n winner is %d ( -1 is RED 1 is BLUE ) \n\n\n", do_one_try_tmpewn_version(OAO_game_2, OAO_game));
	
	// do and undo test
	fprintf(stderr, " after \n\n" );
	OAO_game_2.print_tmp_board();
	OAO_game_2.undo_tmp_move( saved_move);

	fprintf(stderr, " before \n\n" );
	OAO_game_2.print_tmp_board();
	saved_move = OAO_game_2.do_tmp_move_return_record(  this_move );

	fprintf(stderr, " after \n\n" );
	OAO_game_2.print_tmp_board();


}



/*
int main(){
	board_function_test();
	return 0;
}
*/












