import argparse
import os
from elftools.elf.elffile import ELFFile
# 导入 Capstone 反汇编框架 , 用于解析 ELF 文件
from capstone import *

def main():
    parser = argparse.ArgumentParser(description='manual to this script')
    parser.add_argument("-t", type=str, default="")
    args = parser.parse_args()

    inst_num=0
    size=0

    file = open(args.t, 'rb')
    elf_file = ELFFile(file)
    for section in elf_file.iter_sections():
        

        # 使用 Capstone 反汇编框架
        if section.name == '.text' or section.name == '.text.startup':
            print('name:', section.name)
            print('header', section.header)

        # 获取节区地址
            file.seek(section.header['sh_offset'])
            # 获取节区大小
            sh_size = section.header['sh_size']
            # 读取 节区 二进制数据
            #   这是需要反汇编的机器码数据
            raw = file.read(sh_size)

            # 创建 Capstone 实例对象
            capstone = Cs(CS_ARCH_X86, CS_MODE_64)
            capstone.detail = True
            disasm = capstone.disasm(raw, 0)
            size+=sh_size
            inst_num=len(list(disasm))

            
    # 关闭文件
    file.close()
    pass
    print("Size of .text: %d in bytes" % size)
    print("Number of instructions: %d" % inst_num)
    print("Average length of instructions: %f in bytes" % (size/inst_num))


if __name__=="__main__":
    main()
