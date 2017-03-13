[![Build Status](https://travis-ci.org/MidnightCommercial/cpp_project.svg?branch=single-proj)](https://travis-ci.org/MidnightCommercial/cpp_project)

# Skipping a CI build 
If you don’t want to run a build for a particular commit any reason add `[ci skip]` or `[skip ci]` to the git commit message.
Commits that have `[ci skip]` or `[skip ci]` anywhere in the commit messages are ignored by Travis CI.