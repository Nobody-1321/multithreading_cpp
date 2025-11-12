#include <iostream>
#include <thread>

void hello() {
    std::cout << "Hello Concurrent World\n";
}

int main() {
    std::thread t(hello);  // Launches a new thread
    t.join();              // Waits for the thread to finish
}