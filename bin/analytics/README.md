# GGUI Analytics Tools

#### Note that some scripts have positional arguments whereas others don't, i haven't made my mind yet on which one to use only.

#### You can add the build type into each script to test different build types and their optimization impacts.
**Example:**
```bash
./bin/analytics/benchmark.sh profile         # build types: debug, release, profile
```

## Scripts

### `benchmark.sh` - Callgrind Profiling
Run benchmark.sh with sudo if you are in WSL2, otherwise the kcachegrind will not work (Not positional)

**Usage:**
```bash
./bin/analytics/benchmark.sh -F              # All callgrind flags on for most detailed info
./bin/analytics/benchmark.sh --enableDRM     # Enable DRM mode, if you are testing with GGDirect
```

### `benchmark2.sh` - Linux Perf Performance Analysis
Modern performance profiling using Linux perf tools with web-based pprof analysis. (Not positional)

**Usage:**
```bash
./bin/analytics/benchmark2.sh              # Branch misses analysis (default)
./bin/analytics/benchmark2.sh -c           # CPU cycles analysis
./bin/analytics/benchmark2.sh -i           # Instructions analysis
./bin/analytics/benchmark2.sh -a           # Comprehensive multi-event analysis
./bin/analytics/benchmark2.sh -F           # Maximum profiling with all events
```

### `leaks.sh` - Memory Analysis
Memory leak detection and error analysis using Valgrind's Memcheck tool. (Not positional)

**Usage:**
```bash
./bin/analytics/leaks.sh                   # Basic memory checks (faster)
./bin/analytics/leaks.sh -F                # Full memory analysis (comprehensive)
```

#### When using `-F`, valgrind.log will report so called `on exit` memory leaks, these are ok. As long as no runtime memory leaks exist. 

### `time.sh` - Performance Growth Analysis
Measures instruction execution over time to detect performance degradation and memory leaks. (Positional!)

**Usage:**
```bash
./bin/analytics/time.sh 5 30               # Compare 5-second vs 30-second runs
```

### `time2.sh` - time.sh but advanced
Runs same programs as time.sh, but then outputs an callgrind file which contains only the functions that grew over the threshold of the average grow ratio over time. (WIP) (Positional!)

**Usage:**
```bash
./bin/analytics/time2.sh 3 10               # Compare 5-second vs 30-second runs
```

## List of used tools:
### Analysis Tools
- **Main**
  - `valgrind` - Core tool
  - `callgrind_annotate` - Callgrind analysis
  - `kcachegrind` - Visual profile analysis
- **additional (optionals)**
   - **Linux Perf**
     - `perf` - Recording and basic analysis
     - `pprof` - Web-based analysis
     - `perf_data_converter`

## Interpreting Results

### Growth Analysis (`time.sh`)
- **Ratio ~1**: Stable performance per time growth
- **Ratio >= 1.5**: Opcode explosion on long time runs
- **Ratio < 1**: GGUI is optimizing workloads

   #### Time Growth Analysis (`<GGUI_0.1.8.3> time.sh 3 10`)
   - **debug**:
      ```
      Standing growth ratio (long vs short): .2055341138
      Busy growth ratio (long vs short):     1.0100357923
      Relative (busy/standing) ratio:        4.9142002445
      ```
   - **profile**:
      ```
      Standing growth ratio (long vs short): .0041735486
      Busy growth ratio (long vs short):     .9528843190
      Relative (busy/standing) ratio:        228.3151366681
      ```
   - **release**:
      ```
      Standing growth ratio (long vs short): .0055511702
      Busy growth ratio (long vs short):     .9700304118
      Relative (busy/standing) ratio:        174.7434102813
      ```