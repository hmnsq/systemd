/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* IWYU pragma: always_keep */

#அந்தர்பாவ <errno.h>                      /* IWYU pragma: export */
#அந்தர்பாவ <inttypes.h>                   /* IWYU pragma: export */
#அந்தர்பாவ <limits.h>                     /* IWYU pragma: export */
#அந்தர்பாவ <paths.h>                      /* IWYU pragma: export */
#அந்தர்பாவ <stdarg.h>                     /* IWYU pragma: export */
#அந்தர்பாவ <stdbool.h>                    /* IWYU pragma: export */
#அந்தர்பாவ <stddef.h>                     /* IWYU pragma: export */
#அந்தர்பாவ <stdint.h>                     /* IWYU pragma: export */
#அந்தர்பாவ <sys/types.h>                  /* IWYU pragma: export */
#அந்தர்பாவ <uchar.h>                      /* IWYU pragma: export */

#அந்தர்பாவ "assert-util.h"                /* IWYU pragma: export */
#அந்தர்பாவ "cleanup-util.h"               /* IWYU pragma: export */
#அந்தர்பாவ "macro.h"                      /* IWYU pragma: export */

#அந்தர்பாவ "../fundamental/string-table.h"   /* IWYU pragma: export */

/* Generic types */

வகைவரை uint64_t usec_t;
வகைவரை uint64_t nsec_t;

/* Libc/Linux forward declarations */

கட்டமைப்பு __kernel_timespec;
கட்டமைப்பு dirent;
கட்டமைப்பு ether_addr;
கட்டமைப்பு fiemap;
கட்டமைப்பு file_handle;
கட்டமைப்பு glob_t;
கட்டமைப்பு group;
கட்டமைப்பு icmp6_hdr;
கட்டமைப்பு in_addr;
கட்டமைப்பு in6_addr;
கட்டமைப்பு inotify_event;
கட்டமைப்பு iovec;
கட்டமைப்பு mount_attr;
கட்டமைப்பு msghdr;
கட்டமைப்பு passwd;
கட்டமைப்பு pollfd;
கட்டமைப்பு rlimit;
கட்டமைப்பு sgrp;
கட்டமைப்பு shadow;
கட்டமைப்பு signalfd_siginfo;
கட்டமைப்பு siphash;
கட்டமைப்பு sockaddr;
கட்டமைப்பு spwd;
கட்டமைப்பு stat;
கட்டமைப்பு statfs;
கட்டமைப்பு statx_timestamp;
கட்டமைப்பு statx;
கட்டமைப்பு termios;
கட்டமைப்பு tm;
கட்டமைப்பு ucred;
கட்டமைப்பு unix_diag_செய்தி;

/* To forward declare FILE and DIR, we have to declare the internal struct names for them. Since these are
 * used for C++ symbol name mangling, they're effectively part of the ABI and won't actually change. */
வகைவரை கட்டமைப்பு _IO_FILE FILE;
வகைவரை கட்டமைப்பு __dirstream DIR;

/* 3rd-party library forward declarations */

எண்ணிக்கை bpf_map_type;

கட்டமைப்பு fdisk_context;
கட்டமைப்பு fdisk_table;
கட்டமைப்பு crypt_device;

வகைவரை கட்டமைப்பு buf_mem_st BUF_MEM;
வகைவரை கட்டமைப்பு evp_pkey_st EVP_PKEY;
வகைவரை கட்டமைப்பு evp_md_st EVP_MD;
வகைவரை கட்டமைப்பு evp_md_ctx_st EVP_MD_CTX;
வகைவரை கட்டமைப்பு ssl_st SSL;
வகைவரை கட்டமைப்பு ssl_ctx_st SSL_CTX;
வகைவரை கட்டமைப்பு ssl_session_st SSL_SESSION;
வகைவரை கட்டமைப்பு x509_st X509;

/* basic/ forward declarations */

வகைவரை வெறுமை (*hash_func_t)(மாறிலி வெறுமை *p, கட்டமைப்பு siphash *state);
வகைவரை முழு (*compare_func_t)(மாறிலி வெறுமை *a, மாறிலி வெறுமை *b);
வகைவரை compare_func_t comparison_fn_t;
வகைவரை முழு (*comparison_userdata_fn_t)(மாறிலி வெறுமை *, மாறிலி வெறுமை *, வெறுமை *);

கட்டமைப்பு hash_ops;
கட்டமைப்பு hw_addr_data;
கட்டமைப்பு in_addr_data;
கட்டமைப்பு iovec_wrapper;
ஒன்றிப்பு in_addr_union;
ஒன்றிப்பு sockaddr_union;

வகைவரை எண்ணிக்கை Architecture Architecture;
வகைவரை எண்ணிக்கை CGroupFlags CGroupFlags;
வகைவரை எண்ணிக்கை CGroupMask CGroupMask;
வகைவரை எண்ணிக்கை ChaseFlags ChaseFlags;
வகைவரை எண்ணிக்கை ConfFilesFlags ConfFilesFlags;
வகைவரை எண்ணிக்கை ExtractFlags ExtractFlags;
வகைவரை எண்ணிக்கை ForkFlags ForkFlags;
வகைவரை எண்ணிக்கை Glyph Glyph;
வகைவரை எண்ணிக்கை ImageClass ImageClass;
வகைவரை எண்ணிக்கை JobMode JobMode;
வகைவரை எண்ணிக்கை NamespaceEnterFlags NamespaceEnterFlags;
வகைவரை எண்ணிக்கை RuntimeScope RuntimeScope;
வகைவரை எண்ணிக்கை StringSafeFlags StringSafeFlags;
வகைவரை எண்ணிக்கை TimestampStyle TimestampStyle;
வகைவரை எண்ணிக்கை UnitActiveState UnitActiveState;
வகைவரை எண்ணிக்கை UnitDependency UnitDependency;
வகைவரை எண்ணிக்கை UnitNameMangle UnitNameMangle;
வகைவரை எண்ணிக்கை UnitType UnitType;
வகைவரை எண்ணிக்கை WaitFlags WaitFlags;
வகைவரை எண்ணிக்கை XOpenFlags XOpenFlags;

வகைவரை கட்டமைப்பு Fiber Fiber;
வகைவரை கட்டமைப்பு FiberOps FiberOps;
வகைவரை கட்டமைப்பு Hashmap Hashmap;
வகைவரை கட்டமைப்பு HashmapBase HashmapBase;
வகைவரை கட்டமைப்பு IteratedCache IteratedCache;
வகைவரை கட்டமைப்பு Iterator Iterator;
வகைவரை கட்டமைப்பு LogContext LogContext;
வகைவரை கட்டமைப்பு OrderedHashmap OrderedHashmap;
வகைவரை கட்டமைப்பு OrderedSet OrderedSet;
வகைவரை கட்டமைப்பு Set Set;

வகைவரை கட்டமைப்பு dual_timestamp dual_timestamp;
வகைவரை கட்டமைப்பு triple_timestamp triple_timestamp;
வகைவரை கட்டமைப்பு Compressor Compressor;
வகைவரை கட்டமைப்பு ConfFile ConfFile;
வகைவரை கட்டமைப்பு LabelContext LabelContext;
வகைவரை கட்டமைப்பு LockFile LockFile;
வகைவரை கட்டமைப்பு PidRef PidRef;
வகைவரை கட்டமைப்பு Prioq Prioq;
வகைவரை கட்டமைப்பு RateLimit RateLimit;
வகைவரை கட்டமைப்பு SocketAddress SocketAddress;

/* Constants */

/* We duplicate various commonly used constants here so we can keep most static inline functions without
 * having to include the full header that provides these constants. */

/* glibc defines AT_FDCWD as -100, but musl defines it as (-100). */
#ifdef __GLIBC__
#define AT_FDCWD                -100
#இல்லையெனில்
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
