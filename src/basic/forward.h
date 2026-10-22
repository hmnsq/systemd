/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* IWYU pragma: always_keep */

#অন্তর্ভাব <errno.h>                      /* IWYU pragma: export */
#অন্তর্ভাব <inttypes.h>                   /* IWYU pragma: export */
#অন্তর্ভাব <limits.h>                     /* IWYU pragma: export */
#অন্তর্ভাব <paths.h>                      /* IWYU pragma: export */
#অন্তর্ভাব <stdarg.h>                     /* IWYU pragma: export */
#অন্তর্ভাব <stdbool.h>                    /* IWYU pragma: export */
#অন্তর্ভাব <stddef.h>                     /* IWYU pragma: export */
#অন্তর্ভাব <stdint.h>                     /* IWYU pragma: export */
#অন্তর্ভাব <sys/types.h>                  /* IWYU pragma: export */
#অন্তর্ভাব <uchar.h>                      /* IWYU pragma: export */

#অন্তর্ভাব "assert-util.h"                /* IWYU pragma: export */
#অন্তর্ভাব "cleanup-util.h"               /* IWYU pragma: export */
#অন্তর্ভাব "macro.h"                      /* IWYU pragma: export */

#অন্তর্ভাব "../fundamental/string-table.h"   /* IWYU pragma: export */

/* Generic types */

প্রকারলেখ uint64_t usec_t;
প্রকারলেখ uint64_t nsec_t;

/* Libc/Linux forward declarations */

কাঠামো __kernel_timespec;
কাঠামো dirent;
কাঠামো ether_addr;
কাঠামো fiemap;
কাঠামো file_handle;
কাঠামো glob_t;
কাঠামো group;
কাঠামো icmp6_hdr;
কাঠামো in_addr;
কাঠামো in6_addr;
কাঠামো inotify_event;
কাঠামো iovec;
কাঠামো mount_attr;
কাঠামো msghdr;
কাঠামো passwd;
কাঠামো pollfd;
কাঠামো rlimit;
কাঠামো sgrp;
কাঠামো shadow;
কাঠামো signalfd_siginfo;
কাঠামো siphash;
কাঠামো sockaddr;
কাঠামো spwd;
কাঠামো stat;
কাঠামো statfs;
কাঠামো statx_timestamp;
কাঠামো statx;
কাঠামো termios;
কাঠামো tm;
কাঠামো ucred;
কাঠামো unix_diag_বার্তা;

/* To forward declare FILE and DIR, we have to declare the internal struct names for them. Since these are
 * used for C++ symbol name mangling, they're effectively part of the ABI and won't actually change. */
প্রকারলেখ কাঠামো _IO_FILE FILE;
প্রকারলেখ কাঠামো __dirstream DIR;

/* 3rd-party library forward declarations */

গণনা bpf_map_type;

কাঠামো fdisk_context;
কাঠামো fdisk_table;
কাঠামো crypt_device;

প্রকারলেখ কাঠামো buf_mem_st BUF_MEM;
প্রকারলেখ কাঠামো evp_pkey_st EVP_PKEY;
প্রকারলেখ কাঠামো evp_md_st EVP_MD;
প্রকারলেখ কাঠামো evp_md_ctx_st EVP_MD_CTX;
প্রকারলেখ কাঠামো ssl_st SSL;
প্রকারলেখ কাঠামো ssl_ctx_st SSL_CTX;
প্রকারলেখ কাঠামো ssl_session_st SSL_SESSION;
প্রকারলেখ কাঠামো x509_st X509;

/* basic/ forward declarations */

প্রকারলেখ শূন্য (*hash_func_t)(ধ্রুবক শূন্য *p, কাঠামো siphash *state);
প্রকারলেখ পূর্ণসংখ্যা (*compare_func_t)(ধ্রুবক শূন্য *a, ধ্রুবক শূন্য *b);
প্রকারলেখ compare_func_t comparison_fn_t;
প্রকারলেখ পূর্ণসংখ্যা (*comparison_userdata_fn_t)(ধ্রুবক শূন্য *, ধ্রুবক শূন্য *, শূন্য *);

কাঠামো hash_ops;
কাঠামো hw_addr_তথ্য;
কাঠামো in_addr_তথ্য;
কাঠামো iovec_wrapper;
সংঘ in_addr_union;
সংঘ sockaddr_union;

প্রকারলেখ গণনা Architecture Architecture;
প্রকারলেখ গণনা CGroupFlags CGroupFlags;
প্রকারলেখ গণনা CGroupMask CGroupMask;
প্রকারলেখ গণনা ChaseFlags ChaseFlags;
প্রকারলেখ গণনা ConfFilesFlags ConfFilesFlags;
প্রকারলেখ গণনা ExtractFlags ExtractFlags;
প্রকারলেখ গণনা ForkFlags ForkFlags;
প্রকারলেখ গণনা Glyph Glyph;
প্রকারলেখ গণনা ImageClass ImageClass;
প্রকারলেখ গণনা JobMode JobMode;
প্রকারলেখ গণনা NamespaceEnterFlags NamespaceEnterFlags;
প্রকারলেখ গণনা RuntimeScope RuntimeScope;
প্রকারলেখ গণনা StringSafeFlags StringSafeFlags;
প্রকারলেখ গণনা TimestampStyle TimestampStyle;
প্রকারলেখ গণনা UnitActiveState UnitActiveState;
প্রকারলেখ গণনা UnitDependency UnitDependency;
প্রকারলেখ গণনা UnitNameMangle UnitNameMangle;
প্রকারলেখ গণনা UnitType UnitType;
প্রকারলেখ গণনা WaitFlags WaitFlags;
প্রকারলেখ গণনা XOpenFlags XOpenFlags;

প্রকারলেখ কাঠামো Fiber Fiber;
প্রকারলেখ কাঠামো FiberOps FiberOps;
প্রকারলেখ কাঠামো Hashmap Hashmap;
প্রকারলেখ কাঠামো HashmapBase HashmapBase;
প্রকারলেখ কাঠামো IteratedCache IteratedCache;
প্রকারলেখ কাঠামো Iterator Iterator;
প্রকারলেখ কাঠামো LogContext LogContext;
প্রকারলেখ কাঠামো OrderedHashmap OrderedHashmap;
প্রকারলেখ কাঠামো OrderedSet OrderedSet;
প্রকারলেখ কাঠামো Set Set;

প্রকারলেখ কাঠামো dual_timestamp dual_timestamp;
প্রকারলেখ কাঠামো triple_timestamp triple_timestamp;
প্রকারলেখ কাঠামো Compressor Compressor;
প্রকারলেখ কাঠামো ConfFile ConfFile;
প্রকারলেখ কাঠামো LabelContext LabelContext;
প্রকারলেখ কাঠামো LockFile LockFile;
প্রকারলেখ কাঠামো PidRef PidRef;
প্রকারলেখ কাঠামো Prioq Prioq;
প্রকারলেখ কাঠামো RateLimit RateLimit;
প্রকারলেখ কাঠামো SocketAddress SocketAddress;

/* Constants */

/* We duplicate various commonly used constants here so we can keep most static inline functions without
 * having to include the full header that provides these constants. */

/* glibc defines AT_FDCWD as -100, but musl defines it as (-100). */
#ifdef __GLIBC__
#define AT_FDCWD                -100
#অন্যথা
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
#define HW_ADDR_MAX_আকার 32
