#include <iostream>
#include <thread>
#include <chrono>

void task(std::stop_token st) {
    while (!st.stop_requested()) {
        std::cout << "Working...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    std::cout << "Canceled\n";
}

int main() {
    std::jthread t(task);  // thread with cancellation support
    std::this_thread::sleep_for(std::chrono::seconds(2));
    t.request_stop();      // request stop
}