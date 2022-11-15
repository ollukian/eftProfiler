#!/usr/bin/env bash

for idx in {0..1100}
do
  find res_cHG/ "*worker*" |
  if ! grep -q worker_"${idx}"
    then echo "${idx}" >> not_found_jobs.txt
  fi
done