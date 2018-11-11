
#include "omp.h"

#include <thread>
#include <chrono>
#include <iostream>

#define AS 1000

#include <memory>

#include "tasking.h"
#include "/tmp/tasking_functions/all.hpp"
void test(int a[AS], int* p) {
    for(int i = 0; i < AS; i++) {
        std::cout << "generating task " << i << std::endl;
        #pragma //omp task untied mergeable if(i == 3) final(i == 5) depend(in: a)
auto t_813322099 = std::make_shared<Task>(813322099);
t_813322099->if_clause = (i == 3);
t_813322099->final = (i == 5);
t_813322099->untied = true;
t_813322099->mergeable = true;
{
	Var a_var = {"a", &(*a), at_firstprivate, 4000, 0};
	t_813322099->vars.emplace_back(a_var);
}
{
	Var i_var = {"i", &(i), at_firstprivate, 4, 1};
	t_813322099->vars.emplace_back(i_var);
}
{
	Var p_var = {"p", &(*p), at_firstprivate, 0, 0};
	t_813322099->vars.emplace_back(p_var);
}
t_813322099->in.emplace_back("a");

        current_task->worker->handle_create_task(t_813322099);
    }
}

int __main__1(int argc, char *argv[]) {
{

    std::cout << "hello from main" << std::endl;

    int* a = new int[AS];
    int b[AS];
    int c = 0;
    int* p = &c;

    test(a, p);
    return a[0];
}return 0;
}
void __main__(int argc, char *argv[]) {
    __main__1(argc, argv);
    current_task->worker->handle_finish_task(current_task);
}
int main(int argc, char** argv) { do_tasking(argc, argv); }
