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

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ image_class_ಕೋಷ್ಟಕ[_IMAGE_CLASS_MAX] = {
        [IMAGE_MACHINE]  = "machine",
        [IMAGE_PORTABLE] = "portable",
        [IMAGE_SYSEXT]   = "sysext",
        [IMAGE_CONFEXT]  = "confext",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(image_class, ImageClass);

/* Helper struct for naming simplicity and reusability */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *release_file_directory;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *release_file_ಮಾರ್ಗ_ಪೂರ್ವಪ್ರತ್ಯಯ;
} image_class_release_info[_IMAGE_CLASS_MAX] = {
        [IMAGE_SYSEXT] = {
                .release_file_directory = "/usr/lib/extension-release.d/",
                .release_file_ಮಾರ್ಗ_ಪೂರ್ವಪ್ರತ್ಯಯ = "/usr/lib/extension-release.d/extension-release.",
        },
        [IMAGE_CONFEXT] = {
                .release_file_directory = "/etc/extension-release.d/",
                .release_file_ಮಾರ್ಗ_ಪೂರ್ವಪ್ರತ್ಯಯ = "/etc/extension-release.d/extension-release.",
        }
};

bool image_name_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        ಆದರೆ (!filename_is_valid(s))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (string_has_cc(s, ಖಾಲಿ))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (!utf8_is_valid(s))
                ಹಿಂತಿರುಗಿ false;

        /* Temporary files for atomically creating new files */
        ಆದರೆ (startswith(s, ".#"))
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ true;
}

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_ಹೊರತೆಗೆ_image_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *fn = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Extract last component from path, without any "/" suffixes. */
        r = ಮಾರ್ಗ_ಹೊರತೆಗೆ_filename(ಮಾರ್ಗ, &fn);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r != O_DIRECTORY) {
                /* Chop off any image suffixes we recognize (unless we already know this must refer to some dir) */
                ಅಕ್ಷರ *m = ENDSWITH_ಗಣ(fn, ".sysext.raw", ".confext.raw", ".raw");
                ಆದರೆ (m)
                        *m = 0;
        }

        /* Truncate the version/counting suffixes */
        fn[strcspn(fn, "_+")] = 0;

        ಆದರೆ (!image_name_is_valid(fn))
                ಹಿಂತಿರುಗಿ -EINVAL;

        *ret = TAKE_PTR(fn);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_is_extension_ವೃಕ್ಷ(ImageClass image_class, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extension, bool relax_extension_release_check) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        /* Does the path exist at all? If not, generate an error immediately. This is useful so that a missing root dir
         * always results in -ENOENT, and we can properly distinguish the case where the whole root doesn't exist from
         * the case where just the os-release file is missing. */
        r = access_nofollow(ಮಾರ್ಗ, F_OK);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* We use /usr/lib/extension-release.d/extension-release[.NAME] as flag for something being a system extension,
         * /etc/extension-release.d/extension-release[.NAME] as flag for something being a system configuration, and finally,
         * and {/etc|/usr/lib}/os-release as a flag for something being an OS (when not an extension). */
        r = open_extension_release(ಮಾರ್ಗ, image_class, extension, relax_extension_release_check, ಖಾಲಿ, ಖಾಲಿ);
        ಆದರೆ (r == -ENOENT) /* We got nothing */
                ಹಿಂತಿರುಗಿ 0;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ 1;
}

ಪೂರ್ಣಾಂಕ fd_is_os_ವೃಕ್ಷ(ಪೂರ್ಣಾಂಕ fd) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        r = open_extension_release_at(
                        fd,
                        IMAGE_MACHINE,
                        /* extension= */ ಖಾಲಿ,
                        /* relax_extension_release_check= */ false,
                        /* ret_path= */ ಖಾಲಿ,
                        /* ret_fd= */ ಖಾಲಿ);
        ಆದರೆ (r == -ENOENT)
                ಹಿಂತಿರುಗಿ false;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ true;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ extension_release_strict_xattr_ಮೌಲ್ಯ(ಪೂರ್ಣಾಂಕ extension_release_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extension_release_dir_ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(extension_release_fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(extension_release_dir_ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(filename);

        /* No xattr or cannot parse it? Then skip this. */
        r = getxattr_at_bool(extension_release_fd, /* path= */ ಖಾಲಿ, "user.extension-release.strict", /* at_flags= */ 0);
        ಆದರೆ (ERRNO_IS_NEG_XATTR_ABSENT(r))
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "%s/%s does not have user.extension-release.strict xattr, ignoring.",
                                       extension_release_dir_ಮಾರ್ಗ, filename);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "%s/%s: Failed to read 'user.extension-release.strict' extended attribute from file, ignoring: %m",
                                       extension_release_dir_ಮಾರ್ಗ, filename);

        /* Explicitly set to request strict matching? Skip it. */
        ಆದರೆ (r > 0) {
                log_debug("%s/%s: 'user.extension-release.strict' attribute is true, ignoring file.",
                          extension_release_dir_ಮಾರ್ಗ, filename);
                ಹಿಂತಿರುಗಿ true;
        }

        log_debug("%s/%s: 'user.extension-release.strict' attribute is false%s",
                  extension_release_dir_ಮಾರ್ಗ, filename,
                  glyph(GLYPH_ELLIPSIS));

        ಹಿಂತಿರುಗಿ false;
}

ಪೂರ್ಣಾಂಕ open_os_release_at(ಪೂರ್ಣಾಂಕ rfd, ಅಕ್ಷರ **ret_ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ *ret_fd) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(rfd));

        e = secure_getenv("SYSTEMD_OS_RELEASE");
        ಆದರೆ (e)
                ಹಿಂತಿರುಗಿ chaseat(rfd, rfd, e, /* flags= */ 0, ret_ಮಾರ್ಗ, ret_fd);

        FOREACH_STRING(ಮಾರ್ಗ, "/etc/os-release", "/usr/lib/os-release") {
                r = chaseat(rfd, rfd, ಮಾರ್ಗ, /* flags= */ 0, ret_ಮಾರ್ಗ, ret_fd);
                ಆದರೆ (r != -ENOENT)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ -ENOENT;
}

ಪೂರ್ಣಾಂಕ open_os_release(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ಅಕ್ಷರ **ret_ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ *ret_fd) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (!ಖಾಲಿಗಳು_or_ಬೇರು(ಬೇರು)) {
                rfd = open(ಬೇರು, O_CLOEXEC | O_DIRECTORY | O_ಮಾರ್ಗ);
                ಆದರೆ (rfd < 0)
                        ಹಿಂತಿರುಗಿ -errno;
        }

        r = open_os_release_at(rfd, ret_ಮಾರ್ಗ ? &p : ಖಾಲಿ, ret_fd ? &fd : ಖಾಲಿ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret_ಮಾರ್ಗ) {
                r = chaseat_ಪೂರ್ವಪ್ರತ್ಯಯ_ಬೇರು(p, ಬೇರು, ret_ಮಾರ್ಗ);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (ret_fd)
                *ret_fd = TAKE_FD(fd);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ open_extension_release_at(
                ಪೂರ್ಣಾಂಕ rfd,
                ImageClass image_class,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extension,
                bool relax_extension_release_check,
                ಅಕ್ಷರ **ret_ಮಾರ್ಗ,
                ಪೂರ್ಣಾಂಕ *ret_fd) {

        _cleanup_free_ ಅಕ್ಷರ *dir_ಮಾರ್ಗ = ಖಾಲಿ, *ಮಾರ್ಗ_ಸಿಕ್ಕಿತು = ಖಾಲಿ;
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd_ಸಿಕ್ಕಿತು = -EBADF;
        _cleanup_closedir_ DIR *dir = ಖಾಲಿ;
        bool ಸಿಕ್ಕಿತು = false;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(rfd));
        ಪ್ರತಿಜ್ಞಾ(!extension || (image_class >= 0 && image_class < _IMAGE_CLASS_MAX));

        ಆದರೆ (!extension)
                ಹಿಂತಿರುಗಿ open_os_release_at(rfd, ret_ಮಾರ್ಗ, ret_fd);

        ಆದರೆ (!ಒಳ_ಗಣ(image_class, IMAGE_SYSEXT, IMAGE_CONFEXT))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (!image_name_is_valid(extension))
                ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The extension name %s is invalid.", extension);

        p = strjoina(image_class_release_info[image_class].release_file_ಮಾರ್ಗ_ಪೂರ್ವಪ್ರತ್ಯಯ, extension);
        r = chaseat(rfd, rfd, p, /* flags= */ 0, ret_ಮಾರ್ಗ, ret_fd);
        log_ತುಂಬಿದ_errno_zerook(LOG_DEBUG, MIN(r, 0), "Checking for %s: %m", p);
        ಆದರೆ (r != -ENOENT)
                ಹಿಂತಿರುಗಿ r;

        /* Cannot find the expected extension-release file? The image filename might have been mangled on
         * deployment, so fallback to checking for any file in the extension-release.d directory, and return
         * the first one with a user.extension-release xattr instead. The user.extension-release.strict
         * xattr is checked to ensure the author of the image considers it OK if names do not match. */

        p = image_class_release_info[image_class].release_file_directory;
        r = chase_and_opendirat(rfd, rfd, p, /* chase_flags= */ 0, &dir_ಮಾರ್ಗ, &dir);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Cannot open %s, ignoring: %m", p);

        FOREACH_DIRENT(de, dir, ಹಿಂತಿರುಗಿ -errno) {
                _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *image_name;

                ಆದರೆ (!ಒಳ_ಗಣ(de->d_type, DT_REG, DT_UNKNOWN))
                        ಮುಂದುವರಿ;

                image_name = startswith(de->d_name, "extension-release.");
                ಆದರೆ (!image_name)
                        ಮುಂದುವರಿ;

                ಆದರೆ (!image_name_is_valid(image_name)) {
                        log_debug("%s/%s is not a valid release file name, ignoring.", dir_ಮಾರ್ಗ, de->d_name);
                        ಮುಂದುವರಿ;
                }

                /* We already chased the directory, and checked that this is a real file, so we shouldn't
                 * fail to open it. */
                fd = openat(dirfd(dir), de->d_name, O_ಮಾರ್ಗ|O_CLOEXEC|O_NOFOLLOW);
                ಆದರೆ (fd < 0)
                        ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Failed to open release file %s/%s: %m", dir_ಮಾರ್ಗ, de->d_name);

                /* Really ensure it is a regular file after we open it. */
                r = fd_verify_regular(fd);
                ಆದರೆ (r < 0) {
                        log_debug_errno(r, "%s/%s is not a regular file, ignoring: %m", dir_ಮಾರ್ಗ, de->d_name);
                        ಮುಂದುವರಿ;
                }

                ಆದರೆ (!relax_extension_release_check) {
                        _cleanup_free_ ಅಕ್ಷರ *ತಳ_extension = ಖಾಲಿ;

                        r = ಮಾರ್ಗ_ಹೊರತೆಗೆ_image_name(extension, &ತಳ_extension);
                        ಆದರೆ (r < 0) {
                                log_debug_errno(r, "Failed to extract image name from %s, ignoring: %m", extension);
                                ಮುಂದುವರಿ;
                        }

                        ಆದರೆ (!streq(image_name, ತಳ_extension) &&
                            extension_release_strict_xattr_ಮೌಲ್ಯ(fd, dir_ಮಾರ್ಗ, image_name) != 0)
                                ಮುಂದುವರಿ;
                }

                /* We already found what we were looking for, but there's another candidate? We treat this as
                 * an error, as we want to enforce that there are no ambiguities in case we are in the
                 * fallback path. */
                ಆದರೆ (ಸಿಕ್ಕಿತು)
                        ಹಿಂತಿರುಗಿ -ENOTUNIQ;

                ಸಿಕ್ಕಿತು = true;

                ಆದರೆ (ret_fd)
                        fd_ಸಿಕ್ಕಿತು = TAKE_FD(fd);

                ಆದರೆ (ret_ಮಾರ್ಗ) {
                        ಮಾರ್ಗ_ಸಿಕ್ಕಿತು = ಮಾರ್ಗ_join(dir_ಮಾರ್ಗ, de->d_name);
                        ಆದರೆ (!ಮಾರ್ಗ_ಸಿಕ್ಕಿತು)
                                ಹಿಂತಿರುಗಿ -ENOMEM;
                }
        }
        ಆದರೆ (!ಸಿಕ್ಕಿತು)
                ಹಿಂತಿರುಗಿ -ENOENT;

        ಆದರೆ (ret_fd)
                *ret_fd = TAKE_FD(fd_ಸಿಕ್ಕಿತು);
        ಆದರೆ (ret_ಮಾರ್ಗ)
                *ret_ಮಾರ್ಗ = TAKE_PTR(ಮಾರ್ಗ_ಸಿಕ್ಕಿತು);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ open_extension_release(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು,
                ImageClass image_class,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extension,
                bool relax_extension_release_check,
                ಅಕ್ಷರ **ret_ಮಾರ್ಗ,
                ಪೂರ್ಣಾಂಕ *ret_fd) {

        _cleanup_close_ ಪೂರ್ಣಾಂಕ rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (!ಖಾಲಿಗಳು_or_ಬೇರು(ಬೇರು)) {
                rfd = open(ಬೇರು, O_CLOEXEC | O_DIRECTORY | O_ಮಾರ್ಗ);
                ಆದರೆ (rfd < 0)
                        ಹಿಂತಿರುಗಿ -errno;
        }

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check,
                                      ret_ಮಾರ್ಗ ? &p : ಖಾಲಿ, ret_fd ? &fd : ಖಾಲಿ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret_ಮಾರ್ಗ) {
                r = chaseat_ಪೂರ್ವಪ್ರತ್ಯಯ_ಬೇರು(p, ಬೇರು, ret_ಮಾರ್ಗ);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (ret_fd)
                *ret_fd = TAKE_FD(fd);

        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parse_extension_release_atv(
                ಪೂರ್ಣಾಂಕ rfd,
                ImageClass image_class,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extension,
                bool relax_extension_release_check,
                va_list ap) {

        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(rfd));

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check, &p, &fd);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ parse_env_file_fdv(fd, p, ap);
}

ಪೂರ್ಣಾಂಕ parse_extension_release_at_sentinel(
                ಪೂರ್ಣಾಂಕ rfd,
                ImageClass image_class,
                bool relax_extension_release_check,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extension,
                ...) {

        va_list ap;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(rfd));

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);
        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ parse_extension_release_sentinel(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು,
                ImageClass image_class,
                bool relax_extension_release_check,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extension,
                ...) {

        _cleanup_close_ ಪೂರ್ಣಾಂಕ rfd = XAT_FDROOT;
        va_list ap;
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (!ಖಾಲಿಗಳು_or_ಬೇರು(ಬೇರು)) {
                rfd = open(ಬೇರು, O_CLOEXEC | O_DIRECTORY | O_ಮಾರ್ಗ);
                ಆದರೆ (rfd < 0)
                        ಹಿಂತಿರುಗಿ -errno;
        }

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ ತುಂಬು_os_release_ಜೋಡಿಗಳು_with_ಪೂರ್ವಪ್ರತ್ಯಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, ಅಕ್ಷರ ***ret) {
        _cleanup_strv_free_ ಅಕ್ಷರ **os_release_ಜೋಡಿಗಳು = ಖಾಲಿ, **os_release_ಜೋಡಿಗಳು_prefixed = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = ತುಂಬು_os_release_ಜೋಡಿಗಳು(ಬೇರು, &os_release_ಜೋಡಿಗಳು);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        STRV_FOREACH_PAIR(p, q, os_release_ಜೋಡಿಗಳು) {
                ಅಕ್ಷರ *ಸಾಲು;

                /* We strictly return only the four main ID fields and ignore the rest */
                ಆದರೆ (!STR_ಒಳ_ಗಣ(*p, "ID", "VERSION_ID", "BUILD_ID", "VARIANT_ID"))
                        ಮುಂದುವರಿ;

                ascii_strlower(*p);
                ಸಾಲು = strjoin(ಪೂರ್ವಪ್ರತ್ಯಯ, *p, "=", *q);
                ಆದರೆ (!ಸಾಲು)
                        ಹಿಂತಿರುಗಿ -ENOMEM;
                r = strv_consume(&os_release_ಜೋಡಿಗಳು_prefixed, ಸಾಲು);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        *ret = TAKE_PTR(os_release_ಜೋಡಿಗಳು_prefixed);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ತುಂಬು_extension_release_ಜೋಡಿಗಳು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ImageClass image_class, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extension, bool relax_extension_release_check, ಅಕ್ಷರ ***ret) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        r = open_extension_release(ಬೇರು, image_class, extension, relax_extension_release_check, &p, &fd);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ ತುಂಬು_env_file_ಜೋಡಿಗಳು_fd(fd, p, ret);
}

ಪೂರ್ಣಾಂಕ os_release_support_ended(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *support_ಕೊನೆ, bool quiet, usec_t *ret_eol) {
        _cleanup_free_ ಅಕ್ಷರ *support_ಕೊನೆ_alloc = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (!support_ಕೊನೆ) {
                /* If the caller has the variably handy, they can pass it in. If not, we'll read it
                 * ourselves. */

                r = parse_os_release(ಖಾಲಿ,
                                     "SUPPORT_END", &support_ಕೊನೆ_alloc);
                ಆದರೆ (r < 0 && r != -ENOENT)
                        ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                              "Failed to read os-release file, ignoring: %m");

                support_ಕೊನೆ = support_ಕೊನೆ_alloc;
        }

        ಆದರೆ (isempty(support_ಕೊನೆ)) { /* An empty string is a explicit way to say "no EOL exists" */
                ಆದರೆ (ret_eol)
                        *ret_eol = USEC_INFINITY;

                ಹಿಂತಿರುಗಿ false;  /* no end date defined */
        }

        ರಚನೆ tm tm = {};
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *k = strptime(support_ಕೊನೆ, "%Y-%m-%d", &tm);
        ಆದರೆ (!k || *k)
                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(quiet ? LOG_DEBUG : LOG_WARNING, SYNTHETIC_ERRNO(EINVAL),
                                      "Failed to parse SUPPORT_END= from os-release file, ignoring: %s", support_ಕೊನೆ);

        usec_t eol;
        r = mktime_or_timegm_usec(&tm, /* utc= */ true, &eol);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                      "Failed to convert SUPPORT_END= time from os-release file, ignoring: %m");

        ಆದರೆ (ret_eol)
                *ret_eol = eol;

        ಹಿಂತಿರುಗಿ now(CLOCK_REALTIME) > eol;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* os_release_pretty_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pretty_name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        /* Distills a "pretty" name to show from os-release data. First argument is supposed to be the
         * PRETTY_NAME= field, the second one the NAME= field. This function is trivial, of course, and
         * exists mostly to ensure we use the same logic wherever possible. */

        ಹಿಂತಿರುಗಿ ಖಾಲಿಗಳು_ಗಮ್ಯ_null(pretty_name) ?:
                ಖಾಲಿಗಳು_ಗಮ್ಯ_null(name) ?: "Linux";
}

ಅಕ್ಷರ *unescape_fancy_name(ಅಕ್ಷರ **fancy_name) {
        ಪ್ರತಿಜ್ಞಾ(fancy_name);

        /* Checks if the fancy name is valid, unescapes if it is, nullifies it if not */

        _cleanup_free_ ಅಕ್ಷರ *unescaped_fancy_name = ಖಾಲಿ;

        ಆದರೆ (isempty(*fancy_name))
                ಹೋಗು clear;

        /* We undo one level of C escapes on this */
        ssize_t n = cunescape(*fancy_name, /* flags= */ 0, &unescaped_fancy_name);
        ಆದರೆ (n < 0) {
                log_debug_errno((ಪೂರ್ಣಾಂಕ) n, "Failed to unescape FANCY_NAME= string, suppressing: %m");
                ಹೋಗು clear;
        }

        ಆದರೆ (!utf8_is_valid(unescaped_fancy_name)) {
                log_debug("Unescaped FANCY_NAME= string is not valid UTF-8, suppressing.");
                ಹೋಗು clear;
        }

        free_and_replace(*fancy_name, unescaped_fancy_name);
        ಹಿಂತಿರುಗಿ *fancy_name;

clear:
        *fancy_name = mfree(*fancy_name);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

bool use_fancy_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fancy_name) {

        /* Decides whether to show the specified fancy name */

        ಆದರೆ (isempty(fancy_name))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (!colors_enabled())
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ emoji_enabled() || ascii_is_valid(fancy_name);
}
