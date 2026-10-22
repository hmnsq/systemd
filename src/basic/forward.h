/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* IWYU pragma: always_keep */

#شامل <errno.h>                      /* IWYU pragma: export */
#شامل <inttypes.h>                   /* IWYU pragma: export */
#شامل <limits.h>                     /* IWYU pragma: export */
#شامل <paths.h>                      /* IWYU pragma: export */
#شامل <stdarg.h>                     /* IWYU pragma: export */
#شامل <stdbool.h>                    /* IWYU pragma: export */
#شامل <stddef.h>                     /* IWYU pragma: export */
#شامل <stdint.h>                     /* IWYU pragma: export */
#شامل <sys/types.h>                  /* IWYU pragma: export */
#شامل <uchar.h>                      /* IWYU pragma: export */

#شامل "assert-util.h"                /* IWYU pragma: export */
#شامل "cleanup-util.h"               /* IWYU pragma: export */
#شامل "macro.h"                      /* IWYU pragma: export */

#شامل "../fundamental/string-table.h"   /* IWYU pragma: export */

/* Generic types */

تعریفنوع uint64_t usec_t;
تعریفنوع uint64_t nsec_t;

/* Libc/Linux forward declarations */

ساختار __kernel_timespec;
ساختار dirent;
ساختار ether_addr;
ساختار fiemap;
ساختار file_handle;
ساختار glob_t;
ساختار group;
ساختار icmp6_hdr;
ساختار in_addr;
ساختار in6_addr;
ساختار inotify_event;
ساختار iovec;
ساختار mount_attr;
ساختار msghdr;
ساختار passwd;
ساختار pollfd;
ساختار rlimit;
ساختار sgrp;
ساختار shadow;
ساختار signalfd_siginfo;
ساختار siphash;
ساختار sockaddr;
ساختار spwd;
ساختار stat;
ساختار statfs;
ساختار statx_timestamp;
ساختار statx;
ساختار termios;
ساختار tm;
ساختار ucred;
ساختار unix_diag_پیام;

/* To forward declare FILE and DIR, we have to declare the internal struct names for them. Since these are
 * used for C++ symbol name mangling, they're effectively part of the ABI and won't actually change. */
تعریفنوع ساختار _IO_FILE FILE;
تعریفنوع ساختار __dirstream DIR;

/* 3rd-party library forward declarations */

شمارش bpf_map_type;

ساختار fdisk_context;
ساختار fdisk_table;
ساختار crypt_device;

تعریفنوع ساختار buf_mem_st BUF_MEM;
تعریفنوع ساختار evp_pkey_st EVP_PKEY;
تعریفنوع ساختار evp_md_st EVP_MD;
تعریفنوع ساختار evp_md_ctx_st EVP_MD_CTX;
تعریفنوع ساختار ssl_st SSL;
تعریفنوع ساختار ssl_ctx_st SSL_CTX;
تعریفنوع ساختار ssl_session_st SSL_SESSION;
تعریفنوع ساختار x509_st X509;

/* basic/ forward declarations */

تعریفنوع تهی (*hash_func_t)(ثابت تهی *p, ساختار siphash *state);
تعریفنوع صحیح (*compare_func_t)(ثابت تهی *a, ثابت تهی *b);
تعریفنوع compare_func_t comparison_fn_t;
تعریفنوع صحیح (*comparison_userdata_fn_t)(ثابت تهی *, ثابت تهی *, تهی *);

ساختار hash_ops;
ساختار hw_addr_data;
ساختار in_addr_data;
ساختار iovec_wrapper;
اجتماع in_addr_union;
اجتماع sockaddr_union;

تعریفنوع شمارش Architecture Architecture;
تعریفنوع شمارش CGroupFlags CGroupFlags;
تعریفنوع شمارش CGroupMask CGroupMask;
تعریفنوع شمارش ChaseFlags ChaseFlags;
تعریفنوع شمارش ConfFilesFlags ConfFilesFlags;
تعریفنوع شمارش ExtractFlags ExtractFlags;
تعریفنوع شمارش ForkFlags ForkFlags;
تعریفنوع شمارش Glyph Glyph;
تعریفنوع شمارش ImageClass ImageClass;
تعریفنوع شمارش JobMode JobMode;
تعریفنوع شمارش NamespaceEnterFlags NamespaceEnterFlags;
تعریفنوع شمارش RuntimeScope RuntimeScope;
تعریفنوع شمارش StringSafeFlags StringSafeFlags;
تعریفنوع شمارش TimestampStyle TimestampStyle;
تعریفنوع شمارش UnitActiveState UnitActiveState;
تعریفنوع شمارش UnitDependency UnitDependency;
تعریفنوع شمارش UnitNameMangle UnitNameMangle;
تعریفنوع شمارش UnitType UnitType;
تعریفنوع شمارش WaitFlags WaitFlags;
تعریفنوع شمارش XOpenFlags XOpenFlags;

تعریفنوع ساختار Fiber Fiber;
تعریفنوع ساختار FiberOps FiberOps;
تعریفنوع ساختار Hashmap Hashmap;
تعریفنوع ساختار HashmapBase HashmapBase;
تعریفنوع ساختار IteratedCache IteratedCache;
تعریفنوع ساختار Iterator Iterator;
تعریفنوع ساختار LogContext LogContext;
تعریفنوع ساختار OrderedHashmap OrderedHashmap;
تعریفنوع ساختار OrderedSet OrderedSet;
تعریفنوع ساختار Set Set;

تعریفنوع ساختار dual_timestamp dual_timestamp;
تعریفنوع ساختار triple_timestamp triple_timestamp;
تعریفنوع ساختار Compressor Compressor;
تعریفنوع ساختار ConfFile ConfFile;
تعریفنوع ساختار LabelContext LabelContext;
تعریفنوع ساختار LockFile LockFile;
تعریفنوع ساختار PidRef PidRef;
تعریفنوع ساختار Prioq Prioq;
تعریفنوع ساختار RateLimit RateLimit;
تعریفنوع ساختار SocketAddress SocketAddress;

/* Constants */

/* We duplicate various commonly used constants here so we can keep most static inline functions without
 * having to include the full header that provides these constants. */

/* glibc defines AT_FDCWD as -100, but musl defines it as (-100). */
#ifdef __GLIBC__
#define AT_FDCWD                -100
#وگرنه
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
