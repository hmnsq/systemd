/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

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

प्रकारव्याख्या uint64_t usec_t;
प्रकारव्याख्या uint64_t nsec_t;

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
संरचना पंचांग;
संरचना ucred;
संरचना unix_diag_संदेश;

/* To forward declare FILE and DIR, we have to declare the internal struct names for them. Since these are
 * used for C++ symbol name mangling, they're effectively part of the ABI and won't actually change. */
प्रकारव्याख्या संरचना _IO_FILE खाता;
प्रकारव्याख्या संरचना __dirstream सूची;

/* 3rd-party library forward declarations */

गणना bpf_map_type;

संरचना fdisk_context;
संरचना fdisk_table;
संरचना crypt_device;

प्रकारव्याख्या संरचना buf_mem_st BUF_MEM;
प्रकारव्याख्या संरचना evp_pkey_st EVP_PKEY;
प्रकारव्याख्या संरचना evp_md_st EVP_MD;
प्रकारव्याख्या संरचना evp_md_ctx_st EVP_MD_CTX;
प्रकारव्याख्या संरचना ssl_st SSL;
प्रकारव्याख्या संरचना ssl_ctx_st SSL_CTX;
प्रकारव्याख्या संरचना ssl_session_st SSL_SESSION;
प्रकारव्याख्या संरचना x509_st X509;

/* basic/ forward declarations */

प्रकारव्याख्या शून्य (*hash_func_t)(स्थिरांक शून्य *p, संरचना siphash *state);
प्रकारव्याख्या पूर्णांक (*compare_func_t)(स्थिरांक शून्य *a, स्थिरांक शून्य *b);
प्रकारव्याख्या compare_func_t comparison_fn_t;
प्रकारव्याख्या पूर्णांक (*comparison_userdata_fn_t)(स्थिरांक शून्य *, स्थिरांक शून्य *, शून्य *);

संरचना hash_ops;
संरचना hw_addr_data;
संरचना in_addr_data;
संरचना iovec_wrapper;
संघ in_addr_union;
संघ sockaddr_union;

प्रकारव्याख्या गणना Architecture Architecture;
प्रकारव्याख्या गणना CGroupFlags CGroupFlags;
प्रकारव्याख्या गणना CGroupMask CGroupMask;
प्रकारव्याख्या गणना ChaseFlags ChaseFlags;
प्रकारव्याख्या गणना ConfFilesFlags ConfFilesFlags;
प्रकारव्याख्या गणना ExtractFlags ExtractFlags;
प्रकारव्याख्या गणना ForkFlags ForkFlags;
प्रकारव्याख्या गणना Glyph Glyph;
प्रकारव्याख्या गणना ImageClass ImageClass;
प्रकारव्याख्या गणना JobMode JobMode;
प्रकारव्याख्या गणना NamespaceEnterFlags NamespaceEnterFlags;
प्रकारव्याख्या गणना RuntimeScope RuntimeScope;
प्रकारव्याख्या गणना StringSafeFlags StringSafeFlags;
प्रकारव्याख्या गणना TimestampStyle TimestampStyle;
प्रकारव्याख्या गणना UnitActiveState UnitActiveState;
प्रकारव्याख्या गणना UnitDependency UnitDependency;
प्रकारव्याख्या गणना UnitNameMangle UnitNameMangle;
प्रकारव्याख्या गणना UnitType UnitType;
प्रकारव्याख्या गणना WaitFlags WaitFlags;
प्रकारव्याख्या गणना XOpenFlags XOpenFlags;

प्रकारव्याख्या संरचना Fiber Fiber;
प्रकारव्याख्या संरचना FiberOps FiberOps;
प्रकारव्याख्या संरचना Hashmap Hashmap;
प्रकारव्याख्या संरचना HashmapBase HashmapBase;
प्रकारव्याख्या संरचना IteratedCache IteratedCache;
प्रकारव्याख्या संरचना Iterator Iterator;
प्रकारव्याख्या संरचना LogContext LogContext;
प्रकारव्याख्या संरचना OrderedHashmap OrderedHashmap;
प्रकारव्याख्या संरचना OrderedSet OrderedSet;
प्रकारव्याख्या संरचना Set Set;

प्रकारव्याख्या संरचना dual_timestamp dual_timestamp;
प्रकारव्याख्या संरचना triple_timestamp triple_timestamp;
प्रकारव्याख्या संरचना Compressor Compressor;
प्रकारव्याख्या संरचना ConfFile ConfFile;
प्रकारव्याख्या संरचना LabelContext LabelContext;
प्रकारव्याख्या संरचना LockFile LockFile;
प्रकारव्याख्या संरचना PidRef PidRef;
प्रकारव्याख्या संरचना Prioq Prioq;
प्रकारव्याख्या संरचना RateLimit RateLimit;
प्रकारव्याख्या संरचना SocketAddress SocketAddress;

/* Constants */

/* We duplicate various commonly used constants here so we can keep most static inline functions without
 * having to include the full header that provides these constants. */

/* glibc defines AT_FDCWD as -100, but musl defines it as (-100). */
#ifdef __GLIBC__
#define AT_FDCWD                -100
#अन्यथा
#define AT_FDCWD                (-100)
#endif
#define AT_EMPTY_PATH           0x1000
#define AT_SYMLINK_FOLLOW       0x400
#define AT_SYMLINK_NOFOLLOW     0x100

#define MODE_INVALID            ((mode_t) -1)

#define UID_INVALID             ((uid_t) -1)
#define GID_INVALID             ((gid_t) -1)

#define USEC_INFINITY           ((usec_t) UINT64_MAX)
#define NSEC_INFINITY           ((nsec_t) UINT64_MAX)

/* MAX_ERRNO is defined as 4095 in linux/err.h. We use the same value here. */
#define ERRNO_MAX               4095

/* This is MAX_ADDR_LEN as defined in linux/netdevice.h, but net/if_arp.h
 * defines a macro of the same name with a much lower size. */
#define HW_ADDR_MAX_SIZE 32
