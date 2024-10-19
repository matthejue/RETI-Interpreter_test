#!/usr/bin/env bash

if [[ $1 == "all" ]]; then
  paths=(./sys_test/*.reti)
elif [[ -n "$1" ]]; then
  paths=(./sys_test/*$1*.reti)
else
  paths=(./sys_test/{basic,advanced,special,example,error,tobias}*.reti)
fi

for test in "${paths[@]}"; do
  sed -n '1p' "$test" | sed -e 's/^\/\/ input://' > "${test%.reti}.input"
  sed -n '2p' "$test" | sed -e 's/^\/\/ expected://' > "${test%.reti}.expected"
done
