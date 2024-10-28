#include "Mystar.h"
#include "MyAI.h"

#define is_red_cube_fast(x) ((x) > (PIECE_NUM))
#define is_blue_cube_fast(x) ((x) <= (PIECE_NUM))
#define change_player(x) ((x)^=1)
#define MY_MAX(a,b) ( (a) > (b) ? (a) : (b) )
#define MY_MIN(a,b) ( (a) < (b) ? (a) : (b) )

#define PART1_SCALE 50
#define PART2_SCALE 100
#define EATEN_SCALE 10 // 6 - 36
#define DEAD_SCORE 0

//KEY_DICT Key_manager;
//TIME_RECORD Time_manager;
//HASH_TABLE Transposition_table[2]; // 0 for red, 1 for blue
//DICE_SEQ Dice_manager;

void print_ulli_bit(const unsigned long long int print_key, int print_end){
	for(int i =63; i >= 0; i--){
		unsigned long long int tmp_key = ((unsigned long long int)1) << i;
		if(tmp_key&print_key){
			fprintf(stderr, "1");
		}
		else{
			fprintf(stderr, "0");
		}
	}
	if(print_end){
		fprintf(stderr, "\n");
	}
	return;
}

void print_int_bit_with_len(const int print_key, int print_len){
	for(int i = print_len-1; i >= 0; i--){
		int tmp_key = 1 << i;
		if(tmp_key&print_key){
			fprintf(stderr, "1");
		}
		else{
			fprintf(stderr, "0");
		}
	}
	fprintf(stderr, "\n");
	return;
}


void KEY_DICT::init_key(void){
	for(int i=1; i<= (PIECE_NUM*2) ; i++){
		for( int j=0; j< MAX_BOARD_NUM; j++ ){
			this->init_a_key(this->key_pos[i][j]);
		}
	}
	for(int i=1; i<= PIECE_NUM ; i++){ // dice for 1 to 6
		this->init_a_key(this->key_dice[i]);
	}
	return;
}
void KEY_DICT::init_a_key( unsigned long long int dst[]){
	for(int i=0; i< MAX_KEY_NUM; i++){
		unsigned long long int tmp = ( (unsigned long long int) arc4random_uniform(COSTANT_UNSIGNED_MAX) ) << 32 ;

		
		dst[i] = ( (unsigned long long int) arc4random_uniform(COSTANT_UNSIGNED_MAX) ) | tmp;
	}
	return;
}
void KEY_DICT::debug_func(void){
	fprintf(stderr, "key for pos is:\n\n");
	for(int i=1; i<= (PIECE_NUM*2) ; i++){
		for( int j=0; j< MAX_BOARD_NUM; j++ ){
			fprintf(stderr,"piece %3d pos %3d  ", i,j);
			print_ulli_bit(this->key_pos[i][j][1], 0);
			print_ulli_bit(this->key_pos[i][j][0], 1);

		}
	}
	fprintf(stderr, "\n\nkey for dice is:\n\n");
	for(int i=1; i<= PIECE_NUM ; i++){ // dice for 1 to 6
		print_ulli_bit(this->key_dice[i][1], 0);
		print_ulli_bit(this->key_dice[i][0], 1);
	}

	return;
}

void KEY_DICT::gen_hash_key(const TMP_board& tmp_game, const int& now_dice ,unsigned long long int dst[]){
	for(int j =0; j<MAX_KEY_NUM; j++){
		dst[j] = this->key_dice[ now_dice ][j] ;
	}

	for(int i =1; i<= (PIECE_NUM*2) ; i++){
		if(tmp_game.piece_pos[i] >= 0){
			for(int j =0; j< MAX_KEY_NUM; j++){
				dst[j] ^= this->key_pos[i][ tmp_game.piece_pos[i] ][j] ;
			}
		}
	}

	// return [0] last HASH_INDEX_BIT 
	//unsigned long long int hash_index = ( dst[0] << (64 - HASH_INDEX_BIT )) >> (64 - HASH_INDEX_BIT );

	//return ((int) hash_index);
	return;
}

// I don't add undo, so remember memcpy to new array first
void KEY_DICT::move_hash_key(const TMP_board& tmp_game, const int& move_piece, const int& start_pos, const int& end_pos, const int& prev_dice, const int& now_dice ,unsigned long long int dst[]){
	
	//fprintf(stderr, "\ndebug: %d %d %d %d %d\n", move_piece, start_pos, end_pos, prev_dice, now_dice);
	int eaten_piece = tmp_game.board_state[ end_pos ];
	//fprintf(stderr, "\neaten piece: %d \n", eaten_piece);

	for(int j =0; j<MAX_KEY_NUM; j++){
		// dice
		dst[j] ^= this->key_dice[prev_dice][j];
		dst[j] ^= this->key_dice[now_dice][j];

		// pos
		dst[j] ^= this->key_pos[move_piece][start_pos][j];
		dst[j] ^= this->key_pos[move_piece][end_pos][j];
		
		if( eaten_piece > 0){
			dst[j]^= this->key_pos[ eaten_piece ][ end_pos ][j];
		}

	}

	// return [0] last HASH_INDEX_BIT 
	// unsigned long long int hash_index = ( dst[0] << (64 - HASH_INDEX_BIT )) >> (64 - HASH_INDEX_BIT );

	// return ((int) hash_index);
	return;
}

void KEY_DICT::move_hash_key_only_pos(const TMP_board& tmp_game, const int& move_piece, const int& start_pos, const int& end_pos ,unsigned long long int dst[]){
	
	//fprintf(stderr, "\ndebug: %d %d %d %d %d\n", move_piece, start_pos, end_pos, prev_dice, now_dice);
	int eaten_piece = tmp_game.board_state[ end_pos ];
	//fprintf(stderr, "\neaten piece: %d \n", eaten_piece);

	for(int j =0; j<MAX_KEY_NUM; j++){

		// pos
		dst[j] ^= this->key_pos[move_piece][start_pos][j];
		dst[j] ^= this->key_pos[move_piece][end_pos][j];
		
		if( eaten_piece > 0){
			dst[j]^= this->key_pos[ eaten_piece ][ end_pos ][j];
		}

	}

	return;
}

void KEY_DICT::move_hash_key_only_dice( const int& now_dice , unsigned long long int dst[]){

	for(int j =0; j<MAX_KEY_NUM; j++){
		// dice
		dst[j] ^= this->key_dice[now_dice][j];
	}

	// return [0] last HASH_INDEX_BIT 
	// unsigned long long int hash_index = ( dst[0] << (64 - HASH_INDEX_BIT )) >> (64 - HASH_INDEX_BIT );

	//return ((int) hash_index);
	return;
}

void KEY_DICT::unmove_hash_key_only_dice( const int& prev_dice , unsigned long long int dst[]){

	for(int j =0; j<MAX_KEY_NUM; j++){
		// dice
		dst[j] ^= this->key_dice[prev_dice][j];
	}

	return;
}
int KEY_DICT::return_hash_index(const unsigned long long int dst[]){
	// return [0] last HASH_INDEX_BIT 
	unsigned long long int hash_index = ( dst[0] << (64 - HASH_INDEX_BIT )) >> (64 - HASH_INDEX_BIT );

	return ((int) hash_index);

}



void TMP_board::set_board(const MyAI& game){
	memset(this->piece_pos ,-1, sizeof(this->piece_pos));

	for(int i=0; i< MAX_BOARD_NUM; i++){
		int col = i % BOARD_SIZE; 
		int row = i / BOARD_SIZE;

		this->board_state[i] = game.board[row][col];
		if( this->board_state[i] > 0){
			this->piece_pos[ this->board_state[i]  ] = i; //reverse map
		}
	}


	this->next = game.color;
	this->this_turn_role = game.color;

	this->num_cubes[ RED ] = game.red_piece_num;
	this->num_cubes[ BLUE ] = game.blue_piece_num;


	memset(this->alive, 0, sizeof(this->alive));

	//blue
	for(int i=1; i<= PIECE_NUM; i++){
		if(this->piece_pos[i]>=0){
			this->alive[BLUE] |= ( 1 << (i-1));
		}
	}
	//red
	for(int i= PIECE_NUM+1; i<= PIECE_NUM *2 ; i++){
		if(this->piece_pos[i]>=0){
			this->alive[ RED ] |= ( 1 << (i- (PIECE_NUM+1)));
		}
	}

	this->record_index = -1;

	return;

}

// 0-3 piece, 4-8 start pose, 9-12 eaten piece
void TMP_board::do_move(const int piece, const int start_point, const int end_point){
	int saved_move = 0;

	saved_move |= piece;
	saved_move |= (start_point << 4);

	this->board_state[start_point] = 0;

	if( this->board_state[end_point] > 0){
		if( is_red_cube_fast(this->board_state[end_point]) ){
			this->alive[RED] ^= ( 1 << (this->board_state[end_point] - 7 ) ); // 7 is piece num +1
			this->num_cubes[RED]--;
			
		}
		else{
			this->alive[BLUE] ^= ( 1 << (this->board_state[end_point] - 1 ) );
			this->num_cubes[BLUE]--;
		}

		saved_move |= (this->board_state[end_point] << 9);
		this->piece_pos[ this->board_state[end_point] ] = -1;

	}

	this->board_state[end_point] = piece;
	this->piece_pos[ piece ] = end_point;

	this->record_ply[ ++this->record_index ] = saved_move;

	change_player(this->next);

	return;
}

void TMP_board::undo_move(void){
	//piece = saved_move & 0xf;
	//start_point = (saved_move & 0xff) >> 4;
	//eaten_piece = saved_move >> 8;

	int saved_move = this->record_ply[this->record_index--];
	int piece = saved_move & 0xf;

	int end_point = this->piece_pos[piece];
	int start_point = (saved_move & 0x1ff) >> 4;


	int eaten_piece = saved_move >> 9;

	if( eaten_piece > 0 ){
		if( is_red_cube_fast(eaten_piece) ){
			this->alive[RED] |= 1 << (eaten_piece - 7 );
			this->num_cubes[RED]++;

		}
		else{
			this->alive[BLUE] |= 1 << (eaten_piece - 1 );
			this->num_cubes[BLUE]++;

		}
		this-> board_state[end_point] = eaten_piece;
		this-> piece_pos[eaten_piece] = end_point;

	}
	else{
		this-> board_state[end_point] = 0;
	}


	this-> board_state[ start_point ] = piece;
	this-> piece_pos[ piece ] = start_point;

	change_player(this->next);

	return;
}

/*
// don't call a dead piece
int TMP_board::get_distance(int piece){
	int col = (this->piece_pos[piece] % BOARD_SIZE);
	int row = (this->piece_pos[piece] / BOARD_SIZE);

	if(is_red_cube_fast(piece)){
		return MY_MAX( BOARD_SIZE - 1 -  col , BOARD_SIZE - 1 -  row );
	}
	else{
		return MY_MAX( row, col);
	}

}
*/

void TMP_board::print_status(void){
	fprintf(stderr,"\n\n==========================\n\n");
	fprintf(stderr, "red: 7-12  blue: 1-6  \n");
	fprintf(stderr, "now player is: %d (0 for red 1 for blue) \nboard is", this->next);
	for(int i=0; i<BOARD_SIZE; i++){
		fprintf(stderr, "\n");
		for(int j=0; j<BOARD_SIZE; j++){
			fprintf(stderr, "%4d", this->board_state[ i* BOARD_SIZE + j]);

		}
	}
	fprintf(stderr, "\nrecord piece pos is:");
	fprintf(stderr, "\nblue:  ");
	for(int i=1; i <= PIECE_NUM; i++){
		fprintf(stderr, "%4d", this->piece_pos[i]);
	}
	fprintf(stderr, "\nred:  ");
	for(int i=7; i <= 2*PIECE_NUM; i++){
		fprintf(stderr, "%4d", this->piece_pos[i]);
	}
	fprintf(stderr,"\nred alive %d ,and bit string: \n", this->num_cubes[RED]);
	print_int_bit_with_len(this->alive[RED] , 6);
	fprintf(stderr,"blue alive %d ,and bit string: \n", this->num_cubes[BLUE]);
	print_int_bit_with_len(this->alive[BLUE] , 6);
	fprintf(stderr,"\n");


	return;
}

void TMP_board::init_border(void){
	// init border red
	for(int i =0; i< BOARD_SIZE; i++){ //row
		for(int j=0; j<BOARD_SIZE; j++){ //col
			int use_index = i * BOARD_SIZE + j;

			if(i<(BOARD_SIZE-1)){
				if(j<(BOARD_SIZE-1)){
					this->bolder[RED][use_index][0] = use_index + 6; // down,right 
					this->bolder[RED][use_index][1] = use_index + 1; // right
					this->bolder[RED][use_index][2] = use_index + 5; // down
					this->bolder_count[RED][use_index] = 3;
				}
				else{
					this->bolder[RED][use_index][0] = use_index + 5; // down
					this->bolder_count[RED][use_index] = 1;
				}
			}
			else{
				if(j<(BOARD_SIZE-1)){
					this->bolder[RED][use_index][0] = use_index + 1; // right
					this->bolder_count[RED][use_index] = 1;
				}
				else{
					this->bolder_count[RED][use_index] = 0;
				}
			}
		}
	}
	// init border blue
	for(int i =0; i< BOARD_SIZE; i++){ //row
		for(int j=0; j<BOARD_SIZE; j++){ //col
			int use_index = i * BOARD_SIZE + j;

			if(i>0){
				if(j>0){
					this->bolder[BLUE][use_index][0] = use_index - 6; // up, left 
					this->bolder[BLUE][use_index][1] = use_index - 1; // left
					this->bolder[BLUE][use_index][2] = use_index - 5; // up
					this->bolder_count[BLUE][use_index] = 3;
				}
				else{
					this->bolder[BLUE][use_index][0] = use_index - 5; // up
					this->bolder_count[BLUE][use_index] = 1;
				}
			}
			else{
				if(j>0){
					this->bolder[BLUE][use_index][0] = use_index - 1; // left
					this->bolder_count[BLUE][use_index] = 1;
				}
				else{
					this->bolder_count[BLUE][use_index] = 0;
				}
			}
		}
	}
	//debug
	/*
	
	for(int i =0; i< BOARD_SIZE; i++){
		for(int j=0; j<BOARD_SIZE; j++){ 
			fprintf(stderr, "%4d", this->bolder_count[BLUE][i*5+j]);
		}
		fprintf(stderr, "\n");
	}
	
	for(int i =0; i<MAX_BOARD_NUM; i++){

		for(int j=0; j<this->bolder_count[BLUE][i]; j++){
			fprintf(stderr, "%4d", this->bolder[BLUE][i][j] );
		}

		fprintf(stderr,"\n");

	}*/

	//init movable piece

	memset(this->moveable_piece_determinacy, 0, sizeof(this->moveable_piece_determinacy));

	for(int now_dice =1; now_dice <= PIECE_NUM ; now_dice ++ ){
		for(int tmp_state = 0; tmp_state < (2 << PIECE_NUM); tmp_state ++){
			int tmp_count = 0;

			if( ( (1 << (now_dice - 1) ) & tmp_state ) ){
				this->moveable_piece[ tmp_state ][ now_dice ][ tmp_count++  ] = now_dice;

				this->moveable_piece_determinacy[ tmp_state ][ now_dice ]++;

			}
			else{
				for( int tmp_dice = now_dice -1 ; tmp_dice >=1; tmp_dice--){
					if( ( (1 << (tmp_dice - 1) ) & tmp_state)  ){
						this->moveable_piece[ tmp_state ][ now_dice ][ tmp_count++ ] = tmp_dice;

						this->moveable_piece_determinacy[ tmp_state ][ tmp_dice ]++;
						break;
					}
				}
				for( int tmp_dice = now_dice +1 ; tmp_dice <= PIECE_NUM ; tmp_dice++){
					if( ( (1 << (tmp_dice - 1) ) & tmp_state ) ){

						this->moveable_piece[ tmp_state ][ now_dice ][ tmp_count++ ] = tmp_dice;

						this->moveable_piece_determinacy[ tmp_state ][ tmp_dice ]++;
						break;
					}
				}

			}

			this->moveable_piece_count[ tmp_state ][ now_dice ] = tmp_count;
		}
	}


	//debug 
	/*
	int alive_1 = 34;

	print_int_bit_with_len( alive_1 , 6);

	for(int i =1; i<=6; i++){
		fprintf(stderr,"dice: %d count: %d, ", i, this->moveable_piece_count[alive_1][i]);
		for(int j =0; j< this->moveable_piece_count[alive_1][i] ; j++){
			fprintf(stderr,"%4d", this->moveable_piece[alive_1][i][j]);	
		}
		fprintf(stderr,"\n");	
	}
	fprintf(stderr, "determinacy: ");
	for(int i =1; i<=6; i++){
		fprintf(stderr, "%4d", this->moveable_piece_determinacy[alive_1][i]);

	}
	fprintf(stderr,"\n");
	*/

	return;

}
void TMP_board::init_score(void){
	// now best 20 10 5 0
	// int pos_map[BOARD_SIZE] = { MAX_SCORE, 20, 10, 5, 0 }; // 0-4
	// now best: 9 10 7 5 3 1
	int alive_count_map[PIECE_NUM+2] = { -MAX_SCORE, 9, 9, 7, 5, 3, 1}; // 0-6
	// now best: 0 1 3 5 8 10 10 10
	int determinacy_map[PIECE_NUM+2] = { 0, 1, 3, 5, 8, 9, 9, 9}; // 0-6 , 7 for not overflow?
	/*
	int dist;
	for(int i=0; i<BOARD_SIZE; i++){
		for(int j=0; j<BOARD_SIZE; j++){
			int now_index = i*BOARD_SIZE+j;
			//red 

			dist = MY_MAX( BOARD_SIZE - 1 -  i , BOARD_SIZE - 1 -  j );
			this->position_score[RED][now_index] = pos_map[ dist ];

			//blue

			dist = MY_MAX( i , j);
			this->position_score[BLUE][now_index] = pos_map[ dist ];


		}
	}
	// clear bad pos
	this->position_score[RED][ (BOARD_SIZE) * (BOARD_SIZE-1) ]= -5;
	this->position_score[RED][ (BOARD_SIZE) * ((BOARD_SIZE-2))]= -1;
	this->position_score[RED][ (BOARD_SIZE-1) ]= -5;
	this->position_score[RED][ (BOARD_SIZE-2) ]= -1;

	this->position_score[BLUE][ (BOARD_SIZE) - 1 ] = -5;
	this->position_score[BLUE][ 2*(BOARD_SIZE) - 1 ] = -1;
	this->position_score[BLUE][ (BOARD_SIZE) * (BOARD_SIZE-1) + 1 ] = -5;
	this->position_score[BLUE][ (BOARD_SIZE) * (BOARD_SIZE-1) + 2 ] = -1;
	*/


	int new_pos_map[ MAX_BOARD_NUM ] = {
		1,  1,  0,  0,  -1,
		1,  5,  5,  5,  5,
		0,  5,  10, 10, 10,
	    0,  5,  10, 16, 16,
	   -1,  5,  10, 16, MAX_SCORE,
	};
	for(int i=0; i<MAX_BOARD_NUM; i++){
		this->position_score[RED][i] = new_pos_map[i];
		this->position_score[BLUE][i] = new_pos_map[ ( MAX_BOARD_NUM-1) - i];
	}






	memcpy(this->alive_count_score, alive_count_map, sizeof(this->alive_count_score));
	memcpy(this->alive_type_score, determinacy_map, sizeof(this->alive_type_score));

	return;

}




int TMP_board::get_move_all( const int& now_dice , int result[]){
	int total_count = 0;
	int three_times_index = 0;
	int tmp_piece, tmp_pos;

	int active_alive = this->alive[ this->next ];

	for(int i=0; i< this->moveable_piece_count[ active_alive ][ now_dice ]; i++){
		if( this->next == RED ){
			tmp_piece = this->moveable_piece[ active_alive ][ now_dice ][ i ] + 6;
		}
		else{
			tmp_piece = this->moveable_piece[ active_alive ][ now_dice ][ i ];
		}
		tmp_pos = this->piece_pos[ tmp_piece ];
		for(int j =0; j< bolder_count[ this->next ][tmp_pos]; j++){
			//new_pos = bolder[this->next][tmp_pos][j]
			result[three_times_index++] = tmp_piece;
			result[three_times_index++] = tmp_pos;
			result[three_times_index++] = this->bolder[this->next][tmp_pos][j];
			total_count++;
		}

	}

	return total_count;
}

int TMP_board::little_heurtistic( const int& piece_bias,const int& cacu_color, int part1_array[] ){
	int total_score = 0;
	int part1_score , part2_score, determinacy_score, total_determinacy, pos_score;

	part1_score = 0;
	total_determinacy = 0;

	int tmp_alive_count = (this->num_cubes[ cacu_color ] > 0 )? this->num_cubes[ cacu_color ]: 1;

	// determicy * distance 
	for(int i = piece_bias ; i <= piece_bias + 5 ; i++){
		if((1 <<(i- piece_bias ) ) & this->alive[ cacu_color ]){
			pos_score = this->position_score[ cacu_color ][ this->piece_pos[i] ];
			determinacy_score = this->moveable_piece_determinacy[ this->alive[ cacu_color ] ][ i+1 - piece_bias ];
			total_determinacy += determinacy_score;

			// part1_array[i] = pos_score * determinacy_score ;
			// part1_array[i] = ( PART1_SCALE * pos_score * determinacy_score ) / ( tmp_alive_count );
			part1_array[i] += ( PART1_SCALE * pos_score * determinacy_score ) ;
			part1_score += part1_array[i] ;
		}
		
		
		else{
			// part1_score += DEAD_SCORE;
			part1_score += PART1_SCALE * DEAD_SCORE;

		}
	}

	


	// alive piece score, ex: 3 2 1 1 2 3 for alive piece and 4 10 7 4 3 1 alive count 
	part2_score = this->alive_count_score [ this->num_cubes[ cacu_color ] ];
	
	int flow_bit = total_determinacy % tmp_alive_count;
	if(flow_bit!=0){
		part2_score += this->alive_type_score [ (total_determinacy / tmp_alive_count)+1 ];
	}
	else{
		part2_score += this->alive_type_score [ total_determinacy / tmp_alive_count];
	}
	

	// PART1_SCALE move to above func 
	total_score =  part1_score + PART2_SCALE * part2_score;

	// total_score =  PART2_SCALE * part1_score + PART2_SCALE * part2_score;

	return total_score;
}



int TMP_board::heuristic(){
	int total_score, record_score[2];
	int part1_array[ DOUBLE_PIECE_NUM ] = {0}; 
	// red
	record_score[RED] = this->little_heurtistic(7, RED, part1_array);

	// blue
	record_score[BLUE] = this->little_heurtistic(1, BLUE, part1_array);

	//fprintf(stderr, "red %d blue %d\n", record_score[RED], record_score[BLUE]);
	// if the next player can eat piece, get additional determicy * eaten piece score 
	int next_player_score = 0;
	int at_least_score, enemy_leave;

	int sum_array[ DOUBLE_PIECE_NUM ][PIECE_NUM+2];
	memset(sum_array, 0, sizeof(sum_array));


	if(this->next == RED){
		/*
		for(int i=7; i<= 2*PIECE_NUM; i++){
			int base_pos = this->piece_pos[i];
			if( base_pos >= 0 ){
				for(int j=0; j< this->bolder_count[ RED ][ base_pos ]; j++){
					int tmp_pos = this->bolder[ RED ][ base_pos ][j];
					int tmp_piece = this->board_state[ tmp_pos ] ;
					if( (tmp_piece > 0) && (is_blue_cube_fast( tmp_piece ) ) ){
						int tmp_score = part1_array[ tmp_piece ];
						int now_deter = this -> moveable_piece_determinacy [this->alive[ RED ]][ i - 6];
						tmp_score = tmp_score * now_deter * PART1_SCALE / 6;

						
						next_player_score = ( tmp_score > next_player_score ) ? tmp_score : next_player_score;
					}

				}


			}

		}*/

		for(int i=1; i<= PIECE_NUM; i++){ // i is dice


			for(int j=0; j< this->moveable_piece_count[ this-> alive[RED] ][i]; j++){ 
				int base_piece = this -> moveable_piece[ this-> alive[RED] ][i][j] + 6;
				int base_pos = this->piece_pos[ base_piece ];
				for(int k=0; k< this->bolder_count[ RED ][ base_pos ]; k++){
					int tmp_pos = this->bolder[ RED ][ base_pos ][k];


					int tmp_piece = this->board_state[ tmp_pos ] ;
					if( (tmp_piece > 0) && (is_blue_cube_fast( tmp_piece ) ) ){
						sum_array[ tmp_piece ][i] = 1;
					}

				}
			}

		}

		for(int i=1; i<= PIECE_NUM; i++){
			int eaten_count = 0;
			for(int j= 1; j<=PIECE_NUM; j++){
				if( sum_array[i][j] ){
					eaten_count++;
				}
			}

			//int tmp_score = PART1_SCALE * eaten_count * part1_array[ i ] / EATEN_SCALE ;

			int tmp_score = eaten_count * part1_array[ i ] / EATEN_SCALE ;

			next_player_score = ( tmp_score > next_player_score ) ? tmp_score : next_player_score;

		}

		enemy_leave = (this->num_cubes[ BLUE ] >= 1) ? this->num_cubes[ BLUE ] : 1;
		at_least_score = PART2_SCALE * (this-> alive_count_score [ enemy_leave - 1 ] - this->alive_count_score[ enemy_leave ] ) + PART1_SCALE * DEAD_SCORE;




		if( next_player_score > at_least_score ){
			record_score[RED] += next_player_score ;
		}
		
	}
	else{
		/*
		for(int i=1; i<= PIECE_NUM; i++){
			int base_pos = this->piece_pos[i];
			if( base_pos >= 0 ){
				for(int j=0; j< this->bolder_count[ BLUE ][ base_pos ]; j++){
					int tmp_pos = this->bolder[ BLUE  ][ base_pos ][j];
					int tmp_piece = this->board_state[ tmp_pos ] ;
					if( (tmp_piece > 0) && (is_red_cube_fast( tmp_piece ) ) ){

						int tmp_score = part1_array[ tmp_piece ];
						int now_deter = this -> moveable_piece_determinacy [this->alive[ BLUE ]][ i ];
						tmp_score = tmp_score * now_deter * PART1_SCALE / 6;

						
						next_player_score = ( tmp_score > next_player_score ) ? tmp_score : next_player_score;
					}

				}


			}

		}*/


		
		for(int i=1; i<= PIECE_NUM; i++){


			for(int j=0; j< this->moveable_piece_count[ this-> alive[ BLUE ] ][i]; j++){
				int base_piece = this -> moveable_piece[ this-> alive[BLUE] ][i][j];
				int base_pos = this->piece_pos[ base_piece ];
				for(int k=0; k< this->bolder_count[ BLUE ][ base_pos ]; k++){
					int tmp_pos = this->bolder[ BLUE ][ base_pos ][k];


					int tmp_piece = this->board_state[ tmp_pos ] ;
					if( (tmp_piece > 0) && (is_red_cube_fast( tmp_piece ) ) ){
						sum_array[ tmp_piece ][i] = 1;
					}
				}
			}


		}



		for(int i=7; i<= (2*PIECE_NUM); i++){
			int eaten_count = 0;
			for(int j= 1; j<=PIECE_NUM; j++){
				if( sum_array[i][j] ){
					eaten_count++;
				}
			}

			
			// int tmp_score = PART1_SCALE * eaten_count * part1_array[ i ] / EATEN_SCALE ;
			int tmp_score = eaten_count * part1_array[ i ] / EATEN_SCALE;

			next_player_score = ( tmp_score > next_player_score ) ? tmp_score : next_player_score;

		}

		enemy_leave = (this->num_cubes[ RED ] >= 1) ? this->num_cubes[ RED ] : 1;
		at_least_score = PART2_SCALE * (this-> alive_count_score [ enemy_leave - 1 ] - this->alive_count_score[ enemy_leave] );

		if( next_player_score > at_least_score ){
			record_score[BLUE] += next_player_score;
		}

	}

	//fprintf(stderr, "add next play advantage, red %d blue %d\n", record_score[RED], record_score[BLUE]);
	

	total_score = record_score[ (this->next) ] - record_score[ ( 1 ^ this->next ) ]; 



	// clamp to between MAX_SCORE and -MAX_SCORE
	total_score = (total_score < MAX_SCORE) ? total_score: MAX_SCORE;
	total_score = (total_score > -MAX_SCORE) ? total_score : -MAX_SCORE;

	return total_score;

};

// RED: -1 , BLUE: 1
int TMP_board::is_end(void){
	if( (this->board_state[ 0 ]> 0) && ( is_blue_cube_fast( this->board_state[ 0 ] ) ) ){
		return 1;
	}
	else if( (this->board_state[ 24 ]> 0) && ( is_red_cube_fast( this->board_state[ 24 ] ) ) ){
		return -1;
	}
	else if( this->num_cubes[RED] == 0){
		return 1;
	}
	else if( this->num_cubes[BLUE] == 0){
		return -1;
	}

	return 0; // not end
}






/*
int main(){
	Key_manager.init_key();
	unsigned long long int game_key[MAX_KEY_NUM];
	unsigned long long int game_key2[MAX_KEY_NUM];
	int tmp_hash;

	//Key_manager.debug_func();

	MyAI myai;
	TMP_board tmp_game;

	myai.color = BLUE;
	myai.dice = 5;
	char setstring[25] = "E3E5D4D5E4C5A3B20B1A2C1";
	myai.board_size = 5;
	myai.Set_board(setstring);

	tmp_game.set_board(myai);
	
	
	//tmp_game.print_status();
	//tmp_game.do_move(3, 18, 12);
	//tmp_game.print_status();

	
	//tmp_game.do_move(8, 6, 12);
	//tmp_game.print_status();


	//tmp_game.undo_move();
	//tmp_game.print_status();

	//tmp_game.undo_move();
	//tmp_game.print_status();
	
	

	tmp_game.init_border();
	tmp_game.init_score();
	Dice_manager.init();

	
	//int used_array[102];
	//int prev_dice = 2;
	//Key_manager.gen_hash_key(tmp_game, prev_dice, game_key2);

	//for(int i =0; i<10; i++){
	//	int tmp_dice = arc4random_uniform(6)+1;
	//	int ans = tmp_game.get_move_all( tmp_dice, used_array);


		

	//	fprintf(stderr, "\n\nmove count %d\n", ans);
	//	for(int i=0; i<ans; i++){
	//		fprintf(stderr, "%4d %4d %4d\n", used_array[3*i], used_array[3*i+1], used_array[3*i+2]);
	//	}
	//	fprintf(stderr,"\n");


	//	int next_index =  arc4random_uniform(ans) ;
	//	fprintf(stderr, "do %dth move\n", next_index);

	//	Key_manager.move_hash_key(tmp_game, used_array[ next_index *3] , used_array[ next_index *3+1], used_array[ next_index *3+2], prev_dice, tmp_dice, game_key2);
	//	prev_dice = tmp_dice;
	//	print_ulli_bit(game_key2[1], 0);
	//	print_ulli_bit(game_key2[0], 1);

	//	tmp_game.do_move(used_array[ next_index *3] ,used_array[ next_index *3 + 1], used_array[ next_index *3 +2]);

		
		//Key_manager.gen_hash_key(tmp_game, tmp_dice, game_key);
		//print_ulli_bit(game_key[1], 0);
		//print_ulli_bit(game_key[0], 1);
		
		//fprintf(stderr, "the difference between two method are \n");
		//print_ulli_bit( game_key[1] ^ game_key2[1], 0);
		//print_ulli_bit( game_key[0] ^ game_key2[0], 1);
		

	//	tmp_game.print_status();
	//	tmp_game.heuristic();
	//}
	
	for(int i =0; i<30; i++){
		int tmp_dice = arc4random_uniform(6)+1;
		fprintf(stderr, "dice is %d\n", tmp_dice);

		int best_move[3];
		Transposition_table[RED].init();
		Transposition_table[BLUE].init();
		NegaScout_return_move(tmp_game, tmp_dice, 7, best_move);

		tmp_game.do_move(best_move[0], best_move[1], best_move[2]);

		tmp_game.print_status();

		if(tmp_game.is_end()){
			break;
		}

	}
	

	return 0;
}
*/


void TIME_RECORD::init(int total_time){

	this->remain_time = (double) total_time;

	this->now_turn = 1;
	this->estimate_max_turn = 10;

	this->step_time = (double) 4;
	this->addition_time = (double) 11;

	this->now_depth = 6;
	this->prev_status = 0;

	return;
}


int TIME_RECORD::set_depth(void){
	this->start_time = std::clock();
	this->limit_time = ( (clock_t) ( ( this->step_time + this->addition_time ) * CLOCKS_PER_SEC ) ) + this->start_time;
	
	return this->now_depth;

}
int TIME_RECORD::is_timeout(void){
	clock_t tmp_t= std::clock(); 

	if(tmp_t > this->limit_time){
		return 1;
	}
	return 0;
}



void TIME_RECORD::turn_over(void){
	clock_t tmp_time = std::clock();
	double use_sec = ( ( (double) ( tmp_time - this->start_time ) ) / CLOCKS_PER_SEC );

	this->remain_time -= use_sec;

	fprintf(stderr, "remain: %lf , use: %lf sec\n", this->remain_time, use_sec);


	// record state
	if( use_sec > (this->step_time + 5.0 ) ){
		this->prev_status = 1;
	}
	else if( use_sec < (this->step_time - 0.5 )){
		this->prev_status = -1;
	}else{
		this->prev_status = 0;
	}

	// get addition 3 step if max_turn > this_turn
	int remain_turn = ( (this->estimate_max_turn - this->now_turn ) > 0) ? (this->estimate_max_turn - this->now_turn ) : 3; 
	this->step_time = this->remain_time / (remain_turn);

	double available_sec = this->remain_time - this->step_time - 3.0 ;
	if( available_sec < this->addition_time ){
		this->addition_time = ( available_sec > 0 ) ?  available_sec : 0;
	}

	fprintf(stderr, "next turn available time is %lf + %lf\n", this->step_time, this->addition_time);

	// modified search depth after 3rd turn
	if( this->now_turn >= 1){
		if( this->prev_status == 1){ // exceed time, depth--
			if( (this->now_depth > 6 ) && (this->remain_time < 30.0)){
				this->now_depth--;
			}
		}else if( (this->prev_status == -1) || ( this->now_turn > 10 ) ){ // use less time, depth ++
			if(this->now_depth < 7){

				this->now_depth++;
			}
		}
	}

	this->now_turn++;
	return;
}





void HASH_TABLE::init(void){
	memset(this->occupy, 0, sizeof(this->occupy));

	return;
}


int HASH_TABLE::match(int index, int search_depth , const unsigned long long int tmp_key[]){
	if(!this->occupy[index]){
		return 0; // no entry
	}
	for(int i=0; i< MAX_KEY_NUM; i++){
		if( (this->entry[index].saved_key[i] ^ tmp_key[i]) ){
			//fprintf(stderr, "collision!!\n");
			return 0; // collision
		}
	}
	if( search_depth > this->entry[index].depth ){
		return -1;
	}
	return 1;

}

void HASH_TABLE::register_entry(int index, const int search_depth, const unsigned long long int tmp_key[], const int in_move[], const int in_status, const int in_score ){
	this->occupy[index] = 1; // set avaible

	this->entry[index].depth = search_depth;
	memcpy( this->entry[index].saved_key, tmp_key, sizeof(this->entry[index].saved_key) );
	memcpy( this->entry[index].move_direction, in_move, sizeof(this->entry[index].move_direction) );
	this->entry[index].status = in_status;
	this->entry[index].score = in_score;

	return;

}

void HASH_TABLE::get_status(int index, int out_move[], int& out_status, int& out_score){
	memcpy( out_move , this->entry[index].move_direction,  3 * sizeof(int) ); // 3 int entry
	out_status = this->entry[index].status;
	out_score = this->entry[index].score;

	return;
}

void DICE_SEQ::init(void){
	for(int i=0; i<PIECE_NUM; i++){
		this->table[0][i] = i+1; // origin 1- 6
	}
	int reverse_chain[PIECE_NUM+2][PIECE_NUM];
	int reverse_chain_count[PIECE_NUM+2];
	int wait_count[PIECE_NUM+2];
	int table_index;

	for(int state =1; state < ( 1 << (PIECE_NUM)); state++){
		memset(wait_count, 0, sizeof(wait_count));
		memset(reverse_chain, 0, sizeof(reverse_chain));
		memset(reverse_chain_count, 0, sizeof(reverse_chain_count));

		int little_index = -1;
		for(int i=1; i<= PIECE_NUM ; i++){
			if( ( 1 << (i-1) ) & state ){
				little_index = i;
			}
			else{ // not alive
				if(little_index>0){
					reverse_chain[ little_index ][ reverse_chain_count[little_index]++ ] = i;
					wait_count[i]++;
				}
			}
		}
		int large_index = 7;
		for(int i=PIECE_NUM; i>=1 ; i--){
			if( ( 1 << (i-1) ) & state ){
				large_index = i;
			}
			else{ // not alive
				if( large_index <= PIECE_NUM ){
					reverse_chain[ large_index ][ reverse_chain_count[ large_index ]++ ] = i;
					wait_count[i]++;
				}
			}
		}



		table_index = 0;
		for(int i=1; i<=PIECE_NUM; i++){
			if(wait_count[i]==0){
				this->table[state][ table_index++] = i;

				for(int j = 0; j< reverse_chain_count[i]; j++){
					int wait_index = reverse_chain[i][j];
					wait_count[ wait_index ]--;
					if( wait_count[ wait_index ] == 0 ){
						this->table[state][ table_index++] = wait_index;

						wait_count[ wait_index ] = -1; // set to -1 or will write again
					}


				}
			}
		}

	}

	//debug

	//for(int state =1; state < ( 1 << (PIECE_NUM)); state++){
	//	for(int j=0; j<PIECE_NUM; j++){
	//		fprintf(stderr, "%3d", this->table[state][j]);
	//
	//	}
	//	fprintf(stderr,"\n");
	//}


	return;

}


// func here


int NegaScout(TMP_board& tmp_game, const int this_dice, unsigned long long int game_key[], int alpha, int beta, int now_depth){
	// game end here
	if(tmp_game.is_end()){
		return - MAX_SCORE; // because this player must lose.
	}

	// time out here

	if(Time_manager.is_timeout() && (now_depth % 2 ==0) ){
		return tmp_game.heuristic();
	}


	// depth = 0 here
	if( now_depth == 0){
		return tmp_game.heuristic();
	}

	///////////////////////////////// hash table here //////////////////////////////

	int best_status = 4; // no use 
	int best_score = -MAX_SCORE; 
	int best_move[3];

	int hash_index = Key_manager.return_hash_index(game_key);
	int response = Transposition_table[ tmp_game.next ].match(hash_index, now_depth, game_key);

	// create new hashed key
	unsigned long long int new_game_key[MAX_KEY_NUM];
	unsigned long long int new_game_key2[MAX_KEY_NUM];


	if(response > 0){
		
		Transposition_table[ tmp_game.next ].get_status( hash_index, best_move, best_status, best_score);

		if( best_status == EXACT_SCORE ){

			return best_score;
		}
		else if (best_status == FAIL_HIGH ){ // lower bound
			alpha = MY_MAX( alpha , best_score);

			if(alpha >= beta){
				return alpha;
			}

		}
		else if (best_status == FAIL_LOW ){ // upper bound
			beta = MY_MIN( beta ,  best_score);

			if(beta <= alpha){
				return beta;
			}
		}

	}

	// special situation, use upper and lower dice to decide move
	if( (response == 0 ) && (tmp_game.moveable_piece[  tmp_game.alive[tmp_game.next]  ][ this_dice ][0] != this_dice) ){
		// carefully check upper and lower dice is record before
		int can_use = 1;

		for( int i = 0; i< tmp_game.moveable_piece_count[  tmp_game.alive[tmp_game.next]  ][ this_dice ]; i++){

			memcpy(new_game_key, game_key, sizeof( new_game_key ));
			Key_manager.unmove_hash_key_only_dice( this_dice, new_game_key );
			Key_manager.move_hash_key_only_dice(  tmp_game.moveable_piece[  tmp_game.alive[tmp_game.next]  ][ this_dice ][i] , new_game_key );
			int new_hash_index = Key_manager.return_hash_index( new_game_key );


			if( Transposition_table[ tmp_game.next ].match( new_hash_index, now_depth, new_game_key) <= 0){

				can_use = 0;
				break;
			}
			else{
				int tmp_best_move[3];
				int tmp_best_status;
				int tmp_best_score;

				Transposition_table[ tmp_game.next ].get_status( new_hash_index, tmp_best_move, tmp_best_status, tmp_best_score);
				// && (tmp_best_score != FAIL_LOW)
				if((tmp_best_score >= best_score ) ){
					best_score = tmp_best_score;
					memcpy( best_move, tmp_best_move, sizeof(best_move) );
					best_status = tmp_best_status; 				
				}
			}
		}
		if( can_use ){
			//fprintf(stderr,"hit\n");
			
			//same as above, except need set up table 

			if( best_status == EXACT_SCORE ){
				// setup transition table 
				Transposition_table[ tmp_game.next ].register_entry( hash_index, now_depth, game_key, best_move, best_status, best_score );

				return best_score;
			}
			
			else if (best_status == FAIL_HIGH ){ // lower bound
				alpha = MY_MAX( alpha , best_score);

				if(alpha >= beta){

					Transposition_table[ tmp_game.next ].register_entry( hash_index, now_depth, game_key, best_move, best_status, best_score );
					return alpha;
				}

			}
			else if (best_status == FAIL_LOW ){ // upper bound
				beta = MY_MIN( beta ,  best_score);

				if(beta <= alpha){

					Transposition_table[ tmp_game.next ].register_entry( hash_index, now_depth, game_key, best_move, best_status, best_score );
					return beta;
				}
			}

			

		}


	}


	////////////////////////////////////////////////  begin  ///////////////////////////////////////////

	int m = -MAX_SCORE;
	int n = beta;

	// get move 
	int move_array[ 20 ]; // max 6 *3 
	int move_count = tmp_game.get_move_all( this_dice, move_array );

	// save un-diced key
	memcpy(new_game_key2, game_key, sizeof( new_game_key2 ));
	Key_manager.unmove_hash_key_only_dice( this_dice, new_game_key2 );


	for(int i=0; i<move_count; i++){

		// move hash key
		memcpy(new_game_key, new_game_key2, sizeof( new_game_key ));
		Key_manager.move_hash_key_only_pos( tmp_game, move_array[i*3], move_array[i*3+1], move_array[i*3+2] , new_game_key);

		// move board
		tmp_game.do_move( move_array[i*3], move_array[i*3+1], move_array[i*3+2] );



		int t = -STAR_FINAL(tmp_game, new_game_key, -n, -MY_MAX(m, alpha), now_depth -1 );

		if( t > m){
			if( (n == beta) || (now_depth < 3) || (t>=beta) ){
				m = t;
				best_move[0] = move_array[i*3];
				best_move[1] = move_array[i*3+1];
				best_move[2] = move_array[i*3+2];

			}
			else{
				t = -STAR_FINAL(tmp_game, new_game_key, -beta, -t, now_depth -1);
				if(t > m){
					m = t;
					best_move[0] = move_array[i*3];
					best_move[1] = move_array[i*3+1];
					best_move[2] = move_array[i*3+2];

				}

			}

			

		}

		// reset here
		tmp_game.undo_move();

		if( (m == MAX_SCORE) || (m>=beta) ){

			if( m == MAX_SCORE){

				best_status = EXACT_SCORE;
			}
			else{

				best_status = FAIL_HIGH;
			}

			// record hash table
			Transposition_table[ tmp_game.next ].register_entry( hash_index, now_depth, game_key, best_move, best_status, m );


			return m;

		}

		n = MY_MAX( alpha , m ) + 1;

	}
	if(m > alpha){
		best_status = EXACT_SCORE;
	}
	else{
		best_status = FAIL_LOW;
	}

	// record hash table
	Transposition_table[ tmp_game.next ].register_entry( hash_index, now_depth, game_key, best_move, best_status, m );

	return m;

}

void smart_dice_seq(int dice_seq[], int alive_bit){
	// alive dice do first
	int alive_piece[PIECE_NUM +2];
	int dead_piece[PIECE_NUM +2];
	int alive_count = 0;
	int dead_count = 0;
	for(int i =1; i<=PIECE_NUM; i++ ){
		if( (1<<( i-1) ) & alive_bit ){
			alive_piece[alive_count++] = i;
		}
		else{
			dead_piece[dead_count++] = i;
		}
	}
	for(int i=0 ; i<alive_count; i++){
		dice_seq[i] = alive_piece[ i ];
	}
	for(int i=0; i<dead_count; i++){
		dice_seq[ i+alive_count ] = dead_piece[i];
	}
	/*
	fprintf(stderr, "\n");
	for(int i=0; i<PIECE_NUM; i++){
		fprintf(stderr, "%4d", dice_seq[i]);
	}
	fprintf(stderr, "\n");
	*/
	return;

}


// star 0
// need memcpy game_key[] before use, or will have bug
/*
int STAR_FINAL(TMP_board& tmp_game, unsigned long long int game_key[], int alpha, int beta, int now_depth){
	// int use_dice[PIECE_NUM+2];
	unsigned long long int new_game_key[MAX_KEY_NUM];

	// smart_dice_seq(use_dice, tmp_game.alive[tmp_game.next]);


	int total_score = 0;
	for(int i=0; i<PIECE_NUM; i++){
		int good_dice = Dice_manager.table[ tmp_game.alive[tmp_game.next] ][i];

		memcpy(new_game_key, game_key, sizeof(new_game_key));
		// Key_manager.move_hash_key_only_dice( use_dice[i], new_game_key);
		Key_manager.move_hash_key_only_dice( good_dice, new_game_key);

		// total_score += NegaScout(tmp_game, use_dice[i], new_game_key, - MAX_SCORE, MAX_SCORE, now_depth);
		total_score += NegaScout(tmp_game, good_dice, new_game_key, - MAX_SCORE, MAX_SCORE, now_depth);

	}

	return ( total_score / PIECE_NUM );
}*/

int STAR_FINAL(TMP_board& tmp_game, unsigned long long int game_key[], int alpha, int beta, int now_depth){

	unsigned long long int new_game_key[MAX_KEY_NUM];

	int A_0 = PIECE_NUM * ( alpha - MAX_SCORE ) + MAX_SCORE;
	int B_0 = PIECE_NUM * ( beta + MAX_SCORE ) - MAX_SCORE;

	int m_0 = - MAX_SCORE;
	int M_0 = MAX_SCORE;
	int total_score = 0;
	int t;

	for(int i=0; i<PIECE_NUM; i++){
		int good_dice = Dice_manager.table[ tmp_game.alive[tmp_game.next] ][i];

		memcpy(new_game_key, game_key, sizeof(new_game_key));
		Key_manager.move_hash_key_only_dice( good_dice, new_game_key);

		t = NegaScout(tmp_game, good_dice, new_game_key, MY_MAX( A_0, - MAX_SCORE )  , MY_MIN( B_0, MAX_SCORE ), now_depth);
		m_0 += ( ( t + MAX_SCORE ) / PIECE_NUM ) ;
		M_0 += ( ( t - MAX_SCORE ) / PIECE_NUM ) ;
		if(t >= B_0){
			//fprintf(stderr, "high: t %d B_0 %d m_0 %d \n", t, B_0, m_0);

			return m_0;
		}
		if(t <= A_0 ){
			//fprintf(stderr, "low\n");

			return M_0;
		}
		total_score += t;

		A_0 += (MAX_SCORE - t);
		B_0 += (-MAX_SCORE - t);


	}

	return total_score / PIECE_NUM;


}





void NegaScout_return_move(TMP_board& tmp_game, int this_dice ,int set_depth, int out_move[]){
	int set_alpha = - MAX_SCORE;
	int set_beta = MAX_SCORE;
	int move_array[20];
	int move_count = tmp_game.get_move_all(this_dice ,move_array);
	unsigned long long int game_key[MAX_KEY_NUM];
	unsigned long long int new_game_key[MAX_KEY_NUM];

	Key_manager.gen_hash_key(tmp_game, this_dice, game_key );


	int m = -MAX_SCORE;
	int n = set_beta;


	int best_index = 0;
	int best_score = - MAX_SCORE;

	for(int i =0; i<move_count; i++){


		// move hash key
		memcpy(new_game_key, game_key, sizeof( new_game_key ));
		Key_manager.move_hash_key_only_pos( tmp_game, move_array[i*3], move_array[i*3+1], move_array[i*3+2] , new_game_key);

		// move board
		tmp_game.do_move( move_array[i*3], move_array[i*3+1], move_array[i*3+2] );

		if(tmp_game.is_end()){ // must I win
			tmp_game.undo_move();
			best_score = MAX_SCORE;
			best_index = i;
			break;

		}


		int t = -STAR_FINAL(tmp_game, new_game_key, - n, - MY_MAX( set_alpha, m), set_depth -1 );

		fprintf(stderr, "scout move %d %d %d score: %d \n", move_array[i*3], move_array[i*3+1], move_array[i*3+2], t);

		if( t > m){
			if( (n==set_beta) || (t >= set_beta) || (set_depth<3) ){
				m = t;
				best_score = m;
				best_index = i;

			}
			else{
				t = -STAR_FINAL(tmp_game, new_game_key, - set_beta , -t , set_depth -1 );

				fprintf(stderr, "research move %d %d %d score: %d \n", move_array[i*3], move_array[i*3+1], move_array[i*3+2], t);

				if(t>m){
					m =t;
					best_score = m;
					best_index = i;


				}

			}
		}

		// reset here
		tmp_game.undo_move();

		if( (m == MAX_SCORE) || (m >= set_beta) ){
			break;
		}

		
		n = MY_MAX( set_alpha, m ) +1;


	}
	fprintf(stderr, "best_index: %d \n", best_index);

	out_move[0] = move_array[ best_index * 3];
	out_move[1] = move_array[ best_index * 3 +1];
	out_move[2] = move_array[ best_index * 3 +2];

	return;
}













