#include <thread>
#include <chrono>

#define AS 1000

void test(int a[AS], int* p) {
    for(int i = 0; i < AS; i++) {
        #pragma omp task untied mergeable if(i == 3) final(i == 5) depend(in: a)
        {
            a[i] = i + *p;
            // simulate a non trivial task
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

int main(int argc, char** argv) {
    int* a = new int[AS];
    int c = 0;
    int* p = &c;

    test(a, p);
    return a[0];
}
