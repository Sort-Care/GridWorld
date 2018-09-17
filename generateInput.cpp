/*
 * Grid World: Generate the transition table in the input file.
 * Author: Haoyu Ji
 * Usage: 
 * > g++-7 generateInput.cpp
 * > ./a.out > [out_filename]
 * Output File Structure:
 * First Line: number of states |S|
 * Next |S| lines: rewards for entering each state
 * Next |S|x|A| lines: Transition table
 * Within Transition table:
 *       {state_i transit with action |A|} * |S|
 *
 * TODO: 
 * 1. Adding absorbing state
 */


#include <stdio.h>
#include <algorithm>
#include <stdlib.h> // div, div_t

#define REP(i, a, b) for (int i = int(a); i <= b; i++)

//Total number of states
const int STATE_NUM = 23;
const int NEG_INF = -100000;
``

//Grid World Size by rows x columns
const int row = 5;
const int column = 5;
const int NUM_ACTION = 4; //AU, AD, AL, AR
const int NUM_OUTCOME = 4;
const int X = 0;
const int Y = 1;



// Four actions
enum ACTIONS {AU, AD, AL, AR};
enum OUTCOMES {SUC, STAY, VL, VR}; 
const int coordinate_change[NUM_ACTION][NUM_OUTCOME][2] = {
    {//AU
        {-1, 0}, //SUC
        {0, 0},  //STAY
        {0, -1}, //VL
        {0, 1}   //VR
    },
    {//AD
        {1, 0},
        {0, 0},
        {0, 1},
        {0, -1}
    },
    {//AL
        {0, -1},
        {0, 0},
        {1, 0},
        {-1, 0}
    },
    {//AR
        {0, 1},
        {0, 0},
        {-1, 0},
        {1, 0}
    }
};


//Grid World Model Probabilities on moving effects.
const double probs[NUM_OUTCOME] = {0.8, 0.1, 0.05, 0.05};

const double GW[row][column] = {
    {0.0, 0.0, 0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0, 0.0, 0.0},
    {0.0, 0.0, NEG_INF, 0.0, 0.0},
    {0.0, 0.0, NEG_INF, 0.0, 0.0},
    {0.0, 0.0, -10.0, 0.0, 10.0}
};

//map from state number to coordinates
const int state_to_coor[STATE_NUM][2] = {
    {0, 0},//1
    {0, 1},
    {0, 2},
    {0, 3},
    {0, 4},
    
    {1, 0},
    {1, 1},
    {1, 2},
    {1, 3},
    {1, 4},
    
    {2, 0},//11
    {2, 1},
    {2, 3},
    {2, 4},
    
    {3, 0},//15
    {3, 1},
    {3, 3},//17
    {3, 4},
    
    {4, 0},//19
    {4, 1},//20
    {4, 2},//21
    {4, 3},//22
    {4, 4}// 23
};

const int coor_to_state[row][column] = {
    {1,2,3,4,5},
    {6,7,8,9,10},
    {11,12,NEG_INF,13,14},
    {15,16,NEG_INF,17,18},
    {19,20,21,22,23}
};


int main(){
        //initialize all to 0.0
        //printf("Trans table row: %d, column: %d\n", STATE_NUM*NUM_ACTION, STATE_NUM);
    double trans_table[STATE_NUM * NUM_ACTION][STATE_NUM] = {0.0};
    REP (s, 0, STATE_NUM-1){//Genrate Transition probability row for state_i
        REP (a, 0, NUM_ACTION-1 ){//for each action
            REP (o, 0, NUM_OUTCOME-1 ){//For each outcome
                    /*
                      Code Block for 
                      STATE: s
                      ACTION: a
                      OUTCOME: o
                    */
                    //compute current position, using the map
                    // printf("In LOOP: For state: %d, Action: %d, Outcome: %d\n",
                    //        s,
                    //        a,
                    //        o);
                
                int x, y;
                x = state_to_coor[s][X];
                y = state_to_coor[s][Y];
                
                int off_x, off_y;
                off_x = coordinate_change[a][o][X];
                off_y = coordinate_change[a][o][Y];

                    //printf("X: %d, Y: %d, OFF_X: %d, OFF_Y: %d\n", x,y,off_x,off_y);
                

                    //tell if the coordinates are valid or not, if not shift to original
                int new_x, new_y;
                new_x = x + off_x;
                new_y = y + off_y;
                
                if (new_x < 0 || new_x > 4 || new_y < 0 || new_y > 4 || GW[new_x][new_y] == NEG_INF){// invalid new position
                        //reset to original, which means not moved
                    new_x = x; new_y = y;
                }
                    // printf("New_x: %d\t New_y: %d \n",
                    //        new_x,
                    //        new_y);
                    //compute probability
                int result_state = coor_to_state[new_x][new_y];

                    //update trans_table[s * NUM]
                trans_table[s * NUM_ACTION + a][result_state-1] += probs[o];
                    // printf("Updating transition table at (%d, %d) adding prob %f\n",
                    //        s*NUM_ACTION+a,
                    //        result_state-1,
                    //        probs[o]);
                
            }
                //print trans_table[s*a][ALL]
            
        }
        
    }

    printf("%d\n", STATE_NUM);//first line number of state
    REP (i, 0, STATE_NUM-1){//the next STATE_NUM line: rewards for entering each state
        printf("%.1f\n", GW[state_to_coor[i][X]][state_to_coor[i][Y]]);
    }
    REP (i, 0, STATE_NUM*NUM_ACTION-1){// next STATE_NUM * NUM_ACTION lines: Transition table
        REP (j, 0, STATE_NUM-1){
            printf("%.2f, ", trans_table[i][j]);
        }
        printf("\n");
    }

    
    
    return 0;
    
}
