# Pre-Publication Checklist

This file is for the private review stage. Remove it before the repository is
made public if you do not want the checklist included in the public release.

- [ ] Confirm the software copyright holder and authority to release the code
      under Apache-2.0, including any applicable university/employer IP policy.
- [ ] Confirm `Copyright 2026 Amin Amini` in source headers and `NOTICE`, or
      replace it with the correct rights holder.
- [ ] Verify the paper title, journal metadata and DOI.
- [ ] Validate `CITATION.cff` and check GitHub's generated citation.
- [ ] Confirm all CI jobs pass.
- [ ] Run the test suite locally in Release and sanitiser builds.
- [ ] Run the benchmark with several valid configurations and with `runs=0`
      to confirm invalid CLI parameters fail cleanly.
- [ ] Confirm the repository contains no material from later TwinArray work
      beyond the scope of the 2026 Electronics publication.
- [ ] Review README wording and benchmark caveats.
- [ ] When ready to release, update `CHANGELOG.md` from `[Unreleased]` to
      `[1.0.0] - YYYY-MM-DD`.
- [ ] Add `date-released` to `CITATION.cff` when the release date is fixed.
- [ ] Tag `v1.0.0` only after the public-release review is complete.
