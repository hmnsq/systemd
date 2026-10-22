/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

/* IWYU pragma: always_keep */

#समावेश <errno.h>                      /* IWYU pragma: export */
#समावेश <inttypes.h>                   /* IWYU pragma: export */
#समावेश <limits.h>                     /* IWYU pragma: export */
#समावेश <paths.h>                      /* IWYU pragma: export */
#समावेश <stdarg.h>                     /* IWYU pragma: export */
#समावेश <stdbool.h>                    /* IWYU pragma: export */
#समावेश <stddef.h>                     /* IWYU pragma: export */
#समावेश <stdint.h>                     /* IWYU pragma: export */
#समावेश <sys/types.h>                  /* IWYU pragma: export */
#समावेश <uchar.h>                      /* IWYU pragma: export */

#समावेश "assert-util.h"                /* IWYU pragma: export */
#समावेश "cleanup-util.h"               /* IWYU pragma: export */
#समावेश "macro.h"                      /* IWYU pragma: export */

#समावेश "../fundamental/string-table.h"   /* IWYU pragma: export */

/* Generic types */

प्रकार uint64_t usec_t;
प्रकार uint64_t nsec_t;

/* Libc/Linux forward declarations */

काष्ठा __kernel_timespec;
काष्ठा dirent;
काष्ठा ether_addr;
काष्ठा fiemap;
काष्ठा file_handle;
काष्ठा glob_t;
काष्ठा group;
काष्ठा icmp6_hdr;
काष्ठा in_addr;
काष्ठा in6_addr;
काष्ठा inotify_event;
काष्ठा iovec;
काष्ठा mount_attr;
काष्ठा msghdr;
काष्ठा passwd;
काष्ठा pollfd;
काष्ठा rlimit;
काष्ठा sgrp;
काष्ठा shadow;
काष्ठा signalfd_siginfo;
काष्ठा siphash;
काष्ठा sockaddr;
काष्ठा spwd;
काष्ठा stat;
काष्ठा statfs;
काष्ठा statx_timestamp;
काष्ठा statx;
काष्ठा termios;
काष्ठा पंचांग;
काष्ठा ucred;
काष्ठा unix_diag_संदेश;

/* To forward declare FILE and DIR, we have to declare the internal struct names for them. Since these are
 * used for C++ symbol name mangling, they're effectively part of the ABI and won't actually change. */
प्रकार काष्ठा _IO_FILE खाता;
प्रकार काष्ठा __dirstream सूची;

/* 3rd-party library forward declarations */

क्रमागत bpf_map_type;

काष्ठा fdisk_context;
काष्ठा fdisk_table;
काष्ठा crypt_device;

प्रकार काष्ठा buf_mem_st BUF_MEM;
प्रकार काष्ठा evp_pkey_st EVP_PKEY;
प्रकार काष्ठा evp_md_st EVP_MD;
प्रकार काष्ठा evp_md_ctx_st EVP_MD_CTX;
प्रकार काष्ठा ssl_st SSL;
प्रकार काष्ठा ssl_ctx_st SSL_CTX;
प्रकार काष्ठा ssl_session_st SSL_SESSION;
प्रकार काष्ठा x509_st X509;

/* basic/ forward declarations */

प्रकार व्योम (*hash_func_t)(स्थिर व्योम *p, काष्ठा siphash *state);
प्रकार पूर्णांक (*compare_func_t)(स्थिर व्योम *a, स्थिर व्योम *b);
प्रकार compare_func_t comparison_fn_t;
प्रकार पूर्णांक (*comparison_userdata_fn_t)(स्थिर व्योम *, स्थिर व्योम *, व्योम *);

काष्ठा hash_ops;
काष्ठा hw_addr_तथ्य;
काष्ठा in_addr_तथ्य;
काष्ठा iovec_wrapper;
जोड़ in_addr_union;
जोड़ sockaddr_union;

प्रकार क्रमागत Architecture Architecture;
प्रकार क्रमागत CGroupFlags CGroupFlags;
प्रकार क्रमागत CGroupMask CGroupMask;
प्रकार क्रमागत ChaseFlags ChaseFlags;
प्रकार क्रमागत ConfFilesFlags ConfFilesFlags;
प्रकार क्रमागत ExtractFlags ExtractFlags;
प्रकार क्रमागत ForkFlags ForkFlags;
प्रकार क्रमागत Glyph Glyph;
प्रकार क्रमागत ImageClass ImageClass;
प्रकार क्रमागत JobMode JobMode;
प्रकार क्रमागत NamespaceEnterFlags NamespaceEnterFlags;
प्रकार क्रमागत RuntimeScope RuntimeScope;
प्रकार क्रमागत StringSafeFlags StringSafeFlags;
प्रकार क्रमागत TimestampStyle TimestampStyle;
प्रकार क्रमागत UnitActiveState UnitActiveState;
प्रकार क्रमागत UnitDependency UnitDependency;
प्रकार क्रमागत UnitNameMangle UnitNameMangle;
प्रकार क्रमागत UnitType UnitType;
प्रकार क्रमागत WaitFlags WaitFlags;
प्रकार क्रमागत XOpenFlags XOpenFlags;

प्रकार काष्ठा Fiber Fiber;
प्रकार काष्ठा FiberOps FiberOps;
प्रकार काष्ठा Hashmap Hashmap;
प्रकार काष्ठा HashmapBase HashmapBase;
प्रकार काष्ठा IteratedCache IteratedCache;
प्रकार काष्ठा Iterator Iterator;
प्रकार काष्ठा LogContext LogContext;
प्रकार काष्ठा OrderedHashmap OrderedHashmap;
प्रकार काष्ठा OrderedSet OrderedSet;
प्रकार काष्ठा Set Set;

प्रकार काष्ठा dual_timestamp dual_timestamp;
प्रकार काष्ठा triple_timestamp triple_timestamp;
प्रकार काष्ठा Compressor Compressor;
प्रकार काष्ठा ConfFile ConfFile;
प्रकार काष्ठा LabelContext LabelContext;
प्रकार काष्ठा LockFile LockFile;
प्रकार काष्ठा PidRef PidRef;
प्रकार काष्ठा Prioq Prioq;
प्रकार काष्ठा RateLimit RateLimit;
प्रकार काष्ठा SocketAddress SocketAddress;

/* Constants */

/* We duplicate various commonly used constants here so we can keep most static inline functions without
 * having to include the full header that provides these constants. */

/* glibc defines AT_FDCWD as -100, but musl defines it as (-100). */
#अगर_घोषित __GLIBC__
#घोषणा AT_FDCWD                -100
#अन्यथा
#घोषणा AT_FDCWD                (-100)
#पूर्ण_अगर
#घोषणा AT_EMPTY_PATH           0x1000
#घोषणा AT_SYMLINK_FOLLOW       0x400
#घोषणा AT_SYMLINK_NOFOLLOW     0x100

#घोषणा MODE_INVALID            ((mode_t) -1)

#घोषणा UID_INVALID             ((uid_t) -1)
#घोषणा GID_INVALID             ((gid_t) -1)

#घोषणा USEC_INFINITY           ((usec_t) UINT64_MAX)
#घोषणा NSEC_INFINITY           ((nsec_t) UINT64_MAX)

/* MAX_ERRNO is defined as 4095 in linux/err.h. We use the same value here. */
#घोषणा ERRNO_MAX               4095

/* This is MAX_ADDR_LEN as defined in linux/netdevice.h, but net/if_arp.h
 * defines a macro of the same name with a much lower size. */
#घोषणा HW_ADDR_MAX_आकार 32
