# GitHub Copilot Refactoring Guide for C/C++

This guide provides best practices and instructions for using GitHub Copilot to refactor C/C++ code in Visual Studio Code.

## Table of Contents

1. [Overview](#overview)
2. [Prerequisites](#prerequisites)
3. [Refactoring Workflow](#refactoring-workflow)
4. [Common Refactoring Patterns](#common-refactoring-patterns)
5. [Copilot Chat Commands](#copilot-chat-commands)
6. [Best Practices](#best-practices)
7. [Examples](#examples)

---

## Overview

GitHub Copilot can help automate and accelerate C/C++ code refactoring tasks while maintaining code quality and following project standards. This guide demonstrates how to leverage Copilot effectively for various refactoring scenarios.

### Benefits of Using Copilot for Refactoring

- **Consistency**: Apply coding standards uniformly across the codebase
- **Speed**: Automate repetitive refactoring tasks
- **Safety**: Get suggestions that maintain functionality while improving structure
- **Documentation**: Generate or update documentation during refactoring
- **Testing**: Create or update tests to verify refactoring correctness

---

## Prerequisites

### Required Extensions

1. **GitHub Copilot** - AI pair programmer
2. **GitHub Copilot Chat** - Interactive AI assistant
3. **C/C++ Extension** (Microsoft) - Language support and IntelliSense

### Optional but Recommended

- **CMake Tools** - For CMake-based projects
- **Test Explorer** - For running and managing tests
- **Doxygen Documentation Generator** - For documentation generation

### Configuration

Ensure your VS Code `settings.json` includes:

```json
{
  "github.copilot.enable": {
    "*": true,
    "c": true,
    "cpp": true
  },
  "github.copilot.inlineSuggest.enable": true,
  "C_Cpp.intelliSenseEngine": "default",
  "C_Cpp.default.cStandard": "c99",
  "C_Cpp.default.cppStandard": "c++14"
}
```

---

## Refactoring Workflow

### Step 1: Analyze Current Code

Before refactoring, use Copilot Chat to analyze the code:

```
@workspace Analyze this C file for potential refactoring opportunities:
- Magic numbers that should be constants
- Long functions that should be split
- Missing error handling
- Code duplication
- Documentation gaps
```

### Step 2: Plan Refactoring

Ask Copilot to create a refactoring plan:

```
Create a refactoring plan for [filename] following these goals:
1. Extract magic numbers to named constants
2. Split functions longer than 50 lines
3. Add comprehensive error handling
4. Add Doxygen documentation
5. Improve naming conventions
```

### Step 3: Execute Refactoring

Use Copilot to implement changes incrementally:

```
Refactor this function following the project's coding standards in .github/COPILOT_INSTRUCTIONS.md
```

### Step 4: Verify Changes

After refactoring, validate the changes:

```
@workspace Review the refactored code and:
1. Verify all tests still pass
2. Check for any introduced bugs
3. Confirm coding standards compliance
4. Suggest additional improvements
```

---

## Common Refactoring Patterns

### 1. Extract Constants

**Before:**
```c
if (price > 100) {
    discount = price * 0.1;
}
```

**Copilot Prompt:**
```
Extract magic numbers to named constants following C99 standards
```

**After:**
```c
#define DISCOUNT_THRESHOLD 100
#define DISCOUNT_RATE 0.1f

if (price > DISCOUNT_THRESHOLD) {
    discount = price * DISCOUNT_RATE;
}
```

### 2. Function Extraction

**Before:**
```c
int process_data(char* data) {
    // 100+ lines of code doing multiple things
}
```

**Copilot Prompt:**
```
Split this long function into smaller, focused functions with single responsibilities
```

**After:**
```c
static int validate_input(const char* data);
static int parse_data(const char* data, ParsedData* result);
static int transform_data(ParsedData* data);
static int save_results(const ParsedData* data);

int process_data(char* data) {
    if (validate_input(data) != 0) return -1;
    ParsedData parsed;
    if (parse_data(data, &parsed) != 0) return -1;
    if (transform_data(&parsed) != 0) return -1;
    return save_results(&parsed);
}
```

### 3. Add Error Handling

**Before:**
```c
void process_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    // process file
    fclose(file);
}
```

**Copilot Prompt:**
```
Add comprehensive error handling with NULL checks and proper error messages
```

**After:**
```c
int process_file(const char* filename) {
    if (filename == NULL) {
        fprintf(stderr, "Error: filename is NULL\n");
        return -1;
    }
    
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Failed to open file '%s': %s\n", 
                filename, strerror(errno));
        return -1;
    }
    
    // process file
    
    if (fclose(file) != 0) {
        fprintf(stderr, "Error: Failed to close file '%s'\n", filename);
        return -1;
    }
    
    return 0;
}
```

### 4. Improve Naming

**Before:**
```c
int calc(int a, int b, int c) {
    int x = a * b;
    int y = x + c;
    return y;
}
```

**Copilot Prompt:**
```
Improve function and variable names to be more descriptive following snake_case convention
```

**After:**
```c
int calculate_total_price(int unit_price, int quantity, int shipping_cost) {
    int subtotal = unit_price * quantity;
    int total_price = subtotal + shipping_cost;
    return total_price;
}
```

### 5. Add Documentation

**Copilot Prompt:**
```
Add Doxygen-style documentation for this function including @file, @brief, @param, @return, and @example
```

**Result:**
```c
/**
 * @brief Calculate total price including shipping
 * 
 * Computes the total price by multiplying unit price by quantity
 * and adding the shipping cost.
 * 
 * @param unit_price Price per unit in cents
 * @param quantity Number of units to purchase
 * @param shipping_cost Shipping cost in cents
 * @return Total price in cents, or -1 on error
 * 
 * @warning All prices must be non-negative
 * 
 * @example
 * int total = calculate_total_price(1000, 5, 500);  // Returns 5500
 */
int calculate_total_price(int unit_price, int quantity, int shipping_cost);
```

### 6. Convert to Header/Implementation

**Copilot Prompt:**
```
Create a header file for this C file with proper include guards, extern "C" for C++, and Doxygen documentation
```

### 7. Modernize C Code

**Copilot Prompt:**
```
Modernize this C code to use C99 features: designated initializers, variable declarations, and inline functions
```

### 8. Memory Safety Improvements

**Before:**
```c
char* create_message(const char* name) {
    char* msg = malloc(100);
    sprintf(msg, "Hello, %s!", name);
    return msg;
}
```

**Copilot Prompt:**
```
Improve memory safety: use snprintf, check malloc return, add size parameters
```

**After:**
```c
/**
 * @brief Create a greeting message (caller must free)
 * 
 * @param name Name to include in greeting
 * @param buffer Buffer to store message
 * @param buffer_size Size of the buffer
 * @return 0 on success, -1 on error
 */
int create_message(const char* name, char* buffer, size_t buffer_size) {
    if (name == NULL || buffer == NULL || buffer_size == 0) {
        return -1;
    }
    
    int written = snprintf(buffer, buffer_size, "Hello, %s!", name);
    if (written < 0 || (size_t)written >= buffer_size) {
        fprintf(stderr, "Error: Buffer too small for message\n");
        return -1;
    }
    
    return 0;
}
```

---

## Copilot Chat Commands

### Workspace-Level Refactoring

```bash
# Analyze entire workspace for refactoring opportunities
@workspace Find all functions longer than 50 lines that should be refactored

# Find code duplication
@workspace Find duplicated code patterns across all C files

# Check coding standards compliance
@workspace Review all files for compliance with .github/COPILOT_INSTRUCTIONS.md

# Generate refactoring report
@workspace Create a report of all refactoring opportunities in the codebase
```

### File-Level Refactoring

```bash
# Refactor current file
/fix Refactor this file following project coding standards

# Add documentation to entire file
Add Doxygen documentation to all functions in this file

# Improve error handling throughout file
Add comprehensive error handling to all functions in this file

# Extract constants
Extract all magic numbers in this file to named constants
```

### Function-Level Refactoring

Select a function and use:

```bash
# Extract function
Extract this code block into a separate function with descriptive name

# Simplify complex logic
Simplify this function's logic while maintaining the same behavior

# Add parameter validation
Add input validation and error handling to this function

# Optimize performance
Suggest performance optimizations for this function
```

### Creating Tests

```bash
# Generate unit tests
Create comprehensive Google Test unit tests for this function

# Create test suite
Create a complete test suite for this module with edge cases and error scenarios

# Add test cases
Add test cases for error handling and boundary conditions
```

---

## Best Practices

### 1. Use Incremental Refactoring

- Refactor one function or module at a time
- Run tests after each change
- Commit working changes frequently

**Example workflow:**
```bash
1. Select function → Ask Copilot to refactor
2. Review suggested changes
3. Run tests: ctest or make test
4. Commit if tests pass
5. Move to next function
```

### 2. Maintain Test Coverage

Always create or update tests when refactoring:

```
Before refactoring [function_name], create unit tests that verify current behavior. 
Then refactor the function and ensure all tests still pass.
```

### 3. Follow Project Standards

Reference your project's coding standards:

```
Refactor this code following the guidelines in .github/COPILOT_INSTRUCTIONS.md
```

### 4. Use Context Effectively

Provide relevant context to Copilot:

```
Given that this project uses:
- C99 standard
- SQLite database
- CMake build system
- Google Test framework

Refactor this database access function to follow best practices
```

### 5. Verify Copilot Suggestions

Always review Copilot's suggestions:

- Check for logical errors
- Verify memory management
- Ensure error handling is correct
- Run static analysis tools
- Execute tests

### 6. Document Refactoring Decisions

Ask Copilot to explain refactoring:

```
Explain why you chose this refactoring approach and what improvements it provides
```

### 7. Handle Legacy Code Carefully

When refactoring legacy code:

```
This is legacy C code from 2005. Refactor it to modern C99 standards while:
1. Maintaining backward compatibility
2. Preserving existing behavior
3. Adding tests to verify correctness
4. Documenting any breaking changes
```

---

## Examples

### Example 1: Complete Function Refactoring

**Prompt:**
```
Refactor this function following all best practices:
1. Extract constants
2. Improve naming
3. Add error handling
4. Add documentation
5. Split into smaller functions if needed
```

**Input:**
```c
int proc(char* s, int n) {
    if (n > 100) {
        char* r = malloc(n * 2);
        strcpy(r, s);
        strcat(r, s);
        printf("%s\n", r);
        free(r);
        return 0;
    }
    return -1;
}
```

**Output:**
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_STRING_LENGTH 100
#define BUFFER_MULTIPLIER 2

/**
 * @brief Duplicate and concatenate string if it exceeds threshold
 * 
 * @param input_string String to process
 * @param string_length Length of input string
 * @return int 0 on success, -1 on error
 */
int process_and_print_doubled_string(const char* input_string, int string_length) {
    if (input_string == NULL) {
        fprintf(stderr, "Error: input_string is NULL\n");
        return -1;
    }
    
    if (string_length <= 0) {
        fprintf(stderr, "Error: Invalid string length: %d\n", string_length);
        return -1;
    }
    
    if (string_length <= MAX_STRING_LENGTH) {
        fprintf(stderr, "Error: String length %d does not exceed threshold %d\n",
                string_length, MAX_STRING_LENGTH);
        return -1;
    }
    
    size_t buffer_size = string_length * BUFFER_MULTIPLIER + 1;
    char* doubled_string = malloc(buffer_size);
    if (doubled_string == NULL) {
        fprintf(stderr, "Error: Memory allocation failed: %s\n", strerror(errno));
        return -1;
    }
    
    strncpy(doubled_string, input_string, buffer_size - 1);
    doubled_string[buffer_size - 1] = '\0';
    
    if (strlen(doubled_string) + strlen(input_string) >= buffer_size) {
        fprintf(stderr, "Error: Buffer overflow prevented\n");
        free(doubled_string);
        return -1;
    }
    
    strcat(doubled_string, input_string);
    printf("%s\n", doubled_string);
    
    free(doubled_string);
    return 0;
}
```

### Example 2: Module Restructuring

**Prompt:**
```
Restructure this monolithic C file into proper module with:
- Header file with API declarations
- Implementation file with static helper functions
- Proper documentation for all public functions
```

### Example 3: Creating Tests During Refactoring

**Prompt:**
```
I'm refactoring this function. First, create Google Test unit tests that verify its current behavior.
Then refactor the function and ensure all tests pass.
```

**Copilot will:**
1. Generate comprehensive tests for existing behavior
2. Suggest refactored version
3. Verify tests still pass with refactored code

---

## Troubleshooting

### Issue: Copilot suggests breaking changes

**Solution:**
```
Refactor this function while maintaining exact same API and behavior. 
Only improve internal implementation.
```

### Issue: Suggestions don't follow project standards

**Solution:**
```
Refactor following the exact coding standards in .github/COPILOT_INSTRUCTIONS.md.
Use snake_case, add NULL checks, and include Doxygen documentation.
```

### Issue: Unsafe memory operations suggested

**Solution:**
```
Review this refactoring for memory safety issues:
- Check all malloc/free pairs
- Verify buffer sizes
- Check for memory leaks
- Validate pointer dereferencing
```

### Issue: Complex refactoring too large

**Solution:**
Break into smaller steps:
```
Step 1: Extract constants only
Step 2: Improve naming only  
Step 3: Add error handling only
Step 4: Add documentation only
```

---

## Integration with CI/CD

### Pre-commit Checks

Use Copilot to help create pre-commit hooks:

```
Create a pre-commit hook that:
1. Runs clang-format on changed C/C++ files
2. Checks for common issues (magic numbers, missing docs)
3. Runs unit tests
4. Validates coding standards compliance
```

### Automated Refactoring Reports

```
Generate a weekly refactoring report showing:
1. Functions that need refactoring (by complexity)
2. Files with low test coverage
3. Code duplication instances
4. Coding standards violations
```

---

## Advanced Techniques

### 1. Batch Refactoring

Use workspace search and replace with Copilot:

```
@workspace Find all instances of pattern [X] and refactor them to pattern [Y]
```

### 2. Refactoring with Context from Multiple Files

```
@workspace Considering the entire codebase, refactor this function to be consistent 
with similar functions in other modules
```

### 3. Cross-Language Refactoring

```
Refactor this C code to work with the existing C++ test framework while maintaining 
C compatibility using extern "C"
```

### 4. Performance-Oriented Refactoring

```
Refactor this function for better performance:
- Reduce memory allocations
- Minimize string copying
- Use more efficient algorithms
- Add inline hints for hot paths
```

---

## Keyboard Shortcuts

Essential VS Code shortcuts for refactoring with Copilot:

- `Ctrl+I` - Open Copilot Chat inline
- `Ctrl+Shift+I` - Open Copilot Chat panel
- `F2` - Rename symbol (works with Copilot suggestions)
- `Ctrl+.` - Quick fix (Copilot will suggest fixes)
- `Alt+/` - Trigger inline Copilot suggestion
- `Tab` - Accept Copilot suggestion
- `Esc` - Dismiss Copilot suggestion

---

## Additional Resources

### Official Documentation

- [GitHub Copilot Documentation](https://docs.github.com/en/copilot)
- [VS Code C/C++ Extension](https://code.visualstudio.com/docs/languages/cpp)
- [Doxygen Documentation](https://www.doxygen.nl/)

### Project-Specific Guides

- [Project Coding Standards](.github/COPILOT_INSTRUCTIONS.md)
- [Build Guide](../task03/BUILD_GUIDE.md)
- [Testing Guide](../task03/docs/test-style.md)

### C/C++ Best Practices

- [SEI CERT C Coding Standard](https://wiki.sei.cmu.edu/confluence/display/c/SEI+CERT+C+Coding+Standard)
- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [Modern C Best Practices](https://modernc.gforge.inria.fr/)

---

## Feedback and Improvements

This guide is a living document. When you discover new effective refactoring patterns with Copilot:

1. Document the pattern
2. Add examples
3. Share with the team
4. Update this guide

**Last Updated:** 2025-11-29
**Version:** 1.0.0
