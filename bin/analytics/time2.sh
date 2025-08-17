#!/usr/bin/env bash

# Wrapper for generating two Callgrind profiles via analytics/time.sh and
# then comparing them using the C++ tool analytics/time2.cpp (built as 'time2').
# Finally, open the resulting filtered.out in KCachegrind and optionally
# clean up the inputs and ask about keeping the output.

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="${SCRIPT_DIR}/../.."
BIN_DIR="${ROOT_DIR}/bin"
BUILD_DIR="${BIN_DIR}/build"

source "${SCRIPT_DIR}/utils/common.sh"
source "${SCRIPT_DIR}/utils/valgrind.sh"

show_help() {
  echo "Usage: $(basename "$0") <short_sec> <long_sec> <threshold>"
  echo "  short_sec  - duration for first run (seconds)"
  echo "  long_sec   - duration for second run (seconds)"
  echo "  threshold  - ratio threshold (e.g., 5.0)"
}

if [[ "$#" -lt 3 || "$1" =~ ^(-h|--help)$ ]]; then
  show_help
  exit 0
fi

SHORT_SEC="$1"; shift
LONG_SEC="$1"; shift
THRESHOLD="$1"; shift

# Ensure we're in bin and build the 'time2' tool
ensure_bin_directory
if [ ! -d "${BUILD_DIR}" ]; then
  meson setup "${BUILD_DIR}" "${BIN_DIR}" || handle_error "Meson setup failed"
else
  meson setup --reconfigure "${BUILD_DIR}" "${BIN_DIR}" || handle_error "Meson reconfigure failed"
fi
meson compile -C "${BUILD_DIR}" time2 timingStanding timingBusy || handle_error "Failed to build tools"

# Generate two callgrind outputs using analytics/time.sh but keep files
STAMP="cg$(date +%Y%m%d_%H%M%S)"
RUN1="${STAMP}_standing_long.out" # we will run standing short/long and busy short/long, but for diffing we need two files
RUN2="${STAMP}_busy_long.out"

# Use time.sh to emit 4 profiles with a prefix; it will name them <prefix>_<label>.out
"${SCRIPT_DIR}/time.sh" "${SHORT_SEC}" "${LONG_SEC}" --emit-callgrind-prefix "${STAMP}" || handle_error "time.sh failed"

# time.sh created files: ${STAMP}_standing_short.out, ${STAMP}_standing_long.out, ${STAMP}_busy_short.out, ${STAMP}_busy_long.out
# We'll compare long vs long to highlight steady-state growth differences between modes, as an example.
RUN1="${STAMP}_standing_long.out"
RUN2="${STAMP}_busy_long.out"

# Run the comparison tool (from build dir)
TOOL="${BUILD_DIR}/time2"
if [[ ! -x "${TOOL}" ]]; then
  handle_error "time2 tool not found at ${TOOL}"
fi
"${TOOL}" "${RUN1}" "${RUN2}" "${THRESHOLD}" || handle_error "time2 comparison failed"

# Open the filtered file in KCachegrind
validate_tools "kcachegrind"
open_profile_in_kcachegrind "filtered.out"

# Clean up input profiles now that we're done
rm -f "${STAMP}_standing_short.out" "${STAMP}_standing_long.out" "${STAMP}_busy_short.out" "${STAMP}_busy_long.out"

# Ask to keep the filtered output
if prompt_yes_no "Keep filtered.out?" "y"; then
  echo "Keeping filtered.out"
else
  rm -f filtered.out
  echo "filtered.out removed"
fi
