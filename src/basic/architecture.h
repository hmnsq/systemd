/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <endian.h>

#ಅನ್ತರ್ಭಾವ "forward.h"

/* A cleaned up architecture definition. We don't want to get lost in
 * processor features, models, generations or even ABIs. Hence we
 * focus on general family, and distinguish word width and endianness. */

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ Architecture {
        ARCHITECTURE_ALPHA,
        ARCHITECTURE_ARC,
        ARCHITECTURE_ARC_BE,
        ARCHITECTURE_ARM,
        ARCHITECTURE_ARM64,
        ARCHITECTURE_ARM64_BE,
        ARCHITECTURE_ARM_BE,
        ARCHITECTURE_CRIS,
        ARCHITECTURE_IA64,
        ARCHITECTURE_LOONGARCH64,
        ARCHITECTURE_M68K,
        ARCHITECTURE_MIPS,
        ARCHITECTURE_MIPS64,
        ARCHITECTURE_MIPS64_LE,
        ARCHITECTURE_MIPS_LE,
        ARCHITECTURE_NIOS2,
        ARCHITECTURE_PARISC,
        ARCHITECTURE_PARISC64,
        ARCHITECTURE_PPC,
        ARCHITECTURE_PPC64,
        ARCHITECTURE_PPC64_LE,
        ARCHITECTURE_PPC_LE,
        ARCHITECTURE_RISCV32,
        ARCHITECTURE_RISCV64,
        ARCHITECTURE_S390,
        ARCHITECTURE_S390X,
        ARCHITECTURE_SH,
        ARCHITECTURE_SH64,
        ARCHITECTURE_SPARC,
        ARCHITECTURE_SPARC64,
        ARCHITECTURE_TILEGX,
        ARCHITECTURE_X86,
        ARCHITECTURE_X86_64,
        _ARCHITECTURE_MAX,
        _ARCHITECTURE_INVALID = -EINVAL,
} Architecture;

Architecture uname_architecture(ಶೂನ್ಯ);

/*
 * LIB_ARCH_TUPLE should resolve to the local library path
 * architecture tuple systemd is built for, according to the Debian
 * tuple list:
 *
 * https://wiki.debian.org/Multiarch/Tuples
 *
 * This is used in library search paths that should understand
 * Debian's paths on all distributions.
 */

#ಆದರೆ defined(__x86_64__)
#  define native_architecture() ARCHITECTURE_X86_64
#  ಆದರೆ defined(__ILP32__)
#    define LIB_ARCH_TUPLE "x86_64-linux-gnux32"
#  ಇಲ್ಲದಿದ್ದರೆ
#    define LIB_ARCH_TUPLE "x86_64-linux-gnu"
#  endif
#  define ARCHITECTURE_SECONDARY ARCHITECTURE_X86
#ಅಥವಾಆದರೆ defined(__i386__)
#  define native_architecture() ARCHITECTURE_X86
#  define LIB_ARCH_TUPLE "i386-linux-gnu"
#ಅಥವಾಆದರೆ defined(__powerpc64__)
#  ಆದರೆ __BYTE_ORDER == __BIG_ENDIAN
#    define native_architecture() ARCHITECTURE_PPC64
#    define LIB_ARCH_TUPLE "ppc64-linux-gnu"
#    define ARCHITECTURE_SECONDARY ARCHITECTURE_PPC
#  ಇಲ್ಲದಿದ್ದರೆ
#    define native_architecture() ARCHITECTURE_PPC64_LE
#    define LIB_ARCH_TUPLE  "powerpc64le-linux-gnu"
#    define ARCHITECTURE_SECONDARY ARCHITECTURE_PPC_LE
#  endif
#ಅಥವಾಆದರೆ defined(__powerpc__)
#  ಆದರೆ __BYTE_ORDER == __BIG_ENDIAN
#    define native_architecture() ARCHITECTURE_PPC
#    ಆದರೆ defined(__NO_FPRS__)
#      define LIB_ARCH_TUPLE "powerpc-linux-gnuspe"
#    ಇಲ್ಲದಿದ್ದರೆ
#      define LIB_ARCH_TUPLE "powerpc-linux-gnu"
#    endif
#  ಇಲ್ಲದಿದ್ದರೆ
#    define native_architecture() ARCHITECTURE_PPC_LE
#    error "Missing LIB_ARCH_TUPLE for PPCLE"
#  endif
#ಅಥವಾಆದರೆ defined(__ia64__)
#  define native_architecture() ARCHITECTURE_IA64
#  define LIB_ARCH_TUPLE "ia64-linux-gnu"
#ಅಥವಾಆದರೆ defined(__hppa64__)
#  define native_architecture() ARCHITECTURE_PARISC64
#  error "Missing LIB_ARCH_TUPLE for HPPA64"
#ಅಥವಾಆದರೆ defined(__hppa__)
#  define native_architecture() ARCHITECTURE_PARISC
#  define LIB_ARCH_TUPLE "hppa‑linux‑gnu"
#ಅಥವಾಆದರೆ defined(__s390x__)
#  define native_architecture() ARCHITECTURE_S390X
#  define LIB_ARCH_TUPLE "s390x-linux-gnu"
#  define ARCHITECTURE_SECONDARY ARCHITECTURE_S390
#ಅಥವಾಆದರೆ defined(__s390__)
#  define native_architecture() ARCHITECTURE_S390
#  define LIB_ARCH_TUPLE "s390-linux-gnu"
#ಅಥವಾಆದರೆ defined(__sparc__) && defined (__arch64__)
#  define native_architecture() ARCHITECTURE_SPARC64
#  define LIB_ARCH_TUPLE "sparc64-linux-gnu"
#ಅಥವಾಆದರೆ defined(__sparc__)
#  define native_architecture() ARCHITECTURE_SPARC
#  define LIB_ARCH_TUPLE "sparc-linux-gnu"
#ಅಥವಾಆದರೆ defined(__mips64) && defined(__LP64__)
#  ಆದರೆ __BYTE_ORDER == __BIG_ENDIAN
#    define native_architecture() ARCHITECTURE_MIPS64
#    define LIB_ARCH_TUPLE "mips64-linux-gnuabi64"
#  ಇಲ್ಲದಿದ್ದರೆ
#    define native_architecture() ARCHITECTURE_MIPS64_LE
#    define LIB_ARCH_TUPLE "mips64el-linux-gnuabi64"
#  endif
#ಅಥವಾಆದರೆ defined(__mips64)
#  ಆದರೆ __BYTE_ORDER == __BIG_ENDIAN
#    define native_architecture() ARCHITECTURE_MIPS64
#    define LIB_ARCH_TUPLE "mips64-linux-gnuabin32"
#  ಇಲ್ಲದಿದ್ದರೆ
#    define native_architecture() ARCHITECTURE_MIPS64_LE
#    define LIB_ARCH_TUPLE "mips64el-linux-gnuabin32"
#  endif
#ಅಥವಾಆದರೆ defined(__mips__)
#  ಆದರೆ __BYTE_ORDER == __BIG_ENDIAN
#    define native_architecture() ARCHITECTURE_MIPS
#    define LIB_ARCH_TUPLE "mips-linux-gnu"
#  ಇಲ್ಲದಿದ್ದರೆ
#    define native_architecture() ARCHITECTURE_MIPS_LE
#    define LIB_ARCH_TUPLE "mipsel-linux-gnu"
#  endif
#ಅಥವಾಆದರೆ defined(__alpha__)
#  define native_architecture() ARCHITECTURE_ALPHA
#  define LIB_ARCH_TUPLE "alpha-linux-gnu"
#ಅಥವಾಆದರೆ defined(__aarch64__)
#  ಆದರೆ __BYTE_ORDER == __BIG_ENDIAN
#    define native_architecture() ARCHITECTURE_ARM64_BE
#    define LIB_ARCH_TUPLE "aarch64_be-linux-gnu"
#  ಇಲ್ಲದಿದ್ದರೆ
#    define native_architecture() ARCHITECTURE_ARM64
#    define LIB_ARCH_TUPLE "aarch64-linux-gnu"
#    define ARCHITECTURE_SECONDARY ARCHITECTURE_ARM
#  endif
#ಅಥವಾಆದರೆ defined(__arm__)
#  ಆದರೆ __BYTE_ORDER == __BIG_ENDIAN
#    define native_architecture() ARCHITECTURE_ARM_BE
#    ಆದರೆ defined(__ARM_EABI__)
#      ಆದರೆ defined(__ARM_PCS_VFP)
#        define LIB_ARCH_TUPLE "armeb-linux-gnueabihf"
#      ಇಲ್ಲದಿದ್ದರೆ
#        define LIB_ARCH_TUPLE "armeb-linux-gnueabi"
#      endif
#    ಇಲ್ಲದಿದ್ದರೆ
#      define LIB_ARCH_TUPLE "armeb-linux-gnu"
#    endif
#  ಇಲ್ಲದಿದ್ದರೆ
#    define native_architecture() ARCHITECTURE_ARM
#    ಆದರೆ defined(__ARM_EABI__)
#      ಆದರೆ defined(__ARM_PCS_VFP)
#        define LIB_ARCH_TUPLE "arm-linux-gnueabihf"
#      ಇಲ್ಲದಿದ್ದರೆ
#        define LIB_ARCH_TUPLE "arm-linux-gnueabi"
#      endif
#    ಇಲ್ಲದಿದ್ದರೆ
#      define LIB_ARCH_TUPLE "arm-linux-gnu"
#    endif
#  endif
#ಅಥವಾಆದರೆ defined(__sh64__)
#  define native_architecture() ARCHITECTURE_SH64
#  error "Missing LIB_ARCH_TUPLE for SH64"
#ಅಥವಾಆದರೆ defined(__sh__)
#  define native_architecture() ARCHITECTURE_SH
#  ಆದರೆ defined(__SH1__)
#    define LIB_ARCH_TUPLE "sh1-linux-gnu"
#  elif defined(__SH2__)
#    define LIB_ARCH_TUPLE "sh2-linux-gnu"
#  elif defined(__SH2A__)
#    define LIB_ARCH_TUPLE "sh2a-linux-gnu"
#  elif defined(__SH2E__)
#    define LIB_ARCH_TUPLE "sh2e-linux-gnu"
#  elif defined(__SH3__)
#    define LIB_ARCH_TUPLE "sh3-linux-gnu"
#  elif defined(__SH3E__)
#    define LIB_ARCH_TUPLE "sh3e-linux-gnu"
#  elif defined(__SH4__) && !defined(__SH4A__)
#    define LIB_ARCH_TUPLE "sh4-linux-gnu"
#  elif defined(__SH4A__)
#    define LIB_ARCH_TUPLE "sh4a-linux-gnu"
#  endif
#ಅಥವಾಆದರೆ defined(__loongarch_lp64)
#  define native_architecture() ARCHITECTURE_LOONGARCH64
#  ಆದರೆ defined(__loongarch_double_float)
#    define LIB_ARCH_TUPLE "loongarch64-linux-gnu"
#  elif defined(__loongarch_single_float)
#    define LIB_ARCH_TUPLE "loongarch64-linux-gnuf32"
#  elif defined(__loongarch_soft_float)
#    define LIB_ARCH_TUPLE "loongarch64-linux-gnusf"
#  ಇಲ್ಲದಿದ್ದರೆ
#    error "Unrecognized loongarch architecture variant"
#  endif
#ಅಥವಾಆದರೆ defined(__m68k__)
#  define native_architecture() ARCHITECTURE_M68K
#  define LIB_ARCH_TUPLE "m68k-linux-gnu"
#ಅಥವಾಆದರೆ defined(__tilegx__)
#  define native_architecture() ARCHITECTURE_TILEGX
#  define LIB_ARCH_TUPLE "tilegx-linux-gnu"
#ಅಥವಾಆದರೆ defined(__cris__)
#  define native_architecture() ARCHITECTURE_CRIS
#  error "Missing LIB_ARCH_TUPLE for CRIS"
#ಅಥವಾಆದರೆ defined(__nios2__)
#  define native_architecture() ARCHITECTURE_NIOS2
#  define LIB_ARCH_TUPLE "nios2-linux-gnu"
#ಅಥವಾಆದರೆ defined(__riscv)
#  ಆದರೆ __SIZEOF_POINTER__ == 4
#    define native_architecture() ARCHITECTURE_RISCV32
#    define LIB_ARCH_TUPLE "riscv32-linux-gnu"
#  elif __SIZEOF_POINTER__ == 8
#    define native_architecture() ARCHITECTURE_RISCV64
#    define LIB_ARCH_TUPLE "riscv64-linux-gnu"
#  ಇಲ್ಲದಿದ್ದರೆ
#    error "Unrecognized riscv architecture variant"
#  endif
#ಅಥವಾಆದರೆ defined(__arc__)
#  ಆದರೆ __BYTE_ORDER == __BIG_ENDIAN
#    define native_architecture() ARCHITECTURE_ARC_BE
#    define LIB_ARCH_TUPLE "arceb-linux"
#  ಇಲ್ಲದಿದ್ದರೆ
#    define native_architecture() ARCHITECTURE_ARC
#    define LIB_ARCH_TUPLE "arc-linux"
#  endif
#ಇಲ್ಲದಿದ್ದರೆ
#  error "Please register your architecture here!"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(__hppa__) || defined(__hppa64__)
#  define ರಾಶಿ_GROWS_ಮೇಲೆ 1
#ಇಲ್ಲದಿದ್ದರೆ
#  define ರಾಶಿ_GROWS_ಮೇಲೆ 0
#ಆದರೆ_ಅಂತ್ಯ

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(architecture, Architecture);
