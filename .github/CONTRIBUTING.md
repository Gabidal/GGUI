# Contributing to GGUI

Thanks for considering contributing!

## Quick Start
```bash
./bin/init.sh
./bin/test.sh
./bin/analytics/benchmark.sh    # For testing your code in main.cpp and see how it performs.
```

## Coding Style
- C++17
- 4 spaces
- K&R Indentation style
- Keep includes grouped: standard, third-party (if any), project-local
- Avoid global `using namespace`; internal-only within functions or small scopes
- Prefer `constexpr`, `noexcept`, `[[nodiscard]]` where meaningful
- Cover user accessible API's with doxygen comments
- Avoid using abbreviations
- Add enough comments, so that other developers can easily follow the code structure.

## Tests
Unit tests live under `test/`. Add focused tests for new logic.

## Commit Messages
Conventional-ish style encouraged:
`feat: add XYZ`, `fix: correct crash in renderer`, `chore: update CI`.

## Questions
Open a discussion or draft PR early if unsure.

## Releases
For larger releases FixCom and DMC is required to comb through all changes for proper release note creation.
