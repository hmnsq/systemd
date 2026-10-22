/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <stdlib.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "ansi-color.h"
#અન્તર્ભાવ "chase.h"
#અન્તર્ભાવ "dirent-util.h"
#અન્તર્ભાવ "env-file.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "escape.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fs-util.h"
#અન્તર્ભાવ "glyph-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "os-util.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "stat-util.h"
#અન્તર્ભાવ "string-table.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"
#અન્તર્ભાવ "time-util.h"
#અન્તર્ભાવ "utf8.h"
#અન્તર્ભાવ "xattr-util.h"

સ્થિર અચલ અક્ષર* અચલ image_class_table[_IMAGE_CLASS_MAX] = {
        [IMAGE_MACHINE]  = "machine",
        [IMAGE_PORTABLE] = "portable",
        [IMAGE_SYSEXT]   = "sysext",
        [IMAGE_CONFEXT]  = "confext",
};

DEFINE_STRING_TABLE_LOOKUP(image_class, ImageClass);

/* Helper struct for naming simplicity and reusability */
સ્થિર અચલ સંરચના {
        અચલ અક્ષર *release_file_directory;
        અચલ અક્ષર *release_file_path_prefix;
} image_class_release_info[_IMAGE_CLASS_MAX] = {
        [IMAGE_SYSEXT] = {
                .release_file_directory = "/usr/lib/extension-release.d/",
                .release_file_path_prefix = "/usr/lib/extension-release.d/extension-release.",
        },
        [IMAGE_CONFEXT] = {
                .release_file_directory = "/etc/extension-release.d/",
                .release_file_path_prefix = "/etc/extension-release.d/extension-release.",
        }
};

bool image_name_is_valid(અચલ અક્ષર *s) {
        જો (!filename_is_valid(s))
                પાછા false;

        જો (string_has_cc(s, ખાલી))
                પાછા false;

        જો (!utf8_is_valid(s))
                પાછા false;

        /* Temporary files for atomically creating new files */
        જો (startswith(s, ".#"))
                પાછા false;

        પાછા true;
}

પૂર્ણાંક path_extract_image_name(અચલ અક્ષર *path, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *fn = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(ret);

        /* Extract last component from path, without any "/" suffixes. */
        r = path_extract_filename(path, &fn);
        જો (r < 0)
                પાછા r;
        જો (r != O_DIRECTORY) {
                /* Chop off any image suffixes we recognize (unless we already know this must refer to some dir) */
                અક્ષર *m = ENDSWITH_SET(fn, ".sysext.raw", ".confext.raw", ".raw");
                જો (m)
                        *m = 0;
        }

        /* Truncate the version/counting suffixes */
        fn[strcspn(fn, "_+")] = 0;

        જો (!image_name_is_valid(fn))
                પાછા -EINVAL;

        *ret = TAKE_PTR(fn);
        પાછા 0;
}

પૂર્ણાંક path_is_extension_વૃક્ષ(ImageClass image_class, અચલ અક્ષર *path, અચલ અક્ષર *extension, bool relax_extension_release_check) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);

        /* Does the path exist at all? If not, generate an error immediately. This is useful so that a missing root dir
         * always results in -ENOENT, and we can properly distinguish the case where the whole root doesn't exist from
         * the case where just the os-release file is missing. */
        r = access_nofollow(path, F_OK);
        જો (r < 0)
                પાછા r;

        /* We use /usr/lib/extension-release.d/extension-release[.NAME] as flag for something being a system extension,
         * /etc/extension-release.d/extension-release[.NAME] as flag for something being a system configuration, and finally,
         * and {/etc|/usr/lib}/os-release as a flag for something being an OS (when not an extension). */
        r = open_extension_release(path, image_class, extension, relax_extension_release_check, ખાલી, ખાલી);
        જો (r == -ENOENT) /* We got nothing */
                પાછા 0;
        જો (r < 0)
                પાછા r;

        પાછા 1;
}

પૂર્ણાંક fd_is_os_વૃક્ષ(પૂર્ણાંક fd) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);

        r = open_extension_release_at(
                        fd,
                        IMAGE_MACHINE,
                        /* extension= */ ખાલી,
                        /* relax_extension_release_check= */ false,
                        /* ret_path= */ ખાલી,
                        /* ret_fd= */ ખાલી);
        જો (r == -ENOENT)
                પાછા false;
        જો (r < 0)
                પાછા r;

        પાછા true;
}

સ્થિર પૂર્ણાંક extension_release_strict_xattr_કિંમત(પૂર્ણાંક extension_release_fd, અચલ અક્ષર *extension_release_dir_path, અચલ અક્ષર *filename) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(extension_release_fd >= 0);
        પ્રતિજ્ઞા(extension_release_dir_path);
        પ્રતિજ્ઞા(filename);

        /* No xattr or cannot parse it? Then skip this. */
        r = getxattr_at_bool(extension_release_fd, /* path= */ ખાલી, "user.extension-release.strict", /* at_flags= */ 0);
        જો (ERRNO_IS_NEG_XATTR_ABSENT(r))
                પાછા log_debug_errno(r, "%s/%s does not have user.extension-release.strict xattr, ignoring.",
                                       extension_release_dir_path, filename);
        જો (r < 0)
                પાછા log_debug_errno(r, "%s/%s: Failed to read 'user.extension-release.strict' extended attribute from file, ignoring: %m",
                                       extension_release_dir_path, filename);

        /* Explicitly set to request strict matching? Skip it. */
        જો (r > 0) {
                log_debug("%s/%s: 'user.extension-release.strict' attribute is true, ignoring file.",
                          extension_release_dir_path, filename);
                પાછા true;
        }

        log_debug("%s/%s: 'user.extension-release.strict' attribute is false%s",
                  extension_release_dir_path, filename,
                  glyph(GLYPH_ELLIPSIS));

        પાછા false;
}

પૂર્ણાંક open_os_release_at(પૂર્ણાંક rfd, અક્ષર **ret_path, પૂર્ણાંક *ret_fd) {
        અચલ અક્ષર *e;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(wildcard_fd_is_valid(rfd));

        e = secure_getenv("SYSTEMD_OS_RELEASE");
        જો (e)
                પાછા chaseat(rfd, rfd, e, /* flags= */ 0, ret_path, ret_fd);

        FOREACH_STRING(path, "/etc/os-release", "/usr/lib/os-release") {
                r = chaseat(rfd, rfd, path, /* flags= */ 0, ret_path, ret_fd);
                જો (r != -ENOENT)
                        પાછા r;
        }

        પાછા -ENOENT;
}

પૂર્ણાંક open_os_release(અચલ અક્ષર *મૂળ, અક્ષર **ret_path, પૂર્ણાંક *ret_fd) {
        _cleanup_close_ પૂર્ણાંક rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ અક્ષર *p = ખાલી;
        પૂર્ણાંક r;

        જો (!empty_or_મૂળ(મૂળ)) {
                rfd = open(મૂળ, O_CLOEXEC | O_DIRECTORY | O_PATH);
                જો (rfd < 0)
                        પાછા -errno;
        }

        r = open_os_release_at(rfd, ret_path ? &p : ખાલી, ret_fd ? &fd : ખાલી);
        જો (r < 0)
                પાછા r;

        જો (ret_path) {
                r = chaseat_prefix_મૂળ(p, મૂળ, ret_path);
                જો (r < 0)
                        પાછા r;
        }

        જો (ret_fd)
                *ret_fd = TAKE_FD(fd);

        પાછા 0;
}

પૂર્ણાંક open_extension_release_at(
                પૂર્ણાંક rfd,
                ImageClass image_class,
                અચલ અક્ષર *extension,
                bool relax_extension_release_check,
                અક્ષર **ret_path,
                પૂર્ણાંક *ret_fd) {

        _cleanup_free_ અક્ષર *dir_path = ખાલી, *path_found = ખાલી;
        _cleanup_close_ પૂર્ણાંક fd_found = -EBADF;
        _cleanup_closedir_ DIR *dir = ખાલી;
        bool found = false;
        અચલ અક્ષર *p;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(wildcard_fd_is_valid(rfd));
        પ્રતિજ્ઞા(!extension || (image_class >= 0 && image_class < _IMAGE_CLASS_MAX));

        જો (!extension)
                પાછા open_os_release_at(rfd, ret_path, ret_fd);

        જો (!IN_SET(image_class, IMAGE_SYSEXT, IMAGE_CONFEXT))
                પાછા -EINVAL;

        જો (!image_name_is_valid(extension))
                પાછા log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The extension name %s is invalid.", extension);

        p = strjoina(image_class_release_info[image_class].release_file_path_prefix, extension);
        r = chaseat(rfd, rfd, p, /* flags= */ 0, ret_path, ret_fd);
        log_full_errno_zerook(LOG_DEBUG, MIN(r, 0), "Checking for %s: %m", p);
        જો (r != -ENOENT)
                પાછા r;

        /* Cannot find the expected extension-release file? The image filename might have been mangled on
         * deployment, so fallback to checking for any file in the extension-release.d directory, and return
         * the first one with a user.extension-release xattr instead. The user.extension-release.strict
         * xattr is checked to ensure the author of the image considers it OK if names do not match. */

        p = image_class_release_info[image_class].release_file_directory;
        r = chase_and_opendirat(rfd, rfd, p, /* chase_flags= */ 0, &dir_path, &dir);
        જો (r < 0)
                પાછા log_debug_errno(r, "Cannot open %s, ignoring: %m", p);

        FOREACH_DIRENT(de, dir, પાછા -errno) {
                _cleanup_close_ પૂર્ણાંક fd = -EBADF;
                અચલ અક્ષર *image_name;

                જો (!IN_SET(de->d_type, DT_REG, DT_UNKNOWN))
                        ચાલુ;

                image_name = startswith(de->d_name, "extension-release.");
                જો (!image_name)
                        ચાલુ;

                જો (!image_name_is_valid(image_name)) {
                        log_debug("%s/%s is not a valid release file name, ignoring.", dir_path, de->d_name);
                        ચાલુ;
                }

                /* We already chased the directory, and checked that this is a real file, so we shouldn't
                 * fail to open it. */
                fd = openat(dirfd(dir), de->d_name, O_PATH|O_CLOEXEC|O_NOFOLLOW);
                જો (fd < 0)
                        પાછા log_debug_errno(errno, "Failed to open release file %s/%s: %m", dir_path, de->d_name);

                /* Really ensure it is a regular file after we open it. */
                r = fd_verify_regular(fd);
                જો (r < 0) {
                        log_debug_errno(r, "%s/%s is not a regular file, ignoring: %m", dir_path, de->d_name);
                        ચાલુ;
                }

                જો (!relax_extension_release_check) {
                        _cleanup_free_ અક્ષર *base_extension = ખાલી;

                        r = path_extract_image_name(extension, &base_extension);
                        જો (r < 0) {
                                log_debug_errno(r, "Failed to extract image name from %s, ignoring: %m", extension);
                                ચાલુ;
                        }

                        જો (!streq(image_name, base_extension) &&
                            extension_release_strict_xattr_કિંમત(fd, dir_path, image_name) != 0)
                                ચાલુ;
                }

                /* We already found what we were looking for, but there's another candidate? We treat this as
                 * an error, as we want to enforce that there are no ambiguities in case we are in the
                 * fallback path. */
                જો (found)
                        પાછા -ENOTUNIQ;

                found = true;

                જો (ret_fd)
                        fd_found = TAKE_FD(fd);

                જો (ret_path) {
                        path_found = path_join(dir_path, de->d_name);
                        જો (!path_found)
                                પાછા -ENOMEM;
                }
        }
        જો (!found)
                પાછા -ENOENT;

        જો (ret_fd)
                *ret_fd = TAKE_FD(fd_found);
        જો (ret_path)
                *ret_path = TAKE_PTR(path_found);

        પાછા 0;
}

પૂર્ણાંક open_extension_release(
                અચલ અક્ષર *મૂળ,
                ImageClass image_class,
                અચલ અક્ષર *extension,
                bool relax_extension_release_check,
                અક્ષર **ret_path,
                પૂર્ણાંક *ret_fd) {

        _cleanup_close_ પૂર્ણાંક rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ અક્ષર *p = ખાલી;
        પૂર્ણાંક r;

        જો (!empty_or_મૂળ(મૂળ)) {
                rfd = open(મૂળ, O_CLOEXEC | O_DIRECTORY | O_PATH);
                જો (rfd < 0)
                        પાછા -errno;
        }

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check,
                                      ret_path ? &p : ખાલી, ret_fd ? &fd : ખાલી);
        જો (r < 0)
                પાછા r;

        જો (ret_path) {
                r = chaseat_prefix_મૂળ(p, મૂળ, ret_path);
                જો (r < 0)
                        પાછા r;
        }

        જો (ret_fd)
                *ret_fd = TAKE_FD(fd);

        પાછા 0;
}

સ્થિર પૂર્ણાંક parse_extension_release_atv(
                પૂર્ણાંક rfd,
                ImageClass image_class,
                અચલ અક્ષર *extension,
                bool relax_extension_release_check,
                va_list ap) {

        _cleanup_close_ પૂર્ણાંક fd = -EBADF;
        _cleanup_free_ અક્ષર *p = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(wildcard_fd_is_valid(rfd));

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check, &p, &fd);
        જો (r < 0)
                પાછા r;

        પાછા parse_env_file_fdv(fd, p, ap);
}

પૂર્ણાંક parse_extension_release_at_sentinel(
                પૂર્ણાંક rfd,
                ImageClass image_class,
                bool relax_extension_release_check,
                અચલ અક્ષર *extension,
                ...) {

        va_list ap;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(wildcard_fd_is_valid(rfd));

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);
        પાછા r;
}

પૂર્ણાંક parse_extension_release_sentinel(
                અચલ અક્ષર *મૂળ,
                ImageClass image_class,
                bool relax_extension_release_check,
                અચલ અક્ષર *extension,
                ...) {

        _cleanup_close_ પૂર્ણાંક rfd = XAT_FDROOT;
        va_list ap;
        પૂર્ણાંક r;

        જો (!empty_or_મૂળ(મૂળ)) {
                rfd = open(મૂળ, O_CLOEXEC | O_DIRECTORY | O_PATH);
                જો (rfd < 0)
                        પાછા -errno;
        }

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);

        પાછા r;
}

પૂર્ણાંક load_os_release_pairs_with_prefix(અચલ અક્ષર *મૂળ, અચલ અક્ષર *prefix, અક્ષર ***ret) {
        _cleanup_strv_free_ અક્ષર **os_release_pairs = ખાલી, **os_release_pairs_prefixed = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        r = load_os_release_pairs(મૂળ, &os_release_pairs);
        જો (r < 0)
                પાછા r;

        STRV_FOREACH_PAIR(p, q, os_release_pairs) {
                અક્ષર *લીટી;

                /* We strictly return only the four main ID fields and ignore the rest */
                જો (!STR_IN_SET(*p, "ID", "VERSION_ID", "BUILD_ID", "VARIANT_ID"))
                        ચાલુ;

                ascii_strlower(*p);
                લીટી = strjoin(prefix, *p, "=", *q);
                જો (!લીટી)
                        પાછા -ENOMEM;
                r = strv_consume(&os_release_pairs_prefixed, લીટી);
                જો (r < 0)
                        પાછા r;
        }

        *ret = TAKE_PTR(os_release_pairs_prefixed);

        પાછા 0;
}

પૂર્ણાંક load_extension_release_pairs(અચલ અક્ષર *મૂળ, ImageClass image_class, અચલ અક્ષર *extension, bool relax_extension_release_check, અક્ષર ***ret) {
        _cleanup_close_ પૂર્ણાંક fd = -EBADF;
        _cleanup_free_ અક્ષર *p = ખાલી;
        પૂર્ણાંક r;

        r = open_extension_release(મૂળ, image_class, extension, relax_extension_release_check, &p, &fd);
        જો (r < 0)
                પાછા r;

        પાછા load_env_file_pairs_fd(fd, p, ret);
}

પૂર્ણાંક os_release_support_ended(અચલ અક્ષર *support_end, bool quiet, usec_t *ret_eol) {
        _cleanup_free_ અક્ષર *_support_end_alloc = ખાલી;
        પૂર્ણાંક r;

        જો (!support_end) {
                /* If the caller has the variably handy, they can pass it in. If not, we'll read it
                 * ourselves. */

                r = parse_os_release(ખાલી,
                                     "SUPPORT_END", &_support_end_alloc);
                જો (r < 0 && r != -ENOENT)
                        પાછા log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                              "Failed to read os-release file, ignoring: %m");

                support_end = _support_end_alloc;
        }

        જો (isempty(support_end)) { /* An empty string is a explicit way to say "no EOL exists" */
                જો (ret_eol)
                        *ret_eol = USEC_INFINITY;

                પાછા false;  /* no end date defined */
        }

        સંરચના tm tm = {};
        અચલ અક્ષર *k = strptime(support_end, "%Y-%m-%d", &tm);
        જો (!k || *k)
                પાછા log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, SYNTHETIC_ERRNO(EINVAL),
                                      "Failed to parse SUPPORT_END= from os-release file, ignoring: %s", support_end);

        usec_t eol;
        r = mktime_or_timegm_usec(&tm, /* utc= */ true, &eol);
        જો (r < 0)
                પાછા log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                      "Failed to convert SUPPORT_END= time from os-release file, ignoring: %m");

        જો (ret_eol)
                *ret_eol = eol;

        પાછા now(CLOCK_REALTIME) > eol;
}

અચલ અક્ષર* os_release_pretty_name(અચલ અક્ષર *pretty_name, અચલ અક્ષર *name) {
        /* Distills a "pretty" name to show from os-release data. First argument is supposed to be the
         * PRETTY_NAME= field, the second one the NAME= field. This function is trivial, of course, and
         * exists mostly to ensure we use the same logic wherever possible. */

        પાછા empty_to_null(pretty_name) ?:
                empty_to_null(name) ?: "Linux";
}

અક્ષર *unescape_fancy_name(અક્ષર **fancy_name) {
        પ્રતિજ્ઞા(fancy_name);

        /* Checks if the fancy name is valid, unescapes if it is, nullifies it if not */

        _cleanup_free_ અક્ષર *unescaped_fancy_name = ખાલી;

        જો (isempty(*fancy_name))
                જાઓ clear;

        /* We undo one level of C escapes on this */
        ssize_t n = cunescape(*fancy_name, /* flags= */ 0, &unescaped_fancy_name);
        જો (n < 0) {
                log_debug_errno((પૂર્ણાંક) n, "Failed to unescape FANCY_NAME= string, suppressing: %m");
                જાઓ clear;
        }

        જો (!utf8_is_valid(unescaped_fancy_name)) {
                log_debug("Unescaped FANCY_NAME= string is not valid UTF-8, suppressing.");
                જાઓ clear;
        }

        free_and_replace(*fancy_name, unescaped_fancy_name);
        પાછા *fancy_name;

clear:
        *fancy_name = mfree(*fancy_name);
        પાછા ખાલી;
}

bool use_fancy_name(અચલ અક્ષર *fancy_name) {

        /* Decides whether to show the specified fancy name */

        જો (isempty(fancy_name))
                પાછા false;

        જો (!colors_enabled())
                પાછા false;

        પાછા emoji_enabled() || ascii_is_valid(fancy_name);
}
