#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <windows.h>

#include "libelf.h"
#include "common.h"
#include "Language.h"



#define DEFINE_MACHINE(x)   {x,     #x}

typedef struct tagEM_NAME{
    Elf32_Half              em_type;
    char *                  em_name;
}EM_NAME;

static EM_NAME machine[] = {
    DEFINE_MACHINE(EM_NONE), /*    0    /* No machine */
    DEFINE_MACHINE(EM_M32), /*    1 /* AT&T WE 32100 */
    DEFINE_MACHINE(EM_SPARC), /*  2  /* SPARC */
    DEFINE_MACHINE(EM_386), /*    3 /* Intel 80386 */
    DEFINE_MACHINE(EM_68K), /*    4 /* Motorola 68000 */
    DEFINE_MACHINE(EM_88K), /*    5 /* Motorola 88000 */
    DEFINE_MACHINE(EM_486), /*    6 /* Intel i486 (DO NOT USE THIS ONE) */
    DEFINE_MACHINE(EM_860), /*    7 /* Intel 80860 */
    DEFINE_MACHINE(EM_MIPS), /*    8    /* MIPS I Architecture */
    DEFINE_MACHINE(EM_S370), /*    9    /* IBM System/370 Processor */
    DEFINE_MACHINE(EM_MIPS_RS3_LE), /*    10    /* MIPS RS3000 Little-endian */
    DEFINE_MACHINE(EM_SPARC64), /*    11    /* SPARC 64-bit */
    DEFINE_MACHINE(EM_PARISC), /*    15 /* Hewlett-Packard PA-RISC */
    DEFINE_MACHINE(EM_VPP500), /*    17 /* Fujitsu VPP500 */
    DEFINE_MACHINE(EM_SPARC32PLUS), /*    18    /* Enhanced instruction set SPARC */
    DEFINE_MACHINE(EM_960), /*    19    /* Intel 80960 */
    DEFINE_MACHINE(EM_PPC), /*    20    /* PowerPC */
    DEFINE_MACHINE(EM_PPC64), /*    21  /* 64-bit PowerPC */
    DEFINE_MACHINE(EM_S390), /*    22   /* IBM System/390 Processor */
    DEFINE_MACHINE(EM_V800), /*    36   /* NEC V800 */
    DEFINE_MACHINE(EM_FR20), /*    37   /* Fujitsu FR20 */
    DEFINE_MACHINE(EM_RH32), /*    38   /* TRW RH-32 */
    DEFINE_MACHINE(EM_RCE), /*    39    /* Motorola RCE */
    DEFINE_MACHINE(EM_ARM), /*    40    /* Advanced RISC Machines ARM */
    DEFINE_MACHINE(EM_ALPHA), /*    41  /* Digital Alpha */
    DEFINE_MACHINE(EM_SH), /*    42 /* Hitachi SH */
    DEFINE_MACHINE(EM_SPARCV9), /*    43    /* SPARC Version 9 */
    DEFINE_MACHINE(EM_TRICORE), /*    44    /* Siemens TriCore embedded processor */
    DEFINE_MACHINE(EM_ARC), /*    45    /* Argonaut RISC Core, Argonaut Technologies Inc. */
    DEFINE_MACHINE(EM_H8_300), /*    46 /* Hitachi H8/300 */
    DEFINE_MACHINE(EM_H8_300H), /*    47    /* Hitachi H8/300H */
    DEFINE_MACHINE(EM_H8S), /*    48    /* Hitachi H8S */
    DEFINE_MACHINE(EM_H8_500), /*    49 /* Hitachi H8/500 */
    DEFINE_MACHINE(EM_IA_64), /*    50  /* Intel IA-64 processor architecture */
    DEFINE_MACHINE(EM_MIPS_X), /*    51 /* Stanford MIPS-X */
    DEFINE_MACHINE(EM_COLDFIRE), /*    52   /* Motorola ColdFire */
    DEFINE_MACHINE(EM_68HC12), /*    53 /* Motorola M68HC12 */
    DEFINE_MACHINE(EM_MMA), /*    54    /* Fujitsu MMA Multimedia Accelerator */
    DEFINE_MACHINE(EM_PCP), /*    55    /* Siemens PCP */
    DEFINE_MACHINE(EM_NCPU), /*    56   /* Sony nCPU embedded RISC processor */
    DEFINE_MACHINE(EM_NDR1), /*    57   /* Denso NDR1 microprocessor */
    DEFINE_MACHINE(EM_STARCORE), /*    58   /* Motorola Star*Core processor */
    DEFINE_MACHINE(EM_ME16), /*    59   /* Toyota ME16 processor */
    DEFINE_MACHINE(EM_ST100), /*    60  /* STMicroelectronics ST100 processor */
    DEFINE_MACHINE(EM_TINYJ), /*    61  /* Advanced Logic Corp. TinyJ embedded processor family */
    DEFINE_MACHINE(EM_X86_64), /*    62 /* AMD x86-64 architecture */
    DEFINE_MACHINE(EM_PDSP), /*    63   /* Sony DSP Processor */
    DEFINE_MACHINE(EM_FX66), /*    66   /* Siemens FX66 microcontroller */
    DEFINE_MACHINE(EM_ST9PLUS), /*    67    /* STMicroelectronics ST9+ 8/16 bit microcontroller */
    DEFINE_MACHINE(EM_ST7), /*    68    /* STMicroelectronics ST7 8-bit microcontroller */
    DEFINE_MACHINE(EM_68HC16), /*    69 /* Motorola MC68HC16 Microcontroller */
    DEFINE_MACHINE(EM_68HC11), /*    70 /* Motorola MC68HC11 Microcontroller */
    DEFINE_MACHINE(EM_68HC08), /*    71 /* Motorola MC68HC08 Microcontroller */
    DEFINE_MACHINE(EM_68HC05), /*    72 /* Motorola MC68HC05 Microcontroller */
    DEFINE_MACHINE(EM_SVX), /*    73    /* Silicon Graphics SVx */
    DEFINE_MACHINE(EM_ST19), /*    74   /* STMicroelectronics ST19 8-bit microcontroller */
    DEFINE_MACHINE(EM_VAX), /*    75    /* Digital VAX */
    DEFINE_MACHINE(EM_CRIS), /*    76   /* Axis Communications 32-bit embedded processor */
    DEFINE_MACHINE(EM_JAVELIN), /*    77    /* Infineon Technologies 32-bit embedded processor */
    DEFINE_MACHINE(EM_FIREPATH), /*    78   /* Element 14 64-bit DSP Processor */
    DEFINE_MACHINE(EM_ZSP), /*    79    /* LSI Logic 16-bit DSP Processor */
    DEFINE_MACHINE(EM_MMIX), /*    80   /* Donald Knuth's educational 64-bit processor */
    DEFINE_MACHINE(EM_HUANY), /*    81  /* Harvard University machine-independent object files */
    DEFINE_MACHINE(EM_PRISM), /*    82  /* SiTera Prism */
    DEFINE_MACHINE(EM_AVR), /*    83    /* Atmel AVR 8-bit microcontroller */
    DEFINE_MACHINE(EM_FR30), /*    84   /* Fujitsu FR30 */
    DEFINE_MACHINE(EM_D10V), /*    85   /* Mitsubishi D10V */
    DEFINE_MACHINE(EM_D30V), /*    86   /* Mitsubishi D30V */
    DEFINE_MACHINE(EM_V850), /*    87   /* NEC v850 */
    DEFINE_MACHINE(EM_M32R), /*    88   /* Mitsubishi M32R */
    DEFINE_MACHINE(EM_MN10300), /*    89    /* Matsushita MN10300 */
    DEFINE_MACHINE(EM_MN10200), /*    90    /* Matsushita MN10200 */
    DEFINE_MACHINE(EM_PJ), /*    91 /* picoJava */
    DEFINE_MACHINE(EM_OPENRISC), /*    92   /* OpenRISC 32-bit embedded processor */
    DEFINE_MACHINE(EM_ARC_A5), /*    93 /* ARC Cores Tangent-A5 */
    DEFINE_MACHINE(EM_XTENSA), /*    94 /* Tensilica Xtensa Architecture */
    DEFINE_MACHINE(EM_VIDEOCORE), /*    95  /* Alphamosaic VideoCore processor */
    DEFINE_MACHINE(EM_TMM_GPP), /*    96    /* Thompson Multimedia General Purpose Processor */
    DEFINE_MACHINE(EM_NS32K), /*    97  /* National Semiconductor 32000 series */
    DEFINE_MACHINE(EM_TPC), /*    98    /* Tenor Network TPC processor */
    DEFINE_MACHINE(EM_SNP1K), /*    99  /* Trebia SNP 1000 processor */
    DEFINE_MACHINE(EM_ST200), /*    100 /* STMicroelectronics (www.st.com) ST200 microcontroller */
    DEFINE_MACHINE(EM_IP2K), /*    101  /* Ubicom IP2xxx microcontroller family */
    DEFINE_MACHINE(EM_MAX), /*    102   /* MAX Processor */
    DEFINE_MACHINE(EM_CR), /*    103    /* National Semiconductor CompactRISC microprocessor */
    DEFINE_MACHINE(EM_F2MC16), /*    104    /* Fujitsu F2MC16 */
    DEFINE_MACHINE(EM_MSP430), /*    105    /* Texas Instruments embedded microcontroller msp430 */
    DEFINE_MACHINE(EM_BLACKFIN), /*    106  /* Analog Devices Blackfin (DSP) processor */
    DEFINE_MACHINE(EM_SE_C33), /*    107    /* S1C33 Family of Seiko Epson processors */
    DEFINE_MACHINE(EM_SEP), /*    108   /* Sharp embedded microprocessor */
    DEFINE_MACHINE(EM_ARCA), /*    109  /* Arca RISC Microprocessor */
    DEFINE_MACHINE(EM_UNICORE), /*    110   /* Microprocessor series from PKU-Unity Ltd. and MPRC of Peking University */
    DEFINE_MACHINE(EM_NUM), /*    111*/
};



static char * ehdr_type_string(Elf32_Half e_type)
{
    switch(e_type)
    {
    case ET_NONE:
        return "ET_NONE";
    case ET_REL:
        return "ET_REL";
    case ET_EXEC:
        return "ET_EXEC";
    case ET_DYN:
        return "ET_DYN";
    case ET_CORE:
        return "ET_CORE";
    case ET_NUM:
        return "ET_NUM";
    case ET_LOOS:
        return "ET_LOOS";
    case ET_HIOS:
        return "ET_HIOS";
    case ET_LOPROC:
        return "ET_LOPROC";
    case ET_HIPROC:
        return "ET_HIPROC";
    }

    return "ET_UNKNOW";
}



static char * ehdr_machine_string(Elf32_Half e_machine)
{
    int i = 0;

    for (i=0; i<sizeof(machine)/sizeof(machine[0]); i++)
    {
        if (e_machine == machine[i].em_type)
        {
            return machine[i].em_name;
        }
    }

    return "EM_UNKNOW";
}


static void show_ehdr(Elf32_Ehdr * Ehdr, size_t Length)
{
    printf(MSG_EHDRINFOR_SHOW);
    printf(MSG_FILELENGTH_SHOW, Length);
    printf(MSG_EHDRMAGIC_SHOW, Ehdr->e_ident[0] & 0xff, Ehdr->e_ident[1] & 0xff,
                Ehdr->e_ident[2] & 0xff, Ehdr->e_ident[3] & 0xff);
    printf(MSG_EHDRETYPE_SHOW, ehdr_type_string(Ehdr->e_type));
    printf(MSG_EHDRMACHINE_SHOW, ehdr_machine_string(Ehdr->e_machine));
    printf(MSG_EHDRVERSION_SHOW, Ehdr->e_version);
    printf(MSG_EHDRENTRY_SHOW, Ehdr->e_entry);
    printf(MSG_EHDREFLAGS_SHOW, Ehdr->e_flags & 0xffff);
    printf(MSG_EHDRPHDROFF_SHOW, Ehdr->e_phoff);
    printf(MSG_EHDRSHDROFF_SHOW, Ehdr->e_shoff);
    printf(MSG_EHDREFLAGS_SHOW, Ehdr->e_flags);
    printf(MSG_EHDREHSIZE_SHOW, Ehdr->e_ehsize);
    printf(MSG_EHDRPHSIZE_SHOW, Ehdr->e_phentsize);
    printf(MSG_EHDRPHNUM_SHOW, Ehdr->e_phnum);
    printf(MSG_EHDRSHSIZE_SHOW, Ehdr->e_shentsize);
    printf(MSG_EHDRSHNUM_SHOW, Ehdr->e_shnum);
    printf(MSG_EHDRSHSTR_SHOW, Ehdr->e_shstrndx);
}



ELFABI * ELF_OpenFile(LPSTR lpFileName)
{
    int fd;
    size_t Length = 0;
    Elf * Elf = NULL;
    char * Buffer = NULL;
    ELFABI * Elfabi = NULL;
    Elf32_Ehdr * Ehdr = NULL;

    if (NULL == lpFileName)
    {
        printf (MSG_NULLPOINTER_FAIL, "lpFileName");
        return NULL;
    }

    elf_version(EV_CURRENT);

    fd = open(lpFileName, O_RDONLY | O_BINARY);

    if (fd < 0)
    {
        printf(MSG_FAILED_OPEN, lpFileName);
        return NULL;
    }

    Elf = elf_begin(fd, ELF_C_READ, NULL);

    if (NULL == Elf)
    {
        
        printf(MSG_ELFBEGIN_FAIL, lpFileName);
        close(fd);
        return NULL;
    }

    Ehdr = elf32_getehdr(Elf);

    if (NULL == Ehdr)
    {
        printf(MSG_EHDRINVALID_FAIL);
        elf_end(Elf);
        close(fd);
        return NULL;
    }

    Buffer = elf_rawfile(Elf, &Length);

    if (NULL == Buffer)
    {
        printf(MSG_ELFBUFFER_FAIL, lpFileName);
        elf_end(Elf);
        close(fd);
        return NULL;
    }

    show_ehdr(Ehdr, Length);

    Elfabi = malloc(sizeof(ELFABI));

    if (NULL == Elfabi)
    {
        printf(MSG_MEMALLOC_FAIL, sizeof(ELFABI));
        elf_end(Elf);
        close(fd);
        return NULL;
    }

    Elfabi->fd = fd;
    Elfabi->Elf = Elf;
    Elfabi->PhdrCount = Ehdr->e_phnum;
    Elfabi->Length = Length;
    Elfabi->Buffer = Buffer;

    return Elfabi;
}

BOOL ELF_CloseFile(ELFABI * Elfabi)
{
    if (NULL == Elfabi)
    {
        return FALSE;
    }

    if (Elfabi->Elf)
    {
        elf_end(Elfabi->Elf);
    }

    if (Elfabi->fd >= 0)
    {
        close(Elfabi->fd);
    }

    free(Elfabi);

    return TRUE;
}



static char * phdr_type_string(Elf32_Word p_type)
{
    switch(p_type)
    {
    case PT_NULL:
        return "PT_NULL";
    case PT_LOAD:
        return "PT_LOAD";
    case PT_DYNAMIC:
        return "PT_DYNAMIC";
    case PT_INTERP:
        return "PT_INTERP";
    case PT_NOTE:
        return "PT_NOTE";
    case PT_SHLIB:
        return "PT_SHLIB";
    case PT_PHDR:
        return "PT_PHDR";
    case PT_TLS:
        return "PT_TLS";
    case PT_NUM:
        return "PT_NUM";
    case PT_LOOS:
        return "PT_LOOS";
    case PT_HIOS:
        return "PT_HIOS";
    case PT_LOPROC:
        return "PT_LOPROC";
    case PT_HIPROC:
        return "PT_HIPROC";
    default:
        return "PT_UNKNOW";
    }

}

#if 0
    printf("p_type: %X\n",Phdr-> p_type);
    printf("p_offset: %08X\n",Phdr-> p_offset);
    printf("p_vaddr: %08X\n",Phdr-> p_vaddr);
    printf("p_paddr: %08X\n",Phdr-> p_paddr);
    printf("p_filesz: %d\n",Phdr-> p_filesz);
    printf("p_memsz: %d\n",Phdr-> p_memsz);
    printf("p_flags: %x\n",Phdr-> p_flags);
    printf("p_align: %x\n",Phdr-> p_align);
#endif

static void show_phdr(Elf32_Phdr * Phdr)
{
    printf("%-12s ", phdr_type_string(Phdr->p_type));
    printf("%8d  ", Phdr-> p_offset);
    printf("%08X  ", Phdr-> p_vaddr);
    printf("%8d  ", Phdr-> p_filesz);
    printf("%8d  ", Phdr-> p_memsz);
    printf("%08X  ", Phdr-> p_flags);
    printf("%08X\n", Phdr-> p_align);
}




BOOL ELF_LoadSection(ELFABI * Elfabi, ELF_SECTION * lpSection, UINT index)
{
    Elf32_Ehdr * Ehdr = NULL;
    Elf32_Phdr * Phdr = NULL;

    if (NULL == Elfabi)
    {
        printf(MSG_NULLPOINTER_FAIL, "Elfabi");
        return FALSE;
    }

    if (NULL == lpSection)
    {
        printf(MSG_NULLPOINTER_FAIL, "lpSection");
        return FALSE;
    }

    if (index >= Elfabi->PhdrCount)
    {
        printf(MSG_PHDROUTRANGE_FAIL, index, Elfabi->PhdrCount-1);
        return FALSE;
    }

    Ehdr = elf32_getehdr(Elfabi->Elf);

    if (NULL == Ehdr)
    {
        printf(MSG_EHDRINVALID_FAIL);
        return FALSE;
    }

    Phdr = (void *) (Elfabi->Buffer + Ehdr->e_phoff + Ehdr->e_phentsize * index);

    show_phdr(Phdr);

    lpSection->Address  =   Phdr-> p_vaddr;
    lpSection->SectLen  =   Phdr-> p_memsz;
    lpSection->Buffer   =   Elfabi->Buffer + Phdr-> p_offset;
    lpSection->DataLen  =   Phdr-> p_filesz;

    return TRUE;

}



#if 0



static BOOL Elf_FindStrtab(Elf * Elf, size_t * Section)
{
    size_t idx = 0;
    Elf_Scn * Scn = NULL;
    Elf32_Shdr * Shdr = NULL;

    for(idx=0;; idx++)
    {
        Scn = elf_getscn(Elf, idx);

        if (NULL == Scn)
        {
            break;
        }

        Shdr = elf32_getshdr(Scn);

        if (NULL == Shdr)
        {
            break;
        }

        if (Shdr->sh_type != SHT_STRTAB)
        {
            continue;
        }

        *Section = elf_ndxscn(Scn);

        return TRUE;
    }

    return FALSE;
}


BOOL Elf_LoadSection(ELFABI * Elfabi, ELF_SECTION * Section, char * lpSecName)
{
    size_t Strtab = -1;
    Elf_Scn * Scn = NULL;
    Elf32_Shdr * Shdr = NULL;
    char * Name = NULL;

    unsigned int idx = 0;

    if (NULL == Section)
    {
        return FALSE;
    }

    if (TRUE != Elf_FindStrtab(Elfabi->Elf, &Strtab))
    {
        return FALSE;
    }

    for(idx=0;;idx++)
    {
        Scn = elf_getscn(Elfabi->Elf, idx);

        if (NULL == Scn)
        {
            break;
        }

        Shdr = elf32_getshdr(Scn);

        if (NULL == Shdr)
        {
            break;
        }

        Name = elf_strptr(Elfabi->Elf, Strtab, Shdr->sh_name);

        printf("section: %-18s    0x%08x     0x%08x    0x%08x\n", Name, Shdr->sh_type, Shdr->sh_addr, Shdr->sh_size);

    }

    return TRUE;
}

UINT ELF_GetProgramHeaders(ELFABI * Elfabi)
{
    char * data =  NULL;
    size_t length = 0;
    Elf32_Ehdr * Ehdr = NULL;

    if (NULL == Elfabi)
    {
        printf(MSG_NULLPOINTER_FAIL, "Elfabi");
        return 0;
    }

    Ehdr = elf32_getehdr(Elfabi->Elf);

    if (NULL == Ehdr)
    {
        printf(MSG_EHDRINVALID_FAIL);
        return 0;
    }

    data = elf_rawfile(Elfabi->Elf, &length);

    show_ehdr(Ehdr, length);

    printf(MSG_EHDRMAGIC_SHOW, data[0] & 0xff, data[1] & 0xff, data[2] & 0xff, data[3] & 0xff);

    return Ehdr->e_phnum;
}

#endif


