#!/usr/bin/env python3

import argparse
import os
import sys
import subprocess

def asm_label(label, ltype = "object"):
    def decorator(function):
        def wrapper(*args, **kwargs):
            print("\t.global\t" + label)
            print("\t.type\t" + label + ", %" + ltype)
            print(label + ":")
            result = function(*args, **kwargs)
            print("\t.size\t" + label + ", .-" + label)
            print("")
            return result
        return wrapper
    return decorator

def gen_file():
    print("\t.syntax\tunified")
    #print("\t.cpu\tcortex-m4")
    #print("\t.fpu\tsoftvfp")
    #print("\t.thumb")
    print("")
    print("\t.macro\tgensym addr size flags")
    print("\t.word\t\\addr")
    print("\t.word\t(\\size & 0xFFFFFF) | (\\flags << 24)")
    print("\t.endm")
    print("")
    print(".section\t.dbgsym, \"a\", %progbits")
    print("")
    return True

@asm_label("dbgsym_header")
def gen_header(symbols, flags = 0, checksum = 0):
    print("\t.asciz\t\"DBG\"\t/* Magic Number */")
    print("\t.word\t" + str(len(symbols)) + "\t/* Number of Symbols */")
    print("\t.word\t" + str(flags) + "\t/* Global Flags / Features */")
    print("\t.word\t" + str(checksum) + "\t/* Checksum */")
    return True

@asm_label("dbgsym_symdata")
def gen_symdata(symbols):
    for s in symbols:
        flag = 0

        if s["scope"] == "g":
            flag = flag | 0x80
        if s["weak"] == "w":
            flag = flag | 0x40
        if s["ctor"] == "C":
            flag = flag | 0x20
        if s["warn"] == "W":
            flag = flag | 0x10

        if s["hidden"] == "h":
            flag = flag | 0x08
        if s["type"] == "F":
            flag = flag | 0x01

        # print("\tgensym\t" + hex(s["off"]) + ", " + hex(s["size"]) + ", " + hex(flag))
        print(f"\tgensym\t0x{s['off']:08x}, 0x{s['size']:08x}, 0x{flag:02x}\t/* {s['name']} */")
    return True

@asm_label("dbgsym_symnames")
def gen_symnames(symbols):
    for s in symbols:
        print("\t.asciz\t\"" + s["name"] + "\"")
    return True

def filter_duplicates(symbols):
    result = []
    for s in symbols:
        # Always include global/local symbols
        if s['scope'] == 'l' or s['scope'] == 'g':
            result.append(s)
            continue

        duplicates = [ds for ds in symbols if ds['off'] == s['off']]
        if len(duplicates) == 1:
            result.append(s)
            continue

        foundStronger = False
        for ds in duplicates:
            if ds['weak'] != 'w':
                foundStronger = True
                break

        if foundStronger:
            continue

        result.append(s)

    return result

def load_mcu_svd(mcu):
    try:
        from cmsis_svd.parser import SVDParser
    except:
        # No symbols available
        print(f"/* Unable to load SVD parser module. */")
        return []

    try:
        parser = SVDParser.for_mcu(mcu)
    except:
        print(f"/* Unable to find hardware description for MCU {mcu}. */")
        return []

    if parser is None:
        print(f"/* Unable to find hardware description for MCU {mcu}: No such device. */")
        return []
    d = parser.get_device()
    if d is None:
        print(f"/* Unable to find hardware description for MCU {mcu}: No device? */")
        return []

    result = []
    for peripheral in d.peripherals:
        # Generate pseudo-symbols for this device

        l_name = peripheral.name

        l_off = peripheral.base_address

        addrblock = peripheral._address_block
        derived = peripheral.get_derived_from()
        derived_addrblock = derived._address_block if derived is not None else None

        if addrblock is not None:
            l_size = addrblock.size
        elif derived_addrblock is not None:
            l_size = derived_addrblock.size
        elif peripheral._size is not None:
            l_size = peripheral._size / 8
        elif derived._size is not None:
            l_size = derived._size / 8
        else:
            l_size = 4

        l_flags = "g    dO"
        l_section = ".hw"
        l_scope, l_bind, l_ctor, l_warn, l_indirect, l_dbg, l_type = l_flags
        l_hidden = ' '

        result.append({'off': l_off, 'size': l_size, 'flags': l_flags, 'scope': l_scope, 'weak': l_bind, 'ctor': l_ctor, 'warn': l_warn, 'indirect': l_indirect, 'hidden': l_hidden, 'dbg': l_dbg, 'type': l_type, 'section': l_section, 'name': l_name})

    return result

def load_symbols(infile):
    procenv = os.environ.copy()
    procenv['LC_ALL'] = 'C.UTF-8'

    result = []

    proc = subprocess.Popen(['objdump', '-t', '-w', infile], env=procenv, stdout=subprocess.PIPE)
    for line in proc.stdout:
        l = line.decode('utf-8').rstrip()
        if '\t' not in l:
            continue

        l_p1, l_p2 = l.split('\t', 1)
        l_off, l_flags = l_p1.split(' ', 1)
        l_section = l_flags[8:]
        l_flags = l_flags[:7]
        if ' ' in l_p2:
            l_size, l_name = l_p2.split(' ', 1)
        else:
            l_size, l_name = l_p2, ""

        l_scope, l_bind, l_ctor, l_warn, l_indirect, l_dbg, l_type = l_flags

        l_hidden = ' '
        if ".hidden " == l_name[0:8]:
            l_hidden = 'h'
            l_name = l_name[8:]

        l_off = int(l_off, base=16)
        l_size = int(l_size, base=16)

        if ' ' == l_type:
            continue
        if 'f' == l_type:
            continue

        if 0 == l_size:
            continue

        result.append({'off': l_off, 'size': l_size, 'flags': l_flags, 'scope': l_scope, 'weak': l_bind, 'ctor': l_ctor, 'warn': l_warn, 'indirect': l_indirect, 'hidden': l_hidden, 'dbg': l_dbg, 'type': l_type, 'section': l_section, 'name': l_name})

    return result

if __name__ == "__main__":
    argp = argparse.ArgumentParser(description="Debug Symbol Generator")
    argp.add_argument("--mcu", help="MCU to generate symbols for")
    argp.add_argument("-i", "--infile", help="The file to generate symbol information for")
    argp.add_argument("-o", "--outfile", help="The file to write symbol information to")
    argp.add_argument("--dump-util", default="objdump", help="The executable used to dump the symbol table")
    args = argp.parse_args()

    stdout = sys.stdout
    dataout = None
    if args.outfile != "-":
        dataout = open(args.outfile, "w")
        sys.stdout = dataout

    st_sym = load_symbols(args.infile)

    hw_sym = load_mcu_svd("STM32L496ZGTx")
    for hw in hw_sym:
        st_sym.append(hw)

    st_sym = filter_duplicates(st_sym)

    st_sym = sorted(st_sym, key=lambda s: s["off"])

    # print(st_sym)

    gen_file()
    gen_header(st_sym)
    gen_symdata(st_sym)
    gen_symnames(st_sym)

    if dataout is not None:
        sys.stdout = stdout
        dataout.close()
