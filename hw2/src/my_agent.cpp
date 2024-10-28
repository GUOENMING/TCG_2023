#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>

#include "ewn.h"
//#include "random_walk.cpp"

#define MAX_TREE 1500000
#define NODE_NOT_USE -1
#define NODE_NO_DEPTH_2_PARENT -2

//#define DO_TRIAL_PER_NODE 20
#define TIME_OUT_SEC 1.7

#define get_parent(ptr) (mcts_tree[ptr].parent_id)
#define get_child(ptr,i) (mcts_tree[ptr].child_ids[i])

#define MORE_TRIED 15
// 10 is best now
#define TOTAL_NEED_TRIED 200
#define DO_TRIAL_PER_NODE_NO_EXTEND 30


#define SCALE_TERM ((double) 1.0 ) 
// 3.5 is best now, not use
#define SPECIAL_C1 ((double) 1.414 )   
/////// important!!! the actual C1 will be CONSTANT_C * SPECIAL_C1 , 0.5 is best now
#define SPECIAL_C2 ((double) 0.25 )   
// 1.7 is best now

#define CONSTANT_C ((double) 1.0 )   
// 1.5 is best now

 //[ 300, 300, 150, 100, 50, 30, 30, 20, 20, 20, 20 ,20 ,20, 20, 20, 20, 20, 20 ]

#define ADDITIONAL_TRY 1
// 1 is best now
#define MODE_CHANGE_THRESHOLD 3

int ESLAPLSE_STEP = 0;

long long int limit_long_long = 9223372036854770000;

double check_threshold= 0.5;

//long long int limit_long_long = 100000000;

int DO_TRIAL_PER_NODE[ MAX_MOVES + 1 ];

// be sure to MAX_MOVES > 6 or will be index error
void init_do_trial_per_node(void){
    DO_TRIAL_PER_NODE[0]=75;
    DO_TRIAL_PER_NODE[1]=25;
    DO_TRIAL_PER_NODE[2]=25;
    DO_TRIAL_PER_NODE[3]=25;
    DO_TRIAL_PER_NODE[4]=25;
    DO_TRIAL_PER_NODE[5]=25;
    DO_TRIAL_PER_NODE[6]=20;
    for(int i=7; i<=MAX_MOVES; i++){
        DO_TRIAL_PER_NODE[i]=20;
    }
        return;
}


int recv_move(int enemy) {
    int num = getchar() - '0';
    int dir = getchar() - '0';
    if (enemy == BLUE) num += MAX_CUBES;
    return num << 4 | dir;
}

void send_move(int move) {
    int num = move >> 4;
    int dir = move & 0xf;
    if (num >= MAX_CUBES) num -= MAX_CUBES;
    printf("%d%d", num, dir);
    fflush(stdout);
}




int THIS_ROUND_ROLE; // RED: -1 , BLUE: 1


// this part maintain data structure

class MCTSTree{
public:
    int parent_id;
    int child_ids[MAX_MOVES]; // 8 
    int move; // the move from parent's to this node, in the format of TA code
    int depth; // root is 0, so  (depth % 2) == 1 is opponent player !!!! 
    int child_num; // always <= 8 , 0 if it is a leaf

    long long int Ntotal; // simulated times
    double CsqrtlogN; // c * sqrt( log( Ntotal ) )
    double sqrtN; 

    long long int sum; // record "root player" win times, so if need to check this node player win times, use ( Ntotal - sum ) while this node player != root player 
    //long long int sum2;

    double mean;
    double variance;


    MCTSTree(){};
    void init_mcts_tree(const int& parent, const int& recv_move, const int& now_depth);

};




MCTSTree mcts_tree[MAX_TREE]; 
int root_index = -1;


void MCTSTree::init_mcts_tree(const int& parent, const int& recv_move, const int& now_depth){
    parent_id = parent;
    move = recv_move;
    depth = now_depth;

    // clean up other attribute
    child_num = 0;
    Ntotal = 0;
    //CsqrtlogN = 0;
    //sqrtN =0;
    sum=0;
    //sum2=0;
    //mean=0;
    //variance=0;

    return;

}





// for efficiently track and record
// store NODE_NOT_USE if it is free, store NODE_NO_DEPTH_2_PARENT if depth < 1 ( opponent next move node and root node)
// else store DEPTH_2_PARENT id 



class TREE_STATUS{
public:

    int treenode_status[ MAX_TREE ];
    int tree_index; // 0 to (MAX_TREE-1)
    int free_index_count; // remain NODE_NOT_USE count


    TREE_STATUS(){};

    void init_status(int ignore_index);
    int return_free_tree_node(int depth_2_node);

};


void TREE_STATUS::init_status(int ignore_index){
    
    tree_index = 0;
    free_index_count=0;
    if(ignore_index != NODE_NOT_USE){
        for(int i=0 ; i < MAX_TREE; i++){
            if ( treenode_status[i] != ignore_index ){
                treenode_status[i] =  NODE_NOT_USE;
                free_index_count++;
            }
        }
    }
    else{ // clear all
        for(int i=0 ; i < MAX_TREE; i++){
            treenode_status[i]= NODE_NOT_USE;
            free_index_count++;
        }
    }
    /*
    //debug 
    fprintf(stderr, "reset tree_index to %d and free index has %d\n", tree_index, free_index_count);
    
    
    if(ignore_index!=NODE_NOT_USE){
        fprintf(stderr,"leave:\n");
        for(int i=0 ; i < MAX_TREE; i++){
            if ( treenode_status[i] == ignore_index)
                fprintf(stderr, "%d ", i);
        }
        fprintf(stderr, "\n\n");
    }*/
    
    return;

}


int TREE_STATUS::return_free_tree_node(int depth_2_node){

    while ( treenode_status[ tree_index ] != NODE_NOT_USE ){
        tree_index ++;
    };

    treenode_status[ tree_index ] = depth_2_node; 
    free_index_count--;

    return tree_index ; 

}

TREE_STATUS tree_status_instance;


void reuse_routine(int start_id, int depth_2_node){
    // traverse the tree, dfs version

    mcts_tree[ start_id ].depth -= 2;

    for(int i=0; i < mcts_tree[start_id].child_num; i++){
        int child_node = get_child(start_id, i);

        
        if(mcts_tree[ child_node ].depth == 4 ){
            tree_status_instance.treenode_status[ child_node ] = child_node;
            reuse_routine( child_node , child_node);
        }
        else{
            tree_status_instance.treenode_status[ child_node ] = depth_2_node;
            reuse_routine( child_node , depth_2_node);

        }


    }
    return;

}






// this part is my MCTS algorithm


void update_node( MCTSTree& this_node ,  int win, int win_2, int Ntrials ){

    // --------- update ----------
    this_node.Ntotal += (long long int) Ntrials;
    double saved_Ntotal = ((double) this_node.Ntotal );

    this_node.CsqrtlogN = CONSTANT_C * std::sqrt( std::log( saved_Ntotal ));
    this_node.sqrtN = std::sqrt( saved_Ntotal );

    this_node.sum += (long long int) win;
    ////////this_node.sum2 += win_2;

    this_node.mean = ((double) this_node.sum) / ( saved_Ntotal );
    ////////this_node.variance = ((double) this_node.sum2) * ( ( 2 * (double) this_node.sum )  - this_node.mean ) / ( (double) this_node.Ntotal );
    


    //this_node.variance = ((double) this_node.sum) * ( ( 2 * (double) this_node.sum )  - this_node.mean ) / ( (double) this_node.Ntotal );
    //this_node.variance = (1.0 - this_node.mean) * (this_node.mean) * SCALE_TERM ;
    this_node.variance = ( 1.0 - this_node.mean ) * (this_node.mean);

    // ------- end update ----------
    return;

}


//don't update Csqrt because it is a leaf
void update_node_leaf_ver( MCTSTree& this_node , int win, int win_2, int Ntrials ){

    // --------- update ----------
    this_node.Ntotal += (long long int) Ntrials;
    double saved_Ntotal = ((double) this_node.Ntotal );
    //this_node.CsqrtlogN = std::sqrt( std::log( (double) this_node.Ntotal ) );
    this_node.sqrtN = CONSTANT_C * std::sqrt( saved_Ntotal );

    this_node.sum += (long long int) win;
    ///////this_node.sum2 += win_2;

    this_node.mean = ((double) this_node.sum) / ( saved_Ntotal );
    ///////this_node.variance = ((double) this_node.sum2) * ( ( 2 * (double) this_node.sum )  - this_node.mean ) / ( (double) this_node.Ntotal );
    

    //this_node.variance = ((double) this_node.sum) * ( ( 2 * (double) this_node.sum )  - this_node.mean ) / ( (double) this_node.Ntotal );
    //this_node.variance = ( 1.0 - this_node.mean ) * (this_node.mean) * SCALE_TERM ;
    this_node.variance = ( 1.0 - this_node.mean ) * (this_node.mean);

    // ------- end update ----------
    return ;

}




void bottom_up_update(int start_id, const int& total_win, const int& total_win_2, const int& total_trials){
    do{
        update_node(mcts_tree[start_id], total_win, total_win_2, total_trials);
        start_id = get_parent(start_id);
    }
    while(start_id != -1); // root is the only parent id is -1

    return;
}



//double MIN_S = 0.0;
//double MAX_S = 1.0;
//double RANGE_S = 1.0;


// ucb implement here (for not func call, I may copy it inline ? )
// dont forget root has no parent, so don't call ucb on root
// 
double UCB(const int& node_id){
    //double normalized_score = ( mcts_tree[node_id].mean - MIN_S ) / RANGE_S;
    
    double normalized_score = mcts_tree[node_id].mean;

    return ( (mcts_tree[node_id].depth % 2) ? (normalized_score) : (1.0 - normalized_score) ) + ( mcts_tree[get_parent(node_id)].CsqrtlogN / mcts_tree[node_id].sqrtN);

}

double UCB_VARIANCE(const int& node_id){
    //double normalized_score = ( mcts_tree[node_id].mean - MIN_S ) / RANGE_S;
    
    double tmp_double = ( mcts_tree[get_parent(node_id)].CsqrtlogN / mcts_tree[node_id].sqrtN );
    double variance_term = ( mcts_tree[node_id].variance * mcts_tree[node_id].variance ) + SPECIAL_C1 * tmp_double ;
    variance_term = (variance_term < SPECIAL_C2 ) ?  variance_term : SPECIAL_C2 ;

    return ( (mcts_tree[node_id].depth % 2) ? (mcts_tree[node_id].mean) : (1.0 - mcts_tree[node_id].mean) ) + tmp_double * std::sqrt(variance_term);

}



int PV_times;

// go deep, and make the TMP_EWN go deeper too.
void FIND_PV( int& start_id , TMP_EWN& tmp_game, int& depth_2){
    int go_depth = 0;

    int max_child;
    double max_ucb_score;

    // find_PV
    while (mcts_tree[start_id].child_num > 0){
        max_child = get_child(start_id, 0);
        max_ucb_score = UCB( max_child );

        for(int i=1; i < mcts_tree[start_id].child_num; i++){
            int tmp_child = get_child(start_id, i);
            // double tmp_ucb_score = UCB(tmp_child);
            double tmp_ucb_score = UCB_VARIANCE(tmp_child);

            if( tmp_ucb_score > max_ucb_score){
                max_child = tmp_child;
                max_ucb_score = tmp_ucb_score;
            }
        }

        // go deep
        start_id = max_child;
        go_depth++;
        tmp_game.do_tmp_move(mcts_tree[max_child].move);

        // mark the ascent with depth 2  
        if(go_depth==2){
            depth_2 = max_child;
        }
    }

    // return the max ucb score node and depth 2 node by inference
    return;
}

int main_reuse_move[MAX_MOVES];

// return 1 or -1 if it's sure this board is end
int Expand_node(int start_id, TMP_EWN& tmp_game, const int& depth_2){
    


    if(tmp_game.tmp_is_over()){
        return tmp_game.tmp_is_over();
    }

    mcts_tree[start_id].child_num = tmp_game.tmp_move_gen_all(main_reuse_move);

    //fprintf(stderr, "node %d child rum %d\n", start_id, mcts_tree[start_id].child_num);

    for(int i=0; i < mcts_tree[start_id].child_num; i++){

        int new_node = tree_status_instance.return_free_tree_node(depth_2);

        // add back !!!!!!!!!!!!!!!!!!!
        if( mcts_tree[start_id].depth == 1){
            tree_status_instance.treenode_status[new_node] = new_node;
        }


        //fprintf(stderr, "new_node: %d\n", new_node);
        // parent, move, depth
        mcts_tree[new_node].init_mcts_tree( start_id , main_reuse_move[i], mcts_tree[start_id].depth + 1 ); 
        mcts_tree[start_id].child_ids[i]=new_node;


    }

    return 0;

}

// mode 1 / -1  for "already" red/blue win board, mode 0 for normal board
void Simulation_Propagation( const int& start_id, int end, TMP_EWN& tmp_game, TMP_EWN& tmp_game_2){
    if(!end){
        int parent_win_times = 0;
        int parent_tried_times = 0;
        int win_times;
        int tried_times;
        int saved_move;
        int saved_child_num = mcts_tree[start_id].child_num;

        int who_win;

        for(int i=0; i < saved_child_num; i++){

            int child_id = get_child(start_id, i);
            // may pruned here



            // do simulate

            saved_move = tmp_game.do_tmp_move_return_record(mcts_tree[child_id].move);


            win_times = 0;
            tried_times = 0;
            /*
            if( ESLAPLSE_STEP <= MODE_CHANGE_THRESHOLD ){
                for( ;  tried_times < DO_TRIAL_PER_NODE[ saved_child_num ]; tried_times++){

                    // the "root" role win 
                    who_win = do_one_try_tmpewn_version(tmp_game, tmp_game_2);
                    // who_win = do_one_try_tmpewn_version_and_with_check(tmp_game, tmp_game_2);

                    //fprintf(stderr, "on node %d , %d win \n", child_id, who_win);
                    if( who_win == THIS_ROUND_ROLE ){
                        win_times++;
                    }
                }
            }
            else{*/
                for( ;  tried_times < DO_TRIAL_PER_NODE[ saved_child_num ]; tried_times++){

                    // the "root" role win 
                    // who_win = do_one_try_tmpewn_version(tmp_game, tmp_game_2);
                    who_win = do_one_try_tmpewn_version_and_with_check(tmp_game, tmp_game_2);

                    //fprintf(stderr, "on node %d , %d win \n", child_id, who_win);
                    if( who_win == THIS_ROUND_ROLE ){
                        win_times++;
                    }
                }
            //}

            //fprintf(stderr, "on node %d, %d root win and %d tried\n", child_id, win_times, tried_times);
            parent_win_times += win_times;
            parent_tried_times += tried_times;

            // update win rate
            // MCTSTree& this_node , int win, int win_2, int Ntrials

            update_node_leaf_ver(mcts_tree[child_id], win_times, win_times, tried_times);

            // restore for next child 
            tmp_game.undo_tmp_move( saved_move );
        }

        
        // add tried time
        int more_try = ADDITIONAL_TRY;

        if( saved_child_num > 0){

            //while( parent_tried_times < TOTAL_NEED_TRIED ){
            while(more_try--){
                // choose the highest win rate to run
                int highest_prob_child = get_child( start_id, 0);

                if( mcts_tree[ start_id ].depth % 2 ){ // 0 for max node, 1 for min node

                    //min node

                    for (int i =1; i< saved_child_num; i++){

                        int tmp_child = get_child(start_id, i);


                            if( mcts_tree[ tmp_child ].mean < mcts_tree[ highest_prob_child ].mean ){
                                highest_prob_child = tmp_child;
                            }
                        
                    }

                    if (mcts_tree[ highest_prob_child ].mean > check_threshold ){
                        break;
                    }
                }
                else{

                    // max node

                    for (int i =1; i< saved_child_num; i++){

                        int tmp_child = get_child(start_id, i);

                            if( mcts_tree[ tmp_child ].mean > mcts_tree[ highest_prob_child ].mean ){
                                highest_prob_child = tmp_child;
                            }
                        
                    }
                    if (mcts_tree[ highest_prob_child ].mean < check_threshold ){
                        break;
                    }


                }

                // do simulate

                saved_move = tmp_game.do_tmp_move_return_record( mcts_tree[ highest_prob_child ].move );

                win_times = 0;
                tried_times = 0;


                for( ;  tried_times < MORE_TRIED ; tried_times++){

                    // the "root" role win 
                    // who_win = do_one_try_tmpewn_version(tmp_game, tmp_game_2);
                    who_win = do_one_try_tmpewn_version_and_with_check(tmp_game, tmp_game_2);

                    //fprintf(stderr, "on node %d , %d win \n", highest_prob_child , who_win);
                    if( who_win == THIS_ROUND_ROLE ){
                        win_times++;
                    }
                }
                //fprintf(stderr, "on node %d, %d root win and %d tried\n", highest_prob_child, win_times, tried_times);
                parent_win_times += win_times;
                parent_tried_times += tried_times;

                update_node_leaf_ver( mcts_tree[ highest_prob_child ] , win_times, win_times, tried_times);

                // restore for next child 
                tmp_game.undo_tmp_move( saved_move );

            }
        }
        



        // call bottom up update
        bottom_up_update( start_id , parent_win_times , parent_win_times , parent_tried_times);

        return;
    }
    else{
        if( tmp_game.tmp_is_over() == THIS_ROUND_ROLE){
            bottom_up_update( start_id , DO_TRIAL_PER_NODE[0], DO_TRIAL_PER_NODE[0], DO_TRIAL_PER_NODE[0]);
        }
        else{
            bottom_up_update( start_id , 0, 0, DO_TRIAL_PER_NODE[0]);
        }
        return;

    }

}


void Simulation_Propagation_no_expand_ver( const int& start_id, int end, TMP_EWN& tmp_game, TMP_EWN& tmp_game_2){
    if(!end){

        int win_times = 0;
        int tried_times = 0;
        int who_win;

        /*
        if(ESLAPLSE_STEP <= MODE_CHANGE_THRESHOLD){
            for( ;  tried_times < DO_TRIAL_PER_NODE_NO_EXTEND ; tried_times++){

                // the "root" role win 
                who_win = do_one_try_tmpewn_version(tmp_game, tmp_game_2);
                // who_win = do_one_try_tmpewn_version_and_with_check(tmp_game, tmp_game_2);

                //fprintf(stderr, "on node %d , %d win \n", child_id, who_win);
                if( who_win == THIS_ROUND_ROLE ){
                    win_times++;
                }
            }
        }
        else{*/
            for( ;  tried_times < DO_TRIAL_PER_NODE_NO_EXTEND ; tried_times++){

                // the "root" role win 
                // who_win = do_one_try_tmpewn_version(tmp_game, tmp_game_2);
                who_win = do_one_try_tmpewn_version_and_with_check(tmp_game, tmp_game_2);

                //fprintf(stderr, "on node %d , %d win \n", child_id, who_win);
                if( who_win == THIS_ROUND_ROLE ){
                    win_times++;
                }
            }
        //}


        // call bottom up update
        bottom_up_update( start_id , win_times , win_times , tried_times);

        return;
    }
    else{
        if( tmp_game.tmp_is_over() == THIS_ROUND_ROLE){
            bottom_up_update( start_id , DO_TRIAL_PER_NODE[0], DO_TRIAL_PER_NODE[0], DO_TRIAL_PER_NODE[0]);
        }
        else{
            bottom_up_update( start_id , 0, 0, DO_TRIAL_PER_NODE[0]);
        }
        return;

    }

}



void print_mcts_tree(int find_root, int depth){
    for(int i =0; i<depth; i++){
        fprintf(stderr, "     ");
    }
    fprintf(stderr,"%d", find_root);
    fprintf(stderr," ( %d, win rate %lf, %lld, %lld ) \n", mcts_tree[find_root].child_num, mcts_tree[find_root].mean, mcts_tree[find_root].sum, mcts_tree[find_root].Ntotal);



    /*if( depth != mcts_tree[find_root].depth ){
        fprintf(stderr, "why?????\n");
    }*/
    if(depth <= 2){ // limit print depth
        for(int i=0; i < mcts_tree[find_root].child_num; i++){
            print_mcts_tree( get_child(find_root,i) , depth+1);
        }
    }
    return;
}


clock_t start_time, limit_time, tmp_clock;


int main() {
    EWN game;
    bool my_turn;
    int enemy;
    int move;
    TMP_EWN tmp_game;
    TMP_EWN tmp_game_2;

    init_do_trial_per_node();




    do {




        game.init_board();

        my_turn = getchar() == 'f';
        enemy = my_turn ? BLUE : RED;

        // initialize 

        // add time constrain for first move 
        start_time = std::clock();
        limit_time = start_time + (clock_t)(((double) TIME_OUT_SEC ) * CLOCKS_PER_SEC );


        ESLAPLSE_STEP = 0;

        // first add this turn dice seq
        tmp_game.set_tmp_ewn_dice_seq(game);
        tmp_game_2.set_tmp_ewn_dice_seq(game);


        
        THIS_ROUND_ROLE = my_turn ? -1 : 1 ;
        
        tree_status_instance.init_status(NODE_NOT_USE);
        int previous_record_root = -1;

        root_index = tree_status_instance.return_free_tree_node(NODE_NO_DEPTH_2_PARENT);

        // parent = -1, prev_move = don't care, depth = 0
        mcts_tree[root_index].init_mcts_tree(-1, 0, 0);

        //fprintf(stderr, "\n\n===================================\n\n");

        while (!game.is_over()) {
            if (!my_turn) {
                move = recv_move(enemy);

                // timeout start here
                start_time = std::clock();
                limit_time = start_time + (clock_t)( ((double) TIME_OUT_SEC ) * CLOCKS_PER_SEC ) ; 

                //

                game.do_move(move);

                if(previous_record_root != -1){

                    if(mcts_tree[root_index].child_num == 0){
                    //if(mcts_tree[root_index].child_num == 0 || ESLAPLSE_STEP == MODE_CHANGE_THRESHOLD ){

                        previous_record_root = root_index;
                        tree_status_instance.init_status(NODE_NOT_USE);
                        root_index = tree_status_instance.return_free_tree_node(NODE_NO_DEPTH_2_PARENT);
                        mcts_tree[root_index].init_mcts_tree(-1, 0, 0);

                    }
                    else{
                        for(int i =0; i < mcts_tree[root_index].child_num; i++){

                            int this_child = get_child(root_index, i);

                            if( mcts_tree[ this_child ].move == move ){

                                previous_record_root = root_index;
                                root_index = this_child;

                                // important!!!
                                mcts_tree[this_child].parent_id = -1;

                                // clear node not on new subtree
                                //fprintf(stderr, "before free_node_count: %d\n", tree_status_instance.free_index_count);
                                tree_status_instance.init_status(this_child);
                                //fprintf(stderr, "after free_node_count: %d\n", tree_status_instance.free_index_count);

                                reuse_routine(this_child, NODE_NO_DEPTH_2_PARENT);

                                //clock_t tmp_clock_1 = std::clock();
                                //fprintf(stderr, "use_time: %f\n", ((double) (tmp_clock_1 - start_time )) / CLOCKS_PER_SEC );
                                break ;
                            }

                        }
                    }

                }
                // debug
                //print_mcts_tree(root_index, 0);


            }
            else {
                ESLAPLSE_STEP += 1;

                PV_times=1;  

                //may not overflow :)             
                while( PV_times++ ){

                    // timeout detect here
                    //if(PV_times % 200 == 0){
                        tmp_clock = std::clock();
                        if ( tmp_clock >= limit_time ){
                            //fprintf( stderr, "clock %ld and limit %ld\n", tmp_clock, limit_time);
                            break;
                        }
                    //}
                    //
                    // preserve 5000 step for not overflow
                    if(mcts_tree[root_index].Ntotal > limit_long_long){
                        break;
                    }

                    //tmp_game.set_tmp_ewn(game);
                    tmp_game.set_tmp_ewn_without_diceseq(game);

                    int depth_2 = NODE_NO_DEPTH_2_PARENT;
                    int now_index = root_index;

                    //fprintf(stderr, "before: depth_2_node %d now_index %d\n\n", depth_2, now_index);

                    //void FIND_PV( int& start_id , TMP_EWN& tmp_game, int& depth_2)
                    FIND_PV(now_index, tmp_game, depth_2);

                    //fprintf(stderr, "after: depth_2_node %d now_index %d\n\n", depth_2, now_index);

                    // check we have enough node
                    if (tree_status_instance.free_index_count <= (MAX_MOVES+1) ){

                        Simulation_Propagation_no_expand_ver(now_index, tmp_game.tmp_is_over(), tmp_game, tmp_game_2);

                    }
                    //int Expand_node(int start_id, TMP_EWN& tmp_game, const int& depth_2)
                    else{
                        int result = Expand_node( now_index, tmp_game, depth_2);
                        // Simulation_Propagation(const int& start_id, int end, TMP_EWN& tmp_game, TMP_EWN& tmp_game_2)
                        Simulation_Propagation( now_index, result, tmp_game, tmp_game_2);

                    }



                }
                //fprintf(stderr, "PV_times: %d\n\n\n", PV_times);






                // go the highest win rate
                int best_index = get_child(root_index, 0);

                for(int i=1; i < mcts_tree[root_index].child_num; i++){

                    int tmp_index = get_child(root_index, i);
                    if ( mcts_tree[tmp_index].mean > mcts_tree[best_index].mean){
                        best_index = tmp_index;
                    }
                }

                // test 
                /*
                if (mcts_tree[best_index].Ntotal < 100000000){

                    for(int i=0; i < mcts_tree[root_index].child_num; i++){

                    int tmp_simulation_index = get_child(root_index, i);
                        if ( ( mcts_tree[ tmp_simulation_index ].Ntotal > (mcts_tree[best_index].Ntotal + 50000 ) ) && ( mcts_tree[ tmp_simulation_index ].mean >= 0.55 ) ){
                            best_index = tmp_simulation_index;
                        }
                    }
                }
                */

                


                move = mcts_tree[ best_index ].move;
                //fprintf(stderr, "child_num %d\n",mcts_tree[root_index].child_num);
                //debug
                /*
                fprintf(stderr, "estimate win rate %lf\n", mcts_tree[ best_index ].mean);
                
                for(int i =0; i< mcts_tree[root_index].child_num; i++){
                    fprintf(stderr, "%lf %lld %lld\n", mcts_tree[get_child(root_index, i)].mean , mcts_tree[get_child(root_index, i)].sum, mcts_tree[get_child(root_index, i)].Ntotal);
                }
                fprintf(stderr, "\n");
                */

                // debug 2
                //print_mcts_tree(root_index, 0);


                // go next root
                previous_record_root = root_index;
                root_index = best_index;


                // debug again
                // debug
                //print_mcts_tree(root_index, 0);

                mcts_tree[  root_index  ].parent_id = -1;

                //tmp_game.set_tmp_ewn(game);
                //tmp_game.print_tmp_board();

                //fprintf(stderr, "send move\n\n");
                game.do_move(move);

                //tmp_game.set_tmp_ewn(game);
                //tmp_game.print_tmp_board();


                //tmp_clock= std::clock();
                //fprintf(stderr, "clock %ld and limit %ld and start %ld\n", tmp_clock, limit_time, start_time);
                send_move(move);
                
            }
            my_turn = !my_turn;
        }
    } while (getchar() == 'y');
}