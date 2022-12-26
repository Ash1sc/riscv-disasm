# riscv-disasm

本来打算用capstone直接做一个多平台的disassembler, 但是刚支持riscv,有些指令capstone仍然无法识别
## Calc_x86 Config
```
capstone==5.0.0rc2
```

## Build Instructions

```
for riscv: 
    make calc
    ./calc PATH_OF_THE_ELF

for x86_64:
    python3 calc_x86.py -t=PATH_OF_THE_ELF
```

## calc_x86 result
![x86 result](/img/result_x86.png)
## calc_x86 result
![riscv result](/img/result_riscv.png)
