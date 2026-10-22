/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <stdlib.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "ansi-color.h"
#அந்தர்பாவ "chase.h"
#அந்தர்பாவ "dirent-util.h"
#அந்தர்பாவ "env-file.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "escape.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fs-util.h"
#அந்தர்பாவ "glyph-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "os-util.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "stat-util.h"
#அந்தர்பாவ "string-table.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"
#அந்தர்பாவ "time-util.h"
#அந்தர்பாவ "utf8.h"
#அந்தர்பாவ "xattr-util.h"

நிலையான மாறிலி எழுத்து* மாறிலி image_class_table[_IMAGE_CLASS_MAX] = {
        [IMAGE_MACHINE]  = "machine",
        [IMAGE_PORTABLE] = "portable",
        [IMAGE_SYSEXT]   = "sysext",
        [IMAGE_CONFEXT]  = "confext",
};

DEFINE_STRING_TABLE_LOOKUP(image_class, ImageClass);

/* Helper struct for naming simplicity and reusability */
நிலையான மாறிலி கட்டமைப்பு {
        மாறிலி எழுத்து *release_file_directory;
        மாறிலி எழுத்து *release_file_path_prefix;
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

bool image_name_is_valid(மாறிலி எழுத்து *s) {
        எனில் (!filename_is_valid(s))
                திரும்பு false;

        எனில் (string_has_cc(s, வெற்று))
                திரும்பு false;

        எனில் (!utf8_is_valid(s))
                திரும்பு false;

        /* Temporary files for atomically creating new files */
        எனில் (startswith(s, ".#"))
                திரும்பு false;

        திரும்பு true;
}

முழு path_extract_image_name(மாறிலி எழுத்து *path, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *fn = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(ret);

        /* Extract last component from path, without any "/" suffixes. */
        r = path_extract_filename(path, &fn);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r != O_DIRECTORY) {
                /* Chop off any image suffixes we recognize (unless we already know this must refer to some dir) */
                எழுத்து *m = ENDSWITH_SET(fn, ".sysext.raw", ".confext.raw", ".raw");
                எனில் (m)
                        *m = 0;
        }

        /* Truncate the version/counting suffixes */
        fn[strcspn(fn, "_+")] = 0;

        எனில் (!image_name_is_valid(fn))
                திரும்பு -EINVAL;

        *ret = TAKE_PTR(fn);
        திரும்பு 0;
}

முழு path_is_extension_மரம்(ImageClass image_class, மாறிலி எழுத்து *path, மாறிலி எழுத்து *extension, bool relax_extension_release_check) {
        முழு r;

        ப்ரதிஜ்ஞா(path);

        /* Does the path exist at all? If not, generate an error immediately. This is useful so that a missing root dir
         * always results in -ENOENT, and we can properly distinguish the case where the whole root doesn't exist from
         * the case where just the os-release file is missing. */
        r = access_nofollow(path, F_OK);
        எனில் (r < 0)
                திரும்பு r;

        /* We use /usr/lib/extension-release.d/extension-release[.NAME] as flag for something being a system extension,
         * /etc/extension-release.d/extension-release[.NAME] as flag for something being a system configuration, and finally,
         * and {/etc|/usr/lib}/os-release as a flag for something being an OS (when not an extension). */
        r = open_extension_release(path, image_class, extension, relax_extension_release_check, வெற்று, வெற்று);
        எனில் (r == -ENOENT) /* We got nothing */
                திரும்பு 0;
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு 1;
}

முழு fd_is_os_மரம்(முழு fd) {
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);

        r = open_extension_release_at(
                        fd,
                        IMAGE_MACHINE,
                        /* extension= */ வெற்று,
                        /* relax_extension_release_check= */ false,
                        /* ret_path= */ வெற்று,
                        /* ret_fd= */ வெற்று);
        எனில் (r == -ENOENT)
                திரும்பு false;
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு true;
}

நிலையான முழு extension_release_strict_xattr_மதிப்பு(முழு extension_release_fd, மாறிலி எழுத்து *extension_release_dir_path, மாறிலி எழுத்து *filename) {
        முழு r;

        ப்ரதிஜ்ஞா(extension_release_fd >= 0);
        ப்ரதிஜ்ஞா(extension_release_dir_path);
        ப்ரதிஜ்ஞா(filename);

        /* No xattr or cannot parse it? Then skip this. */
        r = getxattr_at_bool(extension_release_fd, /* path= */ வெற்று, "user.extension-release.strict", /* at_flags= */ 0);
        எனில் (ERRNO_IS_NEG_XATTR_ABSENT(r))
                திரும்பு log_debug_errno(r, "%s/%s does not have user.extension-release.strict xattr, ignoring.",
                                       extension_release_dir_path, filename);
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "%s/%s: Failed to read 'user.extension-release.strict' extended attribute from file, ignoring: %m",
                                       extension_release_dir_path, filename);

        /* Explicitly set to request strict matching? Skip it. */
        எனில் (r > 0) {
                log_debug("%s/%s: 'user.extension-release.strict' attribute is true, ignoring file.",
                          extension_release_dir_path, filename);
                திரும்பு true;
        }

        log_debug("%s/%s: 'user.extension-release.strict' attribute is false%s",
                  extension_release_dir_path, filename,
                  glyph(GLYPH_ELLIPSIS));

        திரும்பு false;
}

முழு open_os_release_at(முழு rfd, எழுத்து **ret_path, முழு *ret_fd) {
        மாறிலி எழுத்து *e;
        முழு r;

        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(rfd));

        e = secure_getenv("SYSTEMD_OS_RELEASE");
        எனில் (e)
                திரும்பு chaseat(rfd, rfd, e, /* flags= */ 0, ret_path, ret_fd);

        FOREACH_STRING(path, "/etc/os-release", "/usr/lib/os-release") {
                r = chaseat(rfd, rfd, path, /* flags= */ 0, ret_path, ret_fd);
                எனில் (r != -ENOENT)
                        திரும்பு r;
        }

        திரும்பு -ENOENT;
}

முழு open_os_release(மாறிலி எழுத்து *வேர், எழுத்து **ret_path, முழு *ret_fd) {
        _cleanup_close_ முழு rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ எழுத்து *p = வெற்று;
        முழு r;

        எனில் (!empty_or_வேர்(வேர்)) {
                rfd = open(வேர், O_CLOEXEC | O_DIRECTORY | O_PATH);
                எனில் (rfd < 0)
                        திரும்பு -errno;
        }

        r = open_os_release_at(rfd, ret_path ? &p : வெற்று, ret_fd ? &fd : வெற்று);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret_path) {
                r = chaseat_prefix_வேர்(p, வேர், ret_path);
                எனில் (r < 0)
                        திரும்பு r;
        }

        எனில் (ret_fd)
                *ret_fd = TAKE_FD(fd);

        திரும்பு 0;
}

முழு open_extension_release_at(
                முழு rfd,
                ImageClass image_class,
                மாறிலி எழுத்து *extension,
                bool relax_extension_release_check,
                எழுத்து **ret_path,
                முழு *ret_fd) {

        _cleanup_free_ எழுத்து *dir_path = வெற்று, *path_found = வெற்று;
        _cleanup_close_ முழு fd_found = -EBADF;
        _cleanup_closedir_ DIR *dir = வெற்று;
        bool found = false;
        மாறிலி எழுத்து *p;
        முழு r;

        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(rfd));
        ப்ரதிஜ்ஞா(!extension || (image_class >= 0 && image_class < _IMAGE_CLASS_MAX));

        எனில் (!extension)
                திரும்பு open_os_release_at(rfd, ret_path, ret_fd);

        எனில் (!IN_SET(image_class, IMAGE_SYSEXT, IMAGE_CONFEXT))
                திரும்பு -EINVAL;

        எனில் (!image_name_is_valid(extension))
                திரும்பு log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The extension name %s is invalid.", extension);

        p = strjoina(image_class_release_info[image_class].release_file_path_prefix, extension);
        r = chaseat(rfd, rfd, p, /* flags= */ 0, ret_path, ret_fd);
        log_full_errno_zerook(LOG_DEBUG, MIN(r, 0), "Checking for %s: %m", p);
        எனில் (r != -ENOENT)
                திரும்பு r;

        /* Cannot find the expected extension-release file? The image filename might have been mangled on
         * deployment, so fallback to checking for any file in the extension-release.d directory, and return
         * the first one with a user.extension-release xattr instead. The user.extension-release.strict
         * xattr is checked to ensure the author of the image considers it OK if names do not match. */

        p = image_class_release_info[image_class].release_file_directory;
        r = chase_and_opendirat(rfd, rfd, p, /* chase_flags= */ 0, &dir_path, &dir);
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Cannot open %s, ignoring: %m", p);

        FOREACH_DIRENT(de, dir, திரும்பு -errno) {
                _cleanup_close_ முழு fd = -EBADF;
                மாறிலி எழுத்து *image_name;

                எனில் (!IN_SET(de->d_type, DT_REG, DT_UNKNOWN))
                        தொடர்;

                image_name = startswith(de->d_name, "extension-release.");
                எனில் (!image_name)
                        தொடர்;

                எனில் (!image_name_is_valid(image_name)) {
                        log_debug("%s/%s is not a valid release file name, ignoring.", dir_path, de->d_name);
                        தொடர்;
                }

                /* We already chased the directory, and checked that this is a real file, so we shouldn't
                 * fail to open it. */
                fd = openat(dirfd(dir), de->d_name, O_PATH|O_CLOEXEC|O_NOFOLLOW);
                எனில் (fd < 0)
                        திரும்பு log_debug_errno(errno, "Failed to open release file %s/%s: %m", dir_path, de->d_name);

                /* Really ensure it is a regular file after we open it. */
                r = fd_verify_regular(fd);
                எனில் (r < 0) {
                        log_debug_errno(r, "%s/%s is not a regular file, ignoring: %m", dir_path, de->d_name);
                        தொடர்;
                }

                எனில் (!relax_extension_release_check) {
                        _cleanup_free_ எழுத்து *base_extension = வெற்று;

                        r = path_extract_image_name(extension, &base_extension);
                        எனில் (r < 0) {
                                log_debug_errno(r, "Failed to extract image name from %s, ignoring: %m", extension);
                                தொடர்;
                        }

                        எனில் (!streq(image_name, base_extension) &&
                            extension_release_strict_xattr_மதிப்பு(fd, dir_path, image_name) != 0)
                                தொடர்;
                }

                /* We already found what we were looking for, but there's another candidate? We treat this as
                 * an error, as we want to enforce that there are no ambiguities in case we are in the
                 * fallback path. */
                எனில் (found)
                        திரும்பு -ENOTUNIQ;

                found = true;

                எனில் (ret_fd)
                        fd_found = TAKE_FD(fd);

                எனில் (ret_path) {
                        path_found = path_join(dir_path, de->d_name);
                        எனில் (!path_found)
                                திரும்பு -ENOMEM;
                }
        }
        எனில் (!found)
                திரும்பு -ENOENT;

        எனில் (ret_fd)
                *ret_fd = TAKE_FD(fd_found);
        எனில் (ret_path)
                *ret_path = TAKE_PTR(path_found);

        திரும்பு 0;
}

முழு open_extension_release(
                மாறிலி எழுத்து *வேர்,
                ImageClass image_class,
                மாறிலி எழுத்து *extension,
                bool relax_extension_release_check,
                எழுத்து **ret_path,
                முழு *ret_fd) {

        _cleanup_close_ முழு rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ எழுத்து *p = வெற்று;
        முழு r;

        எனில் (!empty_or_வேர்(வேர்)) {
                rfd = open(வேர், O_CLOEXEC | O_DIRECTORY | O_PATH);
                எனில் (rfd < 0)
                        திரும்பு -errno;
        }

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check,
                                      ret_path ? &p : வெற்று, ret_fd ? &fd : வெற்று);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret_path) {
                r = chaseat_prefix_வேர்(p, வேர், ret_path);
                எனில் (r < 0)
                        திரும்பு r;
        }

        எனில் (ret_fd)
                *ret_fd = TAKE_FD(fd);

        திரும்பு 0;
}

நிலையான முழு parse_extension_release_atv(
                முழு rfd,
                ImageClass image_class,
                மாறிலி எழுத்து *extension,
                bool relax_extension_release_check,
                va_list ap) {

        _cleanup_close_ முழு fd = -EBADF;
        _cleanup_free_ எழுத்து *p = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(rfd));

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check, &p, &fd);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு parse_env_file_fdv(fd, p, ap);
}

முழு parse_extension_release_at_sentinel(
                முழு rfd,
                ImageClass image_class,
                bool relax_extension_release_check,
                மாறிலி எழுத்து *extension,
                ...) {

        va_list ap;
        முழு r;

        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(rfd));

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);
        திரும்பு r;
}

முழு parse_extension_release_sentinel(
                மாறிலி எழுத்து *வேர்,
                ImageClass image_class,
                bool relax_extension_release_check,
                மாறிலி எழுத்து *extension,
                ...) {

        _cleanup_close_ முழு rfd = XAT_FDROOT;
        va_list ap;
        முழு r;

        எனில் (!empty_or_வேர்(வேர்)) {
                rfd = open(வேர், O_CLOEXEC | O_DIRECTORY | O_PATH);
                எனில் (rfd < 0)
                        திரும்பு -errno;
        }

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);

        திரும்பு r;
}

முழு load_os_release_pairs_with_prefix(மாறிலி எழுத்து *வேர், மாறிலி எழுத்து *prefix, எழுத்து ***ret) {
        _cleanup_strv_free_ எழுத்து **os_release_pairs = வெற்று, **os_release_pairs_prefixed = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        r = load_os_release_pairs(வேர், &os_release_pairs);
        எனில் (r < 0)
                திரும்பு r;

        STRV_FOREACH_PAIR(p, q, os_release_pairs) {
                எழுத்து *வரி;

                /* We strictly return only the four main ID fields and ignore the rest */
                எனில் (!STR_IN_SET(*p, "ID", "VERSION_ID", "BUILD_ID", "VARIANT_ID"))
                        தொடர்;

                ascii_strlower(*p);
                வரி = strjoin(prefix, *p, "=", *q);
                எனில் (!வரி)
                        திரும்பு -ENOMEM;
                r = strv_consume(&os_release_pairs_prefixed, வரி);
                எனில் (r < 0)
                        திரும்பு r;
        }

        *ret = TAKE_PTR(os_release_pairs_prefixed);

        திரும்பு 0;
}

முழு load_extension_release_pairs(மாறிலி எழுத்து *வேர், ImageClass image_class, மாறிலி எழுத்து *extension, bool relax_extension_release_check, எழுத்து ***ret) {
        _cleanup_close_ முழு fd = -EBADF;
        _cleanup_free_ எழுத்து *p = வெற்று;
        முழு r;

        r = open_extension_release(வேர், image_class, extension, relax_extension_release_check, &p, &fd);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு load_env_file_pairs_fd(fd, p, ret);
}

முழு os_release_support_ended(மாறிலி எழுத்து *support_end, bool quiet, usec_t *ret_eol) {
        _cleanup_free_ எழுத்து *_support_end_alloc = வெற்று;
        முழு r;

        எனில் (!support_end) {
                /* If the caller has the variably handy, they can pass it in. If not, we'll read it
                 * ourselves. */

                r = parse_os_release(வெற்று,
                                     "SUPPORT_END", &_support_end_alloc);
                எனில் (r < 0 && r != -ENOENT)
                        திரும்பு log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                              "Failed to read os-release file, ignoring: %m");

                support_end = _support_end_alloc;
        }

        எனில் (isempty(support_end)) { /* An empty string is a explicit way to say "no EOL exists" */
                எனில் (ret_eol)
                        *ret_eol = USEC_INFINITY;

                திரும்பு false;  /* no end date defined */
        }

        கட்டமைப்பு tm tm = {};
        மாறிலி எழுத்து *k = strptime(support_end, "%Y-%m-%d", &tm);
        எனில் (!k || *k)
                திரும்பு log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, SYNTHETIC_ERRNO(EINVAL),
                                      "Failed to parse SUPPORT_END= from os-release file, ignoring: %s", support_end);

        usec_t eol;
        r = mktime_or_timegm_usec(&tm, /* utc= */ true, &eol);
        எனில் (r < 0)
                திரும்பு log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                      "Failed to convert SUPPORT_END= time from os-release file, ignoring: %m");

        எனில் (ret_eol)
                *ret_eol = eol;

        திரும்பு now(CLOCK_REALTIME) > eol;
}

மாறிலி எழுத்து* os_release_pretty_name(மாறிலி எழுத்து *pretty_name, மாறிலி எழுத்து *name) {
        /* Distills a "pretty" name to show from os-release data. First argument is supposed to be the
         * PRETTY_NAME= field, the second one the NAME= field. This function is trivial, of course, and
         * exists mostly to ensure we use the same logic wherever possible. */

        திரும்பு empty_to_null(pretty_name) ?:
                empty_to_null(name) ?: "Linux";
}

எழுத்து *unescape_fancy_name(எழுத்து **fancy_name) {
        ப்ரதிஜ்ஞா(fancy_name);

        /* Checks if the fancy name is valid, unescapes if it is, nullifies it if not */

        _cleanup_free_ எழுத்து *unescaped_fancy_name = வெற்று;

        எனில் (isempty(*fancy_name))
                செல் clear;

        /* We undo one level of C escapes on this */
        ssize_t n = cunescape(*fancy_name, /* flags= */ 0, &unescaped_fancy_name);
        எனில் (n < 0) {
                log_debug_errno((முழு) n, "Failed to unescape FANCY_NAME= string, suppressing: %m");
                செல் clear;
        }

        எனில் (!utf8_is_valid(unescaped_fancy_name)) {
                log_debug("Unescaped FANCY_NAME= string is not valid UTF-8, suppressing.");
                செல் clear;
        }

        free_and_replace(*fancy_name, unescaped_fancy_name);
        திரும்பு *fancy_name;

clear:
        *fancy_name = mfree(*fancy_name);
        திரும்பு வெற்று;
}

bool use_fancy_name(மாறிலி எழுத்து *fancy_name) {

        /* Decides whether to show the specified fancy name */

        எனில் (isempty(fancy_name))
                திரும்பு false;

        எனில் (!colors_enabled())
                திரும்பு false;

        திரும்பு emoji_enabled() || ascii_is_valid(fancy_name);
}
