/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdlib.h>

#ضمن "alloc-util.h"
#ضمن "ansi-color.h"
#ضمن "chase.h"
#ضمن "dirent-util.h"
#ضمن "env-file.h"
#ضمن "errno-util.h"
#ضمن "escape.h"
#ضمن "fd-util.h"
#ضمن "fs-util.h"
#ضمن "glyph-util.h"
#ضمن "log.h"
#ضمن "os-util.h"
#ضمن "path-util.h"
#ضمن "stat-util.h"
#ضمن "string-table.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "time-util.h"
#ضمن "utf8.h"
#ضمن "xattr-util.h"

ساكن ثابت حرف* ثابت image_class_table[_IMAGE_CLASS_MAX] = {
        [IMAGE_MACHINE]  = "machine",
        [IMAGE_PORTABLE] = "portable",
        [IMAGE_SYSEXT]   = "sysext",
        [IMAGE_CONFEXT]  = "confext",
};

DEFINE_STRING_TABLE_LOOKUP(image_class, ImageClass);

/* Helper struct for naming simplicity and reusability */
ساكن ثابت بنية {
        ثابت حرف *release_file_directory;
        ثابت حرف *release_file_path_prefix;
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

bool image_name_is_valid(ثابت حرف *s) {
        אם (!filename_is_valid(s))
                החזר false;

        אם (string_has_cc(s, عدم))
                החזר false;

        אם (!utf8_is_valid(s))
                החזר false;

        /* Temporary files for atomically creating new files */
        אם (startswith(s, ".#"))
                החזר false;

        החזר true;
}

صحيح path_extract_image_name(ثابت حرف *path, حرف **ret) {
        _cleanup_free_ حرف *fn = عدم;
        صحيح r;

        أكد(path);
        أكد(ret);

        /* Extract last component from path, without any "/" suffixes. */
        r = path_extract_filename(path, &fn);
        אם (r < 0)
                החזר r;
        אם (r != O_DIRECTORY) {
                /* Chop off any image suffixes we recognize (unless we already know this must refer to some dir) */
                حرف *m = ENDSWITH_SET(fn, ".sysext.raw", ".confext.raw", ".raw");
                אם (m)
                        *m = 0;
        }

        /* Truncate the version/counting suffixes */
        fn[strcspn(fn, "_+")] = 0;

        אם (!image_name_is_valid(fn))
                החזר -EINVAL;

        *ret = TAKE_PTR(fn);
        החזר 0;
}

صحيح path_is_extension_tree(ImageClass image_class, ثابت حرف *path, ثابت حرف *extension, bool relax_extension_release_check) {
        صحيح r;

        أكد(path);

        /* Does the path exist at all? If not, generate an error immediately. This is useful so that a missing root dir
         * always results in -ENOENT, and we can properly distinguish the case where the whole root doesn't exist from
         * the case where just the os-release file is missing. */
        r = access_nofollow(path, F_OK);
        אם (r < 0)
                החזר r;

        /* We use /usr/lib/extension-release.d/extension-release[.NAME] as flag for something being a system extension,
         * /etc/extension-release.d/extension-release[.NAME] as flag for something being a system configuration, and finally,
         * and {/etc|/usr/lib}/os-release as a flag for something being an OS (when not an extension). */
        r = open_extension_release(path, image_class, extension, relax_extension_release_check, عدم, عدم);
        אם (r == -ENOENT) /* We got nothing */
                החזר 0;
        אם (r < 0)
                החזר r;

        החזר 1;
}

صحيح fd_is_os_tree(صحيح fd) {
        صحيح r;

        أكد(fd >= 0);

        r = open_extension_release_at(
                        fd,
                        IMAGE_MACHINE,
                        /* extension= */ عدم,
                        /* relax_extension_release_check= */ false,
                        /* ret_path= */ عدم,
                        /* ret_fd= */ عدم);
        אם (r == -ENOENT)
                החזר false;
        אם (r < 0)
                החזר r;

        החזר true;
}

ساكن صحيح extension_release_strict_xattr_value(صحيح extension_release_fd, ثابت حرف *extension_release_dir_path, ثابت حرف *filename) {
        صحيح r;

        أكد(extension_release_fd >= 0);
        أكد(extension_release_dir_path);
        أكد(filename);

        /* No xattr or cannot parse it? Then skip this. */
        r = getxattr_at_bool(extension_release_fd, /* path= */ عدم, "user.extension-release.strict", /* at_flags= */ 0);
        אם (ERRNO_IS_NEG_XATTR_ABSENT(r))
                החזר log_debug_errno(r, "%s/%s does not have user.extension-release.strict xattr, ignoring.",
                                       extension_release_dir_path, filename);
        אם (r < 0)
                החזר log_debug_errno(r, "%s/%s: Failed to read 'user.extension-release.strict' extended attribute from file, ignoring: %m",
                                       extension_release_dir_path, filename);

        /* Explicitly set to request strict matching? Skip it. */
        אם (r > 0) {
                log_debug("%s/%s: 'user.extension-release.strict' attribute is true, ignoring file.",
                          extension_release_dir_path, filename);
                החזר true;
        }

        log_debug("%s/%s: 'user.extension-release.strict' attribute is false%s",
                  extension_release_dir_path, filename,
                  glyph(GLYPH_ELLIPSIS));

        החזר false;
}

صحيح open_os_release_at(صحيح rfd, حرف **ret_path, صحيح *ret_fd) {
        ثابت حرف *e;
        صحيح r;

        أكد(wildcard_fd_is_valid(rfd));

        e = secure_getenv("SYSTEMD_OS_RELEASE");
        אם (e)
                החזר chaseat(rfd, rfd, e, /* flags= */ 0, ret_path, ret_fd);

        FOREACH_STRING(path, "/etc/os-release", "/usr/lib/os-release") {
                r = chaseat(rfd, rfd, path, /* flags= */ 0, ret_path, ret_fd);
                אם (r != -ENOENT)
                        החזר r;
        }

        החזר -ENOENT;
}

صحيح open_os_release(ثابت حرف *root, حرف **ret_path, صحيح *ret_fd) {
        _cleanup_close_ صحيح rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ حرف *p = عدم;
        صحيح r;

        אם (!empty_or_root(root)) {
                rfd = open(root, O_CLOEXEC | O_DIRECTORY | O_PATH);
                אם (rfd < 0)
                        החזר -errno;
        }

        r = open_os_release_at(rfd, ret_path ? &p : عدم, ret_fd ? &fd : عدم);
        אם (r < 0)
                החזר r;

        אם (ret_path) {
                r = chaseat_prefix_root(p, root, ret_path);
                אם (r < 0)
                        החזר r;
        }

        אם (ret_fd)
                *ret_fd = TAKE_FD(fd);

        החזר 0;
}

صحيح open_extension_release_at(
                صحيح rfd,
                ImageClass image_class,
                ثابت حرف *extension,
                bool relax_extension_release_check,
                حرف **ret_path,
                صحيح *ret_fd) {

        _cleanup_free_ حرف *dir_path = عدم, *path_found = عدم;
        _cleanup_close_ صحيح fd_found = -EBADF;
        _cleanup_closedir_ DIR *dir = عدم;
        bool found = false;
        ثابت حرف *p;
        صحيح r;

        أكد(wildcard_fd_is_valid(rfd));
        أكد(!extension || (image_class >= 0 && image_class < _IMAGE_CLASS_MAX));

        אם (!extension)
                החזר open_os_release_at(rfd, ret_path, ret_fd);

        אם (!IN_SET(image_class, IMAGE_SYSEXT, IMAGE_CONFEXT))
                החזר -EINVAL;

        אם (!image_name_is_valid(extension))
                החזר log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The extension name %s is invalid.", extension);

        p = strjoina(image_class_release_info[image_class].release_file_path_prefix, extension);
        r = chaseat(rfd, rfd, p, /* flags= */ 0, ret_path, ret_fd);
        log_full_errno_zerook(LOG_DEBUG, MIN(r, 0), "Checking for %s: %m", p);
        אם (r != -ENOENT)
                החזר r;

        /* Cannot find the expected extension-release file? The image filename might have been mangled on
         * deployment, so fallback to checking for any file in the extension-release.d directory, and return
         * the first one with a user.extension-release xattr instead. The user.extension-release.strict
         * xattr is checked to ensure the author of the image considers it OK if names do not match. */

        p = image_class_release_info[image_class].release_file_directory;
        r = chase_and_opendirat(rfd, rfd, p, /* chase_flags= */ 0, &dir_path, &dir);
        אם (r < 0)
                החזר log_debug_errno(r, "Cannot open %s, ignoring: %m", p);

        FOREACH_DIRENT(de, dir, החזר -errno) {
                _cleanup_close_ صحيح fd = -EBADF;
                ثابت حرف *image_name;

                אם (!IN_SET(de->d_type, DT_REG, DT_UNKNOWN))
                        استمر;

                image_name = startswith(de->d_name, "extension-release.");
                אם (!image_name)
                        استمر;

                אם (!image_name_is_valid(image_name)) {
                        log_debug("%s/%s is not a valid release file name, ignoring.", dir_path, de->d_name);
                        استمر;
                }

                /* We already chased the directory, and checked that this is a real file, so we shouldn't
                 * fail to open it. */
                fd = openat(dirfd(dir), de->d_name, O_PATH|O_CLOEXEC|O_NOFOLLOW);
                אם (fd < 0)
                        החזר log_debug_errno(errno, "Failed to open release file %s/%s: %m", dir_path, de->d_name);

                /* Really ensure it is a regular file after we open it. */
                r = fd_verify_regular(fd);
                אם (r < 0) {
                        log_debug_errno(r, "%s/%s is not a regular file, ignoring: %m", dir_path, de->d_name);
                        استمر;
                }

                אם (!relax_extension_release_check) {
                        _cleanup_free_ حرف *base_extension = عدم;

                        r = path_extract_image_name(extension, &base_extension);
                        אם (r < 0) {
                                log_debug_errno(r, "Failed to extract image name from %s, ignoring: %m", extension);
                                استمر;
                        }

                        אם (!streq(image_name, base_extension) &&
                            extension_release_strict_xattr_value(fd, dir_path, image_name) != 0)
                                استمر;
                }

                /* We already found what we were looking for, but there's another candidate? We treat this as
                 * an error, as we want to enforce that there are no ambiguities in case we are in the
                 * fallback path. */
                אם (found)
                        החזר -ENOTUNIQ;

                found = true;

                אם (ret_fd)
                        fd_found = TAKE_FD(fd);

                אם (ret_path) {
                        path_found = path_join(dir_path, de->d_name);
                        אם (!path_found)
                                החזר -ENOMEM;
                }
        }
        אם (!found)
                החזר -ENOENT;

        אם (ret_fd)
                *ret_fd = TAKE_FD(fd_found);
        אם (ret_path)
                *ret_path = TAKE_PTR(path_found);

        החזר 0;
}

صحيح open_extension_release(
                ثابت حرف *root,
                ImageClass image_class,
                ثابت حرف *extension,
                bool relax_extension_release_check,
                حرف **ret_path,
                صحيح *ret_fd) {

        _cleanup_close_ صحيح rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ حرف *p = عدم;
        صحيح r;

        אם (!empty_or_root(root)) {
                rfd = open(root, O_CLOEXEC | O_DIRECTORY | O_PATH);
                אם (rfd < 0)
                        החזר -errno;
        }

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check,
                                      ret_path ? &p : عدم, ret_fd ? &fd : عدم);
        אם (r < 0)
                החזר r;

        אם (ret_path) {
                r = chaseat_prefix_root(p, root, ret_path);
                אם (r < 0)
                        החזר r;
        }

        אם (ret_fd)
                *ret_fd = TAKE_FD(fd);

        החזר 0;
}

ساكن صحيح parse_extension_release_atv(
                صحيح rfd,
                ImageClass image_class,
                ثابت حرف *extension,
                bool relax_extension_release_check,
                va_list ap) {

        _cleanup_close_ صحيح fd = -EBADF;
        _cleanup_free_ حرف *p = عدم;
        صحيح r;

        أكد(wildcard_fd_is_valid(rfd));

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check, &p, &fd);
        אם (r < 0)
                החזר r;

        החזר parse_env_file_fdv(fd, p, ap);
}

صحيح parse_extension_release_at_sentinel(
                صحيح rfd,
                ImageClass image_class,
                bool relax_extension_release_check,
                ثابت حرف *extension,
                ...) {

        va_list ap;
        صحيح r;

        أكد(wildcard_fd_is_valid(rfd));

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);
        החזר r;
}

صحيح parse_extension_release_sentinel(
                ثابت حرف *root,
                ImageClass image_class,
                bool relax_extension_release_check,
                ثابت حرف *extension,
                ...) {

        _cleanup_close_ صحيح rfd = XAT_FDROOT;
        va_list ap;
        صحيح r;

        אם (!empty_or_root(root)) {
                rfd = open(root, O_CLOEXEC | O_DIRECTORY | O_PATH);
                אם (rfd < 0)
                        החזר -errno;
        }

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);

        החזר r;
}

صحيح load_os_release_pairs_with_prefix(ثابت حرف *root, ثابت حرف *prefix, حرف ***ret) {
        _cleanup_strv_free_ حرف **os_release_pairs = عدم, **os_release_pairs_prefixed = عدم;
        صحيح r;

        أكد(ret);

        r = load_os_release_pairs(root, &os_release_pairs);
        אם (r < 0)
                החזר r;

        STRV_FOREACH_PAIR(p, q, os_release_pairs) {
                حرف *line;

                /* We strictly return only the four main ID fields and ignore the rest */
                אם (!STR_IN_SET(*p, "ID", "VERSION_ID", "BUILD_ID", "VARIANT_ID"))
                        استمر;

                ascii_strlower(*p);
                line = strjoin(prefix, *p, "=", *q);
                אם (!line)
                        החזר -ENOMEM;
                r = strv_consume(&os_release_pairs_prefixed, line);
                אם (r < 0)
                        החזר r;
        }

        *ret = TAKE_PTR(os_release_pairs_prefixed);

        החזר 0;
}

صحيح load_extension_release_pairs(ثابت حرف *root, ImageClass image_class, ثابت حرف *extension, bool relax_extension_release_check, حرف ***ret) {
        _cleanup_close_ صحيح fd = -EBADF;
        _cleanup_free_ حرف *p = عدم;
        صحيح r;

        r = open_extension_release(root, image_class, extension, relax_extension_release_check, &p, &fd);
        אם (r < 0)
                החזר r;

        החזר load_env_file_pairs_fd(fd, p, ret);
}

صحيح os_release_support_ended(ثابت حرف *support_end, bool quiet, usec_t *ret_eol) {
        _cleanup_free_ حرف *_support_end_alloc = عدم;
        صحيح r;

        אם (!support_end) {
                /* If the caller has the variably handy, they can pass it in. If not, we'll read it
                 * ourselves. */

                r = parse_os_release(عدم,
                                     "SUPPORT_END", &_support_end_alloc);
                אם (r < 0 && r != -ENOENT)
                        החזר log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                              "Failed to read os-release file, ignoring: %m");

                support_end = _support_end_alloc;
        }

        אם (isempty(support_end)) { /* An empty string is a explicit way to say "no EOL exists" */
                אם (ret_eol)
                        *ret_eol = USEC_INFINITY;

                החזר false;  /* no end date defined */
        }

        بنية tm tm = {};
        ثابت حرف *k = strptime(support_end, "%Y-%m-%d", &tm);
        אם (!k || *k)
                החזר log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, SYNTHETIC_ERRNO(EINVAL),
                                      "Failed to parse SUPPORT_END= from os-release file, ignoring: %s", support_end);

        usec_t eol;
        r = mktime_or_timegm_usec(&tm, /* utc= */ true, &eol);
        אם (r < 0)
                החזר log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                      "Failed to convert SUPPORT_END= time from os-release file, ignoring: %m");

        אם (ret_eol)
                *ret_eol = eol;

        החזר now(CLOCK_REALTIME) > eol;
}

ثابت حرف* os_release_pretty_name(ثابت حرف *pretty_name, ثابت حرف *name) {
        /* Distills a "pretty" name to show from os-release data. First argument is supposed to be the
         * PRETTY_NAME= field, the second one the NAME= field. This function is trivial, of course, and
         * exists mostly to ensure we use the same logic wherever possible. */

        החזר empty_to_null(pretty_name) ?:
                empty_to_null(name) ?: "Linux";
}

حرف *unescape_fancy_name(حرف **fancy_name) {
        أكد(fancy_name);

        /* Checks if the fancy name is valid, unescapes if it is, nullifies it if not */

        _cleanup_free_ حرف *unescaped_fancy_name = عدم;

        אם (isempty(*fancy_name))
                اذهب clear;

        /* We undo one level of C escapes on this */
        ssize_t n = cunescape(*fancy_name, /* flags= */ 0, &unescaped_fancy_name);
        אם (n < 0) {
                log_debug_errno((صحيح) n, "Failed to unescape FANCY_NAME= string, suppressing: %m");
                اذهب clear;
        }

        אם (!utf8_is_valid(unescaped_fancy_name)) {
                log_debug("Unescaped FANCY_NAME= string is not valid UTF-8, suppressing.");
                اذهب clear;
        }

        free_and_replace(*fancy_name, unescaped_fancy_name);
        החזר *fancy_name;

clear:
        *fancy_name = mfree(*fancy_name);
        החזר عدم;
}

bool use_fancy_name(ثابت حرف *fancy_name) {

        /* Decides whether to show the specified fancy name */

        אם (isempty(fancy_name))
                החזר false;

        אם (!colors_enabled())
                החזר false;

        החזר emoji_enabled() || ascii_is_valid(fancy_name);
}
