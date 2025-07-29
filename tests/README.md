# Running Tests for KungFu Chess

## Setup
1. Download catch.hpp:
   ```powershell
   cd tests
   curl -o catch.hpp https://github.com/catchorg/Catch2/releases/download/v2.13.10/catch.hpp
   ```

2. Build with tests enabled:
   ```powershell
   cd build
   cmake .. -DKFC_BUILD_TESTS=ON
   cmake --build . --config Release
   ```

3. Run tests:
   ```powershell
   .\Release\kungfu_chess_tests.exe
   ```

## Test Structure
- `test_main.cpp` - Contains the main() function for Catch2
- `test_capture_rules.cpp` - Tests for CaptureRules class
- Future test files will follow the pattern `test_<classname>.cpp`

## Current Tests
### CaptureRules
- ✅ are_same_team() - Tests team detection logic
- ✅ calculate_arrival_time() - Tests timing calculations
- ✅ determine_attacker_and_victim() - Tests combat resolution

## Adding New Tests
Create new test files with:
```cpp
#include "catch.hpp"
#include "../src/YourClass.hpp"

TEST_CASE("Description") {
    SECTION("Sub-test") {
        REQUIRE(condition == expected);
    }
}
```
