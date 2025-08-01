# Elevator API

A C++ implementation of an extensible, maintainable, and flexible elevator logic for an N-story office building.

## Structure
- `include/` — Public headers
- `src/` — Implementation files
- `tests/` — Test suite


## Design Overview
- **Elevator**: Manages elevator state, movement, requests, and supports emergency/maintenance modes.
- **Request**: Represents a floor request (with direction, priority, timestamp, and user ID).
- **Event Hooks**: Allows external code to react to elevator events (arrivals, emergencies, etc.).
- **Error Handling**: API methods return error codes for invalid or failed operations.
- **Simulation**: Time passage can be simulated in movement steps.
- **Extensible**: Easily add features (future: multi-elevator, advanced scheduling, etc.)
- **Testable**: Includes unit tests for all core logic and edge cases.


## Usage Example
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

## Contributing
Contributions are welcome! Please fork the repository, create a feature branch, and submit a pull request. For major changes, open an issue first to discuss your ideas.

## License
This project is licensed under the MIT License.

## Contact
For questions or feedback, contact Nitish Tongia or open an issue on GitHub.

## Troubleshooting & FAQ
- **Compilation errors**: Ensure you are using C++17 or newer and all required headers are included.
- **Tests failing**: Check for recent code changes or missing dependencies.
- **How do I add a new feature?** Extend the Elevator or Request classes and add tests.
- **How do I simulate time?** Use the `step(int timeMs)` method to control movement timing.