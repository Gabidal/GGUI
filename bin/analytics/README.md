# GGUI Analytics Tools

Gotta go fast!

## Scripts

### `benchmark.sh` - Valgrind Callgrind Profiling
Comprehensive CPU profiling using Valgrind's Callgrind tool with KCachegrind integration.

**Usage:**
```bash
./bin/analytics/benchmark.sh                 # Basic profiling with standard settings
./bin/analytics/benchmark.sh -F              # Full profiling with cache simulation
./bin/analytics/benchmark.sh --enableDRM     # Enable DRM mode for hardware acceleration
./bin/analytics/benchmark.sh --help          # Display help information
```

**Output:**
- `callgrind.out` - Profile data file
- Interactive KCachegrind session for analysis
- Optional timestamped backup files

### `benchmark2.sh` - Linux Perf Performance Analysis
Modern performance profiling using Linux perf tools with web-based pprof analysis.

**Usage:**
```bash
./bin/analytics/benchmark2.sh              # Branch misses analysis (default)
./bin/analytics/benchmark2.sh -c           # CPU cycles analysis
./bin/analytics/benchmark2.sh -i           # Instructions analysis
./bin/analytics/benchmark2.sh -a           # Comprehensive multi-event analysis
./bin/analytics/benchmark2.sh -F           # Maximum profiling with all events
./bin/analytics/benchmark2.sh --help       # Display help information
```

**Output:**
- `perf.data` - Performance data file
- Web interface at `http://localhost:8080` for analysis
- Optional timestamped backup files

### `leaks.sh` - Memory Analysis
Memory leak detection and error analysis using Valgrind's Memcheck tool.

**Usage:**
```bash
./bin/analytics/leaks.sh                   # Basic memory checks (faster)
./bin/analytics/leaks.sh -F                # Full memory analysis (comprehensive)
./bin/analytics/leaks.sh --help            # Display help information
```

#### When using `-F`, valgrind.log will report so called `on exit` memory leaks, these are ok. As long as no runtime memory leaks exist. 

**Output:**
- `valgrind.log` - Detailed memory analysis results
- Console summary with error counts and recommendations

### `time.sh` - Performance Growth Analysis
Measures instruction execution over time to detect performance degradation and memory leaks.

**Usage:**
```bash
./bin/analytics/time.sh 5 30               # Compare 5-second vs 30-second runs
./bin/analytics/time.sh 10 60              # Compare 10-second vs 60-second runs
./bin/analytics/time.sh --help             # Display help information
```

**Arguments:**
- `time_short` - Duration for short run (seconds)
- `time_long` - Duration for long run (seconds, must be > time_short)

**Output:**
- Performance metrics (slopes and growth ratio)
- Interpretation of results with recommendations

## System Requirements

### Analysis Tools
- **Valgrind** - Memory analysis and profiling
  - `valgrind` - Core tool
  - `callgrind_annotate` - Callgrind analysis
  - `kcachegrind` - Visual profile analysis
- **Linux Perf** - Performance analysis
  - `perf` - Recording and basic analysis (at `/usr/local/bin/perf`)
  - `pprof` - Web-based analysis (at `/root/go/bin/pprof`)
- **Optional Tools**
  - `perf_data_converter` - Enhanced perf analysis

### Installation Verification
```bash
# Check tool availability
valgrind --version
/usr/local/bin/perf --version
/root/go/bin/pprof --help
kcachegrind --version
```

## Quick Start

1. **Navigate to the GGUI project** (scripts auto-detect location):
   ```bash
   cd /path/to/GGUI
   ```

2. **Run basic performance analysis**:
   ```bash
   ./bin/analytics/benchmark.sh       # CPU profiling
   ./bin/analytics/leaks.sh           # Memory analysis
   ```

3. **Run comprehensive analysis**:
   ```bash
   ./bin/analytics/benchmark.sh -F    # Full CPU profiling
   ./bin/analytics/benchmark2.sh -a   # Multi-event perf analysis
   ./bin/analytics/leaks.sh -F        # Full memory analysis
   ```

4. **Performance growth analysis**:
   ```bash
   ./bin/analytics/time.sh 5 30       # 5s vs 30s comparison
   ```

## Interpreting Results

### Memory Analysis (`leaks.sh`)
- **No errors**: Clean memory usage
- **Memory leaks**: Check `valgrind.log` for leak sources
- **Buffer overflows**: Critical security issues requiring immediate attention
- **Use-after-free**: Potential crash sources

### CPU Profiling (`benchmark.sh`)
- **KCachegrind visualization**: Interactive call graph and hotspot analysis
- **High instruction counts**: Performance bottlenecks
- **Cache misses**: Memory access optimization opportunities

### Performance Analysis (`benchmark2.sh`)
- **Branch misses**: Branch prediction optimization opportunities
- **CPU cycles**: Overall computational efficiency
- **Instructions**: Algorithm efficiency analysis

### Growth Analysis (`time.sh`)
- **Ratio ≈ 0**: Stable performance (good)
- **Ratio > 0.1**: Possible memory leak or performance degradation
- **Ratio < -0.1**: Performance improvement or reduced workload
