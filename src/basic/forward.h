/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* IWYU pragma: always_keep */

#include <errno.h>                      /* IWYU pragma: export */
#include <inttypes.h>                   /* IWYU pragma: export */
#include <limits.h>                     /* IWYU pragma: export */
#include <paths.h>                      /* IWYU pragma: export */
#include <stdarg.h>                     /* IWYU pragma: export */
#include <stdbool.h>                    /* IWYU pragma: export */
#include <stddef.h>                     /* IWYU pragma: export */
#include <stdint.h>                     /* IWYU pragma: export */
#include <sys/types.h>                  /* IWYU pragma: export */
#include <uchar.h>                      /* IWYU pragma: export */

#include "assert-util.h"                /* IWYU pragma: export */
#include "cleanup-util.h"               /* IWYU pragma: export */
#include "macro.h"                      /* IWYU pragma: export */

#include "../fundamental/string-table.h"   /* IWYU pragma: export */

/* Generic types */

typedef uint64_t usec_t;
typedef uint64_t nsec_t;

/* Libc/Linux forward declarations */

sa_nracanaa __kernel_timespec;
sa_nracanaa dirent;
sa_nracanaa ether_addr;
sa_nracanaa fiemap;
sa_nracanaa file_handle;
sa_nracanaa glob_t;
sa_nracanaa group;
sa_nracanaa icmp6_hdr;
sa_nracanaa in_addr;
sa_nracanaa in6_addr;
sa_nracanaa inotify_event;
sa_nracanaa iovec;
sa_nracanaa mount_attr;
sa_nracanaa msghdr;
sa_nracanaa passwd;
sa_nracanaa pollfd;
sa_nracanaa rlimit;
sa_nracanaa sgrp;
sa_nracanaa shadow;
sa_nracanaa signalfd_siginfo;
sa_nracanaa siphash;
sa_nracanaa sockaddr;
sa_nracanaa spwd;
sa_nracanaa stat;
sa_nracanaa statfs;
sa_nracanaa statx_timestamp;
sa_nracanaa statx;
sa_nracanaa termios;
sa_nracanaa tm;
sa_nracanaa ucred;
sa_nracanaa unix_diag_msg;

/* To forward declare FILE and DIR, we have to declare the internal struct names for them. Since these are
 * used for C++ symbol name mangling, they're effectively part of the ABI and won't actually change. */
typedef sa_nracanaa _IO_FILE FILE;
typedef sa_nracanaa __dirstream DIR;

/* 3rd-party library forward declarations */

ga_nanaa bpf_map_type;

sa_nracanaa fdisk_context;
sa_nracanaa fdisk_table;
sa_nracanaa crypt_device;

typedef sa_nracanaa buf_mem_st BUF_MEM;
typedef sa_nracanaa evp_pkey_st EVP_PKEY;
typedef sa_nracanaa evp_md_st EVP_MD;
typedef sa_nracanaa evp_md_ctx_st EVP_MD_CTX;
typedef sa_nracanaa ssl_st SSL;
typedef sa_nracanaa ssl_ctx_st SSL_CTX;
typedef sa_nracanaa ssl_session_st SSL_SESSION;
typedef sa_nracanaa x509_st X509;

/* basic/ forward declarations */

typedef void (*hash_func_t)(const void *p, sa_nracanaa siphash *state);
typedef puur_naa_nka (*compare_func_t)(const void *a, const void *b);
typedef compare_func_t comparison_fn_t;
typedef puur_naa_nka (*comparison_userdata_fn_t)(const void *, const void *, void *);

sa_nracanaa hash_ops;
sa_nracanaa hw_addr_data;
sa_nracanaa in_addr_data;
sa_nracanaa iovec_wrapper;
sa_ngha in_addr_union;
sa_ngha sockaddr_union;

typedef ga_nanaa Architecture Architecture;
typedef ga_nanaa CGroupFlags CGroupFlags;
typedef ga_nanaa CGroupMask CGroupMask;
typedef ga_nanaa ChaseFlags ChaseFlags;
typedef ga_nanaa ConfFilesFlags ConfFilesFlags;
typedef ga_nanaa ExtractFlags ExtractFlags;
typedef ga_nanaa ForkFlags ForkFlags;
typedef ga_nanaa Glyph Glyph;
typedef ga_nanaa ImageClass ImageClass;
typedef ga_nanaa JobMode JobMode;
typedef ga_nanaa NamespaceEnterFlags NamespaceEnterFlags;
typedef ga_nanaa RuntimeScope RuntimeScope;
typedef ga_nanaa StringSafeFlags StringSafeFlags;
typedef ga_nanaa TimestampStyle TimestampStyle;
typedef ga_nanaa UnitActiveState UnitActiveState;
typedef ga_nanaa UnitDependency UnitDependency;
typedef ga_nanaa UnitNameMangle UnitNameMangle;
typedef ga_nanaa UnitType UnitType;
typedef ga_nanaa WaitFlags WaitFlags;
typedef ga_nanaa XOpenFlags XOpenFlags;

typedef sa_nracanaa Fiber Fiber;
typedef sa_nracanaa FiberOps FiberOps;
typedef sa_nracanaa Hashmap Hashmap;
typedef sa_nracanaa HashmapBase HashmapBase;
typedef sa_nracanaa IteratedCache IteratedCache;
typedef sa_nracanaa Iterator Iterator;
typedef sa_nracanaa LogContext LogContext;
typedef sa_nracanaa OrderedHashmap OrderedHashmap;
typedef sa_nracanaa OrderedSet OrderedSet;
typedef sa_nracanaa Set Set;

typedef sa_nracanaa dual_timestamp dual_timestamp;
typedef sa_nracanaa triple_timestamp triple_timestamp;
typedef sa_nracanaa Compressor Compressor;
typedef sa_nracanaa ConfFile ConfFile;
typedef sa_nracanaa LabelContext LabelContext;
typedef sa_nracanaa LockFile LockFile;
typedef sa_nracanaa PidRef PidRef;
typedef sa_nracanaa Prioq Prioq;
typedef sa_nracanaa RateLimit RateLimit;
typedef sa_nracanaa SocketAddress SocketAddress;

/* Constants */

/* We duplicate various commonly used constants here so we can keep most static inline functions without
 * having to include the full header that provides these constants. */

/* glibc defines AT_FDCWD as -100, but musl defines it as (-100). */
#ifdef __GLIBC__
#define AT_FDCWD                -100
#ᰀᰧᰀ
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
