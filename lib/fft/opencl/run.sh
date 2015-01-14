#!/bin/sh

../../../lib/power/k20power/K20power ./fft 128 >> log
../../../lib/power/k20power/K20power ./fft 256 >> log
../../../lib/power/k20power/K20power ./fft 512 >> log
../../../lib/power/k20power/K20power ./fft 1024 >> log
../../../lib/power/k20power/K20power ./fft 2048 >> log

sed -n '/Elapsed/p' log >&1
