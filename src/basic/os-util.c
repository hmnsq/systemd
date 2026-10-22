/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdlib.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "ansi-color.h"
#ಅನ್ತರ್ಭಾವ "chase.h"
#ಅನ್ತರ್ಭಾವ "dirent-util.h"
#ಅನ್ತರ್ಭಾವ "env-file.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fs-util.h"
#ಅನ್ತರ್ಭಾವ "glyph-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "os-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "string-table.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"
#ಅನ್ತರ್ಭಾವ "time-util.h"
#ಅನ್ತರ್ಭಾವ "utf8.h"
#ಅನ್ತರ್ಭಾವ "xattr-util.h"

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ image_class_table[_IMAGE_CLASS_MAX] = {
        [IMAGE_MACHINE]  = "machine",
        [IMAGE_PORTABLE] = "portable",
        [IMAGE_SYSEXT]   = "sysext",
        [IMAGE_CONFEXT]  = "confext",
};

DEFINE_STRING_TABLE_LOOKUP(image_class, ImageClass);

/* Helper struct for naming simplicity and reusability */
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ {
        ಧ್ರುವ ವರ್ಣ *release_file_directory;
        ಧ್ರುವ ವರ್ಣ *release_file_path_prefix;
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

bool image_name_is_valid(ಧ್ರುವ ವರ್ಣ *s) {
        ಯದಿ (!filename_is_valid(s))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (string_has_cc(s, ರಿಕ್ತ))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (!utf8_is_valid(s))
                ಪ್ರತ್ಯಾಗಮ false;

        /* Temporary files for atomically creating new files */
        ಯದಿ (startswith(s, ".#"))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ true;
}

ಪೂರ್ಣಾಙ್ಕ path_extract_image_name(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *fn = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Extract last component from path, without any "/" suffixes. */
        r = path_extract_filename(path, &fn);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (r != O_DIRECTORY) {
                /* Chop off any image suffixes we recognize (unless we already know this must refer to some dir) */
                ವರ್ಣ *m = ENDSWITH_SET(fn, ".sysext.raw", ".confext.raw", ".raw");
                ಯದಿ (m)
                        *m = 0;
        }

        /* Truncate the version/counting suffixes */
        fn[strcspn(fn, "_+")] = 0;

        ಯದಿ (!image_name_is_valid(fn))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        *ret = TAKE_PTR(fn);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ path_is_extension_ವೃಕ್ಷ(ImageClass image_class, ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *extension, bool relax_extension_release_check) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);

        /* Does the path exist at all? If not, generate an error immediately. This is useful so that a missing root dir
         * always results in -ENOENT, and we can properly distinguish the case where the whole root doesn't exist from
         * the case where just the os-release file is missing. */
        r = access_nofollow(path, F_OK);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* We use /usr/lib/extension-release.d/extension-release[.NAME] as flag for something being a system extension,
         * /etc/extension-release.d/extension-release[.NAME] as flag for something being a system configuration, and finally,
         * and {/etc|/usr/lib}/os-release as a flag for something being an OS (when not an extension). */
        r = open_extension_release(path, image_class, extension, relax_extension_release_check, ರಿಕ್ತ, ರಿಕ್ತ);
        ಯದಿ (r == -ENOENT) /* We got nothing */
                ಪ್ರತ್ಯಾಗಮ 0;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ 1;
}

ಪೂರ್ಣಾಙ್ಕ fd_is_os_ವೃಕ್ಷ(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        r = open_extension_release_at(
                        fd,
                        IMAGE_MACHINE,
                        /* extension= */ ರಿಕ್ತ,
                        /* relax_extension_release_check= */ false,
                        /* ret_path= */ ರಿಕ್ತ,
                        /* ret_fd= */ ರಿಕ್ತ);
        ಯದಿ (r == -ENOENT)
                ಪ್ರತ್ಯಾಗಮ false;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ true;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ extension_release_strict_xattr_ಮೌಲ್ಯ(ಪೂರ್ಣಾಙ್ಕ extension_release_fd, ಧ್ರುವ ವರ್ಣ *extension_release_dir_path, ಧ್ರುವ ವರ್ಣ *filename) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(extension_release_fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(extension_release_dir_path);
        ಪ್ರತಿಜ್ಞಾ(filename);

        /* No xattr or cannot parse it? Then skip this. */
        r = getxattr_at_bool(extension_release_fd, /* path= */ ರಿಕ್ತ, "user.extension-release.strict", /* at_flags= */ 0);
        ಯದಿ (ERRNO_IS_NEG_XATTR_ABSENT(r))
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "%s/%s does not have user.extension-release.strict xattr, ignoring.",
                                       extension_release_dir_path, filename);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "%s/%s: Failed to read 'user.extension-release.strict' extended attribute from file, ignoring: %m",
                                       extension_release_dir_path, filename);

        /* Explicitly set to request strict matching? Skip it. */
        ಯದಿ (r > 0) {
                log_debug("%s/%s: 'user.extension-release.strict' attribute is true, ignoring file.",
                          extension_release_dir_path, filename);
                ಪ್ರತ್ಯಾಗಮ true;
        }

        log_debug("%s/%s: 'user.extension-release.strict' attribute is false%s",
                  extension_release_dir_path, filename,
                  glyph(GLYPH_ELLIPSIS));

        ಪ್ರತ್ಯಾಗಮ false;
}

ಪೂರ್ಣಾಙ್ಕ open_os_release_at(ಪೂರ್ಣಾಙ್ಕ rfd, ವರ್ಣ **ret_path, ಪೂರ್ಣಾಙ್ಕ *ret_fd) {
        ಧ್ರುವ ವರ್ಣ *e;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(rfd));

        e = secure_getenv("SYSTEMD_OS_RELEASE");
        ಯದಿ (e)
                ಪ್ರತ್ಯಾಗಮ chaseat(rfd, rfd, e, /* flags= */ 0, ret_path, ret_fd);

        FOREACH_STRING(path, "/etc/os-release", "/usr/lib/os-release") {
                r = chaseat(rfd, rfd, path, /* flags= */ 0, ret_path, ret_fd);
                ಯದಿ (r != -ENOENT)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಪ್ರತ್ಯಾಗಮ -ENOENT;
}

ಪೂರ್ಣಾಙ್ಕ open_os_release(ಧ್ರುವ ವರ್ಣ *ಬೇರು, ವರ್ಣ **ret_path, ಪೂರ್ಣಾಙ್ಕ *ret_fd) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (!empty_or_ಬೇರು(ಬೇರು)) {
                rfd = open(ಬೇರು, O_CLOEXEC | O_DIRECTORY | O_PATH);
                ಯದಿ (rfd < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
        }

        r = open_os_release_at(rfd, ret_path ? &p : ರಿಕ್ತ, ret_fd ? &fd : ರಿಕ್ತ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret_path) {
                r = chaseat_prefix_ಬೇರು(p, ಬೇರು, ret_path);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಯದಿ (ret_fd)
                *ret_fd = TAKE_FD(fd);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ open_extension_release_at(
                ಪೂರ್ಣಾಙ್ಕ rfd,
                ImageClass image_class,
                ಧ್ರುವ ವರ್ಣ *extension,
                bool relax_extension_release_check,
                ವರ್ಣ **ret_path,
                ಪೂರ್ಣಾಙ್ಕ *ret_fd) {

        _cleanup_free_ ವರ್ಣ *dir_path = ರಿಕ್ತ, *path_found = ರಿಕ್ತ;
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd_found = -EBADF;
        _cleanup_closedir_ DIR *dir = ರಿಕ್ತ;
        bool found = false;
        ಧ್ರುವ ವರ್ಣ *p;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(rfd));
        ಪ್ರತಿಜ್ಞಾ(!extension || (image_class >= 0 && image_class < _IMAGE_CLASS_MAX));

        ಯದಿ (!extension)
                ಪ್ರತ್ಯಾಗಮ open_os_release_at(rfd, ret_path, ret_fd);

        ಯದಿ (!IN_SET(image_class, IMAGE_SYSEXT, IMAGE_CONFEXT))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (!image_name_is_valid(extension))
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The extension name %s is invalid.", extension);

        p = strjoina(image_class_release_info[image_class].release_file_path_prefix, extension);
        r = chaseat(rfd, rfd, p, /* flags= */ 0, ret_path, ret_fd);
        log_full_errno_zerook(LOG_DEBUG, MIN(r, 0), "Checking for %s: %m", p);
        ಯದಿ (r != -ENOENT)
                ಪ್ರತ್ಯಾಗಮ r;

        /* Cannot find the expected extension-release file? The image filename might have been mangled on
         * deployment, so fallback to checking for any file in the extension-release.d directory, and return
         * the first one with a user.extension-release xattr instead. The user.extension-release.strict
         * xattr is checked to ensure the author of the image considers it OK if names do not match. */

        p = image_class_release_info[image_class].release_file_directory;
        r = chase_and_opendirat(rfd, rfd, p, /* chase_flags= */ 0, &dir_path, &dir);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "Cannot open %s, ignoring: %m", p);

        FOREACH_DIRENT(de, dir, ಪ್ರತ್ಯಾಗಮ -errno) {
                _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
                ಧ್ರುವ ವರ್ಣ *image_name;

                ಯದಿ (!IN_SET(de->d_type, DT_REG, DT_UNKNOWN))
                        ಅನುವರ್ತ;

                image_name = startswith(de->d_name, "extension-release.");
                ಯದಿ (!image_name)
                        ಅನುವರ್ತ;

                ಯದಿ (!image_name_is_valid(image_name)) {
                        log_debug("%s/%s is not a valid release file name, ignoring.", dir_path, de->d_name);
                        ಅನುವರ್ತ;
                }

                /* We already chased the directory, and checked that this is a real file, so we shouldn't
                 * fail to open it. */
                fd = openat(dirfd(dir), de->d_name, O_PATH|O_CLOEXEC|O_NOFOLLOW);
                ಯದಿ (fd < 0)
                        ಪ್ರತ್ಯಾಗಮ log_debug_errno(errno, "Failed to open release file %s/%s: %m", dir_path, de->d_name);

                /* Really ensure it is a regular file after we open it. */
                r = fd_verify_regular(fd);
                ಯದಿ (r < 0) {
                        log_debug_errno(r, "%s/%s is not a regular file, ignoring: %m", dir_path, de->d_name);
                        ಅನುವರ್ತ;
                }

                ಯದಿ (!relax_extension_release_check) {
                        _cleanup_free_ ವರ್ಣ *base_extension = ರಿಕ್ತ;

                        r = path_extract_image_name(extension, &base_extension);
                        ಯದಿ (r < 0) {
                                log_debug_errno(r, "Failed to extract image name from %s, ignoring: %m", extension);
                                ಅನುವರ್ತ;
                        }

                        ಯದಿ (!streq(image_name, base_extension) &&
                            extension_release_strict_xattr_ಮೌಲ್ಯ(fd, dir_path, image_name) != 0)
                                ಅನುವರ್ತ;
                }

                /* We already found what we were looking for, but there's another candidate? We treat this as
                 * an error, as we want to enforce that there are no ambiguities in case we are in the
                 * fallback path. */
                ಯದಿ (found)
                        ಪ್ರತ್ಯಾಗಮ -ENOTUNIQ;

                found = true;

                ಯದಿ (ret_fd)
                        fd_found = TAKE_FD(fd);

                ಯದಿ (ret_path) {
                        path_found = path_join(dir_path, de->d_name);
                        ಯದಿ (!path_found)
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                }
        }
        ಯದಿ (!found)
                ಪ್ರತ್ಯಾಗಮ -ENOENT;

        ಯದಿ (ret_fd)
                *ret_fd = TAKE_FD(fd_found);
        ಯದಿ (ret_path)
                *ret_path = TAKE_PTR(path_found);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ open_extension_release(
                ಧ್ರುವ ವರ್ಣ *ಬೇರು,
                ImageClass image_class,
                ಧ್ರುವ ವರ್ಣ *extension,
                bool relax_extension_release_check,
                ವರ್ಣ **ret_path,
                ಪೂರ್ಣಾಙ್ಕ *ret_fd) {

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (!empty_or_ಬೇರು(ಬೇರು)) {
                rfd = open(ಬೇರು, O_CLOEXEC | O_DIRECTORY | O_PATH);
                ಯದಿ (rfd < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
        }

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check,
                                      ret_path ? &p : ರಿಕ್ತ, ret_fd ? &fd : ರಿಕ್ತ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret_path) {
                r = chaseat_prefix_ಬೇರು(p, ಬೇರು, ret_path);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಯದಿ (ret_fd)
                *ret_fd = TAKE_FD(fd);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parse_extension_release_atv(
                ಪೂರ್ಣಾಙ್ಕ rfd,
                ImageClass image_class,
                ಧ್ರುವ ವರ್ಣ *extension,
                bool relax_extension_release_check,
                va_list ap) {

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(rfd));

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check, &p, &fd);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ parse_env_file_fdv(fd, p, ap);
}

ಪೂರ್ಣಾಙ್ಕ parse_extension_release_at_sentinel(
                ಪೂರ್ಣಾಙ್ಕ rfd,
                ImageClass image_class,
                bool relax_extension_release_check,
                ಧ್ರುವ ವರ್ಣ *extension,
                ...) {

        va_list ap;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(rfd));

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);
        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ parse_extension_release_sentinel(
                ಧ್ರುವ ವರ್ಣ *ಬೇರು,
                ImageClass image_class,
                bool relax_extension_release_check,
                ಧ್ರುವ ವರ್ಣ *extension,
                ...) {

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ rfd = XAT_FDROOT;
        va_list ap;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (!empty_or_ಬೇರು(ಬೇರು)) {
                rfd = open(ಬೇರು, O_CLOEXEC | O_DIRECTORY | O_PATH);
                ಯದಿ (rfd < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
        }

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ load_os_release_pairs_with_prefix(ಧ್ರುವ ವರ್ಣ *ಬೇರು, ಧ್ರುವ ವರ್ಣ *prefix, ವರ್ಣ ***ret) {
        _cleanup_strv_free_ ವರ್ಣ **os_release_pairs = ರಿಕ್ತ, **os_release_pairs_prefixed = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = load_os_release_pairs(ಬೇರು, &os_release_pairs);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        STRV_FOREACH_PAIR(p, q, os_release_pairs) {
                ವರ್ಣ *ಸಾಲು;

                /* We strictly return only the four main ID fields and ignore the rest */
                ಯದಿ (!STR_IN_SET(*p, "ID", "VERSION_ID", "BUILD_ID", "VARIANT_ID"))
                        ಅನುವರ್ತ;

                ascii_strlower(*p);
                ಸಾಲು = strjoin(prefix, *p, "=", *q);
                ಯದಿ (!ಸಾಲು)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                r = strv_consume(&os_release_pairs_prefixed, ಸಾಲು);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        *ret = TAKE_PTR(os_release_pairs_prefixed);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ load_extension_release_pairs(ಧ್ರುವ ವರ್ಣ *ಬೇರು, ImageClass image_class, ಧ್ರುವ ವರ್ಣ *extension, bool relax_extension_release_check, ವರ್ಣ ***ret) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        r = open_extension_release(ಬೇರು, image_class, extension, relax_extension_release_check, &p, &fd);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ load_env_file_pairs_fd(fd, p, ret);
}

ಪೂರ್ಣಾಙ್ಕ os_release_support_ended(ಧ್ರುವ ವರ್ಣ *support_end, bool quiet, usec_t *ret_eol) {
        _cleanup_free_ ವರ್ಣ *_support_end_alloc = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (!support_end) {
                /* If the caller has the variably handy, they can pass it in. If not, we'll read it
                 * ourselves. */

                r = parse_os_release(ರಿಕ್ತ,
                                     "SUPPORT_END", &_support_end_alloc);
                ಯದಿ (r < 0 && r != -ENOENT)
                        ಪ್ರತ್ಯಾಗಮ log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                              "Failed to read os-release file, ignoring: %m");

                support_end = _support_end_alloc;
        }

        ಯದಿ (isempty(support_end)) { /* An empty string is a explicit way to say "no EOL exists" */
                ಯದಿ (ret_eol)
                        *ret_eol = USEC_INFINITY;

                ಪ್ರತ್ಯಾಗಮ false;  /* no end date defined */
        }

        ಸಂರಚನಾ tm tm = {};
        ಧ್ರುವ ವರ್ಣ *k = strptime(support_end, "%Y-%m-%d", &tm);
        ಯದಿ (!k || *k)
                ಪ್ರತ್ಯಾಗಮ log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, SYNTHETIC_ERRNO(EINVAL),
                                      "Failed to parse SUPPORT_END= from os-release file, ignoring: %s", support_end);

        usec_t eol;
        r = mktime_or_timegm_usec(&tm, /* utc= */ true, &eol);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                      "Failed to convert SUPPORT_END= time from os-release file, ignoring: %m");

        ಯದಿ (ret_eol)
                *ret_eol = eol;

        ಪ್ರತ್ಯಾಗಮ now(CLOCK_REALTIME) > eol;
}

ಧ್ರುವ ವರ್ಣ* os_release_pretty_name(ಧ್ರುವ ವರ್ಣ *pretty_name, ಧ್ರುವ ವರ್ಣ *name) {
        /* Distills a "pretty" name to show from os-release data. First argument is supposed to be the
         * PRETTY_NAME= field, the second one the NAME= field. This function is trivial, of course, and
         * exists mostly to ensure we use the same logic wherever possible. */

        ಪ್ರತ್ಯಾಗಮ empty_to_null(pretty_name) ?:
                empty_to_null(name) ?: "Linux";
}

ವರ್ಣ *unescape_fancy_name(ವರ್ಣ **fancy_name) {
        ಪ್ರತಿಜ್ಞಾ(fancy_name);

        /* Checks if the fancy name is valid, unescapes if it is, nullifies it if not */

        _cleanup_free_ ವರ್ಣ *unescaped_fancy_name = ರಿಕ್ತ;

        ಯದಿ (isempty(*fancy_name))
                ಗಚ್ಛ clear;

        /* We undo one level of C escapes on this */
        ssize_t n = cunescape(*fancy_name, /* flags= */ 0, &unescaped_fancy_name);
        ಯದಿ (n < 0) {
                log_debug_errno((ಪೂರ್ಣಾಙ್ಕ) n, "Failed to unescape FANCY_NAME= string, suppressing: %m");
                ಗಚ್ಛ clear;
        }

        ಯದಿ (!utf8_is_valid(unescaped_fancy_name)) {
                log_debug("Unescaped FANCY_NAME= string is not valid UTF-8, suppressing.");
                ಗಚ್ಛ clear;
        }

        free_and_replace(*fancy_name, unescaped_fancy_name);
        ಪ್ರತ್ಯಾಗಮ *fancy_name;

clear:
        *fancy_name = mfree(*fancy_name);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

bool use_fancy_name(ಧ್ರುವ ವರ್ಣ *fancy_name) {

        /* Decides whether to show the specified fancy name */

        ಯದಿ (isempty(fancy_name))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (!colors_enabled())
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ emoji_enabled() || ascii_is_valid(fancy_name);
}
