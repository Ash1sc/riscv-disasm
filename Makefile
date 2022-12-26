
cc = gcc
prom = calc
inclu = ./src
source = calc_riscv.c ./src/riscv-disas.c

$(prom): $(source)
	$(cc) -I $(inclu) $(source) -o $(prom)
	