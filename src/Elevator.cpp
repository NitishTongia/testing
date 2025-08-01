
#include "Elevator.h"
#include <algorithm>
#include <iostream>
#include <thread>



Elevator::Elevator(int numFloors)
    : currentFloor(0), direction(Direction::Idle), numFloors(numFloors), state(ElevatorState::Normal), logFile("") {}
void Elevator::enableLogging(const std::string& filename) {
    logFile = filename;
    log("Logging started.");
}

void Elevator::log(const std::string& message) const {
    if (!logFile.empty()) {
        std::ofstream ofs(logFile, std::ios::app);
        if (ofs) {
            ofs << message << std::endl;
        }
    }
}



Elevator::ErrorCode Elevator::addRequest(const Request& req) {
    if (state != ElevatorState::Normal) return ErrorCode::ElevatorStopped;
    if (req.floor < 0 || req.floor >= numFloors) return ErrorCode::InvalidFloor;
    if (req.floor == currentFloor) return ErrorCode::AlreadyAtFloor;
    requests.push_back(req);
    log("Request added: floor=" + std::to_string(req.floor) + ", direction=" + std::to_string(static_cast<int>(req.direction)) + ", priority=" + std::to_string(req.priority) + ", userId=" + req.userId);
    updateDirection();
    return ErrorCode::Success;
}



void Elevator::step(int timeMs) {
    if (state != ElevatorState::Normal) return;
    if (direction == Direction::Idle) {
        updateDirection();
        return;
    }
    // Simulate time passage
    std::this_thread::sleep_for(std::chrono::milliseconds(timeMs));
    int prevFloor = currentFloor;
    if (direction == Direction::Up) {
        currentFloor++;
        emitEvent("Arrived");
    } else if (direction == Direction::Down) {
        currentFloor--;
        emitEvent("Arrived");
    }
    log("Moved from floor " + std::to_string(prevFloor) + " to " + std::to_string(currentFloor));
    // Remove fulfilled requests
    requests.erase(std::remove_if(requests.begin(), requests.end(), [this](const Request& r) {
        return r.floor == currentFloor;
    }), requests.end());
    updateDirection();
}


int Elevator::getCurrentFloor() const {
    return currentFloor;
}


Direction Elevator::getDirection() const {
    return direction;
}


bool Elevator::isIdle() const {
    return direction == Direction::Idle;
}


std::vector<Request> Elevator::getPendingRequests() const {
    return requests;
}


void Elevator::updateDirection() {
    if (requests.empty()) {
        direction = Direction::Idle;
        return;
    }
    // Simple: go to nearest request
    auto nearest = std::min_element(requests.begin(), requests.end(), [this](const Request& a, const Request& b) {
        return std::abs(a.floor - currentFloor) < std::abs(b.floor - currentFloor);
    });
    if (nearest->floor > currentFloor) direction = Direction::Up;
    else if (nearest->floor < currentFloor) direction = Direction::Down;
    else direction = Direction::Idle;
}

ElevatorState Elevator::getState() const {
    return state;
}


void Elevator::triggerEmergency() {
    state = ElevatorState::Emergency;
    log("Emergency triggered at floor " + std::to_string(currentFloor));
    emitEvent("Emergency");
}


void Elevator::clearEmergency() {
    state = ElevatorState::Normal;
    log("Emergency cleared at floor " + std::to_string(currentFloor));
    emitEvent("EmergencyCleared");
}


void Elevator::setMaintenance(bool on) {
    state = on ? ElevatorState::Maintenance : ElevatorState::Normal;
    log(std::string("Maintenance mode ") + (on ? "enabled" : "disabled") + " at floor " + std::to_string(currentFloor));
    emitEvent(on ? "MaintenanceOn" : "MaintenanceOff");
}

void Elevator::setEventCallback(EventCallback cb) {
    eventCallback = cb;
}

void Elevator::emitEvent(const std::string& event) {
    if (eventCallback) eventCallback(event, currentFloor);
}
