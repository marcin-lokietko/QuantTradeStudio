#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh

FILES=$(find /quant-trade-studio/backend/ -type f \( -name "*.cpp" -o -name "*.hpp" \))

IS_FORMATTING_NOK=0
for FILE in $FILES; do
  if [ "$1" = "validate" ]; then
    if ! clang-format --dry-run --Werror "$FILE"; then
      echo "Invalid formatting in file: $FILE"
      IS_FORMATTING_NOK=1
    fi
  fi
  if [ "$1" = "fix" ]; then
    clang-format -i $FILE
  fi
done

exit $IS_FORMATTING_NOK
