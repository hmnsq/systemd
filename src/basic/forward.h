/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

/* IWYU pragma: always_keep */

#شامل <errno.h>                      /* IWYU pragma: export */
#شامل <inttypes.h>                   /* IWYU pragma: export */
#شامل <limits.h>                     /* IWYU pragma: export */
#شامل <paths.h>                      /* IWYU pragma: export */
#شامل <stdarg.h>                     /* IWYU pragma: export */
#شامل <stdbool.h>                    /* IWYU pragma: export */
#شامل <stddef.h>                     /* IWYU pragma: export */
#شامل <stdint.h>                     /* IWYU pragma: export */
#شامل <sys/types.h>                  /* IWYU pragma: export */
#شامل <uchar.h>                      /* IWYU pragma: export */

#شامل "assert-util.h"                /* IWYU pragma: export */
#شامل "cleanup-util.h"               /* IWYU pragma: export */
#شامل "macro.h"                      /* IWYU pragma: export */

#شامل "../fundamental/string-table.h"   /* IWYU pragma: export */

/* Generic types */

قسمتعریف uint64_t usec_t;
قسمتعریف uint64_t nsec_t;

/* Libc/Linux forward declarations */

ساخت __kernel_timespec;
ساخت dirent;
ساخت ether_addr;
ساخت fiemap;
ساخت file_handle;
ساخت glob_t;
ساخت group;
ساخت icmp6_hdr;
ساخت in_addr;
ساخت in6_addr;
ساخت inotify_event;
ساخت iovec;
ساخت mount_attr;
ساخت msghdr;
ساخت passwd;
ساخت pollfd;
ساخت rlimit;
ساخت sgrp;
ساخت shadow;
ساخت signalfd_siginfo;
ساخت siphash;
ساخت sockaddr;
ساخت spwd;
ساخت stat;
ساخت statfs;
ساخت statx_timestamp;
ساخت statx;
ساخت termios;
ساخت tm;
ساخت ucred;
ساخت unix_diag_پیغام;

/* To forward declare FILE and DIR, we have to declare the internal struct names for them. Since these are
 * used for C++ symbol name mangling, they're effectively part of the ABI and won't actually change. */
قسمتعریف ساخت _IO_FILE FILE;
قسمتعریف ساخت __dirstream DIR;

/* 3rd-party library forward declarations */

شمار bpf_map_type;

ساخت fdisk_context;
ساخت fdisk_جدول;
ساخت crypt_device;

قسمتعریف ساخت buf_mem_st BUF_MEM;
قسمتعریف ساخت evp_pkey_st EVP_PKEY;
قسمتعریف ساخت evp_md_st EVP_MD;
قسمتعریف ساخت evp_md_ctx_st EVP_MD_CTX;
قسمتعریف ساخت ssl_st SSL;
قسمتعریف ساخت ssl_ctx_st SSL_CTX;
قسمتعریف ساخت ssl_session_st SSL_SESSION;
قسمتعریف ساخت x509_st X509;

/* basic/ forward declarations */

قسمتعریف خالی (*hash_func_t)(ثابت خالی *p, ساخت siphash *state);
قسمتعریف صحیح (*compare_func_t)(ثابت خالی *a, ثابت خالی *b);
قسمتعریف compare_func_t comparison_fn_t;
قسمتعریف صحیح (*comparison_userdata_fn_t)(ثابت خالی *, ثابت خالی *, خالی *);

ساخت hash_ops;
ساخت hw_addr_ڈیٹا;
ساخت اندر_addr_ڈیٹا;
ساخت iovec_wrapper;
اتحاد اندر_addr_union;
اتحاد sockaddr_union;

قسمتعریف شمار Architecture Architecture;
قسمتعریف شمار CGroupFlags CGroupFlags;
قسمتعریف شمار CGroup_نقاب CGroup_نقاب;
قسمتعریف شمار ChaseFlags ChaseFlags;
قسمتعریف شمار ConfFilesFlags ConfFilesFlags;
قسمتعریف شمار نکالو_Flags نکالو_Flags;
قسمتعریف شمار ForkFlags ForkFlags;
قسمتعریف شمار Glyph Glyph;
قسمتعریف شمار ImageClass ImageClass;
قسمتعریف شمار کام_منوال کام_منوال;
قسمتعریف شمار NamespaceEnterFlags NamespaceEnterFlags;
قسمتعریف شمار RuntimeScope RuntimeScope;
قسمتعریف شمار String_محفوظ_Flags String_محفوظ_Flags;
قسمتعریف شمار TimestampStyle TimestampStyle;
قسمتعریف شمار Unit_فعال_State Unit_فعال_State;
قسمتعریف شمار UnitDependency UnitDependency;
قسمتعریف شمار UnitNameMangle UnitNameMangle;
قسمتعریف شمار UnitType UnitType;
قسمتعریف شمار WaitFlags WaitFlags;
قسمتعریف شمار XOpenFlags XOpenFlags;

قسمتعریف ساخت Fiber Fiber;
قسمتعریف ساخت FiberOps FiberOps;
قسمتعریف ساخت Hashmap Hashmap;
قسمتعریف ساخت Hashmap_بنیاد Hashmap_بنیاد;
قسمتعریف ساخت IteratedCache IteratedCache;
قسمتعریف ساخت Iterator Iterator;
قسمتعریف ساخت LogContext LogContext;
قسمتعریف ساخت OrderedHashmap OrderedHashmap;
قسمتعریف ساخت Ordered_سیٹ Ordered_سیٹ;
قسمتعریف ساخت سیٹ_2 سیٹ_2;

قسمتعریف ساخت dual_timestamp dual_timestamp;
قسمتعریف ساخت triple_timestamp triple_timestamp;
قسمتعریف ساخت Compressor Compressor;
قسمتعریف ساخت ConfFile ConfFile;
قسمتعریف ساخت LabelContext LabelContext;
قسمتعریف ساخت تالا_File تالا_File;
قسمتعریف ساخت PidRef PidRef;
قسمتعریف ساخت Prioq Prioq;
قسمتعریف ساخت شرح_حد_بندی شرح_حد_بندی;
قسمتعریف ساخت Socket_پتہ Socket_پتہ;

/* Constants */

/* We duplicate various commonly used constants here so we can keep most static inline functions without
 * having to include the full header that provides these constants. */

/* glibc defines AT_FDCWD as -100, but musl defines it as (-100). */
#اگر_تعریف_شدہ __GLIBC__
#تعریف_کرو AT_FDCWD                -100
#ورنہ
#تعریف_کرو AT_FDCWD                (-100)
#اگر_ختم
#تعریف_کرو AT_خالی_خانے_راستہ           0x1000
#تعریف_کرو AT_SYMLINK_FOLLOW       0x400
#تعریف_کرو AT_SYMLINK_NOFOLLOW     0x100

#تعریف_کرو منوال_INVALID            ((mode_t) -1)

#تعریف_کرو UID_INVALID             ((uid_t) -1)
#تعریف_کرو GID_INVALID             ((gid_t) -1)

#تعریف_کرو USEC_INFINITY           ((usec_t) UINT64_MAX)
#تعریف_کرو NSEC_INFINITY           ((nsec_t) UINT64_MAX)

/* MAX_ERRNO is defined as 4095 in linux/err.h. We use the same value here. */
#تعریف_کرو ERRNO_MAX               4095

/* This is MAX_ADDR_LEN as defined in linux/netdevice.h, but net/if_arp.h
 * defines a macro of the same name with a much lower size. */
#تعریف_کرو HW_ADDR_MAX_سائز 32
