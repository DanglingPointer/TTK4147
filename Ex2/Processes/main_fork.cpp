/*  
 * Compile with:
 * g++-4.9 -std=c++14 -o Processes_fork main_fork.cpp
 * 
 * Run with:
 * ./Processes_fork 
 */
#include <unistd.h>
#include <iostream>

#define ITER_COUNT 50

int g_var = 0;

int main(int argc, char** argv)
{
    int loc_var = 0;

    pid_t pid = fork();
    
    if (pid == 0){
        // child
        for(auto i = 0; i < ITER_COUNT; ++i){
            ++g_var;
            ++loc_var;
            std::cout << "Child here: g_var = " << g_var << " loc_var = " << loc_var << std::endl;
        }
    }
    else{
        // parent
        for(auto i = 0; i < ITER_COUNT; ++i){
            ++g_var;
            ++loc_var;
            std::cout << "Parent here: g_var = " << g_var << " loc_var = " << loc_var << std::endl;
        }
    }


    return 0;
}

