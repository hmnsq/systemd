/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "forward.h"

/* strerror(3) says that glibc uses a maximum length of 1024 bytes. */
#define ERRNO_BUF_LEN           1024

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks
 *
 * Note that we use the GNU variant of strerror_r() here. */
#define STRERROR(errnum) strerror_r(ABS(errnum), (അക്ഷരം[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)

/* A helper to print an error message or message for functions that return 0 on EOF. */
സ്ഥിരം അക്ഷരം* strerror_or_eof(പൂർണ്ണസംഖ്യ errnum, അക്ഷരം *buf, size_t buflen);
#define STRERROR_OR_EOF(errnum) strerror_or_eof(errnum, (അക്ഷരം[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)

സ്ഥിതം വരിയിൽ ശൂന്യം _reset_errno_(പൂർണ്ണസംഖ്യ *saved_errno) {
        എങ്കിൽ (*saved_errno < 0) /* Invalidated by UNPROTECT_ERRNO? */
                തിരികെ;

        errno = *saved_errno;
}

#define PROTECT_ERRNO                           \
        _cleanup_(_reset_errno_) _unused_ പൂർണ്ണസംഖ്യ _saved_errno_ = errno

#define UNPROTECT_ERRNO                         \
        ചെയ്യുക {                                    \
                errno = _saved_errno_;          \
                _saved_errno_ = -1;             \
        } ആകുമ്പോൾ (false)

#define LOCAL_ERRNO(മൂല്യം)                      \
        PROTECT_ERRNO;                          \
        errno = ABS(മൂല്യം)

#define return_with_errno(r, err)                     \
        ചെയ്യുക {                                          \
                errno = ABS(err);                     \
                തിരികെ r;                             \
        } ആകുമ്പോൾ (false)

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ negative_errno(ശൂന്യം) {
        /* This helper should be used to shut up gcc if you know 'errno' is
         * negative. Instead of "return -errno;", use "return negative_errno();"
         * It will suppress bogus gcc warnings in case it assumes 'errno' might
         * be 0 and thus the caller's error-handling might not be triggered. */
        assert_return(errno > 0, -EINVAL);
        തിരികെ -errno;
}

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ RET_NERRNO(പൂർണ്ണസംഖ്യ ret) {

        /* Helper to wrap system calls in to make them return negative errno errors. This brings system call
         * error handling in sync with how we usually handle errors in our own code, i.e. with immediate
         * returning of negative errno. Usage is like this:
         *
         *     …
         *     r = RET_NERRNO(unlink(t));
         *     …
         *
         * or
         *
         *     …
         *     fd = RET_NERRNO(open("/etc/fstab", O_RDONLY|O_CLOEXEC));
         *     …
         */

        എങ്കിൽ (ret < 0)
                തിരികെ negative_errno();

        തിരികെ ret;
}

/* Collect possible errors in <acc>, so that the first error can be returned.
 * Returns (possibly updated) <acc>. */
#define RET_GATHER(acc, err)                    \
        ({                                      \
                പൂർണ്ണസംഖ്യ *__a = &(acc), __e = (err); \
                എങ്കിൽ (*__a >= 0 && __e < 0)       \
                        *__a = __e;             \
                *__a;                           \
        })

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ errno_or_else(പൂർണ്ണസംഖ്യ fallback) {
        /* To be used when invoking library calls where errno handling is not defined clearly: we return
         * errno if it is set, and the specified error otherwise. The idea is that the caller initializes
         * errno to zero before doing an API call, and then uses this helper to retrieve a somewhat useful
         * error code */
        എങ്കിൽ (errno > 0)
                തിരികെ -errno;

        തിരികെ -ABS(fallback);
}

/* abs(3) says: Trying to take the absolute value of the most negative integer is not defined. */
#define _DEFINE_ABS_WRAPPER(name)                         \
        സ്ഥിതം വരിയിൽ bool ERRNO_IS_##name(intmax_t r) {  \
                എങ്കിൽ (r == INTMAX_MIN)                      \
                        തിരികെ false;                     \
                തിരികെ ERRNO_IS_NEG_##name(-ABS(r));      \
        }

/* For send()/recv() or read()/write(). */
സ്ഥിതം വരിയിൽ bool ERRNO_IS_NEG_TRANSIENT(intmax_t r) {
        തിരികെ IN_SET(r,
                      -EAGAIN,
                      -EINTR);
}
_DEFINE_ABS_WRAPPER(TRANSIENT);

/* Hint #1: ENETUNREACH happens if we try to connect to "non-existing" special IP addresses, such as ::5.
 *
 * Hint #2: The kernel sends e.g., EHOSTUNREACH or ENONET to userspace in some ICMP error cases.  See the
 *          icmp_err_convert[] in net/ipv4/icmp.c in the kernel sources.
 *
 * Hint #3: When asynchronous connect() on TCP fails because the host never acknowledges a single packet,
 *          kernel tells us that with ETIMEDOUT, see tcp(7). */
സ്ഥിതം വരിയിൽ bool ERRNO_IS_NEG_DISCONNECT(intmax_t r) {
        തിരികെ IN_SET(r,
                      -ECONNABORTED,
                      -ECONNREFUSED,
                      -ECONNRESET,
                      -EHOSTDOWN,
                      -EHOSTUNREACH,
                      -ENETDOWN,
                      -ENETRESET,
                      -ENETUNREACH,
                      -ENONET,
                      -ENOPROTOOPT,
                      -ENOTCONN,
                      -EPIPE,
                      -EPROTO,
                      -ESHUTDOWN,
                      -ETIMEDOUT);
}
_DEFINE_ABS_WRAPPER(DISCONNECT);

/* Transient errors we might get on accept() that we should ignore. As per error handling comment in
 * the accept(2) man page. */
സ്ഥിതം വരിയിൽ bool ERRNO_IS_NEG_ACCEPT_AGAIN(intmax_t r) {
        തിരികെ ERRNO_IS_NEG_DISCONNECT(r) ||
                ERRNO_IS_NEG_TRANSIENT(r) ||
                r == -EOPNOTSUPP;
}
_DEFINE_ABS_WRAPPER(ACCEPT_AGAIN);

/* Resource exhaustion, could be our fault or general system trouble */
സ്ഥിതം വരിയിൽ bool ERRNO_IS_NEG_RESOURCE(intmax_t r) {
        തിരികെ IN_SET(r,
                      -EMFILE,
                      -ENFILE,
                      -ENOMEM);
}
_DEFINE_ABS_WRAPPER(RESOURCE);

/* Seven different errors for "operation/system call/socket feature not supported" */
സ്ഥിതം വരിയിൽ bool ERRNO_IS_NEG_NOT_SUPPORTED(intmax_t r) {
        തിരികെ IN_SET(r,
                      -EOPNOTSUPP,
                      -ENOTTY,
                      -ENOSYS,
                      -EAFNOSUPPORT,
                      -EPFNOSUPPORT,
                      -EPROTONOSUPPORT,
                      -ESOCKTNOSUPPORT,
                      -ENOPROTOOPT);
}
_DEFINE_ABS_WRAPPER(NOT_SUPPORTED);

/* ioctl() with unsupported command/arg might additionally return EINVAL */
സ്ഥിതം വരിയിൽ bool ERRNO_IS_NEG_IOCTL_NOT_SUPPORTED(intmax_t r) {
        തിരികെ ERRNO_IS_NEG_NOT_SUPPORTED(r) || r == -EINVAL;
}
_DEFINE_ABS_WRAPPER(IOCTL_NOT_SUPPORTED);

/* Two different errors for access problems */
സ്ഥിതം വരിയിൽ bool ERRNO_IS_NEG_PRIVILEGE(intmax_t r) {
        തിരികെ IN_SET(r,
                      -EACCES,
                      -EPERM);
}
_DEFINE_ABS_WRAPPER(PRIVILEGE);

/* Three different errors for writing on a filesystem */
സ്ഥിതം വരിയിൽ bool ERRNO_IS_NEG_FS_WRITE_REFUSED(intmax_t r) {
        തിരികെ r == -EROFS || ERRNO_IS_NEG_PRIVILEGE(r);
}
_DEFINE_ABS_WRAPPER(FS_WRITE_REFUSED);

/* Three different errors for "not enough disk space" */
സ്ഥിതം വരിയിൽ bool ERRNO_IS_NEG_DISK_SPACE(intmax_t r) {
        തിരികെ IN_SET(r,
                      -ENOSPC,
                      -EDQUOT,
                      -EFBIG);
}
_DEFINE_ABS_WRAPPER(DISK_SPACE);

/* Three different errors for "this device does not quite exist" */
സ്ഥിതം വരിയിൽ bool ERRNO_IS_NEG_DEVICE_ABSENT(intmax_t r) {
        തിരികെ IN_SET(r,
                      -ENODEV,
                      -ENXIO,
                      -ENOENT);
}
_DEFINE_ABS_WRAPPER(DEVICE_ABSENT);

/* Device is absent or "empty". We get -ENOMEDIUM from CD/DVD devices, also in VMs. */
സ്ഥിതം വരിയിൽ bool ERRNO_IS_NEG_DEVICE_ABSENT_OR_EMPTY(intmax_t r) {
        തിരികെ ERRNO_IS_NEG_DEVICE_ABSENT(r) ||
                r == -ENOMEDIUM;
}
_DEFINE_ABS_WRAPPER(DEVICE_ABSENT_OR_EMPTY);

/* Quite often we want to handle cases where the backing FS doesn't support extended attributes at all,
 * where the path component carrying the xattr is missing, and where it simply doesn't have the requested
 * xattr the same way. Note that getxattr(2) does not enumerate -ENOENT in its own error list, but inherits
 * it via stat(2) (see ERRORS in getxattr(2)) for the path-component-missing case. */
സ്ഥിതം വരിയിൽ bool ERRNO_IS_NEG_XATTR_ABSENT(intmax_t r) {
        തിരികെ r == -ENODATA ||
                r == -ENOENT ||
                ERRNO_IS_NEG_NOT_SUPPORTED(r);
}
_DEFINE_ABS_WRAPPER(XATTR_ABSENT);
