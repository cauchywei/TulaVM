#include <gtest/gtest.h>
#include <atomic>
#include <chrono>

using namespace std;

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();


//    atomic_int a(1);
//    auto start = chrono::system_clock::now();
//    int c;
//    for (int i = 0; i < 1000; ++i) {
//        c = atomic_load_explicit(&a, memory_order::memory_order_relaxed);
//    }
//    auto atomic_store_time = chrono::duration_cast<chrono::nanoseconds >(chrono::system_clock::now() - start);
//    cout << "atomic store time:" << atomic_store_time.count() << endl;
//    int b = 1;
//    start = chrono::system_clock::now();
//    for (int j = 0; j < 1000; ++j) {
//        c = b;
//    }
//    atomic_store_time = chrono::duration_cast<chrono::nanoseconds >(chrono::system_clock::now() - start);
//    cout << "non-atomic store time:" << atomic_store_time.count() << endl;
//    return 0;
}