/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <sys/utsname.h>

#समावेश "architecture.h"
#समावेश "string-table.h"
#समावेश "string-util.h"

Architecture uname_architecture(व्योम) {

        /* Return a sanitized enum identifying the architecture we are running on. This
         * is based on uname(), and the user may hence control what this returns by using
         * personality(). This puts the user in control on systems that can run binaries
         * of multiple architectures.
         *
         * We do not translate the string returned by uname() 1:1. Instead we try to
         * clean it up and break down the confusion on x86 and arm in particular.
         *
         * We try to distinguish CPUs, not CPU features, i.e. actual architectures that
         * have genuinely different code. */

        अटल स्थिर काष्ठा {
                स्थिर अक्षर *machine;
                Architecture arch;
        } arch_map[] = {
#अगर defined(__aarch64__) || defined(__arm__)
                { "aarch64",    ARCHITECTURE_ARM64    },
                { "aarch64_be", ARCHITECTURE_ARM64_BE },
                { "armv8l",     ARCHITECTURE_ARM      },
                { "armv8b",     ARCHITECTURE_ARM_BE   },
                { "armv7ml",    ARCHITECTURE_ARM      },
                { "armv7mb",    ARCHITECTURE_ARM_BE   },
                { "armv7l",     ARCHITECTURE_ARM      },
                { "armv7b",     ARCHITECTURE_ARM_BE   },
                { "armv6l",     ARCHITECTURE_ARM      },
                { "armv6b",     ARCHITECTURE_ARM_BE   },
                { "armv5tl",    ARCHITECTURE_ARM      },
                { "armv5tel",   ARCHITECTURE_ARM      },
                { "armv5tejl",  ARCHITECTURE_ARM      },
                { "armv5tejb",  ARCHITECTURE_ARM_BE   },
                { "armv5teb",   ARCHITECTURE_ARM_BE   },
                { "armv5tb",    ARCHITECTURE_ARM_BE   },
                { "armv4tl",    ARCHITECTURE_ARM      },
                { "armv4tb",    ARCHITECTURE_ARM_BE   },
                { "armv4l",     ARCHITECTURE_ARM      },
                { "armv4b",     ARCHITECTURE_ARM_BE   },

#या_अगर defined(__alpha__)
                { "alpha" ,     ARCHITECTURE_ALPHA    },

#या_अगर defined(__arc__)
                { "arc",        ARCHITECTURE_ARC      },
                { "arceb",      ARCHITECTURE_ARC_BE   },

#या_अगर defined(__cris__)
                { "crisv32",    ARCHITECTURE_CRIS     },

#या_अगर defined(__i386__) || defined(__x86_64__)
                { "x86_64",     ARCHITECTURE_X86_64   },
                { "i686",       ARCHITECTURE_X86      },
                { "i586",       ARCHITECTURE_X86      },
                { "i486",       ARCHITECTURE_X86      },
                { "i386",       ARCHITECTURE_X86      },

#या_अगर defined(__ia64__)
                { "ia64",       ARCHITECTURE_IA64     },

#या_अगर defined(__hppa__) || defined(__hppa64__)
                { "parisc64",   ARCHITECTURE_PARISC64 },
                { "parisc",     ARCHITECTURE_PARISC   },

#या_अगर defined(__loongarch_lp64)
                { "loongarch64", ARCHITECTURE_LOONGARCH64 },

#या_अगर defined(__m68k__)
                { "m68k",       ARCHITECTURE_M68K     },

#या_अगर defined(__mips__) || defined(__mips64__)
                { "mips64",     ARCHITECTURE_MIPS64   },
                { "mips",       ARCHITECTURE_MIPS     },

#या_अगर defined(__nios2__)
                { "nios2",      ARCHITECTURE_NIOS2    },

#या_अगर defined(__powerpc__) || defined(__powerpc64__)
                { "ppc64le",    ARCHITECTURE_PPC64_LE },
                { "ppc64",      ARCHITECTURE_PPC64    },
                { "ppcle",      ARCHITECTURE_PPC_LE   },
                { "ppc",        ARCHITECTURE_PPC      },

#या_अगर defined(__riscv)
                { "riscv64",    ARCHITECTURE_RISCV64  },
                { "riscv32",    ARCHITECTURE_RISCV32  },
#  अगर __SIZEOF_POINTER__ == 4
                { "riscv",      ARCHITECTURE_RISCV32  },
#  elif __SIZEOF_POINTER__ == 8
                { "riscv",      ARCHITECTURE_RISCV64  },
#  endif

#या_अगर defined(__s390__) || defined(__s390x__)
                { "s390x",      ARCHITECTURE_S390X    },
                { "s390",       ARCHITECTURE_S390     },

#या_अगर defined(__sh__) || defined(__sh64__)
                { "sh5",        ARCHITECTURE_SH64     },
                { "sh4a",       ARCHITECTURE_SH       },
                { "sh4",        ARCHITECTURE_SH       },
                { "sh3",        ARCHITECTURE_SH       },
                { "sh2a",       ARCHITECTURE_SH       },
                { "sh2",        ARCHITECTURE_SH       },

#या_अगर defined(__sparc__)
                { "sparc64",    ARCHITECTURE_SPARC64  },
                { "sparc",      ARCHITECTURE_SPARC    },

#या_अगर defined(__tilegx__)
                { "tilegx",     ARCHITECTURE_TILEGX   },

#अन्यथा
#  error "Please register your architecture here!"
#पूर्ण_अगर
        };

        अटल Architecture cached = _ARCHITECTURE_INVALID;
        काष्ठा utsname u;

        अगर (cached != _ARCHITECTURE_INVALID)
                वापस cached;

        assert_se(uname(&u) >= 0);

        FOREACH_ELEMENT(entry, arch_map)
                अगर (streq(entry->machine, u.machine))
                        वापस cached = entry->arch;

        assert_not_reached();
        वापस _ARCHITECTURE_INVALID;
}

/* Maintain same order as in the table above. */
अटल स्थिर अक्षर *स्थिर architecture_table[_ARCHITECTURE_MAX] = {
        [ARCHITECTURE_ARM64]       = "arm64",
        [ARCHITECTURE_ARM64_BE]    = "arm64-be",
        [ARCHITECTURE_ARM]         = "arm",
        [ARCHITECTURE_ARM_BE]      = "arm-be",
        [ARCHITECTURE_ALPHA]       = "alpha",
        [ARCHITECTURE_ARC]         = "arc",
        [ARCHITECTURE_ARC_BE]      = "arc-be",
        [ARCHITECTURE_CRIS]        = "cris",
        [ARCHITECTURE_X86_64]      = "x86-64",
        [ARCHITECTURE_X86]         = "x86",
        [ARCHITECTURE_IA64]        = "ia64",
        [ARCHITECTURE_LOONGARCH64] = "loongarch64",
        [ARCHITECTURE_M68K]        = "m68k",
        [ARCHITECTURE_MIPS64_LE]   = "mips64-le",
        [ARCHITECTURE_MIPS64]      = "mips64",
        [ARCHITECTURE_MIPS_LE]     = "mips-le",
        [ARCHITECTURE_MIPS]        = "mips",
        [ARCHITECTURE_NIOS2]       = "nios2",
        [ARCHITECTURE_PARISC64]    = "parisc64",
        [ARCHITECTURE_PARISC]      = "parisc",
        [ARCHITECTURE_PPC64_LE]    = "ppc64-le",
        [ARCHITECTURE_PPC64]       = "ppc64",
        [ARCHITECTURE_PPC]         = "ppc",
        [ARCHITECTURE_PPC_LE]      = "ppc-le",
        [ARCHITECTURE_RISCV32]     = "riscv32",
        [ARCHITECTURE_RISCV64]     = "riscv64",
        [ARCHITECTURE_S390X]       = "s390x",
        [ARCHITECTURE_S390]        = "s390",
        [ARCHITECTURE_SH64]        = "sh64",
        [ARCHITECTURE_SH]          = "sh",
        [ARCHITECTURE_SPARC64]     = "sparc64",
        [ARCHITECTURE_SPARC]       = "sparc",
        [ARCHITECTURE_TILEGX]      = "tilegx",
};

DEFINE_STRING_TABLE_LOOKUP(architecture, Architecture);
