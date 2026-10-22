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

প্রকারনির্দেশ uint64_t usec_t;
প্রকারনির্দেশ uint64_t nsec_t;

/* Libc/Linux forward declarations */

সংরচনা __kernel_timespec;
সংরচনা dirent;
সংরচনা ether_addr;
সংরচনা fiemap;
সংরচনা file_handle;
সংরচনা glob_t;
সংরচনা group;
সংরচনা icmp6_hdr;
সংরচনা in_addr;
সংরচনা in6_addr;
সংরচনা inotify_event;
সংরচনা iovec;
সংরচনা mount_attr;
সংরচনা msghdr;
সংরচনা passwd;
সংরচনা pollfd;
সংরচনা rlimit;
সংরচনা sgrp;
সংরচনা shadow;
সংরচনা signalfd_siginfo;
সংরচনা siphash;
সংরচনা sockaddr;
সংরচনা spwd;
সংরচনা stat;
সংরচনা statfs;
সংরচনা statx_timestamp;
সংরচনা statx;
সংরচনা termios;
সংরচনা tm;
সংরচনা ucred;
সংরচনা unix_diag_বার্তা;

/* To forward declare FILE and DIR, we have to declare the internal struct names for them. Since these are
 * used for C++ symbol name mangling, they're effectively part of the ABI and won't actually change. */
প্রকারনির্দেশ সংরচনা _IO_FILE FILE;
প্রকারনির্দেশ সংরচনা __dirstream DIR;

/* 3rd-party library forward declarations */

গণনা bpf_map_type;

সংরচনা fdisk_context;
সংরচনা fdisk_table;
সংরচনা crypt_device;

প্রকারনির্দেশ সংরচনা buf_mem_st BUF_MEM;
প্রকারনির্দেশ সংরচনা evp_pkey_st EVP_PKEY;
প্রকারনির্দেশ সংরচনা evp_md_st EVP_MD;
প্রকারনির্দেশ সংরচনা evp_md_ctx_st EVP_MD_CTX;
প্রকারনির্দেশ সংরচনা ssl_st SSL;
প্রকারনির্দেশ সংরচনা ssl_ctx_st SSL_CTX;
প্রকারনির্দেশ সংরচনা ssl_session_st SSL_SESSION;
প্রকারনির্দেশ সংরচনা x509_st X509;

/* basic/ forward declarations */

প্রকারনির্দেশ শূন্য (*hash_func_t)(ধ্রুব শূন্য *p, সংরচনা siphash *state);
প্রকারনির্দেশ পূর্ণাঙ্ক (*compare_func_t)(ধ্রুব শূন্য *a, ধ্রুব শূন্য *b);
প্রকারনির্দেশ compare_func_t comparison_fn_t;
প্রকারনির্দেশ পূর্ণাঙ্ক (*comparison_userdata_fn_t)(ধ্রুব শূন্য *, ধ্রুব শূন্য *, শূন্য *);

সংরচনা hash_ops;
সংরচনা hw_addr_data;
সংরচনা in_addr_data;
সংরচনা iovec_wrapper;
সঙ্ঘ in_addr_union;
সঙ্ঘ sockaddr_union;

প্রকারনির্দেশ গণনা Architecture Architecture;
প্রকারনির্দেশ গণনা CGroupFlags CGroupFlags;
প্রকারনির্দেশ গণনা CGroupMask CGroupMask;
প্রকারনির্দেশ গণনা ChaseFlags ChaseFlags;
প্রকারনির্দেশ গণনা ConfFilesFlags ConfFilesFlags;
প্রকারনির্দেশ গণনা ExtractFlags ExtractFlags;
প্রকারনির্দেশ গণনা ForkFlags ForkFlags;
প্রকারনির্দেশ গণনা Glyph Glyph;
প্রকারনির্দেশ গণনা ImageClass ImageClass;
প্রকারনির্দেশ গণনা JobMode JobMode;
প্রকারনির্দেশ গণনা NamespaceEnterFlags NamespaceEnterFlags;
প্রকারনির্দেশ গণনা RuntimeScope RuntimeScope;
প্রকারনির্দেশ গণনা StringSafeFlags StringSafeFlags;
প্রকারনির্দেশ গণনা TimestampStyle TimestampStyle;
প্রকারনির্দেশ গণনা UnitActiveState UnitActiveState;
প্রকারনির্দেশ গণনা UnitDependency UnitDependency;
প্রকারনির্দেশ গণনা UnitNameMangle UnitNameMangle;
প্রকারনির্দেশ গণনা UnitType UnitType;
প্রকারনির্দেশ গণনা WaitFlags WaitFlags;
প্রকারনির্দেশ গণনা XOpenFlags XOpenFlags;

প্রকারনির্দেশ সংরচনা Fiber Fiber;
প্রকারনির্দেশ সংরচনা FiberOps FiberOps;
প্রকারনির্দেশ সংরচনা Hashmap Hashmap;
প্রকারনির্দেশ সংরচনা HashmapBase HashmapBase;
প্রকারনির্দেশ সংরচনা IteratedCache IteratedCache;
প্রকারনির্দেশ সংরচনা Iterator Iterator;
প্রকারনির্দেশ সংরচনা LogContext LogContext;
প্রকারনির্দেশ সংরচনা OrderedHashmap OrderedHashmap;
প্রকারনির্দেশ সংরচনা OrderedSet OrderedSet;
প্রকারনির্দেশ সংরচনা Set Set;

প্রকারনির্দেশ সংরচনা dual_timestamp dual_timestamp;
প্রকারনির্দেশ সংরচনা triple_timestamp triple_timestamp;
প্রকারনির্দেশ সংরচনা Compressor Compressor;
প্রকারনির্দেশ সংরচনা ConfFile ConfFile;
প্রকারনির্দেশ সংরচনা LabelContext LabelContext;
প্রকারনির্দেশ সংরচনা LockFile LockFile;
প্রকারনির্দেশ সংরচনা PidRef PidRef;
প্রকারনির্দেশ সংরচনা Prioq Prioq;
প্রকারনির্দেশ সংরচনা RateLimit RateLimit;
প্রকারনির্দেশ সংরচনা SocketAddress SocketAddress;

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
#define HW_ADDR_MAX_SIZE 32
