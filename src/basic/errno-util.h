/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "forward.h"

/* strerror(3) says that glibc uses a maximum length of 1024 bytes. */
#define ERRNO_BUF_LEN           1024

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks
 *
 * Note that we use the GNU variant of strerror_r() here. */
#define STRERROR(errnum) strerror_r(ABS(errnum), (ଅକ୍ଷର[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)

/* A helper to print an error message or message for functions that return 0 on EOF. */
ଅଚଳ ଅକ୍ଷର* strerror_or_eof(ପୂର୍ଣ୍ଣସଂଖ୍ୟା errnum, ଅକ୍ଷର *buf, size_t buflen);
#define STRERROR_OR_EOF(errnum) strerror_or_eof(errnum, (ଅକ୍ଷର[ERRNO_BUF_LEN]){}, ERRNO_BUF_LEN)

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ _reset_errno_(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *saved_errno) {
        ଯଦି (*saved_errno < 0) /* Invalidated by UNPROTECT_ERRNO? */
                ଫେରନ୍ତୁ;

        errno = *saved_errno;
}

#define PROTECT_ERRNO                           \
        _cleanup_(_reset_errno_) _unused_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା _saved_errno_ = errno

#define UNPROTECT_ERRNO                         \
        କର {                                    \
                errno = _saved_errno_;          \
                _saved_errno_ = -1;             \
        } ଯେତେବେଳେ (false)

#define LOCAL_ERRNO(ମୂଲ୍ୟ)                      \
        PROTECT_ERRNO;                          \
        errno = ABS(ମୂଲ୍ୟ)

#define return_with_errno(r, err)                     \
        କର {                                          \
                errno = ABS(err);                     \
                ଫେରନ୍ତୁ r;                             \
        } ଯେତେବେଳେ (false)

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା negative_errno(ଶୂନ୍ୟ) {
        /* This helper should be used to shut up gcc if you know 'errno' is
         * negative. Instead of "return -errno;", use "return negative_errno();"
         * It will suppress bogus gcc warnings in case it assumes 'errno' might
         * be 0 and thus the caller's error-handling might not be triggered. */
        assert_return(errno > 0, -EINVAL);
        ଫେରନ୍ତୁ -errno;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା RET_NERRNO(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret) {

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

        ଯଦି (ret < 0)
                ଫେରନ୍ତୁ negative_errno();

        ଫେରନ୍ତୁ ret;
}

/* Collect possible errors in <acc>, so that the first error can be returned.
 * Returns (possibly updated) <acc>. */
#define RET_GATHER(acc, err)                    \
        ({                                      \
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *__a = &(acc), __e = (err); \
                ଯଦି (*__a >= 0 && __e < 0)       \
                        *__a = __e;             \
                *__a;                           \
        })

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା errno_or_else(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fallback) {
        /* To be used when invoking library calls where errno handling is not defined clearly: we return
         * errno if it is set, and the specified error otherwise. The idea is that the caller initializes
         * errno to zero before doing an API call, and then uses this helper to retrieve a somewhat useful
         * error code */
        ଯଦି (errno > 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ -ABS(fallback);
}

/* abs(3) says: Trying to take the absolute value of the most negative integer is not defined. */
#define _DEFINE_ABS_WRAPPER(name)                         \
        ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ERRNO_IS_##name(intmax_t r) {  \
                ଯଦି (r == INTMAX_MIN)                      \
                        ଫେରନ୍ତୁ false;                     \
                ଫେରନ୍ତୁ ERRNO_IS_NEG_##name(-ABS(r));      \
        }

/* For send()/recv() or read()/write(). */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ERRNO_IS_NEG_TRANSIENT(intmax_t r) {
        ଫେରନ୍ତୁ IN_SET(r,
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
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ERRNO_IS_NEG_DISCONNECT(intmax_t r) {
        ଫେରନ୍ତୁ IN_SET(r,
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
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ERRNO_IS_NEG_ACCEPT_AGAIN(intmax_t r) {
        ଫେରନ୍ତୁ ERRNO_IS_NEG_DISCONNECT(r) ||
                ERRNO_IS_NEG_TRANSIENT(r) ||
                r == -EOPNOTSUPP;
}
_DEFINE_ABS_WRAPPER(ACCEPT_AGAIN);

/* Resource exhaustion, could be our fault or general system trouble */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ERRNO_IS_NEG_RESOURCE(intmax_t r) {
        ଫେରନ୍ତୁ IN_SET(r,
                      -EMFILE,
                      -ENFILE,
                      -ENOMEM);
}
_DEFINE_ABS_WRAPPER(RESOURCE);

/* Seven different errors for "operation/system call/socket feature not supported" */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ERRNO_IS_NEG_NOT_SUPPORTED(intmax_t r) {
        ଫେରନ୍ତୁ IN_SET(r,
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
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ERRNO_IS_NEG_IOCTL_NOT_SUPPORTED(intmax_t r) {
        ଫେରନ୍ତୁ ERRNO_IS_NEG_NOT_SUPPORTED(r) || r == -EINVAL;
}
_DEFINE_ABS_WRAPPER(IOCTL_NOT_SUPPORTED);

/* Two different errors for access problems */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ERRNO_IS_NEG_PRIVILEGE(intmax_t r) {
        ଫେରନ୍ତୁ IN_SET(r,
                      -EACCES,
                      -EPERM);
}
_DEFINE_ABS_WRAPPER(PRIVILEGE);

/* Three different errors for writing on a filesystem */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ERRNO_IS_NEG_FS_WRITE_REFUSED(intmax_t r) {
        ଫେରନ୍ତୁ r == -EROFS || ERRNO_IS_NEG_PRIVILEGE(r);
}
_DEFINE_ABS_WRAPPER(FS_WRITE_REFUSED);

/* Three different errors for "not enough disk space" */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ERRNO_IS_NEG_DISK_SPACE(intmax_t r) {
        ଫେରନ୍ତୁ IN_SET(r,
                      -ENOSPC,
                      -EDQUOT,
                      -EFBIG);
}
_DEFINE_ABS_WRAPPER(DISK_SPACE);

/* Three different errors for "this device does not quite exist" */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ERRNO_IS_NEG_DEVICE_ABSENT(intmax_t r) {
        ଫେରନ୍ତୁ IN_SET(r,
                      -ENODEV,
                      -ENXIO,
                      -ENOENT);
}
_DEFINE_ABS_WRAPPER(DEVICE_ABSENT);

/* Device is absent or "empty". We get -ENOMEDIUM from CD/DVD devices, also in VMs. */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ERRNO_IS_NEG_DEVICE_ABSENT_OR_EMPTY(intmax_t r) {
        ଫେରନ୍ତୁ ERRNO_IS_NEG_DEVICE_ABSENT(r) ||
                r == -ENOMEDIUM;
}
_DEFINE_ABS_WRAPPER(DEVICE_ABSENT_OR_EMPTY);

/* Quite often we want to handle cases where the backing FS doesn't support extended attributes at all,
 * where the path component carrying the xattr is missing, and where it simply doesn't have the requested
 * xattr the same way. Note that getxattr(2) does not enumerate -ENOENT in its own error list, but inherits
 * it via stat(2) (see ERRORS in getxattr(2)) for the path-component-missing case. */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ERRNO_IS_NEG_XATTR_ABSENT(intmax_t r) {
        ଫେରନ୍ତୁ r == -ENODATA ||
                r == -ENOENT ||
                ERRNO_IS_NEG_NOT_SUPPORTED(r);
}
_DEFINE_ABS_WRAPPER(XATTR_ABSENT);
