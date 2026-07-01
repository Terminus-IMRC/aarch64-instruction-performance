#!/usr/bin/env jq -r -f

.benchmarks[]
| [
    (.name | split("/")[0]),
    (.group_size | round),
    (.cycles_per_instruction * 100 | round / 100)
  ]
| @tsv
