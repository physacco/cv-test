#!/bin/bash

set -e

if [ "$1" == "" ]; then
  echo "Usage: $0 input_file"
  exit 1
fi

astyle --style=java --indent=spaces=2 --unpad-paren --convert-tabs $1
