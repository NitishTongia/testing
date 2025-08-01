#include "../include/Elevator.h"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

void printBuilding(int numFloors, int elevatorFloor) {
    for (int f = numFloors - 1; f >= 0; --f) {
        std::cout << "[" << f << "] ";
        if (f == elevatorFloor) std::cout << "<E>";
        std::cout << std::endl;
    }
    std::cout << "-------------------" << std::endl;
}

int main() {
    int numFloors = 5;
    Elevator elevator(numFloors);
    elevator.setEventCallback([](const std::string& event, int floor) {
        std::cout << "Event: " << event << " at floor " << floor << std::endl;
    });
    elevator.addRequest(Request(3, Direction::Up, 1, "userA"));
    elevator.addRequest(Request(1, Direction::Down, 2, "userB"));
    while (!elevator.isIdle()) {
        printBuilding(numFloors, elevator.getCurrentFloor());
        elevator.step(500); // 500ms per step for visualization
    }
    printBuilding(numFloors, elevator.getCurrentFloor());
    std::cout << "Elevator is idle." << std::endl;
    return 0;
}
