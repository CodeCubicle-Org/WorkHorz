#!/bin/bash

set -euo pipefail
set -x
trap 'echo "Error occurred at line $LINENO while executing: $BASH_COMMAND"; exit 1' ERR

echo "Starting the vcpkg startup script..."

# Find the full path to vcpkg
VCPKG_PATH="/usr/local/vcpkg/vcpkg"

if [ -z "$VCPKG_PATH" ]; then
  echo "Error: vcpkg is not installed or not in PATH."
  exit 5
fi
echo "Found vcpkg at: $VCPKG_PATH"

# Extract the directory from the path
VCPKG_DIR=$(dirname "$VCPKG_PATH")

if [ ! -d "$VCPKG_DIR" ]; then
  echo "Error: vcpkg directory '$VCPKG_DIR' not found!"
  exit 11
fi
echo "Switching to vcpkg directory: $VCPKG_DIR"
cd "$VCPKG_DIR"

if [ ! -f "./bootstrap-vcpkg.sh" ]; then
  echo "Error: bootstrap-vcpkg.sh not found!"
  exit 111
fi

echo "Running bootstrap-vcpkg.sh..."
./bootstrap-vcpkg.sh

if [ ! -d ".git" ]; then
  echo "Error: Not a Git repository!"
  exit 1111
fi

echo "Updating vcpkg repository..."
git config --global --add safe.directory "$VCPKG_DIR"
git pull

echo "Everything in the startup script ran successfully!"
