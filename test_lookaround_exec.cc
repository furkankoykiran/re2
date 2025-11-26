// Full execution test for lookaround implementation
#include <iostream>
#include "re2/re2.h"

int main() {
  std::cout << "=== RE2 Lookaround Execution Tests ===\n\n";
  
  // Test 1: Negative lookbehind - basic
  {
    std::cout << "Test 1: Negative lookbehind (?<!\\d)test\n";
    RE2 re("(?<!\\d)test");
    if (!re.ok()) {
      std::cout << "  ✗ Pattern failed to compile: " << re.error() << "\n";
    } else {
      std::cout << "  ✓ Pattern compiled successfully\n";
      
      // Should match (no digit before 'test')
      if (RE2::PartialMatch("hello test", re)) {
        std::cout << "  ✓ Matched 'hello test' (expected)\n";
      } else {
        std::cout << "  ✗ Failed to match 'hello test'\n";
      }
      
      // Should NOT match (digit before 'test')
      if (!RE2::PartialMatch("123test", re)) {
        std::cout << "  ✓ Did not match '123test' (expected)\n";
      } else {
        std::cout << "  ✗ Incorrectly matched '123test'\n";
      }
    }
    std::cout << "\n";
  }
  
  // Test 2: Positive lookbehind
  {
    std::cout << "Test 2: Positive lookbehind (?<=hello )world\n";
    RE2 re("(?<=hello )world");
    if (!re.ok()) {
      std::cout << "  ✗ Pattern failed to compile: " << re.error() << "\n";
    } else {
      std::cout << "  ✓ Pattern compiled successfully\n";
      
      // Should match
      if (RE2::PartialMatch("hello world", re)) {
        std::cout << "  ✓ Matched 'hello world' (expected)\n";
      } else {
        std::cout << "  ✗ Failed to match 'hello world'\n";
      }
      
      // Should NOT match
      if (!RE2::PartialMatch("goodbye world", re)) {
        std::cout << "  ✓ Did not match 'goodbye world' (expected)\n";
      } else {
        std::cout << "  ✗ Incorrectly matched 'goodbye world'\n";
      }
    }
    std::cout << "\n";
  }
  
  // Test 3: Negative lookahead
  {
    std::cout << "Test 3: Negative lookahead test(?!\\d)\n";
    RE2 re("test(?!\\d)");
    if (!re.ok()) {
      std::cout << "  ✗ Pattern failed to compile: " << re.error() << "\n";
    } else {
      std::cout << "  ✓ Pattern compiled successfully\n";
      
      // Should match
      if (RE2::PartialMatch("test hello", re)) {
        std::cout << "  ✓ Matched 'test hello' (expected)\n";
      } else {
        std::cout << "  ✗ Failed to match 'test hello'\n";
      }
      
      // Should NOT match
      if (!RE2::PartialMatch("test123", re)) {
        std::cout << "  ✓ Did not match 'test123' (expected)\n";
      } else {
        std::cout << "  ✗ Incorrectly matched 'test123'\n";
      }
    }
    std::cout << "\n";
  }
  
  // Test 4: Positive lookahead
  {
    std::cout << "Test 4: Positive lookahead test(?=\\d)\n";
    RE2 re("test(?=\\d)");
    if (!re.ok()) {
      std::cout << "  ✗ Pattern failed to compile: " << re.error() << "\n";
    } else {
      std::cout << "  ✓ Pattern compiled successfully\n";
      
      // Should match
      if (RE2::PartialMatch("test123", re)) {
        std::cout << "  ✓ Matched 'test123' (expected)\n";
      } else {
        std::cout << "  ✗ Failed to match 'test123'\n";
      }
      
      // Should NOT match
      if (!RE2::PartialMatch("test hello", re)) {
        std::cout << "  ✓ Did not match 'test hello' (expected)\n";
      } else {
        std::cout << "  ✗ Incorrectly matched 'test hello'\n";
      }
    }
    std::cout << "\n";
  }
  
  std::cout << "=== All tests completed ===\n";
  return 0;
}
