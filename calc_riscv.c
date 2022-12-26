#include <stdio.h>
#include <memory.h>
#include <stddef.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <elf.h>

#include "riscv-disas.h"

void dissassemble(uint64_t pc, const uint8_t *data, size_t data_len, size_t *inst_num)
{
    char buf[128] = { 0 };
    size_t offset = 0, inst_len;
    rv_inst inst;
    while (offset < data_len)
    {
        inst_fetch(data + offset, &inst, &inst_len);
        disasm_inst(buf, sizeof(buf), rv64, pc + offset, inst);
        // printf("0x%" PRIx64 ":  %s\n", pc + offset, buf);
        offset += inst_len;
        (*inst_num)++;
    }
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("invalid arguments\n");
		exit(0);
	}

	// 打开文件
	FILE *fp;
	fp = fopen(argv[1], "r");
	if (NULL == fp)
	{
		printf("fail to open the file\n");
		exit(0);
	}

	// 解析head
	Elf64_Ehdr elf_head;
	int shnum, a;

	// 读取 head 到elf_head
	a = fread(&elf_head, sizeof(Elf64_Ehdr), 1, fp); 
	if (0 == a)
	{
		printf("fail to read head\n");
		exit(0);
	}

	// 判断elf文件类型
	if (elf_head.e_ident[0] != 0x7F ||
		elf_head.e_ident[1] != 'E' ||
		elf_head.e_ident[2] != 'L' ||
		elf_head.e_ident[3] != 'F')
	{
		printf("Not a ELF file\n");
		exit(0);
	}

	// 解析section 分配内存 section * 数量
	Elf64_Shdr *shdr = (Elf64_Shdr*)malloc(sizeof(Elf64_Shdr) * elf_head.e_shnum);
	if (NULL == shdr)
	{
		printf("shdr malloc failed\n");
		exit(0);
	}

	// 设置fp偏移量 offset，e_shoff含义
	a = fseek(fp, elf_head.e_shoff, SEEK_SET); //fseek调整指针的位置，采用参考位置+偏移量
	if (0 != a)
	{
		printf("\nfaile to fseek\n");
		exit(0);
	}

	// 读取section 到 shdr, 大小为shdr * 数量
	a = fread(shdr, sizeof(Elf64_Shdr) * elf_head.e_shnum, 1, fp);
	if (0 == a)
	{
		printf("\nfail to read section\n");
		exit(0);
	}

	// 重置指针位置到文件流开头
	rewind(fp);

	// 将fp指针移到 字符串表偏移位置处
	fseek(fp, shdr[elf_head.e_shstrndx].sh_offset, SEEK_SET);

	// 第e_shstrndx项是字符串表 定义 字节 长度 char类型 数组
	char shstrtab[shdr[elf_head.e_shstrndx].sh_size];
	char *temp = shstrtab;

	// 读取内容
	a = fread(shstrtab, shdr[elf_head.e_shstrndx].sh_size, 1, fp);
	if (0 == a)
	{
		printf("\nfaile to read\n");
	}
    
	// 遍历
	for (int i = 0; i < elf_head.e_shnum; i++)
	{
		temp = shstrtab;
		temp = temp + shdr[i].sh_name;
        if (strcmp(temp, ".text") != 0) 
        {
            continue;
        }

		printf("Section name:   %s\n", temp);
		printf("Section offset: 0x%x\n", shdr[i].sh_offset);
		printf("Seciton size:   0x%x\n", shdr[i].sh_size);
        
		// 依据此段在文件中的偏移读取出
        uint8_t *sign_data=(uint8_t*)malloc(sizeof(uint8_t)*shdr[i].sh_size);
		fseek(fp, shdr[i].sh_offset, SEEK_SET);
		fread(sign_data, sizeof(uint8_t)*shdr[i].sh_size, 1, fp);

        int inst_num=0;
        dissassemble(0, sign_data, shdr[i].sh_size, &inst_num);

        printf("Size of .text: %d in bytes\n", shdr[i].sh_size);
        printf("Number of instructions: %d\n", inst_num);
        printf("Average length of instructions: %f in bytes\n", (double)(shdr[i].sh_size)/(inst_num));
    
	}

}
