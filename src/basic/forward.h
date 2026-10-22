/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* IWYU pragma: always_keep */

#અન્તર્ભાવ <errno.h>                      /* IWYU pragma: export */
#અન્તર્ભાવ <inttypes.h>                   /* IWYU pragma: export */
#અન્તર્ભાવ <limits.h>                     /* IWYU pragma: export */
#અન્તર્ભાવ <paths.h>                      /* IWYU pragma: export */
#અન્તર્ભાવ <stdarg.h>                     /* IWYU pragma: export */
#અન્તર્ભાવ <stdbool.h>                    /* IWYU pragma: export */
#અન્તર્ભાવ <stddef.h>                     /* IWYU pragma: export */
#અન્તર્ભાવ <stdint.h>                     /* IWYU pragma: export */
#અન્તર્ભાવ <sys/types.h>                  /* IWYU pragma: export */
#અન્તર્ભાવ <uchar.h>                      /* IWYU pragma: export */

#અન્તર્ભાવ "assert-util.h"                /* IWYU pragma: export */
#અન્તર્ભાવ "cleanup-util.h"               /* IWYU pragma: export */
#અન્તર્ભાવ "macro.h"                      /* IWYU pragma: export */

#અન્તર્ભાવ "../fundamental/string-table.h"   /* IWYU pragma: export */

/* Generic types */

પ્રકારવ્યાખ્યા uint64_t usec_t;
પ્રકારવ્યાખ્યા uint64_t nsec_t;

/* Libc/Linux forward declarations */

સંરચના __kernel_timespec;
સંરચના dirent;
સંરચના ether_addr;
સંરચના fiemap;
સંરચના file_handle;
સંરચના glob_t;
સંરચના group;
સંરચના icmp6_hdr;
સંરચના in_addr;
સંરચના in6_addr;
સંરચના inotify_event;
સંરચના iovec;
સંરચના mount_attr;
સંરચના msghdr;
સંરચના passwd;
સંરચના pollfd;
સંરચના rlimit;
સંરચના sgrp;
સંરચના shadow;
સંરચના signalfd_siginfo;
સંરચના siphash;
સંરચના sockaddr;
સંરચના spwd;
સંરચના stat;
સંરચના statfs;
સંરચના statx_timestamp;
સંરચના statx;
સંરચના termios;
સંરચના tm;
સંરચના ucred;
સંરચના unix_diag_સંદેશ;

/* To forward declare FILE and DIR, we have to declare the internal struct names for them. Since these are
 * used for C++ symbol name mangling, they're effectively part of the ABI and won't actually change. */
પ્રકારવ્યાખ્યા સંરચના _IO_FILE FILE;
પ્રકારવ્યાખ્યા સંરચના __dirstream DIR;

/* 3rd-party library forward declarations */

ગણના bpf_map_type;

સંરચના fdisk_context;
સંરચના fdisk_table;
સંરચના crypt_device;

પ્રકારવ્યાખ્યા સંરચના buf_mem_st BUF_MEM;
પ્રકારવ્યાખ્યા સંરચના evp_pkey_st EVP_PKEY;
પ્રકારવ્યાખ્યા સંરચના evp_md_st EVP_MD;
પ્રકારવ્યાખ્યા સંરચના evp_md_ctx_st EVP_MD_CTX;
પ્રકારવ્યાખ્યા સંરચના ssl_st SSL;
પ્રકારવ્યાખ્યા સંરચના ssl_ctx_st SSL_CTX;
પ્રકારવ્યાખ્યા સંરચના ssl_session_st SSL_SESSION;
પ્રકારવ્યાખ્યા સંરચના x509_st X509;

/* basic/ forward declarations */

પ્રકારવ્યાખ્યા શૂન્ય (*hash_func_t)(અચલ શૂન્ય *p, સંરચના siphash *state);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*compare_func_t)(અચલ શૂન્ય *a, અચલ શૂન્ય *b);
પ્રકારવ્યાખ્યા compare_func_t comparison_fn_t;
પ્રકારવ્યાખ્યા પૂર્ણાંક (*comparison_userdata_fn_t)(અચલ શૂન્ય *, અચલ શૂન્ય *, શૂન્ય *);

સંરચના hash_ops;
સંરચના hw_addr_data;
સંરચના in_addr_data;
સંરચના iovec_wrapper;
સંઘ in_addr_union;
સંઘ sockaddr_union;

પ્રકારવ્યાખ્યા ગણના Architecture Architecture;
પ્રકારવ્યાખ્યા ગણના CGroupFlags CGroupFlags;
પ્રકારવ્યાખ્યા ગણના CGroupMask CGroupMask;
પ્રકારવ્યાખ્યા ગણના ChaseFlags ChaseFlags;
પ્રકારવ્યાખ્યા ગણના ConfFilesFlags ConfFilesFlags;
પ્રકારવ્યાખ્યા ગણના ExtractFlags ExtractFlags;
પ્રકારવ્યાખ્યા ગણના ForkFlags ForkFlags;
પ્રકારવ્યાખ્યા ગણના Glyph Glyph;
પ્રકારવ્યાખ્યા ગણના ImageClass ImageClass;
પ્રકારવ્યાખ્યા ગણના JobMode JobMode;
પ્રકારવ્યાખ્યા ગણના NamespaceEnterFlags NamespaceEnterFlags;
પ્રકારવ્યાખ્યા ગણના RuntimeScope RuntimeScope;
પ્રકારવ્યાખ્યા ગણના StringSafeFlags StringSafeFlags;
પ્રકારવ્યાખ્યા ગણના TimestampStyle TimestampStyle;
પ્રકારવ્યાખ્યા ગણના UnitActiveState UnitActiveState;
પ્રકારવ્યાખ્યા ગણના UnitDependency UnitDependency;
પ્રકારવ્યાખ્યા ગણના UnitNameMangle UnitNameMangle;
પ્રકારવ્યાખ્યા ગણના UnitType UnitType;
પ્રકારવ્યાખ્યા ગણના WaitFlags WaitFlags;
પ્રકારવ્યાખ્યા ગણના XOpenFlags XOpenFlags;

પ્રકારવ્યાખ્યા સંરચના Fiber Fiber;
પ્રકારવ્યાખ્યા સંરચના FiberOps FiberOps;
પ્રકારવ્યાખ્યા સંરચના Hashmap Hashmap;
પ્રકારવ્યાખ્યા સંરચના HashmapBase HashmapBase;
પ્રકારવ્યાખ્યા સંરચના IteratedCache IteratedCache;
પ્રકારવ્યાખ્યા સંરચના Iterator Iterator;
પ્રકારવ્યાખ્યા સંરચના LogContext LogContext;
પ્રકારવ્યાખ્યા સંરચના OrderedHashmap OrderedHashmap;
પ્રકારવ્યાખ્યા સંરચના OrderedSet OrderedSet;
પ્રકારવ્યાખ્યા સંરચના Set Set;

પ્રકારવ્યાખ્યા સંરચના dual_timestamp dual_timestamp;
પ્રકારવ્યાખ્યા સંરચના triple_timestamp triple_timestamp;
પ્રકારવ્યાખ્યા સંરચના Compressor Compressor;
પ્રકારવ્યાખ્યા સંરચના ConfFile ConfFile;
પ્રકારવ્યાખ્યા સંરચના LabelContext LabelContext;
પ્રકારવ્યાખ્યા સંરચના LockFile LockFile;
પ્રકારવ્યાખ્યા સંરચના PidRef PidRef;
પ્રકારવ્યાખ્યા સંરચના Prioq Prioq;
પ્રકારવ્યાખ્યા સંરચના RateLimit RateLimit;
પ્રકારવ્યાખ્યા સંરચના SocketAddress SocketAddress;

/* Constants */

/* We duplicate various commonly used constants here so we can keep most static inline functions without
 * having to include the full header that provides these constants. */

/* glibc defines AT_FDCWD as -100, but musl defines it as (-100). */
#ifdef __GLIBC__
#define AT_FDCWD                -100
#નહીંતર
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
