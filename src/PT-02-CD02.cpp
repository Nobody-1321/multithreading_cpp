#include <iostream>
#include <thread>
#include <chrono>

void background_task() {
    std::cout << "Background task started\n";
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "Background task finished\n";
}

int main() {
    std::thread t(background_task);
    t.detach();  // The thread continues running independently
    std::cout << "Main thread continues without waiting\n";
}