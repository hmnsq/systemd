/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

/* IWYU pragma: always_keep */

#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>                      /* IWYU pragma: export */
#ਅਨ੍ਤਰ੍ਭਾਵ <inttypes.h>                   /* IWYU pragma: export */
#ਅਨ੍ਤਰ੍ਭਾਵ <limits.h>                     /* IWYU pragma: export */
#ਅਨ੍ਤਰ੍ਭਾਵ <paths.h>                      /* IWYU pragma: export */
#ਅਨ੍ਤਰ੍ਭਾਵ <stdarg.h>                     /* IWYU pragma: export */
#ਅਨ੍ਤਰ੍ਭਾਵ <stdbool.h>                    /* IWYU pragma: export */
#ਅਨ੍ਤਰ੍ਭਾਵ <stddef.h>                     /* IWYU pragma: export */
#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>                     /* IWYU pragma: export */
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/types.h>                  /* IWYU pragma: export */
#ਅਨ੍ਤਰ੍ਭਾਵ <uchar.h>                      /* IWYU pragma: export */

#ਅਨ੍ਤਰ੍ਭਾਵ "assert-util.h"                /* IWYU pragma: export */
#ਅਨ੍ਤਰ੍ਭਾਵ "cleanup-util.h"               /* IWYU pragma: export */
#ਅਨ੍ਤਰ੍ਭਾਵ "macro.h"                      /* IWYU pragma: export */

#ਅਨ੍ਤਰ੍ਭਾਵ "../fundamental/string-table.h"   /* IWYU pragma: export */

/* Generic types */

ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint64_t usec_t;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint64_t nsec_t;

/* Libc/Linux forward declarations */

ਬਣਤਰ __kernel_timespec;
ਬਣਤਰ dirent;
ਬਣਤਰ ether_addr;
ਬਣਤਰ fiemap;
ਬਣਤਰ file_handle;
ਬਣਤਰ glob_t;
ਬਣਤਰ group;
ਬਣਤਰ icmp6_hdr;
ਬਣਤਰ in_addr;
ਬਣਤਰ in6_addr;
ਬਣਤਰ inotify_event;
ਬਣਤਰ iovec;
ਬਣਤਰ mount_attr;
ਬਣਤਰ msghdr;
ਬਣਤਰ passwd;
ਬਣਤਰ pollfd;
ਬਣਤਰ rlimit;
ਬਣਤਰ sgrp;
ਬਣਤਰ shadow;
ਬਣਤਰ signalfd_siginfo;
ਬਣਤਰ siphash;
ਬਣਤਰ sockaddr;
ਬਣਤਰ spwd;
ਬਣਤਰ stat;
ਬਣਤਰ statfs;
ਬਣਤਰ statx_timestamp;
ਬਣਤਰ statx;
ਬਣਤਰ termios;
ਬਣਤਰ tm;
ਬਣਤਰ ucred;
ਬਣਤਰ unix_diag_ਸੁਨੇਹਾ;

/* To forward declare FILE and DIR, we have to declare the internal struct names for them. Since these are
 * used for C++ symbol name mangling, they're effectively part of the ABI and won't actually change. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ _IO_FILE FILE;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ __dirstream DIR;

/* 3rd-party library forward declarations */

ਗਿਣਤੀ bpf_map_type;

ਬਣਤਰ fdisk_context;
ਬਣਤਰ fdisk_ਸਾਰਣੀ;
ਬਣਤਰ crypt_device;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ buf_mem_st BUF_MEM;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ evp_pkey_st EVP_PKEY;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ evp_md_st EVP_MD;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ evp_md_ctx_st EVP_MD_CTX;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ssl_st SSL;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ssl_ctx_st SSL_CTX;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ssl_session_st SSL_SESSION;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ x509_st X509;

/* basic/ forward declarations */

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*hash_func_t)(ਅਚਲ ਖਾਲੀ *p, ਬਣਤਰ siphash *state);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*compare_func_t)(ਅਚਲ ਖਾਲੀ *a, ਅਚਲ ਖਾਲੀ *b);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ compare_func_t comparison_fn_t;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*comparison_userdata_fn_t)(ਅਚਲ ਖਾਲੀ *, ਅਚਲ ਖਾਲੀ *, ਖਾਲੀ *);

ਬਣਤਰ hash_ops;
ਬਣਤਰ hw_addr_ਅੰਕੜੇ;
ਬਣਤਰ ਵਿੱਚ_addr_ਅੰਕੜੇ;
ਬਣਤਰ iovec_wrapper;
ਸੰਘ ਵਿੱਚ_addr_union;
ਸੰਘ sockaddr_union;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ Architecture Architecture;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ CGroupFlags CGroupFlags;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ CGroup_ਨਕਾਬ CGroup_ਨਕਾਬ;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ChaseFlags ChaseFlags;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ConfFilesFlags ConfFilesFlags;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ਕੱਢੋ_Flags ਕੱਢੋ_Flags;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ForkFlags ForkFlags;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ Glyph Glyph;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ImageClass ImageClass;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ਕੰਮ_ਬਹੁਲਕ ਕੰਮ_ਬਹੁਲਕ;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ NamespaceEnterFlags NamespaceEnterFlags;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ RuntimeScope RuntimeScope;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ String_ਸੁਰੱਖਿਅਤ_Flags String_ਸੁਰੱਖਿਅਤ_Flags;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ TimestampStyle TimestampStyle;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ Unit_ਸਰਗਰਮ_State Unit_ਸਰਗਰਮ_State;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ UnitDependency UnitDependency;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ UnitNameMangle UnitNameMangle;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ UnitType UnitType;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ WaitFlags WaitFlags;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ XOpenFlags XOpenFlags;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Fiber Fiber;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ FiberOps FiberOps;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Hashmap Hashmap;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Hashmap_ਅਧਾਰ Hashmap_ਅਧਾਰ;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ IteratedCache IteratedCache;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Iterator Iterator;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ LogContext LogContext;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ OrderedHashmap OrderedHashmap;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Ordered_ਸੈੱਟ Ordered_ਸੈੱਟ;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਸੈੱਟ_2 ਸੈੱਟ_2;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ dual_timestamp dual_timestamp;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ triple_timestamp triple_timestamp;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Compressor Compressor;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ConfFile ConfFile;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ LabelContext LabelContext;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਤਾਲਾ_File ਤਾਲਾ_File;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ PidRef PidRef;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Prioq Prioq;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਦਰ_ਹੱਦਬੰਦੀ ਦਰ_ਹੱਦਬੰਦੀ;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Socket_ਪਤਾ Socket_ਪਤਾ;

/* Constants */

/* We duplicate various commonly used constants here so we can keep most static inline functions without
 * having to include the full header that provides these constants. */

/* glibc defines AT_FDCWD as -100, but musl defines it as (-100). */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __GLIBC__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AT_FDCWD                -100
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AT_FDCWD                (-100)
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ           0x1000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AT_SYMLINK_FOLLOW       0x400
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AT_SYMLINK_NOFOLLOW     0x100

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਹੁਲਕ_INVALID            ((mode_t) -1)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UID_INVALID             ((uid_t) -1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GID_INVALID             ((gid_t) -1)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USEC_INFINITY           ((usec_t) UINT64_MAX)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NSEC_INFINITY           ((nsec_t) UINT64_MAX)

/* MAX_ERRNO is defined as 4095 in linux/err.h. We use the same value here. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ERRNO_MAX               4095

/* This is MAX_ADDR_LEN as defined in linux/netdevice.h, but net/if_arp.h
 * defines a macro of the same name with a much lower size. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HW_ADDR_MAX_ਮਾਪ 32
