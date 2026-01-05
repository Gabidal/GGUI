# Contributing to GGUI

Thanks for considering contributing!

## Coding style
- C++17
- 4 spaces in indentation
- K&R brace style
- Avoid global `using namespace`
- Prefer `constexpr`, `noexcept` where meaningful
- Add `const` qualification where possible
- Maintain doxygen documentation for all functions
  - if lazy, at least generate them with AI
- Avoid using abbreviations in naming, unless it is something everyone knows like `RGB`, `TCP`
- Add enough comments, so that other developers can easily follow the code structure
- Avoid std::unique_ptr/std::shared_ptr if possible
  - Objects are registered into global registries or an object tree, and are destroyed during cleanup phase.
- Avoid associative containers (`std::map`, `std::unordered_map`) in hot paths,
  especially with `std::string` or complex keys, unless justified

## Tests
Cover with test cases where possible under `./test/units/*`.

## GPG keys
Please prefer using GPG or some method of git commit verification.

## Commit messages
- Add issue number around parenthesis: `(#132)`
- Prefer imperative mood, instead of past tense
- Examples:
  - `Fix rendering bug. (#123)`
  - `Fix sigsev on exit.`
  - `Rewrite ./foo/bar/abc.cpp.`
  - `Replace UB on condition::notify_all() inside signal handlers with sigwait based cleanup thread (linux side only).`
  - `Implement new input system. (#51)`

## Questions
Open a discussion or draft PR/Issue early when needed.
