"""
Author: Haoyu Ji
Initial Writing Purpose: For course work
"""


import numpy as np

def read_MDP(filename):
    """
    Read in GridWorld MDP file
    Input:
        filename: filename
        
    Output:
        num_state: number of total states
        R_state: an array of size [number_state], giving rewards for each state
        transition_functions: an array of arrays, (4*num_state, num_state)
                            0 ~ |S|-1: LEFT
                            |S| ~ 2|S|-1 : UP
                            2|S| ~ 3|S|-1 : RIGHT
                            3|S| ~ 4|S|-1 : DOWN
    """
    num_state = 0
    R_state = []
    transition_functions = []

    with open(filename) as f:
        # read in the first line
        num_state = int(f.readline().strip(' \t\n\r'))
        # read in the following |S| lines describing rewards for each state
        for i in range(num_state):
            tmp_state = float(f.readline().strip(' \t\n\r'))
            R_state.append(tmp_state)
            
        # read in the following 4|S| lines describing transition mode
        # LEFT - UP - RIGHT - DOWN, each |S| lines
        for i in range(4*num_state):
            transition_functions.append([])
            next_line = f.readline().strip(' \t\n\r').split(',')
            for v in next_line:
                transition_functions[-1].append(float(v))
    
    # convert to numpy array
    R_state = np.array(R_state)
    transition_functions = np.array(transition_functions)
    return num_state, R_state, transition_functions



def value_iteration_gridworld(N, R, T, gamma = 1.0, c = 0.0001, verbose = False):
    """
    Run value iteration on a given gridworld model
    Input:
        N: a number giving number of states
        R: an array of shape (N,), giving rewards for each state
        T: (4N, N), giving transition model of the gridworld
        gamma: discount factor. Default = 1.0
        verbose: print out T/F
        c: multiplied with R_max, giving the error threshold, default is 0.01
    Output:
        U: an array of size (N,) giving utilities value
        best_actions: an array of size (N,) giving best actions for each state
        iterations_used: iterations used
    """
    U = np.zeros((N,))
    U_prime = np.zeros((N,))
    best_actions = np.zeros((N,))
    delta = 0.0
    iterations_used = 0
    R_max = np.amax(R)
    
    while True:
        iterations_used += 1
        U = np.array(U_prime)
        delta = 0.0
        # for each state, do:
        for i in range(N):
            # compute expected utilities for all actions and select the biggest one
            # compute for LEFT, UP, RIGHT, DOWN
            actions = T[(i, N+i, 2*N+i, 3*N+i),:]# (4, N)
            multi = np.multiply(actions, U)
            SEU = np.sum(multi, axis = 1)
            EU_max = np.amax(SEU)
            # best actions: 0,1,2,3 ---> LEFT, UP, RIGHT, DOWN
            best_actions[i] = np.argmax(SEU)
            U_prime[i] = R[i] + gamma * EU_max

            if abs(U[i] - U_prime[i]) > delta:
                delta = abs(U[i] - U_prime[i])
                
            if verbose == True:
                print delta
        if delta < c * R_max:
            break
    return U, best_actions, iterations_used






import sys
import os.path

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

if len(sys.argv) == 2:
    filename = sys.argv[1].strip()
    if os.path.isfile(filename):
        ns, R, T = read_MDP(filename)
        U, best_actions, itera = value_iteration_gridworld(ns,
                                                           R,
                                                           T,
                                                           gamma = 0.95 ,
                                                           verbose=False)
        print bcolors.HEADER+bcolors.BOLD + "Utilities for all states:" + bcolors.ENDC , U
        print bcolors.HEADER+bcolors.BOLD + "Best actions for all states" + bcolors.ENDC , best_actions
        print bcolors.HEADER+bcolors.BOLD + "Iterations used: "+bcolors.ENDC, itera
    else:
        print "File doesn't exist! Try another one."
elif len(sys.argv) == 3:
    filename = sys.argv[1].strip()
    gamma = float(sys.argv[2].strip())

    assert(gamma <= 1.0) , bcolors.FAIL + "Gamma value should be less than or equal to zero!" + bcolors.ENDC
    
    if os.path.isfile(filename):
        ns, R, T = read_MDP(filename)
        U, best_actions, itera = value_iteration_gridworld(ns,
                                                           R,
                                                           T,
                                                           gamma = gamma ,
                                                           verbose=False)
        print bcolors.HEADER+bcolors.BOLD + "Utilities for all states:" + bcolors.ENDC , U
        print bcolors.HEADER+bcolors.BOLD + "Best actions for all states" + bcolors.ENDC , best_actions
        print bcolors.HEADER+bcolors.BOLD + "Iterations used: "+bcolors.ENDC, itera
    else:
        print "File doesn't exist! Try another one."
elif len(sys.argv) == 4:
    filename = sys.argv[1].strip()
    gamma = float(sys.argv[2].strip())
    c = float(sys.argv[3].strip())
    
    assert(gamma <= 1.0) , bcolors.FAIL + "Gamma value should be less than or equal to zero!" + bcolors.ENDC
    assert(c > 0), bcolors.FAIL + "c value should be larger than zero!" + bcolors.ENDC
    
    if os.path.isfile(filename):
        ns, R, T = read_MDP(filename)
        U, best_actions, itera = value_iteration_gridworld(ns,
                                                           R,
                                                           T,
                                                           gamma = gamma,
                                                           c = c,
                                                           verbose=False)
        print bcolors.HEADER+bcolors.BOLD + "Utilities for all states:" + bcolors.ENDC , U
        print bcolors.HEADER+bcolors.BOLD + "Best actions for all states" + bcolors.ENDC , best_actions
        print bcolors.HEADER+bcolors.BOLD + "Iterations used: "+bcolors.ENDC, itera
    else:
        print "File doesn't exist! Try another one."
else:
    print bcolors.FAIL+bcolors.BOLD + "Wrong number of arguments! Try the following commands:" + bcolors.ENDC
    print bcolors.OKBLUE + "python2.7 gridworld.py [filename]" + bcolors.ENDC
    print bcolors.OKBLUE + "python2.7 gridworld.py [filename] [gamma(float)]" + bcolors.ENDC
    print bcolors.OKBLUE + "python2.7 gridworld.py [filename] [gamma(float)] [c(float)]" + bcolors.ENDC
