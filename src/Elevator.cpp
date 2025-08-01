
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
    planRoute();
    return ErrorCode::Success;
}



void Elevator::step(int timeMs) {
    if (state != ElevatorState::Normal) return;
    if (direction == Direction::Idle) {
        if (plannedRoute.empty()) return;
        // Set direction based on next step in plannedRoute
        direction = plannedRoute.front().second;
    }
    // Simulate time passage
    std::this_thread::sleep_for(std::chrono::milliseconds(timeMs));
    int prevFloor = currentFloor;
    if (!plannedRoute.empty()) {
        int nextFloor = plannedRoute.front().first;
        Direction nextDir = plannedRoute.front().second;
        // Before moving, check if we are at a terminal (top/bottom) and about to reverse direction
        bool atTerminal = false;
        if (currentFloor == nextFloor) {
            // If at terminal, fulfill requests at this floor matching the new direction
            requests.erase(std::remove_if(requests.begin(), requests.end(), [this, &nextDir](const Request& r) {
                return r.floor == currentFloor && r.direction == nextDir;
            }), requests.end());
            emitEvent("RequestFulfilled");
            plannedRoute.erase(plannedRoute.begin());
            planRoute();
            if (plannedRoute.empty()) direction = Direction::Idle;
            return;
        }
        if (nextFloor > currentFloor) {
            currentFloor++;
            direction = Direction::Up;
        } else if (nextFloor < currentFloor) {
            currentFloor--;
            direction = Direction::Down;
        }
        emitEvent("Arrived");
        log("Moved from floor " + std::to_string(prevFloor) + " to " + std::to_string(currentFloor));
        // After moving, fulfill requests at this floor if the next movement matches their direction
        if (!plannedRoute.empty() && plannedRoute.front().first == currentFloor) {
            requests.erase(std::remove_if(requests.begin(), requests.end(), [this](const Request& r) {
                return r.floor == currentFloor && r.direction == plannedRoute.front().second;
            }), requests.end());
            emitEvent("RequestFulfilled");
            plannedRoute.erase(plannedRoute.begin());
            planRoute();
        }
    }
    // If plannedRoute is empty, set direction to Idle
    if (plannedRoute.empty()) direction = Direction::Idle;
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


// updateDirection is now replaced by planRoute

void Elevator::planRoute() {
    plannedRoute.clear();
    if (requests.empty()) {
        plannedDirection = Direction::Idle;
        return;
    }
    // Build a route: up requests above, then down requests below, only stopping for requests if next movement matches their direction
    std::vector<std::pair<int, Direction>> route;
    int pos = currentFloor;
    // Find all up requests above
    std::vector<int> upFloors;
    for (const auto& r : requests) {
        if (r.direction == Direction::Up && r.floor > pos) upFloors.push_back(r.floor);
    }
    std::sort(upFloors.begin(), upFloors.end());
    for (int f : upFloors) route.push_back({f, Direction::Up});
    // After up, reverse and fulfill down requests below
    std::vector<int> downFloors;
    for (const auto& r : requests) {
        if (r.direction == Direction::Down && r.floor < pos) downFloors.push_back(r.floor);
    }
    std::sort(downFloors.rbegin(), downFloors.rend());
    for (int f : downFloors) route.push_back({f, Direction::Down});
    plannedRoute = route;
    if (!plannedRoute.empty()) plannedDirection = plannedRoute.front().second;
    else plannedDirection = Direction::Idle;
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
