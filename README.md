<p align="left">
    <img src="./rtd/_static/logo_mini.png" height="80px" />
</p>

# Fork Information

*This is a fork of [Operon](https://github.com/heal-research/operon) for my personal use. Use the original upstream repository instead.*

# Modern C++ framework for Symbolic Regression

[![License](https://img.shields.io/github/license/heal-research/operon?style=flat)](https://github.com/heal-research/operon/blob/master/LICENSE)
[![build-linux](https://github.com/heal-research/operon/actions/workflows/build-linux.yml/badge.svg)](https://github.com/heal-research/operon/actions/workflows/build-linux.yml)
[![build-windows](https://github.com/heal-research/operon/actions/workflows/build-windows.yml/badge.svg)](https://github.com/heal-research/operon/actions/workflows/build-windows.yml)
[![Documentation Status](https://readthedocs.org/projects/operongp/badge/?version=latest)](https://operongp.readthedocs.io/en/latest/?badge=latest)
[![Gitter chat](https://badges.gitter.im/operongp/gitter.png)](https://gitter.im/operongp/community)

*Operon* is a modern C++ framework for [symbolic regression](https://en.wikipedia.org/wiki/Symbolic_regression) that uses [genetic programming](https://en.wikipedia.org/wiki/Genetic_programming) to explore a hypothesis space of possible mathematical expressions in order to find the best-fitting model for a given [regression target](https://en.wikipedia.org/wiki/Regression_analysis).
Its main purpose is to help develop accurate and interpretable white-box models in the area of [system identification](https://en.wikipedia.org/wiki/System_identification). More in-depth documentation available at https://operongp.readthedocs.io/.

## How does it work?

Broadly speaking, genetic programming (GP) is said to evolve a population of "computer programs" ― [AST](https://en.wikipedia.org/wiki/Abstract_syntax_tree)-like structures encoding behavior for a given problem domain ― following the principles of [natural selection](https://en.wikipedia.org/wiki/Natural_selection). It repeatedly combines random program parts keeping only the best results ― the "fittest". Here, the biological concept of [fitness](https://en.wikipedia.org/wiki/Survival_of_the_fittest) is defined as a measure of a program's ability to solve a certain task.

In symbolic regression, the programs represent mathematical expressions typically encoded as [expression trees](https://en.wikipedia.org/wiki/Binary_expression_tree). Fitness is usually defined as [goodness of fit](https://en.wikipedia.org/wiki/Goodness_of_fit) between the dependent variable and the prediction of a tree-encoded model. Iterative selection of best-scoring models followed by random recombination leads naturally to a self-improving process that is able to uncover patterns in the data:

<p align="center">
    <img src="./rtd/_static/evo.gif"  />
</p>

# Build instructions

The project requires CMake and a C++17 compliant compiler. The recommended way to build Operon is via either [nix](https://github.com/NixOS/nix) or [vcpkg](https://github.com/microsoft/vcpkg).

### Required dependencies
- [ceres-solver](http://ceres-solver.org/)
- [eigen](http://eigen.tuxfamily.org)
- [fast-float](https://github.com/fastfloat/fast_float)
- [pratt-parser](https://github.com/foolnotion/pratt-parser-calculator)
- [rapidcsv](https://github.com/d99kris/rapidcsv)
- [robin-hood-hashing](https://github.com/martinus/robin-hood-hashing)
* [scnlib](https://github.com/eliaskosunen/scnlib)
- [span-lite](https://github.com/martinmoene/span-lite)
- [taskflow](https://taskflow.github.io/)
- [vectorclass](https://github.com/vectorclass/version2)
- [vstat](https://github.com/heal-research/vstat)
- [xxhash](https://github.com/Cyan4973/xxHash)
- [{fmt}](https://fmt.dev/latest/index.html)

### Optional dependencies

- [cxxopts](https://github.com/jarro2783/cxxopts) required for the cli app.
- [doctest](https://github.com/onqtam/doctest) required for unit tests.
- [nanobench](https://github.com/martinus/nanobench) required for unit tests.

### Build options
The following options can be passed to CMake:
| Option                      | Description |
|:----------------------------|:------------|
| `-DCERES_TINY_SOLVER=ON` | Use the very small and self-contained tiny solver from the Ceres suite for solving non-linear least squares problem. |
| `-DUSE_SINGLE_PRECISION=ON` | Perform model evaluation using floats (single precision) instead of doubles. Great for reducing runtime, might not be appropriate for all purposes.           |
| `-DUSE_OPENLIBM=ON`         | Link against Julia's openlibm, a high performance mathematical library (recommended to improve consistency across compilers and operating systems).            |
| `-DBUILD_TESTS=ON` | Build the unit tests. |
| `-DBUILD_PYBIND=ON` | Build the Python bindings. |
| `-DUSE_JEMALLOC=ON`         | Link against [jemalloc](http://jemalloc.net/), a general purpose `malloc(3)` implementation that emphasizes fragmentation avoidance and scalable concurrency support (mutually exclusive with `tcmalloc`).           |
| `-DUSE_TCMALLOC=ON`         | Link against [tcmalloc](https://google.github.io/tcmalloc/) (thread-caching malloc), a `malloc(3)` implementation that reduces lock contention for multi-threaded programs (mutually exclusive with `jemalloc`).          |
| `-DUSE_MIMALLOC=ON`         | Link against [mimalloc](https://github.com/microsoft/mimalloc) a compact general purpose `malloc(3)` implementation with excellent performance (mutually exclusive with `jemalloc` or `tcmalloc`).          |

# Publications

If you find _Operon_ useful you can cite our work as:
```
@inproceedings{10.1145/3377929.3398099,
    author = {Burlacu, Bogdan and Kronberger, Gabriel and Kommenda, Michael},
    title = {Operon C++: An Efficient Genetic Programming Framework for Symbolic Regression},
    year = {2020},
    isbn = {9781450371278},
    publisher = {Association for Computing Machinery},
    address = {New York, NY, USA},
    url = {https://doi.org/10.1145/3377929.3398099},
    doi = {10.1145/3377929.3398099},
    booktitle = {Proceedings of the 2020 Genetic and Evolutionary Computation Conference Companion},
    pages = {1562–1570},
    numpages = {9},
    keywords = {symbolic regression, genetic programming, C++},
    location = {Canc\'{u}n, Mexico},
    series = {GECCO '20}
}
```

_Operon_ was also featured in a recent survey of symbolic regression methods, where it showed good results:

```
@misc{lacava2021contemporary,
      title={Contemporary Symbolic Regression Methods and their Relative Performance}, 
      author={William La Cava and Patryk Orzechowski and Bogdan Burlacu and Fabrício Olivetti de França and Marco Virgolin and Ying Jin and Michael Kommenda and Jason H. Moore},
      year={2021},
      eprint={2107.14351},
      archivePrefix={arXiv},
      primaryClass={cs.NE}
}
```

