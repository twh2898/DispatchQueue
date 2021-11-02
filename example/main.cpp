#include <DispatchQueue.hpp>
#include <chrono>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;

int main() {
    DispatchQueue q(DispatchQueue::Concurrent);
    for (int i = 0; i < 10; i++) {
        q.add([i]() {
            std::cout << "Task " << i << std::endl;
            std::this_thread::sleep_for(500ms);
        });
    }

    q.join();

    return 0;
}
