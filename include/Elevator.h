#pragma once
#include <vector>
#include <queue>
#include <set>
#include <string>
#include <functional>
#include <chrono>
#include <fstream>

/**

Elevator API for an N-story office building.

*/

enum class Direction { Up, Down, Idle };
enum class ElevatorState { Normal, Emergency, Maintenance };

/**

@struct Request
@brief Represents a floor request for the elevator.

*/
struct Request {
    int floor; ///< Requested floor number
    Direction direction; ///< Direction of request
    int priority; ///< Priority of request (higher = more urgent)
    std::chrono::system_clock::time_point timestamp; ///< Time of request
    std::string userId; ///< Optional user identifier
    Request(int f, Direction d, int p = 0, std::string uid = "")
        : floor(f), direction(d), priority(p), userId(uid), timestamp(std::chrono::system_clock::now()) {}
};

/**

@class Elevator
@brief Models a single elevator for an N-story building.

*/
class Elevator {
public:
    /**
     * @brief Callback type for elevator events.
     * @param event Event name (e.g., "Arrived", "Emergency")
     * @param floor Current floor
     */
    using EventCallback = std::function<void(const std::string& event, int floor)>;

    /**
     * @brief Construct a new Elevator object.
     * @param numFloors Number of floors in the building
     */
    Elevator(int numFloors);

    /**
     * @enum ErrorCode
     * @brief Error codes for API operations.
     */
    enum class ErrorCode {
        Success, ///< Operation successful
        InvalidFloor, ///< Requested floor is out of range
        InvalidState, ///< Operation not allowed in current state
        AlreadyAtFloor, ///< Elevator is already at requested floor
        ElevatorStopped ///< Elevator is stopped (emergency/maintenance)
    };

    /**
     * @brief Add a floor request to the elevator.
     * @param req Request object
     * @return ErrorCode indicating result
     */
    ErrorCode addRequest(const Request& req);

    /**
     * @brief Move elevator one step, simulating time passage.
     * @param timeMs Time in milliseconds to simulate
     */
    void step(int timeMs = 1000);

    /** @brief Get the current floor. */
    int getCurrentFloor() const;
    /** @brief Get the current direction. */
    Direction getDirection() const;
    /** @brief Check if elevator is idle. */
    bool isIdle() const;
    /** @brief Get the current elevator state. */
    ElevatorState getState() const;
    /** @brief Get all pending requests. */
    std::vector<Request> getPendingRequests() const;

    /** @brief Trigger emergency stop. */
    void triggerEmergency();
    /** @brief Clear emergency stop. */
    void clearEmergency();
    /** @brief Set maintenance mode. */
    void setMaintenance(bool on);

    /**
     * @brief Set callback for elevator events.
     * @param cb Callback function
     */
    void setEventCallback(EventCallback cb);

    /**
     * @brief Enable logging to a file.
     * @param filename Log file path
     */
    void enableLogging(const std::string& filename);

private:
    int currentFloor; ///< Current floor
    Direction direction; ///< Current direction
    int numFloors; ///< Number of floors
    ElevatorState state; ///< Current state
    std::vector<Request> requests; ///< Pending requests
    EventCallback eventCallback; ///< Event callback
    std::string logFile; ///< Log file path
    std::vector<std::pair<int, Direction>> plannedRoute; ///< Sequence of (floor, direction) pairs to visit
    Direction plannedDirection = Direction::Idle; ///< Planned direction, only updated when new request is added
    void log(const std::string& message) const;
    void planRoute();
    void emitEvent(const std::string& event);
};
