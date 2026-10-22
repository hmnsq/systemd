/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

/* IWYU pragma: always_keep */

#అన్తర్భావ <errno.h>                      /* IWYU pragma: export */
#అన్తర్భావ <inttypes.h>                   /* IWYU pragma: export */
#అన్తర్భావ <limits.h>                     /* IWYU pragma: export */
#అన్తర్భావ <paths.h>                      /* IWYU pragma: export */
#అన్తర్భావ <stdarg.h>                     /* IWYU pragma: export */
#అన్తర్భావ <stdbool.h>                    /* IWYU pragma: export */
#అన్తర్భావ <stddef.h>                     /* IWYU pragma: export */
#అన్తర్భావ <stdint.h>                     /* IWYU pragma: export */
#అన్తర్భావ <sys/types.h>                  /* IWYU pragma: export */
#అన్తర్భావ <uchar.h>                      /* IWYU pragma: export */

#అన్తర్భావ "assert-util.h"                /* IWYU pragma: export */
#అన్తర్భావ "cleanup-util.h"               /* IWYU pragma: export */
#అన్తర్భావ "macro.h"                      /* IWYU pragma: export */

#అన్తర్భావ "../fundamental/string-table.h"   /* IWYU pragma: export */

/* Generic types */

రకనిర్వచనం uint64_t usec_t;
రకనిర్వచనం uint64_t nsec_t;

/* Libc/Linux forward declarations */

నిర్మాణం __kernel_timespec;
నిర్మాణం dirent;
నిర్మాణం ether_addr;
నిర్మాణం fiemap;
నిర్మాణం file_handle;
నిర్మాణం glob_t;
నిర్మాణం group;
నిర్మాణం icmp6_hdr;
నిర్మాణం in_addr;
నిర్మాణం in6_addr;
నిర్మాణం inotify_event;
నిర్మాణం iovec;
నిర్మాణం mount_attr;
నిర్మాణం msghdr;
నిర్మాణం passwd;
నిర్మాణం pollfd;
నిర్మాణం rlimit;
నిర్మాణం sgrp;
నిర్మాణం shadow;
నిర్మాణం signalfd_siginfo;
నిర్మాణం siphash;
నిర్మాణం sockaddr;
నిర్మాణం spwd;
నిర్మాణం stat;
నిర్మాణం statfs;
నిర్మాణం statx_timestamp;
నిర్మాణం statx;
నిర్మాణం termios;
నిర్మాణం tm;
నిర్మాణం ucred;
నిర్మాణం unix_diag_సందేశం;

/* To forward declare FILE and DIR, we have to declare the internal struct names for them. Since these are
 * used for C++ symbol name mangling, they're effectively part of the ABI and won't actually change. */
రకనిర్వచనం నిర్మాణం _IO_FILE FILE;
రకనిర్వచనం నిర్మాణం __dirstream DIR;

/* 3rd-party library forward declarations */

గణన bpf_map_type;

నిర్మాణం fdisk_context;
నిర్మాణం fdisk_పట్టిక;
నిర్మాణం crypt_device;

రకనిర్వచనం నిర్మాణం buf_mem_st BUF_MEM;
రకనిర్వచనం నిర్మాణం evp_pkey_st EVP_PKEY;
రకనిర్వచనం నిర్మాణం evp_md_st EVP_MD;
రకనిర్వచనం నిర్మాణం evp_md_ctx_st EVP_MD_CTX;
రకనిర్వచనం నిర్మాణం ssl_st SSL;
రకనిర్వచనం నిర్మాణం ssl_ctx_st SSL_CTX;
రకనిర్వచనం నిర్మాణం ssl_session_st SSL_SESSION;
రకనిర్వచనం నిర్మాణం x509_st X509;

/* basic/ forward declarations */

రకనిర్వచనం శూన్య (*hash_func_t)(స్థిరరాశి శూన్య *p, నిర్మాణం siphash *state);
రకనిర్వచనం పూర్ణాంకం (*compare_func_t)(స్థిరరాశి శూన్య *a, స్థిరరాశి శూన్య *b);
రకనిర్వచనం compare_func_t comparison_fn_t;
రకనిర్వచనం పూర్ణాంకం (*comparison_userdata_fn_t)(స్థిరరాశి శూన్య *, స్థిరరాశి శూన్య *, శూన్య *);

నిర్మాణం hash_ops;
నిర్మాణం hw_addr_దత్తాంశం;
నిర్మాణం లోపల_addr_దత్తాంశం;
నిర్మాణం iovec_wrapper;
సంఘం లోపల_addr_union;
సంఘం sockaddr_union;

రకనిర్వచనం గణన Architecture Architecture;
రకనిర్వచనం గణన CGroupFlags CGroupFlags;
రకనిర్వచనం గణన CGroup_ముసుగు CGroup_ముసుగు;
రకనిర్వచనం గణన ChaseFlags ChaseFlags;
రకనిర్వచనం గణన ConfFilesFlags ConfFilesFlags;
రకనిర్వచనం గణన వెలికితీయి_Flags వెలికితీయి_Flags;
రకనిర్వచనం గణన ForkFlags ForkFlags;
రకనిర్వచనం గణన Glyph Glyph;
రకనిర్వచనం గణన ImageClass ImageClass;
రకనిర్వచనం గణన పని_బహుళకం పని_బహుళకం;
రకనిర్వచనం గణన NamespaceEnterFlags NamespaceEnterFlags;
రకనిర్వచనం గణన RuntimeScope RuntimeScope;
రకనిర్వచనం గణన String_సురక్షిత_Flags String_సురక్షిత_Flags;
రకనిర్వచనం గణన TimestampStyle TimestampStyle;
రకనిర్వచనం గణన Unit_చురుకైన_State Unit_చురుకైన_State;
రకనిర్వచనం గణన UnitDependency UnitDependency;
రకనిర్వచనం గణన UnitNameMangle UnitNameMangle;
రకనిర్వచనం గణన UnitType UnitType;
రకనిర్వచనం గణన WaitFlags WaitFlags;
రకనిర్వచనం గణన XOpenFlags XOpenFlags;

రకనిర్వచనం నిర్మాణం Fiber Fiber;
రకనిర్వచనం నిర్మాణం FiberOps FiberOps;
రకనిర్వచనం నిర్మాణం Hashmap Hashmap;
రకనిర్వచనం నిర్మాణం Hashmap_పునాది Hashmap_పునాది;
రకనిర్వచనం నిర్మాణం IteratedCache IteratedCache;
రకనిర్వచనం నిర్మాణం Iterator Iterator;
రకనిర్వచనం నిర్మాణం LogContext LogContext;
రకనిర్వచనం నిర్మాణం OrderedHashmap OrderedHashmap;
రకనిర్వచనం నిర్మాణం Ordered_సమితి Ordered_సమితి;
రకనిర్వచనం నిర్మాణం సమితి_2 సమితి_2;

రకనిర్వచనం నిర్మాణం dual_timestamp dual_timestamp;
రకనిర్వచనం నిర్మాణం triple_timestamp triple_timestamp;
రకనిర్వచనం నిర్మాణం Compressor Compressor;
రకనిర్వచనం నిర్మాణం ConfFile ConfFile;
రకనిర్వచనం నిర్మాణం LabelContext LabelContext;
రకనిర్వచనం నిర్మాణం తాళం_File తాళం_File;
రకనిర్వచనం నిర్మాణం PidRef PidRef;
రకనిర్వచనం నిర్మాణం Prioq Prioq;
రకనిర్వచనం నిర్మాణం రేటు_హద్దు రేటు_హద్దు;
రకనిర్వచనం నిర్మాణం Socket_చిరునామా Socket_చిరునామా;

/* Constants */

/* We duplicate various commonly used constants here so we can keep most static inline functions without
 * having to include the full header that provides these constants. */

/* glibc defines AT_FDCWD as -100, but musl defines it as (-100). */
#నిర్వచితమైతే __GLIBC__
#నిర్వచించు AT_FDCWD                -100
#లేకపోతే
#నిర్వచించు AT_FDCWD                (-100)
#అయితే_ముగింపు
#నిర్వచించు AT_ఖాళీలు_మార్గం           0x1000
#నిర్వచించు AT_SYMLINK_FOLLOW       0x400
#నిర్వచించు AT_SYMLINK_NOFOLLOW     0x100

#నిర్వచించు బహుళకం_INVALID            ((mode_t) -1)

#నిర్వచించు UID_INVALID             ((uid_t) -1)
#నిర్వచించు GID_INVALID             ((gid_t) -1)

#నిర్వచించు USEC_INFINITY           ((usec_t) UINT64_MAX)
#నిర్వచించు NSEC_INFINITY           ((nsec_t) UINT64_MAX)

/* MAX_ERRNO is defined as 4095 in linux/err.h. We use the same value here. */
#నిర్వచించు ERRNO_MAX               4095

/* This is MAX_ADDR_LEN as defined in linux/netdevice.h, but net/if_arp.h
 * defines a macro of the same name with a much lower size. */
#నిర్వచించు HW_ADDR_MAX_కొలత 32
