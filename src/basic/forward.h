/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* IWYU pragma: always_keep */

#ಅನ್ತರ್ಭಾವ <errno.h>                      /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ <inttypes.h>                   /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ <limits.h>                     /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ <paths.h>                      /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ <stdarg.h>                     /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ <stdbool.h>                    /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ <stddef.h>                     /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ <stdint.h>                     /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ <sys/types.h>                  /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ <uchar.h>                      /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "assert-util.h"                /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ "cleanup-util.h"               /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ "macro.h"                      /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "../fundamental/string-table.h"   /* IWYU pragma: export */

/* Generic types */

ಪ್ರಕಾರನಿರ್ದೇಶ uint64_t usec_t;
ಪ್ರಕಾರನಿರ್ದೇಶ uint64_t nsec_t;

/* Libc/Linux forward declarations */

ಸಂರಚನಾ __kernel_timespec;
ಸಂರಚನಾ dirent;
ಸಂರಚನಾ ether_addr;
ಸಂರಚನಾ fiemap;
ಸಂರಚನಾ file_handle;
ಸಂರಚನಾ glob_t;
ಸಂರಚನಾ group;
ಸಂರಚನಾ icmp6_hdr;
ಸಂರಚನಾ in_addr;
ಸಂರಚನಾ in6_addr;
ಸಂರಚನಾ inotify_event;
ಸಂರಚನಾ iovec;
ಸಂರಚನಾ mount_attr;
ಸಂರಚನಾ msghdr;
ಸಂರಚನಾ passwd;
ಸಂರಚನಾ pollfd;
ಸಂರಚನಾ rlimit;
ಸಂರಚನಾ sgrp;
ಸಂರಚನಾ shadow;
ಸಂರಚನಾ signalfd_siginfo;
ಸಂರಚನಾ siphash;
ಸಂರಚನಾ sockaddr;
ಸಂರಚನಾ spwd;
ಸಂರಚನಾ stat;
ಸಂರಚನಾ statfs;
ಸಂರಚನಾ statx_timestamp;
ಸಂರಚನಾ statx;
ಸಂರಚನಾ termios;
ಸಂರಚನಾ tm;
ಸಂರಚನಾ ucred;
ಸಂರಚನಾ unix_diag_ಸಂದೇಶ;

/* To forward declare FILE and DIR, we have to declare the internal struct names for them. Since these are
 * used for C++ symbol name mangling, they're effectively part of the ABI and won't actually change. */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ _IO_FILE FILE;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ __dirstream DIR;

/* 3rd-party library forward declarations */

ಗಣನಾ bpf_map_type;

ಸಂರಚನಾ fdisk_context;
ಸಂರಚನಾ fdisk_table;
ಸಂರಚನಾ crypt_device;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ buf_mem_st BUF_MEM;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ evp_pkey_st EVP_PKEY;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ evp_md_st EVP_MD;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ evp_md_ctx_st EVP_MD_CTX;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ ssl_st SSL;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ ssl_ctx_st SSL_CTX;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ ssl_session_st SSL_SESSION;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ x509_st X509;

/* basic/ forward declarations */

ಪ್ರಕಾರನಿರ್ದೇಶ ಶೂನ್ಯ (*hash_func_t)(ಧ್ರುವ ಶೂನ್ಯ *p, ಸಂರಚನಾ siphash *state);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*compare_func_t)(ಧ್ರುವ ಶೂನ್ಯ *a, ಧ್ರುವ ಶೂನ್ಯ *b);
ಪ್ರಕಾರನಿರ್ದೇಶ compare_func_t comparison_fn_t;
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*comparison_userdata_fn_t)(ಧ್ರುವ ಶೂನ್ಯ *, ಧ್ರುವ ಶೂನ್ಯ *, ಶೂನ್ಯ *);

ಸಂರಚನಾ hash_ops;
ಸಂರಚನಾ hw_addr_data;
ಸಂರಚನಾ in_addr_data;
ಸಂರಚನಾ iovec_wrapper;
ಸಙ್ಘ in_addr_union;
ಸಙ್ಘ sockaddr_union;

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ Architecture Architecture;
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ CGroupFlags CGroupFlags;
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ CGroupMask CGroupMask;
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ ChaseFlags ChaseFlags;
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ ConfFilesFlags ConfFilesFlags;
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ ExtractFlags ExtractFlags;
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ ForkFlags ForkFlags;
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ Glyph Glyph;
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ ImageClass ImageClass;
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ JobMode JobMode;
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ NamespaceEnterFlags NamespaceEnterFlags;
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ RuntimeScope RuntimeScope;
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ StringSafeFlags StringSafeFlags;
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ TimestampStyle TimestampStyle;
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ UnitActiveState UnitActiveState;
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ UnitDependency UnitDependency;
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ UnitNameMangle UnitNameMangle;
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ UnitType UnitType;
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ WaitFlags WaitFlags;
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ XOpenFlags XOpenFlags;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ Fiber Fiber;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ FiberOps FiberOps;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ Hashmap Hashmap;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ HashmapBase HashmapBase;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ IteratedCache IteratedCache;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ Iterator Iterator;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ LogContext LogContext;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ OrderedHashmap OrderedHashmap;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ OrderedSet OrderedSet;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ Set Set;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ dual_timestamp dual_timestamp;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ triple_timestamp triple_timestamp;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ Compressor Compressor;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ ConfFile ConfFile;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ LabelContext LabelContext;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ LockFile LockFile;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ PidRef PidRef;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ Prioq Prioq;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ RateLimit RateLimit;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ SocketAddress SocketAddress;

/* Constants */

/* We duplicate various commonly used constants here so we can keep most static inline functions without
 * having to include the full header that provides these constants. */

/* glibc defines AT_FDCWD as -100, but musl defines it as (-100). */
#ifdef __GLIBC__
#define AT_FDCWD                -100
#ಅನ್ಯಥಾ
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
