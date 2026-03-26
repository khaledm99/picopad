#!/bin/bash
set -e
git submodule update --init --recursive
cmake -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -B build
ln -sf build/compile_commands.json compile_commands.json
echo "Done. Run 'cd build && ninja' to build."
