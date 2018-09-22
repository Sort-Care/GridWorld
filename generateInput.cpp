/*
 * Grid World: Generate the transition table in the input file and simulate
 *             the running of the grid world
 * Author: Haoyu Ji
 * Usage: 
 * > g++ -I /usr/local/include/eigen3/ generateInput.cpp 
 * > ./a.out > [out_filename]
 * Output File Structure:
 * First Line: number of states |S|
 * Next |S| lines: rewards for entering each state
 * Next |S|x|A| lines: Transition table
 * Within Transition table:
 *       {state_i transit with action |A|} * |S|
 *
 * TODO: 
 * 1. Adding absorbing state -- DONE
 * 2. Simulate the running of the gridworld -- DUE today
 * 3. Value Iteration
 */


#include <stdio.h>
#include <algorithm>
#include <stdlib.h> // div, div_t, rand
#include <time.h>

#include <random>
#include <chrono>
//#include <Eigen/Dense>

//using namespace Eigen;
#define REP(i, a, b) for (int i = int(a); i <= b; i++)

/*** Data Structures for depicting the gridworld problem ***/

//Total number of states
const int STATE_NUM = 23;
const int goal_state = 23;
const int absorbing_state = 24; //only accessible in transition table
const int NEG_INF = -100000;

//Grid World Size by rows x columns
const int row = 5;
const int column = 5;
const int NUM_ACTION = 4; //AU, AD, AL, AR
const int NUM_OUTCOME = 4;
const int X = 0;
const int Y = 1;



// Four actions
enum ACTIONS {AU, AD, AL, AR};//0, 1, 2, 3
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

double trans_table[(STATE_NUM+1) * NUM_ACTION][STATE_NUM+1] = {0.0};


/***** Data structures for simulating an agent in the gridworld ******/
const int NUM_EPISODES = 10000;
double episode_reward[NUM_EPISODES]; //discounted reward
// Simulation: S0 ----> A0 ----> S1(collect R1) ------> ...



/*** Functions ****/
void generateInput();

int get_random_action(int total_num_actions);

void print_normal();

void print_for_py();

void value_iteration();

void simulate_random();

void simulate_optimal();

/*
 * Random Samle from a distribution array:
 * Input: 
 *       double distribution: an double array specifing a distribution, 
 *                            should add up to 1
 *       int size: the array size
 */
int random_sample(double distribution[], int size);

double random_zero_to_one();







int main(){
    
    generateInput();
    
    return 0;
    
}


void generateInput(){
        //initialize all to 0.0
        //printf("Trans table row: %d, column: %d\n", STATE_NUM*NUM_ACTION, STATE_NUM);
    REP (s, 0, STATE_NUM-1){//Genrate Transition probability row for state_i
        if(s == goal_state-1){//skip the one for goal_state
            continue;
        }
        
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

        //modify the transition table according to the terminal state

        //first modify the goals transition to always go to absorbing state
    REP (i, 0, NUM_ACTION-1){
        trans_table[(goal_state-1)*NUM_ACTION + i][absorbing_state-1] = 1.0;
        trans_table[(absorbing_state-1)*NUM_ACTION + i][absorbing_state-1] = 1.0;
    }

    print_normal();
}



void print_normal(){
    printf("%d\n", STATE_NUM+1);//first line number of state
    REP (i, 0, STATE_NUM-1){//the next STATE_NUM line: rewards for entering each state
        printf("%.1f\n", GW[state_to_coor[i][X]][state_to_coor[i][Y]]);
    }
    printf("%.2f\n", 0.00);//absorbing_state
    REP (i, 0, (STATE_NUM+1)*NUM_ACTION-1){// next STATE_NUM * NUM_ACTION lines: Transition table
        REP (j, 0, STATE_NUM){
//            printf("(%d, %d): ",i, j);
            printf("%.2f, ", trans_table[i][j]);
        }
        printf("\n");
    }
}


void print_for_py(){
    int action_order[4] = {2, 0, 3, 1};//U, D, L, R  0, 1, 2, 3, want L, U, R, D
    
        //print to run value iteration with a python file
    printf("%d\n", STATE_NUM+1);
    REP (i, 0, STATE_NUM-1){//the next STATE_NUM line: rewards for entering each state
        printf("%.1f\n", GW[state_to_coor[i][X]][state_to_coor[i][Y]]);
    }

    printf("%.2f\n", 0.00);
    REP (i, 0, NUM_ACTION-1){// 0, 1, 2, 3  No L, U, R, D
            //for each actions print for every state, here it should be printing
            // 24 lines
        REP (j, 0, STATE_NUM){ //0, 1, ..., 23
                //print trans_table[(j-1)*NUM_ACTION+i][all]
            REP(k, 0, STATE_NUM){
                printf("%.2f", trans_table[j*NUM_ACTION+action_order[i]][k]);
                if (k != STATE_NUM) printf(", ");
                else{
                    printf("\n");
                }
            }
        }
    }
}


int random_sample(double distribution[],int size){
        //check if the array adds up to 1
    double sum = 0.0;
    REP (i, 0, size-1){
        printf("%.2f ", distribution[i]);
        sum += distribution[i];
    }
    printf("\n");
    
    

    if(sum == 1.0){//valid
            // will generate [0,1]
        srand(time(0));
        double rnum = random_zero_to_one();
        printf("%f\n", rnum);
        
        REP(i, 0, size-1){
            printf("%d: %.2f\t rnum: %.2f\n", i, distribution[i], rnum);
            if (rnum < distribution[i]) return i;
            else{
                rnum -= distribution[i];
            }
        }
        
        
    }else{
        return(-1);//not valid return an index that doesn't make sense.
    }
}


double random_zero_to_one(){
    std::mt19937_64 rng;
    // initialize the random number generator with time-dependent seed
    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
    rng.seed(ss);
    // initialize a uniform distribution between 0 and 1
    std::uniform_real_distribution<double> unif(0, 1);
    // ready to generate random numbers
    const int nSimulations = 10;
    double currentRandomNumber = unif(rng);
    return currentRandomNumber;
}
