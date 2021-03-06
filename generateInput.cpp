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

#include <random>
#include <chrono>
#include <cmath>
#include <algorithm>
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
enum OUTCOMES {SUC, STAY, VL, VR}; // with probabilities: 0.8, 0.1, 0.05, 0.05
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

const double GW[row][column] = {// rewards
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
const double dis_gamma = 0.9;

double episode_reward[NUM_EPISODES]; //discounted reward

const double pr_actions[NUM_ACTION] = {0.25, 0.25, 0.25, 0.25};
const double d_0[STATE_NUM] = {//23 states not including the absorbing state
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,
    1,1,1,1,
    1,1,1,1,1
};
const double pr_action_outcome[4] = {0.8, 0.1, 0.05, 0.05};


// optimal policy using last semester 683 value iteration python code
const int optimal_policy[STATE_NUM] = {// AU:0, AD:1, AL:2, AR:3
    3,3,3,1,1,
    3,3,3,1,1,
    0,0,1,1,
    0,0,1,1,
    0,0,3,3,0
};

int s21_cnt = 0;// estimate S_19 = 21
int s18_cnt = 0;// given S_8 = 18 






// Simulation: S0 ----> A0 ----> S1(collect R1) ------> ...



/*** Functions ****/
void generateInput();

int get_random_action(int total_num_actions);

void print_normal();

void print_for_py();

void value_iteration();

double simulate_random();// run simulation randomly choose actions

double simulate_optimal();// run simulation using optimal policy

void run_simulation_with_strategy(double (*f)());

double estimate_quantity();


/*
 * Random Samle from a distribution array:
 * Input: 
 *       double distribution: an double array specifing a distribution, 
 *                            should add up to 1
 *       int size: the array size
 */
int random_sample_distribution(const double distribution[],const int size);

int random_sample_weights(const double weights[], const int size);

double random_zero_to_one();

double random_range(double range);

void get_array_statistics(const double array[], const int size);

void get_standard_deviation(const double array[], const int size,
                            double *amean,
                            double *adevia);




int main(int argc, char *argv[]){
    generateInput();
//    run_simulation_with_strategy(simulate_random);

        //options: 1. Run Random, 2. Run optimal, 3, estimate quantity
    if(argc == 0) {
        printf("Enter some options to run the program:\nRandom policy: 1, \nOptimal:2,\nEstimate quantity:3");
        return 0;
    }else{
        int option = std::stoi( argv[1] );
        
        switch(option){
            case 1:
                    //run random
                run_simulation_with_strategy(simulate_random);
                get_array_statistics(episode_reward,NUM_EPISODES);
                break;
            case 2:
                    //run optimal
                run_simulation_with_strategy(simulate_optimal);
                get_array_statistics(episode_reward,NUM_EPISODES);
                break;
            case 3:
                    //estimate quantity
                double res = estimate_quantity();
                printf("%.5f\n", res);
                break;
                
        }
    }
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

    //print_normal();
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


int random_sample_distribution(const double distribution[], const int size){
        //check if the array adds up to 1
    double sum = 0.0;
    REP (i, 0, size-1){
    printf("%.2f ", distribution[i]);
        sum += distribution[i];
    }
    printf("\n");
    
    

    if(sum == 1.0){//valid
            // will generate [0,1]
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

int random_sample_weights(const double weights[], const int size){
        //first compute the sum of all weights
    double sum = 0.0;
    REP (i, 0, size-1){
    sum += weights[i];
    //printf("Adding %.2f\n", weights[i]);
    }
    //printf("%.2f\n", sum);
    double rnum = random_range(sum);
    //printf("%.2f\n", rnum);
    REP (i, 0, size-1){
    if(rnum < weights[i]) return i;
        else{
    rnum -= weights[i];
        }
    }
}

double random_range(double range){
    std::mt19937_64 rng;
    // initialize the random number generator with time-dependent seed
    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
    rng.seed(ss);
    // initialize a uniform distribution between 0 and 1
    std::uniform_real_distribution<double> unif(0, range);
    // ready to generate random numbers
    const int nSimulations = 10;
    double currentRandomNumber = unif(rng);
    return currentRandomNumber;
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


double simulate_random(){
        //get s0 as a start state
    int start_state = random_sample_weights(d_0, 23);
    int S_t, A_t, S_tn;
    double discounted_reward = 0.0;
    int cnt = 0;
    

    S_t = start_state;
    while (S_t != absorbing_state-1){// nor absorbing state
            //sample an action randomly
            //printf("Current State: %d\t", S_t);
        
        A_t = random_sample_weights(pr_actions, 4);
            //printf("Picked Action: %d\t", A_t);
        
            //sample the next state given
        S_tn = random_sample_weights(trans_table[S_t * NUM_ACTION + A_t], STATE_NUM+1);
            //printf("Next State: %d\n", S_tn);
        
            //Get reward
        discounted_reward += pow(dis_gamma, cnt) * GW[state_to_coor[S_tn][X]][state_to_coor[S_tn][Y]];
        cnt ++;
            //printf("discounted: %f\t Reward:%.2f\n", pow(dis_gamma, cnt),
            //GW[state_to_coor[S_tn][X]][state_to_coor[S_tn][Y]]);
        

            //update the discounted reward
        S_t = S_tn;
    }

    return discounted_reward;
        //printf("%.8f\n", discounted_reward);
}


double estimate_quantity(){
    REP(i, 0, NUM_EPISODES-1){
        printf("\rEpisode: %d", i);
        int start_state = random_sample_weights(d_0, 23);
        int S_t, A_t, S_tn;
        int cnt = 0;
        bool valid_trial = false;
        
    
    
        S_t = start_state;
        while (S_t != absorbing_state-1){// nor absorbing state
                //sample an action randomly
                //printf("Current State: %d\t", S_t);
        
            A_t = random_sample_weights(pr_actions, 4);
                //printf("Picked Action: %d\t", A_t);
        
                //sample the next state given
            S_tn = random_sample_weights(trans_table[S_t * NUM_ACTION + A_t], STATE_NUM+1);
                //printf("Next State: %d\n", S_tn);
        
                //Get reward
                //discounted_reward += pow(dis_gamma, cnt) * GW[state_to_coor[S_tn][X]][state_to_coor[S_tn][Y]];
            cnt ++;

            if(cnt == 8 && S_tn == 17){
//                printf("\n adding one to s18");
                s18_cnt++;
                valid_trial = true;
            }
            if(valid_trial && cnt == 19 && S_tn == 20){
//                printf("\nadding one to s21 given s18");
                s21_cnt++;
            }

                //printf("discounted: %f\t Reward:%.2f\n", pow(dis_gamma, cnt),
                //GW[state_to_coor[S_tn][X]][state_to_coor[S_tn][Y]]);
        

                //update the discounted reward
            S_t = S_tn;
        }
    }
    printf("\ns21_cnt: %d, s18_cnt: %d\n", s21_cnt, s18_cnt);
    
    return ((double)s21_cnt / s18_cnt);
}


double simulate_optimal(){
        //get s0 as a start state
    int start_state = random_sample_weights(d_0, 23);
    int S_t, A_t, S_tn;
    double discounted_reward = 0.0;
    int cnt = 0;

    S_t = start_state;
    while (S_t != absorbing_state-1){// nor absorbing state
            //sample an action randomly
            //printf("Current State: %d\t", S_t);
        
    A_t = optimal_policy[S_t];
    //printf("Picked Action: %d\t", A_t);
        
            //sample the next state given
        S_tn = random_sample_weights(trans_table[S_t * NUM_ACTION + A_t], STATE_NUM+1);
    //printf("Next State: %d\n", S_tn);
        
            //Get reward
        discounted_reward += pow(dis_gamma, cnt) * GW[state_to_coor[S_tn][X]][state_to_coor[S_tn][Y]];
        cnt ++;
    //printf("discounted: %f\t Reward:%.2f\n", pow(dis_gamma, cnt),
            //GW[state_to_coor[S_tn][X]][state_to_coor[S_tn][Y]]);
        

            //update the discounted reward
        S_t = S_tn;
    }

    return discounted_reward;
    //printf("%.8f\n", discounted_reward);
}

void run_simulation_with_strategy(double (*f)()){
    printf("Episode: \n");
    REP (i, 0, NUM_EPISODES-1){
        printf("\r %d", i);
        episode_reward[i] = (*f)();
    }
    printf("\n");
}

void get_array_statistics(const double array[],const int size){
        //get mean, standard deviation, maximum, and minimum of the observed discounted returns.

    double amean, adevia, amax, amin;
    amax = *std::max_element(array,array+size);
    amin = *std::min_element(array,array+size);
    get_standard_deviation(array, size, &amean, &adevia);
    printf("Max: %.5f, Min: %.5f, Mean: %.5f, Devia: %.5f\n",
           amax, amin,
           amean, adevia);
    
}

void get_standard_deviation(const double array[],
                            const int size,
                            double *amean,
                            double *adevia){
    double sum = 0.0, mean, deviation = 0.0;

    REP(i, 0, size-1){//compute the sum of the array
        sum += array[i];
    }

        //get the mean
    mean = sum / size;
    *amean = mean;
    REP(i, 0, size-1){
        deviation += pow(array[i]-mean, 2);
    }

    *adevia = sqrt(deviation / size);
}
