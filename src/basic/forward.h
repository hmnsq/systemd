/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* IWYU pragma: always_keep */

#ضمن <errno.h>                      /* IWYU pragma: export */
#ضمن <inttypes.h>                   /* IWYU pragma: export */
#ضمن <limits.h>                     /* IWYU pragma: export */
#ضمن <paths.h>                      /* IWYU pragma: export */
#ضمن <stdarg.h>                     /* IWYU pragma: export */
#ضمن <stdbool.h>                    /* IWYU pragma: export */
#ضمن <stddef.h>                     /* IWYU pragma: export */
#ضمن <stdint.h>                     /* IWYU pragma: export */
#ضمن <sys/types.h>                  /* IWYU pragma: export */
#ضمن <uchar.h>                      /* IWYU pragma: export */

#ضمن "assert-util.h"                /* IWYU pragma: export */
#ضمن "cleanup-util.h"               /* IWYU pragma: export */
#ضمن "macro.h"                      /* IWYU pragma: export */

#ضمن "../fundamental/string-table.h"   /* IWYU pragma: export */

/* Generic types */

تعريفنوع uint64_t usec_t;
تعريفنوع uint64_t nsec_t;

/* Libc/Linux forward declarations */

بنية __kernel_timespec;
بنية dirent;
بنية ether_addr;
بنية fiemap;
بنية file_handle;
بنية glob_t;
بنية group;
بنية icmp6_hdr;
بنية in_addr;
بنية in6_addr;
بنية inotify_event;
بنية iovec;
بنية mount_attr;
بنية msghdr;
بنية passwd;
بنية pollfd;
بنية rlimit;
بنية sgrp;
بنية shadow;
بنية signalfd_siginfo;
بنية siphash;
بنية sockaddr;
بنية spwd;
بنية stat;
بنية statfs;
بنية statx_timestamp;
بنية statx;
بنية termios;
بنية tm;
بنية ucred;
بنية unix_diag_ئۇچۇر;

/* To forward declare FILE and DIR, we have to declare the internal struct names for them. Since these are
 * used for C++ symbol name mangling, they're effectively part of the ABI and won't actually change. */
تعريفنوع بنية _IO_FILE FILE;
تعريفنوع بنية __dirstream DIR;

/* 3rd-party library forward declarations */

تعداد bpf_map_type;

بنية fdisk_context;
بنية fdisk_table;
بنية crypt_device;

تعريفنوع بنية buf_mem_st BUF_MEM;
تعريفنوع بنية evp_pkey_st EVP_PKEY;
تعريفنوع بنية evp_md_st EVP_MD;
تعريفنوع بنية evp_md_ctx_st EVP_MD_CTX;
تعريفنوع بنية ssl_st SSL;
تعريفنوع بنية ssl_ctx_st SSL_CTX;
تعريفنوع بنية ssl_session_st SSL_SESSION;
تعريفنوع بنية x509_st X509;

/* basic/ forward declarations */

تعريفنوع فراغ (*hash_func_t)(ثابت فراغ *p, بنية siphash *state);
تعريفنوع صحيح (*compare_func_t)(ثابت فراغ *a, ثابت فراغ *b);
تعريفنوع compare_func_t comparison_fn_t;
تعريفنوع صحيح (*comparison_userdata_fn_t)(ثابت فراغ *, ثابت فراغ *, فراغ *);

بنية hash_ops;
بنية hw_addr_data;
بنية in_addr_data;
بنية iovec_wrapper;
اتحاد in_addr_union;
اتحاد sockaddr_union;

تعريفنوع تعداد Architecture Architecture;
تعريفنوع تعداد CGroupFlags CGroupFlags;
تعريفنوع تعداد CGroupMask CGroupMask;
تعريفنوع تعداد ChaseFlags ChaseFlags;
تعريفنوع تعداد ConfFilesFlags ConfFilesFlags;
تعريفنوع تعداد ExtractFlags ExtractFlags;
تعريفنوع تعداد ForkFlags ForkFlags;
تعريفنوع تعداد Glyph Glyph;
تعريفنوع تعداد ImageClass ImageClass;
تعريفنوع تعداد JobMode JobMode;
تعريفنوع تعداد NamespaceEnterFlags NamespaceEnterFlags;
تعريفنوع تعداد RuntimeScope RuntimeScope;
تعريفنوع تعداد StringSafeFlags StringSafeFlags;
تعريفنوع تعداد TimestampStyle TimestampStyle;
تعريفنوع تعداد UnitActiveState UnitActiveState;
تعريفنوع تعداد UnitDependency UnitDependency;
تعريفنوع تعداد UnitNameMangle UnitNameMangle;
تعريفنوع تعداد UnitType UnitType;
تعريفنوع تعداد WaitFlags WaitFlags;
تعريفنوع تعداد XOpenFlags XOpenFlags;

تعريفنوع بنية Fiber Fiber;
تعريفنوع بنية FiberOps FiberOps;
تعريفنوع بنية Hashmap Hashmap;
تعريفنوع بنية HashmapBase HashmapBase;
تعريفنوع بنية IteratedCache IteratedCache;
تعريفنوع بنية Iterator Iterator;
تعريفنوع بنية LogContext LogContext;
تعريفنوع بنية OrderedHashmap OrderedHashmap;
تعريفنوع بنية OrderedSet OrderedSet;
تعريفنوع بنية Set Set;

تعريفنوع بنية dual_timestamp dual_timestamp;
تعريفنوع بنية triple_timestamp triple_timestamp;
تعريفنوع بنية Compressor Compressor;
تعريفنوع بنية ConfFile ConfFile;
تعريفنوع بنية LabelContext LabelContext;
تعريفنوع بنية LockFile LockFile;
تعريفنوع بنية PidRef PidRef;
تعريفنوع بنية Prioq Prioq;
تعريفنوع بنية RateLimit RateLimit;
تعريفنوع بنية SocketAddress SocketAddress;

/* Constants */

/* We duplicate various commonly used constants here so we can keep most static inline functions without
 * having to include the full header that provides these constants. */

/* glibc defines AT_FDCWD as -100, but musl defines it as (-100). */
#ifdef __GLIBC__
#define AT_FDCWD                -100
#وإلا
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
