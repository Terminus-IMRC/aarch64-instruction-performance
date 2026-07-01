# [`aarch64-instruction-performance`](https://github.com/Terminus-IMRC/aarch64-instruction-performance): Benchmark results

| Machine | Processor (microarchitecture) | Result files |
| -- | -- | -- |
| MacBook Air M4 | Apple M4 | [Apple-M4-P_Neon](results/Apple-M4-P_Neon.json), [Apple-M4-E_Neon](results/Apple-M4-E_Neon.json), [Apple-M4-P_SVE](results/Apple-M4-P_SVE.json) |
| AWS EC2 c7g | AWS Graviton3 (Arm Neoverse V1) | [AWS-Graviton3_Neon](results/AWS-Graviton3_Neon.json), [AWS-Graviton3_SVE](results/AWS-Graviton3_SVE.json) |
| AWS EC2 c7gn | AWS Graviton3E (Arm Neoverse V1) | [AWS-Graviton3E_Neon](results/AWS-Graviton3E_Neon.json), [AWS-Graviton3E_SVE](results/AWS-Graviton3E_SVE.json) |
| AWS EC2 c8g | AWS Graviton4 (Arm Neoverse V2) | [AWS-Graviton4_Neon](results/AWS-Graviton4_Neon.json), [AWS-Graviton4_SVE](results/AWS-Graviton4_SVE.json) |
| AWS EC2 c9g | AWS Graviton5 (Arm Neoverse V3) | [AWS-Graviton5_Neon](results/AWS-Graviton5_Neon.json), [AWS-Graviton5_SVE](results/AWS-Graviton5_SVE.json) |
| Azure Dpsv6 | Azure Cobalt 100 (Arm Neoverse N2) | [Azure-Cobalt-100_Neon](results/Azure-Cobalt-100_Neon.json), [Azure-Cobalt-100_SVE](results/Azure-Cobalt-100_SVE.json) |

Use the [pretty.jq](pretty.jq) script to pretty-print the JSON result files, e.g.:

```console
$ ./pretty.jq results/Azure-Cobalt-100_SVE.json
AddS    1       2
AddS    2       1
AddS    4       0.5
AddS    32      0.5
AddD    1       2
AddD    2       1
AddD    4       0.5
AddD    32      0.5
...

$ diff --side-by-side --suppress-common-lines <(./pretty.jq results/AWS-Graviton3_SVE.json) <(./pretty.jq results/AWS-Graviton3E_SVE.json) | awk '{ printf("%-15s %2d: %3.2f -> %3.2f\n", $1, $2, $3, $7) }'
AddS             4: 0.56 -> 0.50
AddS            32: 0.56 -> 0.50
AddD             4: 0.56 -> 0.50
AddD            32: 0.56 -> 0.50
FaddS            4: 0.56 -> 0.50
FaddS           32: 0.56 -> 0.50
FaddD            4: 0.56 -> 0.50
FaddD           32: 0.56 -> 0.50
FmlaS           32: 0.74 -> 0.50
FmlaSLagged      4: 0.74 -> 0.50
FmlaSLagged     16: 0.74 -> 0.50
FmlaD           32: 0.74 -> 0.50
FmlaDLagged      4: 0.74 -> 0.50
FmlaDLagged     16: 0.74 -> 0.50
```
