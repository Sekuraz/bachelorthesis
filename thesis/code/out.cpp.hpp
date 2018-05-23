#ifndef __test_cpp__
#define __test_cpp__
#ifndef __TASKING_FUNCTION_MAP_GUARD__
#define __TASKING_FUNCTION_MAP_GUARD__
#include <map>
std::map<unsigned long, void (*)(void **)> tasking_function_map;
#endif

void x_7574561021973165946 (void* arguments[]) {
    void * a_pointer_1 = arguments[0];
    void * a_pointer_0 = &(a_pointer_1);
    int * a = *((int **) a_pointer_0);
    void * i_pointer_0 = arguments[1];
    int i = *((int*) i_pointer_0);
    void * p_pointer_1 = arguments[2];
    void * p_pointer_0 = &(p_pointer_1);
    int * p = *((int **) p_pointer_0);
    {
            a[i] = i + *p;
        }
}
int setup_7574561021973165946() {
    tasking_function_map[7574561021973165946ull] = &x_7574561021973165946;
    return 1;
}

int tasking_setup_7574561021973165946 = setup_7574561021973165946();
#endif
