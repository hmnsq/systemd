/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

/* IWYU pragma: always_keep */

#अन्तर्भाव <errno.h>                      /* IWYU pragma: export */
#अन्तर्भाव <inttypes.h>                   /* IWYU pragma: export */
#अन्तर्भाव <limits.h>                     /* IWYU pragma: export */
#अन्तर्भाव <paths.h>                      /* IWYU pragma: export */
#अन्तर्भाव <stdarg.h>                     /* IWYU pragma: export */
#अन्तर्भाव <stdbool.h>                    /* IWYU pragma: export */
#अन्तर्भाव <stddef.h>                     /* IWYU pragma: export */
#अन्तर्भाव <stdint.h>                     /* IWYU pragma: export */
#अन्तर्भाव <sys/types.h>                  /* IWYU pragma: export */
#अन्तर्भाव <uchar.h>                      /* IWYU pragma: export */

#अन्तर्भाव "assert-util.h"                /* IWYU pragma: export */
#अन्तर्भाव "cleanup-util.h"               /* IWYU pragma: export */
#अन्तर्भाव "macro.h"                      /* IWYU pragma: export */

#अन्तर्भाव "../fundamental/string-table.h"   /* IWYU pragma: export */

/* Generic types */

प्रकारनिर्देश uint64_t usec_t;
प्रकारनिर्देश uint64_t nsec_t;

/* Libc/Linux forward declarations */

संरचना __kernel_timespec;
संरचना dirent;
संरचना ether_addr;
संरचना fiemap;
संरचना file_handle;
संरचना glob_t;
संरचना group;
संरचना icmp6_hdr;
संरचना in_addr;
संरचना in6_addr;
संरचना inotify_event;
संरचना iovec;
संरचना mount_attr;
संरचना msghdr;
संरचना passwd;
संरचना pollfd;
संरचना rlimit;
संरचना sgrp;
संरचना shadow;
संरचना signalfd_siginfo;
संरचना siphash;
संरचना sockaddr;
संरचना spwd;
संरचना stat;
संरचना statfs;
संरचना statx_timestamp;
संरचना statx;
संरचना termios;
संरचना tm;
संरचना ucred;
संरचना unix_diag_सन्देशः;

/* To forward declare FILE and DIR, we have to declare the internal struct names for them. Since these are
 * used for C++ symbol name mangling, they're effectively part of the ABI and won't actually change. */
प्रकारनिर्देश संरचना _IO_FILE FILE;
प्रकारनिर्देश संरचना __dirstream DIR;

/* 3rd-party library forward declarations */

गणना bpf_map_type;

संरचना fdisk_context;
संरचना fdisk_सारणी;
संरचना crypt_device;

प्रकारनिर्देश संरचना buf_mem_st BUF_MEM;
प्रकारनिर्देश संरचना evp_pkey_st EVP_PKEY;
प्रकारनिर्देश संरचना evp_md_st EVP_MD;
प्रकारनिर्देश संरचना evp_md_ctx_st EVP_MD_CTX;
प्रकारनिर्देश संरचना ssl_st SSL;
प्रकारनिर्देश संरचना ssl_ctx_st SSL_CTX;
प्रकारनिर्देश संरचना ssl_session_st SSL_SESSION;
प्रकारनिर्देश संरचना x509_st X509;

/* basic/ forward declarations */

प्रकारनिर्देश शून्य (*hash_func_t)(ध्रुव शून्य *p, संरचना siphash *state);
प्रकारनिर्देश पूर्णाङ्क (*compare_func_t)(ध्रुव शून्य *a, ध्रुव शून्य *b);
प्रकारनिर्देश compare_func_t comparison_fn_t;
प्रकारनिर्देश पूर्णाङ्क (*comparison_userdata_fn_t)(ध्रुव शून्य *, ध्रुव शून्य *, शून्य *);

संरचना hash_ops;
संरचना hw_addr_दत्त;
संरचना अन्तर्_addr_दत्त;
संरचना iovec_wrapper;
सङ्घ अन्तर्_addr_union;
सङ्घ sockaddr_union;

प्रकारनिर्देश गणना Architecture Architecture;
प्रकारनिर्देश गणना CGroupFlags CGroupFlags;
प्रकारनिर्देश गणना CGroup_आवरणम् CGroup_आवरणम्;
प्रकारनिर्देश गणना ChaseFlags ChaseFlags;
प्रकारनिर्देश गणना ConfFilesFlags ConfFilesFlags;
प्रकारनिर्देश गणना ExtractFlags ExtractFlags;
प्रकारनिर्देश गणना ForkFlags ForkFlags;
प्रकारनिर्देश गणना Glyph Glyph;
प्रकारनिर्देश गणना ImageClass ImageClass;
प्रकारनिर्देश गणना Job_बहुलकम् Job_बहुलकम्;
प्रकारनिर्देश गणना NamespaceEnterFlags NamespaceEnterFlags;
प्रकारनिर्देश गणना RuntimeScope RuntimeScope;
प्रकारनिर्देश गणना StringSafeFlags StringSafeFlags;
प्रकारनिर्देश गणना TimestampStyle TimestampStyle;
प्रकारनिर्देश गणना Unit_सक्रिय_State Unit_सक्रिय_State;
प्रकारनिर्देश गणना UnitDependency UnitDependency;
प्रकारनिर्देश गणना UnitNameMangle UnitNameMangle;
प्रकारनिर्देश गणना UnitType UnitType;
प्रकारनिर्देश गणना WaitFlags WaitFlags;
प्रकारनिर्देश गणना XOpenFlags XOpenFlags;

प्रकारनिर्देश संरचना Fiber Fiber;
प्रकारनिर्देश संरचना FiberOps FiberOps;
प्रकारनिर्देश संरचना Hashmap Hashmap;
प्रकारनिर्देश संरचना Hashmap_आधारः Hashmap_आधारः;
प्रकारनिर्देश संरचना IteratedCache IteratedCache;
प्रकारनिर्देश संरचना Iterator Iterator;
प्रकारनिर्देश संरचना LogContext LogContext;
प्रकारनिर्देश संरचना OrderedHashmap OrderedHashmap;
प्रकारनिर्देश संरचना Ordered_समुच्चयः Ordered_समुच्चयः;
प्रकारनिर्देश संरचना समुच्चयः_2 समुच्चयः_2;

प्रकारनिर्देश संरचना dual_timestamp dual_timestamp;
प्रकारनिर्देश संरचना triple_timestamp triple_timestamp;
प्रकारनिर्देश संरचना Compressor Compressor;
प्रकारनिर्देश संरचना ConfFile ConfFile;
प्रकारनिर्देश संरचना LabelContext LabelContext;
प्रकारनिर्देश संरचना तालकम्_File तालकम्_File;
प्रकारनिर्देश संरचना PidRef PidRef;
प्रकारनिर्देश संरचना Prioq Prioq;
प्रकारनिर्देश संरचना दरः_सीमा दरः_सीमा;
प्रकारनिर्देश संरचना Socket_स्थानाङ्कः Socket_स्थानाङ्कः;

/* Constants */

/* We duplicate various commonly used constants here so we can keep most static inline functions without
 * having to include the full header that provides these constants. */

/* glibc defines AT_FDCWD as -100, but musl defines it as (-100). */
#यदि_परिभाषितम् __GLIBC__
#परिभाषय AT_FDCWD                -100
#अन्यथा
#परिभाषय AT_FDCWD                (-100)
#यदि_अन्तः
#परिभाषय AT_रिक्तानि_मार्गः           0x1000
#परिभाषय AT_SYMLINK_FOLLOW       0x400
#परिभाषय AT_SYMLINK_NOFOLLOW     0x100

#परिभाषय बहुलकम्_INVALID            ((mode_t) -1)

#परिभाषय UID_INVALID             ((uid_t) -1)
#परिभाषय GID_INVALID             ((gid_t) -1)

#परिभाषय USEC_INFINITY           ((usec_t) UINT64_MAX)
#परिभाषय NSEC_INFINITY           ((nsec_t) UINT64_MAX)

/* MAX_ERRNO is defined as 4095 in linux/err.h. We use the same value here. */
#परिभाषय ERRNO_MAX               4095

/* This is MAX_ADDR_LEN as defined in linux/netdevice.h, but net/if_arp.h
 * defines a macro of the same name with a much lower size. */
#परिभाषय HW_ADDR_MAX_आकार 32
