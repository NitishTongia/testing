
#include "../include/Elevator.h"
#include <cassert>
#include <iostream>
#include <string>


void test_basic_movement() {
    Elevator elevator(5);
    assert(elevator.getCurrentFloor() == 0);
    assert(elevator.isIdle());
    assert(elevator.addRequest(Request(3, Direction::Up)) == Elevator::ErrorCode::Success);
    elevator.step(0);
    assert(elevator.getCurrentFloor() == 1);
    elevator.step(0);
    elevator.step(0);
    assert(elevator.getCurrentFloor() == 3);
    assert(elevator.isIdle());
}


void test_multiple_requests() {
    Elevator elevator(5);
    assert(elevator.addRequest(Request(2, Direction::Up)) == Elevator::ErrorCode::Success);
    assert(elevator.addRequest(Request(4, Direction::Up)) == Elevator::ErrorCode::Success);
    elevator.step(0);
    elevator.step(0);
    assert(elevator.getCurrentFloor() == 2);
    elevator.step(0);
    elevator.step(0);
    assert(elevator.getCurrentFloor() == 4);
}

void test_error_handling() {
    Elevator elevator(5);
    assert(elevator.addRequest(Request(-1, Direction::Up)) == Elevator::ErrorCode::InvalidFloor);
    assert(elevator.addRequest(Request(5, Direction::Down)) == Elevator::ErrorCode::InvalidFloor);
    assert(elevator.addRequest(Request(0, Direction::Up)) == Elevator::ErrorCode::AlreadyAtFloor);
}

void test_emergency_and_maintenance() {
    Elevator elevator(5);
    elevator.triggerEmergency();
    assert(elevator.getState() == ElevatorState::Emergency);
    assert(elevator.addRequest(Request(2, Direction::Up)) == Elevator::ErrorCode::ElevatorStopped);
    elevator.clearEmergency();
    assert(elevator.getState() == ElevatorState::Normal);
    elevator.setMaintenance(true);
    assert(elevator.getState() == ElevatorState::Maintenance);
    assert(elevator.addRequest(Request(2, Direction::Up)) == Elevator::ErrorCode::ElevatorStopped);
    elevator.setMaintenance(false);
    assert(elevator.getState() == ElevatorState::Normal);
}

void test_event_callback() {
    Elevator elevator(5);
    bool arrived = false;
    elevator.setEventCallback([&arrived](const std::string& event, int floor) {
        if (event == "Arrived" && floor == 2) arrived = true;
    });
    elevator.addRequest(Request(2, Direction::Up));
    elevator.step(0);
    elevator.step(0);
    assert(arrived);
}



void test_priority_and_timestamp() {
    Elevator elevator(5);
    Request r1(2, Direction::Up, 10, "userA");
    Request r2(4, Direction::Up, 5, "userB");
    elevator.addRequest(r1);
    elevator.addRequest(r2);
    auto reqs = elevator.getPendingRequests();
    assert(reqs.size() == 2);
    assert(reqs[0].priority == 10 || reqs[1].priority == 10);
    assert(!reqs[0].userId.empty() && !reqs[1].userId.empty());
    assert(reqs[0].timestamp <= std::chrono::system_clock::now());
}

void test_duplicate_and_rapid_state_changes() {
    Elevator elevator(5);
    elevator.addRequest(Request(2, Direction::Up));
    elevator.addRequest(Request(2, Direction::Up)); // duplicate
    auto reqs = elevator.getPendingRequests();
    assert(reqs.size() == 2); // duplicates allowed for now
    elevator.triggerEmergency();
    elevator.setMaintenance(true);
    elevator.clearEmergency();
    elevator.setMaintenance(false);
    assert(elevator.getState() == ElevatorState::Normal);
}

void test_event_hook_multiple_events() {
    Elevator elevator(5);
    int arrivedCount = 0, emergencyCount = 0;
    elevator.setEventCallback([&](const std::string& event, int floor) {
        if (event == "Arrived") arrivedCount++;
        if (event == "Emergency") emergencyCount++;
    });
    elevator.addRequest(Request(2, Direction::Up));
    elevator.step(0);
    elevator.step(0);
    elevator.triggerEmergency();
    assert(arrivedCount == 2);
    assert(emergencyCount == 1);
}

void test_simulation_timing() {
    Elevator elevator(5);
    elevator.addRequest(Request(2, Direction::Up));
    auto start = std::chrono::system_clock::now();
    elevator.step(10); // simulate 10ms
    auto end = std::chrono::system_clock::now();
    assert(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() >= 10);
}

int main() {
    test_basic_movement();
    test_multiple_requests();
    test_error_handling();
    test_emergency_and_maintenance();
    test_event_callback();
    test_priority_and_timestamp();
    test_duplicate_and_rapid_state_changes();
    test_event_hook_multiple_events();
    test_simulation_timing();
    std::cout << "All tests passed!\n";
    return 0;
}
