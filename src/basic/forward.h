/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* IWYU pragma: always_keep */

#അന്തര്ഭാവ <errno.h>                      /* IWYU pragma: export */
#അന്തര്ഭാവ <inttypes.h>                   /* IWYU pragma: export */
#അന്തര്ഭാവ <limits.h>                     /* IWYU pragma: export */
#അന്തര്ഭാവ <paths.h>                      /* IWYU pragma: export */
#അന്തര്ഭാവ <stdarg.h>                     /* IWYU pragma: export */
#അന്തര്ഭാവ <stdbool.h>                    /* IWYU pragma: export */
#അന്തര്ഭാവ <stddef.h>                     /* IWYU pragma: export */
#അന്തര്ഭാവ <stdint.h>                     /* IWYU pragma: export */
#അന്തര്ഭാവ <sys/types.h>                  /* IWYU pragma: export */
#അന്തര്ഭാവ <uchar.h>                      /* IWYU pragma: export */

#അന്തര്ഭാവ "assert-util.h"                /* IWYU pragma: export */
#അന്തര്ഭാവ "cleanup-util.h"               /* IWYU pragma: export */
#അന്തര്ഭാവ "macro.h"                      /* IWYU pragma: export */

#അന്തര്ഭാവ "../fundamental/string-table.h"   /* IWYU pragma: export */

/* Generic types */

തരനിർവചനം uint64_t usec_t;
തരനിർവചനം uint64_t nsec_t;

/* Libc/Linux forward declarations */

ഘടന __kernel_timespec;
ഘടന dirent;
ഘടന ether_addr;
ഘടന fiemap;
ഘടന file_handle;
ഘടന glob_t;
ഘടന group;
ഘടന icmp6_hdr;
ഘടന in_addr;
ഘടന in6_addr;
ഘടന inotify_event;
ഘടന iovec;
ഘടന mount_attr;
ഘടന msghdr;
ഘടന passwd;
ഘടന pollfd;
ഘടന rlimit;
ഘടന sgrp;
ഘടന shadow;
ഘടന signalfd_siginfo;
ഘടന siphash;
ഘടന sockaddr;
ഘടന spwd;
ഘടന stat;
ഘടന statfs;
ഘടന statx_timestamp;
ഘടന statx;
ഘടന termios;
ഘടന tm;
ഘടന ucred;
ഘടന unix_diag_സന്ദേശം;

/* To forward declare FILE and DIR, we have to declare the internal struct names for them. Since these are
 * used for C++ symbol name mangling, they're effectively part of the ABI and won't actually change. */
തരനിർവചനം ഘടന _IO_FILE FILE;
തരനിർവചനം ഘടന __dirstream DIR;

/* 3rd-party library forward declarations */

എണ്ണൽ bpf_map_type;

ഘടന fdisk_context;
ഘടന fdisk_table;
ഘടന crypt_device;

തരനിർവചനം ഘടന buf_mem_st BUF_MEM;
തരനിർവചനം ഘടന evp_pkey_st EVP_PKEY;
തരനിർവചനം ഘടന evp_md_st EVP_MD;
തരനിർവചനം ഘടന evp_md_ctx_st EVP_MD_CTX;
തരനിർവചനം ഘടന ssl_st SSL;
തരനിർവചനം ഘടന ssl_ctx_st SSL_CTX;
തരനിർവചനം ഘടന ssl_session_st SSL_SESSION;
തരനിർവചനം ഘടന x509_st X509;

/* basic/ forward declarations */

തരനിർവചനം ശൂന്യം (*hash_func_t)(സ്ഥിരം ശൂന്യം *p, ഘടന siphash *state);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*compare_func_t)(സ്ഥിരം ശൂന്യം *a, സ്ഥിരം ശൂന്യം *b);
തരനിർവചനം compare_func_t comparison_fn_t;
തരനിർവചനം പൂർണ്ണസംഖ്യ (*comparison_userdata_fn_t)(സ്ഥിരം ശൂന്യം *, സ്ഥിരം ശൂന്യം *, ശൂന്യം *);

ഘടന hash_ops;
ഘടന hw_addr_data;
ഘടന in_addr_data;
ഘടന iovec_wrapper;
സംഘം in_addr_union;
സംഘം sockaddr_union;

തരനിർവചനം എണ്ണൽ Architecture Architecture;
തരനിർവചനം എണ്ണൽ CGroupFlags CGroupFlags;
തരനിർവചനം എണ്ണൽ CGroupMask CGroupMask;
തരനിർവചനം എണ്ണൽ ChaseFlags ChaseFlags;
തരനിർവചനം എണ്ണൽ ConfFilesFlags ConfFilesFlags;
തരനിർവചനം എണ്ണൽ ExtractFlags ExtractFlags;
തരനിർവചനം എണ്ണൽ ForkFlags ForkFlags;
തരനിർവചനം എണ്ണൽ Glyph Glyph;
തരനിർവചനം എണ്ണൽ ImageClass ImageClass;
തരനിർവചനം എണ്ണൽ JobMode JobMode;
തരനിർവചനം എണ്ണൽ NamespaceEnterFlags NamespaceEnterFlags;
തരനിർവചനം എണ്ണൽ RuntimeScope RuntimeScope;
തരനിർവചനം എണ്ണൽ StringSafeFlags StringSafeFlags;
തരനിർവചനം എണ്ണൽ TimestampStyle TimestampStyle;
തരനിർവചനം എണ്ണൽ UnitActiveState UnitActiveState;
തരനിർവചനം എണ്ണൽ UnitDependency UnitDependency;
തരനിർവചനം എണ്ണൽ UnitNameMangle UnitNameMangle;
തരനിർവചനം എണ്ണൽ UnitType UnitType;
തരനിർവചനം എണ്ണൽ WaitFlags WaitFlags;
തരനിർവചനം എണ്ണൽ XOpenFlags XOpenFlags;

തരനിർവചനം ഘടന Fiber Fiber;
തരനിർവചനം ഘടന FiberOps FiberOps;
തരനിർവചനം ഘടന Hashmap Hashmap;
തരനിർവചനം ഘടന HashmapBase HashmapBase;
തരനിർവചനം ഘടന IteratedCache IteratedCache;
തരനിർവചനം ഘടന Iterator Iterator;
തരനിർവചനം ഘടന LogContext LogContext;
തരനിർവചനം ഘടന OrderedHashmap OrderedHashmap;
തരനിർവചനം ഘടന OrderedSet OrderedSet;
തരനിർവചനം ഘടന Set Set;

തരനിർവചനം ഘടന dual_timestamp dual_timestamp;
തരനിർവചനം ഘടന triple_timestamp triple_timestamp;
തരനിർവചനം ഘടന Compressor Compressor;
തരനിർവചനം ഘടന ConfFile ConfFile;
തരനിർവചനം ഘടന LabelContext LabelContext;
തരനിർവചനം ഘടന LockFile LockFile;
തരനിർവചനം ഘടന PidRef PidRef;
തരനിർവചനം ഘടന Prioq Prioq;
തരനിർവചനം ഘടന RateLimit RateLimit;
തരനിർവചനം ഘടന SocketAddress SocketAddress;

/* Constants */

/* We duplicate various commonly used constants here so we can keep most static inline functions without
 * having to include the full header that provides these constants. */

/* glibc defines AT_FDCWD as -100, but musl defines it as (-100). */
#ifdef __GLIBC__
#define AT_FDCWD                -100
#അല്ലെങ്കിൽ
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
