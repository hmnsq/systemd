/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

/* IWYU pragma: always_keep */

#ಅನ್ತರ್ಭಾವ <errno.h>                      /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ <inttypes.h>                   /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ <limits.h>                     /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ <paths.h>                      /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ <stdarg.h>                     /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ <stdbool.h>                    /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ <stddef.h>                     /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ <stdint.h>                     /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ <sys/types.h>                  /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ <uchar.h>                      /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "assert-util.h"                /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ "cleanup-util.h"               /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ "macro.h"                      /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "../fundamental/string-table.h"   /* IWYU pragma: export */

/* Generic types */

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint64_t usec_t;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint64_t nsec_t;

/* Libc/Linux forward declarations */

ರಚನೆ __kernel_timespec;
ರಚನೆ dirent;
ರಚನೆ ether_addr;
ರಚನೆ fiemap;
ರಚನೆ file_handle;
ರಚನೆ glob_t;
ರಚನೆ group;
ರಚನೆ icmp6_hdr;
ರಚನೆ in_addr;
ರಚನೆ in6_addr;
ರಚನೆ inotify_event;
ರಚನೆ iovec;
ರಚನೆ mount_attr;
ರಚನೆ msghdr;
ರಚನೆ passwd;
ರಚನೆ pollfd;
ರಚನೆ rlimit;
ರಚನೆ sgrp;
ರಚನೆ shadow;
ರಚನೆ signalfd_siginfo;
ರಚನೆ siphash;
ರಚನೆ sockaddr;
ರಚನೆ spwd;
ರಚನೆ stat;
ರಚನೆ statfs;
ರಚನೆ statx_timestamp;
ರಚನೆ statx;
ರಚನೆ termios;
ರಚನೆ tm;
ರಚನೆ ucred;
ರಚನೆ unix_diag_ಸಂದೇಶ;

/* To forward declare FILE and DIR, we have to declare the internal struct names for them. Since these are
 * used for C++ symbol name mangling, they're effectively part of the ABI and won't actually change. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ _IO_FILE FILE;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ __dirstream DIR;

/* 3rd-party library forward declarations */

ಎಣಿಕೆ bpf_map_type;

ರಚನೆ fdisk_context;
ರಚನೆ fdisk_ಕೋಷ್ಟಕ;
ರಚನೆ crypt_device;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ buf_mem_st BUF_MEM;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ evp_pkey_st EVP_PKEY;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ evp_md_st EVP_MD;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ evp_md_ctx_st EVP_MD_CTX;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ssl_st SSL;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ssl_ctx_st SSL_CTX;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ssl_session_st SSL_SESSION;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ x509_st X509;

/* basic/ forward declarations */

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*hash_func_t)(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p, ರಚನೆ siphash *state);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*compare_func_t)(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *a, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *b);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ compare_func_t comparison_fn_t;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*comparison_userdata_fn_t)(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *, ಶೂನ್ಯ *);

ರಚನೆ hash_ops;
ರಚನೆ hw_addr_ದತ್ತಾಂಶ;
ರಚನೆ ಒಳ_addr_ದತ್ತಾಂಶ;
ರಚನೆ iovec_wrapper;
ಒಕ್ಕೂಟ ಒಳ_addr_union;
ಒಕ್ಕೂಟ sockaddr_union;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ Architecture Architecture;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ CGroupFlags CGroupFlags;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ CGroup_ಮುಖವಾಡ CGroup_ಮುಖವಾಡ;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ChaseFlags ChaseFlags;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ConfFilesFlags ConfFilesFlags;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ಹೊರತೆಗೆ_Flags ಹೊರತೆಗೆ_Flags;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ForkFlags ForkFlags;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ Glyph Glyph;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ImageClass ImageClass;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ಕೆಲಸ_ಬಹುಲಕ ಕೆಲಸ_ಬಹುಲಕ;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ NamespaceEnterFlags NamespaceEnterFlags;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ RuntimeScope RuntimeScope;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ String_ಸುರಕ್ಷಿತ_Flags String_ಸುರಕ್ಷಿತ_Flags;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ TimestampStyle TimestampStyle;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ Unit_ಸಕ್ರಿಯ_State Unit_ಸಕ್ರಿಯ_State;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ UnitDependency UnitDependency;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ UnitNameMangle UnitNameMangle;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ UnitType UnitType;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ WaitFlags WaitFlags;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ XOpenFlags XOpenFlags;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Fiber Fiber;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ FiberOps FiberOps;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Hashmap Hashmap;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Hashmap_ತಳ Hashmap_ತಳ;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ IteratedCache IteratedCache;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Iterator Iterator;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ LogContext LogContext;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ OrderedHashmap OrderedHashmap;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Ordered_ಗಣ Ordered_ಗಣ;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ಗಣ_2 ಗಣ_2;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ dual_timestamp dual_timestamp;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ triple_timestamp triple_timestamp;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Compressor Compressor;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ConfFile ConfFile;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ LabelContext LabelContext;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ಬೀಗ_File ಬೀಗ_File;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ PidRef PidRef;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Prioq Prioq;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ದರ_ಎಲ್ಲೆ ದರ_ಎಲ್ಲೆ;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Socket_ವಿಳಾಸ Socket_ವಿಳಾಸ;

/* Constants */

/* We duplicate various commonly used constants here so we can keep most static inline functions without
 * having to include the full header that provides these constants. */

/* glibc defines AT_FDCWD as -100, but musl defines it as (-100). */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __GLIBC__
#ವ್ಯಾಖ್ಯಾನಿಸು AT_FDCWD                -100
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು AT_FDCWD                (-100)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ           0x1000
#ವ್ಯಾಖ್ಯಾನಿಸು AT_SYMLINK_FOLLOW       0x400
#ವ್ಯಾಖ್ಯಾನಿಸು AT_SYMLINK_NOFOLLOW     0x100

#ವ್ಯಾಖ್ಯಾನಿಸು ಬಹುಲಕ_INVALID            ((mode_t) -1)

#ವ್ಯಾಖ್ಯಾನಿಸು UID_INVALID             ((uid_t) -1)
#ವ್ಯಾಖ್ಯಾನಿಸು GID_INVALID             ((gid_t) -1)

#ವ್ಯಾಖ್ಯಾನಿಸು USEC_INFINITY           ((usec_t) UINT64_MAX)
#ವ್ಯಾಖ್ಯಾನಿಸು NSEC_INFINITY           ((nsec_t) UINT64_MAX)

/* MAX_ERRNO is defined as 4095 in linux/err.h. We use the same value here. */
#ವ್ಯಾಖ್ಯಾನಿಸು ERRNO_MAX               4095

/* This is MAX_ADDR_LEN as defined in linux/netdevice.h, but net/if_arp.h
 * defines a macro of the same name with a much lower size. */
#ವ್ಯಾಖ್ಯಾನಿಸು HW_ADDR_MAX_ಗಾತ್ರ 32
