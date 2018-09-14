/*
 * Grid World: Generate the transition table in the input file.
 */


#include <stdio.h>
#include <algorithm>
#include <stdlib.h> // div, div_t

#define REP (i, a, b) for (int i = int(a); i <= b; i++)

//Total number of states
const int STATE_NUM = 23;
const double NEG_INF = -100000;


//Grid World Size by rows x columns
const int row = 5;
const int column = 5;
const int NUM_ACTION = 4; //AU, AD, AL, AR
const int NUM_OUTCOME = 4;
const int X = 0;
const int Y = 0;



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



int main(){
        //initialize all to 0.0
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
                int x, y;
                x = state_to_coor[s][X];
                y = state_to_coor[s][Y];

                int off_x, off_y;
                off_x = coordinate_change[a][o][X];
                off_y = coordinate_change[a][o][Y];

                    //tell if the coordinates are valid or not, if not shift to original

                    //compute probability

                    //update
                
            }
        }
        
    }
    
    return 0;
    
}
