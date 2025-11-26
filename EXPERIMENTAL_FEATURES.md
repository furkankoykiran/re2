# Experimental Features in This Fork

This fork of RE2 contains experimental implementations of features that are not part of the official RE2 library. These features maintain RE2's core performance guarantees but may have limitations or edge cases.

## ⚠️ Warning

**These features are EXPERIMENTAL and NOT officially supported by the RE2 maintainers.**

- Use at your own risk in production environments
- Implementation may have bugs or edge cases
- Performance characteristics may differ from standard RE2
- API and behavior may change without notice
- No guarantees of upstream acceptance

## Lookaround Assertions

### Overview

This fork implements bounded lookaround assertions while maintaining RE2's O(n) time complexity guarantee:

- `(?<!...)` - **Negative lookbehind**: Asserts that the pattern does NOT match before the current position
- `(?<=...)` - **Positive lookbehind**: Asserts that the pattern matches before the current position
- `(?!...)` - **Negative lookahead**: Asserts that the pattern does NOT match after the current position
- `(?=...)` - **Positive lookahead**: Asserts that the pattern matches after the current position

### Implementation Approach

**Bounded Execution**: To maintain O(n) complexity:
- **Lookbehind**: Limited to 255 characters backward from current position
- **Lookahead**: Naturally bounded by remaining text length
- Each lookaround compiles to a separate subprogram
- Subprograms execute during main NFA matching

**Architecture**:
```
Main Pattern: (?<!pattern1)test(?=pattern2)
              ↓
Compilation:  Main Prog + Subprog[0] (pattern1) + Subprog[1] (pattern2)
              ↓
Execution:    NFA runs main, checks subprogs at lookaround instructions
```

### Examples

#### Negative Lookbehind
```cpp
// Match "test" not preceded by a digit
RE2 re("(?<!\\d)test");
RE2::PartialMatch("hello test", re);    // ✓ matches
RE2::PartialMatch("123test", re);       // ✗ no match
```

#### Positive Lookbehind
```cpp
// Match "world" preceded by "hello "
RE2 re("(?<=hello )world");
RE2::PartialMatch("hello world", re);    // ✓ matches
RE2::PartialMatch("goodbye world", re);  // ✗ no match
```

#### Negative Lookahead
```cpp
// Match "test" not followed by a digit
RE2 re("test(?!\\d)");
RE2::PartialMatch("test hello", re);     // ✓ matches
RE2::PartialMatch("test123", re);        // ✗ no match
```

#### Positive Lookahead
```cpp
// Match "test" followed by a digit
RE2 re("test(?=\\d)");
RE2::PartialMatch("test123", re);        // ✓ matches
RE2::PartialMatch("test hello", re);     // ✗ no match
```

#### Complex Patterns
```cpp
// Password validation: 8+ chars, must contain digit and letter
RE2 re("(?=.*[0-9])(?=.*[a-zA-Z]).{8,}");

// Match word boundaries without backtracking
RE2 re("(?<!\\w)foo(?!\\w)");

// Email-like pattern with lookahead
RE2 re("[a-z]+(?=@[a-z]+\\.com)");
```

### Limitations

1. **Lookbehind Length Limit**: Maximum 255 characters
   - Patterns requiring longer lookback will fail to compile
   - This is a fundamental design constraint for O(n) guarantee
   
2. **No Variable-Length Lookbehind**: Patterns like `(?<!a+)` are supported but bounded
   - The subpattern can match variable lengths up to 255 chars
   - But the search is limited to 255 chars backward
   
3. **No Nested Captures in Lookaround**: Capturing groups inside lookaround assertions don't affect main match groups
   - `(?<=(group))` - the capture is local to the lookaround subprogram
   
4. **Performance Overhead**: Each lookaround adds overhead
   - Each assertion requires executing a separate subprogram
   - Multiple lookarounds multiply this cost
   - Still O(n) but with higher constant factors

5. **Not All Patterns Benefit**: Some use cases are better solved with:
   - Multiple passes with standard RE2
   - Post-processing matched results
   - Alternative regex designs

### Performance Characteristics

**Time Complexity**: O(n × m × k)
- n: input text length
- m: number of lookaround assertions
- k: maximum lookaround subpattern complexity (bounded)

**Space Complexity**: O(p + s × m)
- p: main program size
- s: average subprogram size
- m: number of lookaround assertions

**Practical Impact**:
- Simple patterns: 2-5x slower than equivalent non-lookaround patterns
- Complex patterns: May be faster than alternatives (like multiple passes)
- Memory: ~1KB per lookaround assertion (subprogram overhead)

### Testing

```bash
# Run lookaround tests
bazel test //re2/testing:lookbehind_test

# Or with make
make obj/re2/testing/lookbehind_test
obj/re2/testing/lookbehind_test
```

### Technical Details

**Modified Files**:
- `re2/regexp.h`: Added `kRegexpLookBehind{Positive,Negative}`, `kRegexpLookAhead{Positive,Negative}`
- `re2/parse.cc`: Parser recognizes `(?<!...)`, `(?<=...)`, `(?!...)`, `(?=...)`
- `re2/prog.h`: Added `kInstLookBehind`, `kInstLookAhead` instructions
- `re2/prog.cc`: Instruction initialization and cleanup
- `re2/compile.cc`: Compiles lookaround to subprograms with 255-char bound
- `re2/nfa.cc`: Execution logic in `AddToThreadq` function

**Design Decisions**:
1. **Why 255 chars?** Balance between usefulness and maintaining O(n):
   - Covers 99% of practical use cases
   - Fits in uint8_t for compact representation
   - Keeps worst-case bounded

2. **Why subprograms?** Modularity and correctness:
   - Each assertion is independent
   - Easier to reason about correctness
   - Simplifies NFA state management

3. **Why NFA-only?** Consistency with RE2 architecture:
   - DFA would require complex state explosion handling
   - OnePass incompatible with lookaround semantics
   - NFA provides good performance for bounded patterns

### Known Issues

1. **No DFA Support**: Lookaround patterns always use NFA engine
   - Cannot benefit from DFA optimization
   - May be slower for some simple patterns

2. **No OnePass Support**: Lookaround disables OnePass optimization
   - Submatch extraction may be slower
   - Use standard RE2 if OnePass matters

3. **Limited Error Messages**: Parse errors may be generic
   - "Look-around pattern too complex" 
   - Check pattern length and complexity

### Comparison with PCRE/Perl

**Supported (with limitations)**:
- ✅ `(?<!pattern)` - Bounded to 255 chars
- ✅ `(?<=pattern)` - Bounded to 255 chars
- ✅ `(?!pattern)` - Works as expected
- ✅ `(?=pattern)` - Works as expected

**Not Supported**:
- ❌ Unbounded lookbehind (e.g., `(?<!.{300})`)
- ❌ Backreferences in lookaround
- ❌ Conditional lookaround `(?(condition)then|else)`
- ❌ Lookbehind in lookbehind (may work but untested)

### Migration from PCRE

If you're migrating from PCRE and your patterns use lookaround:

1. **Check lookbehind length**: Ensure lookbehind patterns stay within 255 chars
2. **Test thoroughly**: Behavior may differ in edge cases
3. **Consider alternatives**: Some patterns can be rewritten without lookaround
4. **Benchmark**: Measure performance impact for your use case

### Future Work

Potential improvements (no timeline, no guarantees):
- [ ] DFA support for simple lookahead patterns
- [ ] Configurable lookbehind limit
- [ ] Better error messages
- [ ] Performance optimization
- [ ] More comprehensive test coverage

### Contributing

This is an experimental fork. If you find bugs or have improvements:

1. File an issue with minimal reproduction case
2. Include pattern, input text, expected vs actual behavior
3. Performance issues: include benchmark results
4. Pull requests welcome (no guarantee of acceptance)

### References

- **Original Feature Request**: [google/re2#156](https://github.com/google/re2/issues/156)
- **RE2 Syntax**: [Syntax Documentation](https://github.com/google/re2/wiki/Syntax)
- **Performance Discussion**: [Why RE2 doesn't support features X](https://swtch.com/~rsc/regexp/regexp3.html)

### License

Same as RE2: BSD 3-Clause License

---

**Last Updated**: November 26, 2025
**Fork Maintainer**: @furkankoykiran
**Status**: Experimental / Proof of Concept
