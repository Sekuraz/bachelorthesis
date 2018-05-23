
#define AS 10000

#include "processor/tasking.h"
void test(int a[AS], int* p) {
    for(int i = 0; i < AS; i++) {
        #pragma //omp task untied mergeable if(i == 3) final(i == 5) depend(in: a)
Task t(7574561021973165946ull);
t.if_clause = (i == 3);
t.final = (i == 5);
t.untied = true;
t.mergeable = true;
{
	Var a_var = {"a", &(*a), at_firstprivate, 40000};
	t.vars.push_back(a_var);
}
{
	Var i_var = {"i", &(i), at_firstprivate, 4};
	t.vars.push_back(i_var);
}
{
	Var p_var = {"p", &(*p), at_firstprivate, 0};
	t.vars.push_back(p_var);
}
t.in.push_back("a");

        t.schedule();
    }
}

int main(int argc, char** argv) {

    setup_tasking();
int* a = new int[AS];
    int b[AS];
    int c = 0;
    int* p = &c;

    test(a, p);
    teardown_tasking();
return a[0];
}
