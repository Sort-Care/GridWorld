# Run C++ Code for simulation
(Can be skipped because source file is already compiled and the output is named a.out)
> g++-7 generateInput.cpp

Then, there are three options to run the program:
> ./a.out [num]
where the [num] can be 1, 2 or 3, representing running random policy, optimal policy and estimating the required quantity, respectively.

# Python Value Iteration code
The python code requires py2.7 and numpy package.
> python2.7 gridworld.py pyInput.txt
