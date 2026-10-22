/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* IWYU pragma: always_keep */

#ଅନ୍ତର୍ଭାଵ <errno.h>                      /* IWYU pragma: export */
#ଅନ୍ତର୍ଭାଵ <inttypes.h>                   /* IWYU pragma: export */
#ଅନ୍ତର୍ଭାଵ <limits.h>                     /* IWYU pragma: export */
#ଅନ୍ତର୍ଭାଵ <paths.h>                      /* IWYU pragma: export */
#ଅନ୍ତର୍ଭାଵ <stdarg.h>                     /* IWYU pragma: export */
#ଅନ୍ତର୍ଭାଵ <stdbool.h>                    /* IWYU pragma: export */
#ଅନ୍ତର୍ଭାଵ <stddef.h>                     /* IWYU pragma: export */
#ଅନ୍ତର୍ଭାଵ <stdint.h>                     /* IWYU pragma: export */
#ଅନ୍ତର୍ଭାଵ <sys/types.h>                  /* IWYU pragma: export */
#ଅନ୍ତର୍ଭାଵ <uchar.h>                      /* IWYU pragma: export */

#ଅନ୍ତର୍ଭାଵ "assert-util.h"                /* IWYU pragma: export */
#ଅନ୍ତର୍ଭାଵ "cleanup-util.h"               /* IWYU pragma: export */
#ଅନ୍ତର୍ଭାଵ "macro.h"                      /* IWYU pragma: export */

#ଅନ୍ତର୍ଭାଵ "../fundamental/string-table.h"   /* IWYU pragma: export */

/* Generic types */

ପ୍ରକାରବ୍ୟାଖ୍ୟା uint64_t usec_t;
ପ୍ରକାରବ୍ୟାଖ୍ୟା uint64_t nsec_t;

/* Libc/Linux forward declarations */

ସଂରଚନା __kernel_timespec;
ସଂରଚନା dirent;
ସଂରଚନା ether_addr;
ସଂରଚନା fiemap;
ସଂରଚନା file_handle;
ସଂରଚନା glob_t;
ସଂରଚନା group;
ସଂରଚନା icmp6_hdr;
ସଂରଚନା in_addr;
ସଂରଚନା in6_addr;
ସଂରଚନା inotify_event;
ସଂରଚନା iovec;
ସଂରଚନା mount_attr;
ସଂରଚନା msghdr;
ସଂରଚନା passwd;
ସଂରଚନା pollfd;
ସଂରଚନା rlimit;
ସଂରଚନା sgrp;
ସଂରଚନା shadow;
ସଂରଚନା signalfd_siginfo;
ସଂରଚନା siphash;
ସଂରଚନା sockaddr;
ସଂରଚନା spwd;
ସଂରଚନା stat;
ସଂରଚନା statfs;
ସଂରଚନା statx_timestamp;
ସଂରଚନା statx;
ସଂରଚନା termios;
ସଂରଚନା tm;
ସଂରଚନା ucred;
ସଂରଚନା unix_diag_ସନ୍ଦେଶ;

/* To forward declare FILE and DIR, we have to declare the internal struct names for them. Since these are
 * used for C++ symbol name mangling, they're effectively part of the ABI and won't actually change. */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା _IO_FILE FILE;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା __dirstream DIR;

/* 3rd-party library forward declarations */

ଗଣନା bpf_map_type;

ସଂରଚନା fdisk_context;
ସଂରଚନା fdisk_table;
ସଂରଚନା crypt_device;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା buf_mem_st BUF_MEM;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା evp_pkey_st EVP_PKEY;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା evp_md_st EVP_MD;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା evp_md_ctx_st EVP_MD_CTX;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା ssl_st SSL;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା ssl_ctx_st SSL_CTX;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା ssl_session_st SSL_SESSION;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା x509_st X509;

/* basic/ forward declarations */

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଶୂନ୍ୟ (*hash_func_t)(ଅଚଳ ଶୂନ୍ୟ *p, ସଂରଚନା siphash *state);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*compare_func_t)(ଅଚଳ ଶୂନ୍ୟ *a, ଅଚଳ ଶୂନ୍ୟ *b);
ପ୍ରକାରବ୍ୟାଖ୍ୟା compare_func_t comparison_fn_t;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*comparison_userdata_fn_t)(ଅଚଳ ଶୂନ୍ୟ *, ଅଚଳ ଶୂନ୍ୟ *, ଶୂନ୍ୟ *);

ସଂରଚନା hash_ops;
ସଂରଚନା hw_addr_data;
ସଂରଚନା in_addr_data;
ସଂରଚନା iovec_wrapper;
ସଂଘ in_addr_union;
ସଂଘ sockaddr_union;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା Architecture Architecture;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା CGroupFlags CGroupFlags;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା CGroupMask CGroupMask;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା ChaseFlags ChaseFlags;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା ConfFilesFlags ConfFilesFlags;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା ExtractFlags ExtractFlags;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା ForkFlags ForkFlags;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା Glyph Glyph;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା ImageClass ImageClass;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା JobMode JobMode;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା NamespaceEnterFlags NamespaceEnterFlags;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା RuntimeScope RuntimeScope;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା StringSafeFlags StringSafeFlags;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା TimestampStyle TimestampStyle;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା UnitActiveState UnitActiveState;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା UnitDependency UnitDependency;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା UnitNameMangle UnitNameMangle;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା UnitType UnitType;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା WaitFlags WaitFlags;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା XOpenFlags XOpenFlags;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା Fiber Fiber;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା FiberOps FiberOps;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା Hashmap Hashmap;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା HashmapBase HashmapBase;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା IteratedCache IteratedCache;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା Iterator Iterator;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା LogContext LogContext;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା OrderedHashmap OrderedHashmap;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା OrderedSet OrderedSet;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା Set Set;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା dual_timestamp dual_timestamp;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା triple_timestamp triple_timestamp;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା Compressor Compressor;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା ConfFile ConfFile;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା LabelContext LabelContext;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା LockFile LockFile;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା PidRef PidRef;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା Prioq Prioq;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା RateLimit RateLimit;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା SocketAddress SocketAddress;

/* Constants */

/* We duplicate various commonly used constants here so we can keep most static inline functions without
 * having to include the full header that provides these constants. */

/* glibc defines AT_FDCWD as -100, but musl defines it as (-100). */
#ifdef __GLIBC__
#define AT_FDCWD                -100
#ଅନ୍ୟଥା
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
