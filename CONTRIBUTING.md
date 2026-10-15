# Contributing to TwinArray Sort

Thank you for your interest in contributing.

## Scope

This repository is intended to remain a reproducible C++20 reference
implementation of the original TwinArray Sort algorithm published in
*Electronics* 15(3), 609 (2026):

https://doi.org/10.3390/electronics15030609

Contributions are welcome when they preserve the published algorithmic
architecture and input domain.

## Welcome Contributions

- bug fixes and edge-case safety improvements;
- compiler-portability fixes;
- additional deterministic or randomised tests;
- build-system and CI improvements;
- documentation corrections;
- benchmark-harness improvements that preserve methodological transparency.

## Out of Scope

Changes that alter the underlying published algorithm or replace its defining
dual-store/conditional architecture belong in separate work rather than this
reference repository.

Unverified performance claims and fabricated benchmark numbers will not be
accepted.

## Submitting Changes

1. Create a topic branch from `main`.
2. Build with C++20 and the repository's warning settings.
3. Run the complete test suite with CTest.
4. Explain the purpose and scope of the change in the pull request.
5. Contributions accepted into this repository are distributed under
   Apache License 2.0.
