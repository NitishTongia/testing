# Elevator API

A C++ implementation of an extensible, maintainable, and flexible elevator logic for an N-story office building.

## Structure
- `include/` — Public headers
- `src/` — Implementation files
- `tests/` — Test suite


## Design Overview
- **Optimal Route Planning**: Elevator plans a complete route for all current requests, only picks up users if the next movement matches their requested direction, and replans after each fulfillment or new request.
- **Real-World Fulfillment**: Requests are only fulfilled if the elevator actually moves in the requested direction after pickup, including reversal at terminal floors.
- **Elevator**: Manages elevator state, movement, requests, and supports emergency/maintenance modes.
- **Request**: Represents a floor request (with direction, priority, timestamp, and user ID).
- **Event Hooks**: Allows external code to react to elevator events (arrivals, emergencies, etc.).
- **Error Handling**: API methods return error codes for invalid or failed operations.
- **Simulation**: Time passage can be simulated in movement steps.
- **Extensible**: Easily add features (future: multi-elevator, advanced scheduling, etc.)
- **Testable**: Includes unit tests for all core logic and edge cases.


## Usage Example
## Real-World Scenario Example
Here's a simulation of multiple requests with up/down directions:

```cpp
#include "include/Elevator.h"
#include <iostream>

int main() {
    Elevator elevator(10); // 10-story building, starts at floor 4
    elevator.setEventCallback([](const std::string& event, int floor) {
        std::cout << "Event: " << event << " at floor " << floor << std::endl;
    });
    // Set starting floor to 4
    while (elevator.getCurrentFloor() < 4) {
        elevator.addRequest(Request(elevator.getCurrentFloor() + 1, Direction::Up));
        elevator.step(0);
    }
    // Add requests
    elevator.addRequest(Request(3, Direction::Up, 1, "userA"));
    elevator.addRequest(Request(1, Direction::Down, 1, "userB"));
    elevator.addRequest(Request(9, Direction::Down, 1, "userC"));
    elevator.addRequest(Request(7, Direction::Down, 1, "userD"));
    // Simulate elevator movement
    while (!elevator.isIdle()) {
        elevator.step(0);
    }
    return 0;
}
```

This will fulfill requests in the correct order and direction, matching real-world elevator logic.
Here's a simple example of using the Elevator API:

```cpp
#include "include/Elevator.h"
#include <iostream>

int main() {
    Elevator elevator(5); // 5-story building
    elevator.setEventCallback([](const std::string& event, int floor) {
        std::cout << "Event: " << event << " at floor " << floor << std::endl;
    });
    elevator.addRequest(Request(3, Direction::Up, 1, "userA"));
    while (!elevator.isIdle()) {
        elevator.step(0); // Simulate movement
    }
    return 0;
}
```

## Build & Test
You can build and run the tests using a C++ compiler:

```bash
# Compile
c++ -std=c++17 -Iinclude src/Elevator.cpp tests/test_elevator.cpp -o elevator_test

# Run
./elevator_test
```

## Test Suite
- Real-world scenario: up/down requests, optimal route, reversal at terminal floors
The test suite covers:
- Basic movement and request handling
- Error handling and edge cases
- Emergency and maintenance states
- Event hooks and simulation timing
- Request priority, timestamp, and user ID

If all tests pass, the implementation is correct and robust. If any test fails, review the error message and code changes.

## API Design & Extensibility
- Designed for maintainability and future expansion (multi-elevator, advanced scheduling, etc.)
- Event hooks allow integration with UIs or monitoring systems
- Error codes and state management for reliability
- Easily add new request types or elevator features

## Troubleshooting & FAQ
- **Compilation errors**: Ensure you are using C++17 or newer and all required headers are included.
- **Tests failing**: Check for recent code changes or missing dependencies.
- **How do I add a new feature?** Extend the Elevator or Request classes and add tests.
- **How do I simulate time?** Use the `step(int timeMs)` method to control movement timing.