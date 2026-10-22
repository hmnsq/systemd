/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ansi-color.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "chase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "dirent-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "env-file.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "escape.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "glyph-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "os-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-table.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "time-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utf8.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "xattr-util.h"

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ image_class_ਸਾਰਣੀ[_IMAGE_CLASS_MAX] = {
        [IMAGE_MACHINE]  = "machine",
        [IMAGE_PORTABLE] = "portable",
        [IMAGE_SYSEXT]   = "sysext",
        [IMAGE_CONFEXT]  = "confext",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(image_class, ImageClass);

/* Helper struct for naming simplicity and reusability */
ਸਥਿਰ ਅਚਲ ਬਣਤਰ {
        ਅਚਲ ਅੱਖਰ *release_file_directory;
        ਅਚਲ ਅੱਖਰ *release_file_ਰਸਤਾ_ਅਗੇਤਰ;
} image_class_release_info[_IMAGE_CLASS_MAX] = {
        [IMAGE_SYSEXT] = {
                .release_file_directory = "/usr/lib/extension-release.d/",
                .release_file_ਰਸਤਾ_ਅਗੇਤਰ = "/usr/lib/extension-release.d/extension-release.",
        },
        [IMAGE_CONFEXT] = {
                .release_file_directory = "/etc/extension-release.d/",
                .release_file_ਰਸਤਾ_ਅਗੇਤਰ = "/etc/extension-release.d/extension-release.",
        }
};

bool image_name_is_valid(ਅਚਲ ਅੱਖਰ *s) {
        ਜੇ (!filename_is_valid(s))
                ਵਾਪਸ false;

        ਜੇ (string_has_cc(s, ਨਿਰਾ))
                ਵਾਪਸ false;

        ਜੇ (!utf8_is_valid(s))
                ਵਾਪਸ false;

        /* Temporary files for atomically creating new files */
        ਜੇ (startswith(s, ".#"))
                ਵਾਪਸ false;

        ਵਾਪਸ true;
}

ਪੂਰਨ ਰਸਤਾ_ਕੱਢੋ_image_name(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *fn = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* Extract last component from path, without any "/" suffixes. */
        r = ਰਸਤਾ_ਕੱਢੋ_filename(ਰਸਤਾ, &fn);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r != O_DIRECTORY) {
                /* Chop off any image suffixes we recognize (unless we already know this must refer to some dir) */
                ਅੱਖਰ *m = ENDSWITH_ਸੈੱਟ(fn, ".sysext.raw", ".confext.raw", ".raw");
                ਜੇ (m)
                        *m = 0;
        }

        /* Truncate the version/counting suffixes */
        fn[strcspn(fn, "_+")] = 0;

        ਜੇ (!image_name_is_valid(fn))
                ਵਾਪਸ -EINVAL;

        *ret = TAKE_PTR(fn);
        ਵਾਪਸ 0;
}

ਪੂਰਨ ਰਸਤਾ_is_extension_ਰੁੱਖ(ImageClass image_class, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *extension, bool relax_extension_release_check) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        /* Does the path exist at all? If not, generate an error immediately. This is useful so that a missing root dir
         * always results in -ENOENT, and we can properly distinguish the case where the whole root doesn't exist from
         * the case where just the os-release file is missing. */
        r = access_nofollow(ਰਸਤਾ, F_OK);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* We use /usr/lib/extension-release.d/extension-release[.NAME] as flag for something being a system extension,
         * /etc/extension-release.d/extension-release[.NAME] as flag for something being a system configuration, and finally,
         * and {/etc|/usr/lib}/os-release as a flag for something being an OS (when not an extension). */
        r = open_extension_release(ਰਸਤਾ, image_class, extension, relax_extension_release_check, ਨਿਰਾ, ਨਿਰਾ);
        ਜੇ (r == -ENOENT) /* We got nothing */
                ਵਾਪਸ 0;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ 1;
}

ਪੂਰਨ fd_is_os_ਰੁੱਖ(ਪੂਰਨ fd) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        r = open_extension_release_at(
                        fd,
                        IMAGE_MACHINE,
                        /* extension= */ ਨਿਰਾ,
                        /* relax_extension_release_check= */ false,
                        /* ret_path= */ ਨਿਰਾ,
                        /* ret_fd= */ ਨਿਰਾ);
        ਜੇ (r == -ENOENT)
                ਵਾਪਸ false;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ true;
}

ਸਥਿਰ ਪੂਰਨ extension_release_strict_xattr_ਮੁੱਲ(ਪੂਰਨ extension_release_fd, ਅਚਲ ਅੱਖਰ *extension_release_dir_ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *filename) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(extension_release_fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(extension_release_dir_ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(filename);

        /* No xattr or cannot parse it? Then skip this. */
        r = getxattr_at_bool(extension_release_fd, /* path= */ ਨਿਰਾ, "user.extension-release.strict", /* at_flags= */ 0);
        ਜੇ (ERRNO_IS_NEG_XATTR_ABSENT(r))
                ਵਾਪਸ log_debug_errno(r, "%s/%s does not have user.extension-release.strict xattr, ignoring.",
                                       extension_release_dir_ਰਸਤਾ, filename);
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "%s/%s: Failed to read 'user.extension-release.strict' extended attribute from file, ignoring: %m",
                                       extension_release_dir_ਰਸਤਾ, filename);

        /* Explicitly set to request strict matching? Skip it. */
        ਜੇ (r > 0) {
                log_debug("%s/%s: 'user.extension-release.strict' attribute is true, ignoring file.",
                          extension_release_dir_ਰਸਤਾ, filename);
                ਵਾਪਸ true;
        }

        log_debug("%s/%s: 'user.extension-release.strict' attribute is false%s",
                  extension_release_dir_ਰਸਤਾ, filename,
                  glyph(GLYPH_ELLIPSIS));

        ਵਾਪਸ false;
}

ਪੂਰਨ open_os_release_at(ਪੂਰਨ rfd, ਅੱਖਰ **ret_ਰਸਤਾ, ਪੂਰਨ *ret_fd) {
        ਅਚਲ ਅੱਖਰ *e;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(rfd));

        e = secure_getenv("SYSTEMD_OS_RELEASE");
        ਜੇ (e)
                ਵਾਪਸ chaseat(rfd, rfd, e, /* flags= */ 0, ret_ਰਸਤਾ, ret_fd);

        FOREACH_STRING(ਰਸਤਾ, "/etc/os-release", "/usr/lib/os-release") {
                r = chaseat(rfd, rfd, ਰਸਤਾ, /* flags= */ 0, ret_ਰਸਤਾ, ret_fd);
                ਜੇ (r != -ENOENT)
                        ਵਾਪਸ r;
        }

        ਵਾਪਸ -ENOENT;
}

ਪੂਰਨ open_os_release(ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ਅੱਖਰ **ret_ਰਸਤਾ, ਪੂਰਨ *ret_fd) {
        _cleanup_close_ ਪੂਰਨ rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਜੇ (!ਖਾਲੀ_ਥਾਵਾਂ_or_ਜੜ੍ਹ(ਜੜ੍ਹ)) {
                rfd = open(ਜੜ੍ਹ, O_CLOEXEC | O_DIRECTORY | O_ਰਸਤਾ);
                ਜੇ (rfd < 0)
                        ਵਾਪਸ -errno;
        }

        r = open_os_release_at(rfd, ret_ਰਸਤਾ ? &p : ਨਿਰਾ, ret_fd ? &fd : ਨਿਰਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret_ਰਸਤਾ) {
                r = chaseat_ਅਗੇਤਰ_ਜੜ੍ਹ(p, ਜੜ੍ਹ, ret_ਰਸਤਾ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਜੇ (ret_fd)
                *ret_fd = TAKE_FD(fd);

        ਵਾਪਸ 0;
}

ਪੂਰਨ open_extension_release_at(
                ਪੂਰਨ rfd,
                ImageClass image_class,
                ਅਚਲ ਅੱਖਰ *extension,
                bool relax_extension_release_check,
                ਅੱਖਰ **ret_ਰਸਤਾ,
                ਪੂਰਨ *ret_fd) {

        _cleanup_free_ ਅੱਖਰ *dir_ਰਸਤਾ = ਨਿਰਾ, *ਰਸਤਾ_ਮਿਲਿਆ = ਨਿਰਾ;
        _cleanup_close_ ਪੂਰਨ fd_ਮਿਲਿਆ = -EBADF;
        _cleanup_closedir_ DIR *dir = ਨਿਰਾ;
        bool ਮਿਲਿਆ = false;
        ਅਚਲ ਅੱਖਰ *p;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(rfd));
        ਪ੍ਰਤਿਜ੍ਞਾ(!extension || (image_class >= 0 && image_class < _IMAGE_CLASS_MAX));

        ਜੇ (!extension)
                ਵਾਪਸ open_os_release_at(rfd, ret_ਰਸਤਾ, ret_fd);

        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(image_class, IMAGE_SYSEXT, IMAGE_CONFEXT))
                ਵਾਪਸ -EINVAL;

        ਜੇ (!image_name_is_valid(extension))
                ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The extension name %s is invalid.", extension);

        p = strjoina(image_class_release_info[image_class].release_file_ਰਸਤਾ_ਅਗੇਤਰ, extension);
        r = chaseat(rfd, rfd, p, /* flags= */ 0, ret_ਰਸਤਾ, ret_fd);
        log_ਭਰਿਆ_errno_zerook(LOG_DEBUG, MIN(r, 0), "Checking for %s: %m", p);
        ਜੇ (r != -ENOENT)
                ਵਾਪਸ r;

        /* Cannot find the expected extension-release file? The image filename might have been mangled on
         * deployment, so fallback to checking for any file in the extension-release.d directory, and return
         * the first one with a user.extension-release xattr instead. The user.extension-release.strict
         * xattr is checked to ensure the author of the image considers it OK if names do not match. */

        p = image_class_release_info[image_class].release_file_directory;
        r = chase_and_opendirat(rfd, rfd, p, /* chase_flags= */ 0, &dir_ਰਸਤਾ, &dir);
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Cannot open %s, ignoring: %m", p);

        FOREACH_DIRENT(de, dir, ਵਾਪਸ -errno) {
                _cleanup_close_ ਪੂਰਨ fd = -EBADF;
                ਅਚਲ ਅੱਖਰ *image_name;

                ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(de->d_type, DT_REG, DT_UNKNOWN))
                        ਜਾਰੀ;

                image_name = startswith(de->d_name, "extension-release.");
                ਜੇ (!image_name)
                        ਜਾਰੀ;

                ਜੇ (!image_name_is_valid(image_name)) {
                        log_debug("%s/%s is not a valid release file name, ignoring.", dir_ਰਸਤਾ, de->d_name);
                        ਜਾਰੀ;
                }

                /* We already chased the directory, and checked that this is a real file, so we shouldn't
                 * fail to open it. */
                fd = openat(dirfd(dir), de->d_name, O_ਰਸਤਾ|O_CLOEXEC|O_NOFOLLOW);
                ਜੇ (fd < 0)
                        ਵਾਪਸ log_debug_errno(errno, "Failed to open release file %s/%s: %m", dir_ਰਸਤਾ, de->d_name);

                /* Really ensure it is a regular file after we open it. */
                r = fd_verify_regular(fd);
                ਜੇ (r < 0) {
                        log_debug_errno(r, "%s/%s is not a regular file, ignoring: %m", dir_ਰਸਤਾ, de->d_name);
                        ਜਾਰੀ;
                }

                ਜੇ (!relax_extension_release_check) {
                        _cleanup_free_ ਅੱਖਰ *ਅਧਾਰ_extension = ਨਿਰਾ;

                        r = ਰਸਤਾ_ਕੱਢੋ_image_name(extension, &ਅਧਾਰ_extension);
                        ਜੇ (r < 0) {
                                log_debug_errno(r, "Failed to extract image name from %s, ignoring: %m", extension);
                                ਜਾਰੀ;
                        }

                        ਜੇ (!streq(image_name, ਅਧਾਰ_extension) &&
                            extension_release_strict_xattr_ਮੁੱਲ(fd, dir_ਰਸਤਾ, image_name) != 0)
                                ਜਾਰੀ;
                }

                /* We already found what we were looking for, but there's another candidate? We treat this as
                 * an error, as we want to enforce that there are no ambiguities in case we are in the
                 * fallback path. */
                ਜੇ (ਮਿਲਿਆ)
                        ਵਾਪਸ -ENOTUNIQ;

                ਮਿਲਿਆ = true;

                ਜੇ (ret_fd)
                        fd_ਮਿਲਿਆ = TAKE_FD(fd);

                ਜੇ (ret_ਰਸਤਾ) {
                        ਰਸਤਾ_ਮਿਲਿਆ = ਰਸਤਾ_join(dir_ਰਸਤਾ, de->d_name);
                        ਜੇ (!ਰਸਤਾ_ਮਿਲਿਆ)
                                ਵਾਪਸ -ENOMEM;
                }
        }
        ਜੇ (!ਮਿਲਿਆ)
                ਵਾਪਸ -ENOENT;

        ਜੇ (ret_fd)
                *ret_fd = TAKE_FD(fd_ਮਿਲਿਆ);
        ਜੇ (ret_ਰਸਤਾ)
                *ret_ਰਸਤਾ = TAKE_PTR(ਰਸਤਾ_ਮਿਲਿਆ);

        ਵਾਪਸ 0;
}

ਪੂਰਨ open_extension_release(
                ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ,
                ImageClass image_class,
                ਅਚਲ ਅੱਖਰ *extension,
                bool relax_extension_release_check,
                ਅੱਖਰ **ret_ਰਸਤਾ,
                ਪੂਰਨ *ret_fd) {

        _cleanup_close_ ਪੂਰਨ rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਜੇ (!ਖਾਲੀ_ਥਾਵਾਂ_or_ਜੜ੍ਹ(ਜੜ੍ਹ)) {
                rfd = open(ਜੜ੍ਹ, O_CLOEXEC | O_DIRECTORY | O_ਰਸਤਾ);
                ਜੇ (rfd < 0)
                        ਵਾਪਸ -errno;
        }

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check,
                                      ret_ਰਸਤਾ ? &p : ਨਿਰਾ, ret_fd ? &fd : ਨਿਰਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret_ਰਸਤਾ) {
                r = chaseat_ਅਗੇਤਰ_ਜੜ੍ਹ(p, ਜੜ੍ਹ, ret_ਰਸਤਾ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਜੇ (ret_fd)
                *ret_fd = TAKE_FD(fd);

        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ parse_extension_release_atv(
                ਪੂਰਨ rfd,
                ImageClass image_class,
                ਅਚਲ ਅੱਖਰ *extension,
                bool relax_extension_release_check,
                va_list ap) {

        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(rfd));

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check, &p, &fd);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ parse_env_file_fdv(fd, p, ap);
}

ਪੂਰਨ parse_extension_release_at_sentinel(
                ਪੂਰਨ rfd,
                ImageClass image_class,
                bool relax_extension_release_check,
                ਅਚਲ ਅੱਖਰ *extension,
                ...) {

        va_list ap;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(rfd));

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);
        ਵਾਪਸ r;
}

ਪੂਰਨ parse_extension_release_sentinel(
                ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ,
                ImageClass image_class,
                bool relax_extension_release_check,
                ਅਚਲ ਅੱਖਰ *extension,
                ...) {

        _cleanup_close_ ਪੂਰਨ rfd = XAT_FDROOT;
        va_list ap;
        ਪੂਰਨ r;

        ਜੇ (!ਖਾਲੀ_ਥਾਵਾਂ_or_ਜੜ੍ਹ(ਜੜ੍ਹ)) {
                rfd = open(ਜੜ੍ਹ, O_CLOEXEC | O_DIRECTORY | O_ਰਸਤਾ);
                ਜੇ (rfd < 0)
                        ਵਾਪਸ -errno;
        }

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);

        ਵਾਪਸ r;
}

ਪੂਰਨ ਲੱਦੋ_os_release_ਜੋੜੇ_with_ਅਗੇਤਰ(ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, ਅੱਖਰ ***ret) {
        _cleanup_strv_free_ ਅੱਖਰ **os_release_ਜੋੜੇ = ਨਿਰਾ, **os_release_ਜੋੜੇ_prefixed = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = ਲੱਦੋ_os_release_ਜੋੜੇ(ਜੜ੍ਹ, &os_release_ਜੋੜੇ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        STRV_FOREACH_PAIR(p, q, os_release_ਜੋੜੇ) {
                ਅੱਖਰ *ਸਤਰ;

                /* We strictly return only the four main ID fields and ignore the rest */
                ਜੇ (!STR_ਵਿੱਚ_ਸੈੱਟ(*p, "ID", "VERSION_ID", "BUILD_ID", "VARIANT_ID"))
                        ਜਾਰੀ;

                ascii_strlower(*p);
                ਸਤਰ = strjoin(ਅਗੇਤਰ, *p, "=", *q);
                ਜੇ (!ਸਤਰ)
                        ਵਾਪਸ -ENOMEM;
                r = strv_consume(&os_release_ਜੋੜੇ_prefixed, ਸਤਰ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        *ret = TAKE_PTR(os_release_ਜੋੜੇ_prefixed);

        ਵਾਪਸ 0;
}

ਪੂਰਨ ਲੱਦੋ_extension_release_ਜੋੜੇ(ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ImageClass image_class, ਅਚਲ ਅੱਖਰ *extension, bool relax_extension_release_check, ਅੱਖਰ ***ret) {
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;
        ਪੂਰਨ r;

        r = open_extension_release(ਜੜ੍ਹ, image_class, extension, relax_extension_release_check, &p, &fd);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ ਲੱਦੋ_env_file_ਜੋੜੇ_fd(fd, p, ret);
}

ਪੂਰਨ os_release_support_ended(ਅਚਲ ਅੱਖਰ *support_ਸਿਰਾ, bool quiet, usec_t *ret_eol) {
        _cleanup_free_ ਅੱਖਰ *support_ਸਿਰਾ_alloc = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਜੇ (!support_ਸਿਰਾ) {
                /* If the caller has the variably handy, they can pass it in. If not, we'll read it
                 * ourselves. */

                r = parse_os_release(ਨਿਰਾ,
                                     "SUPPORT_END", &support_ਸਿਰਾ_alloc);
                ਜੇ (r < 0 && r != -ENOENT)
                        ਵਾਪਸ log_ਭਰਿਆ_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                              "Failed to read os-release file, ignoring: %m");

                support_ਸਿਰਾ = support_ਸਿਰਾ_alloc;
        }

        ਜੇ (isempty(support_ਸਿਰਾ)) { /* An empty string is a explicit way to say "no EOL exists" */
                ਜੇ (ret_eol)
                        *ret_eol = USEC_INFINITY;

                ਵਾਪਸ false;  /* no end date defined */
        }

        ਬਣਤਰ tm tm = {};
        ਅਚਲ ਅੱਖਰ *k = strptime(support_ਸਿਰਾ, "%Y-%m-%d", &tm);
        ਜੇ (!k || *k)
                ਵਾਪਸ log_ਭਰਿਆ_errno(quiet ? LOG_DEBUG : LOG_WARNING, SYNTHETIC_ERRNO(EINVAL),
                                      "Failed to parse SUPPORT_END= from os-release file, ignoring: %s", support_ਸਿਰਾ);

        usec_t eol;
        r = mktime_or_timegm_usec(&tm, /* utc= */ true, &eol);
        ਜੇ (r < 0)
                ਵਾਪਸ log_ਭਰਿਆ_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                      "Failed to convert SUPPORT_END= time from os-release file, ignoring: %m");

        ਜੇ (ret_eol)
                *ret_eol = eol;

        ਵਾਪਸ now(CLOCK_REALTIME) > eol;
}

ਅਚਲ ਅੱਖਰ* os_release_pretty_name(ਅਚਲ ਅੱਖਰ *pretty_name, ਅਚਲ ਅੱਖਰ *name) {
        /* Distills a "pretty" name to show from os-release data. First argument is supposed to be the
         * PRETTY_NAME= field, the second one the NAME= field. This function is trivial, of course, and
         * exists mostly to ensure we use the same logic wherever possible. */

        ਵਾਪਸ ਖਾਲੀ_ਥਾਵਾਂ_ਮੰਜ਼ਿਲ_null(pretty_name) ?:
                ਖਾਲੀ_ਥਾਵਾਂ_ਮੰਜ਼ਿਲ_null(name) ?: "Linux";
}

ਅੱਖਰ *unescape_fancy_name(ਅੱਖਰ **fancy_name) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fancy_name);

        /* Checks if the fancy name is valid, unescapes if it is, nullifies it if not */

        _cleanup_free_ ਅੱਖਰ *unescaped_fancy_name = ਨਿਰਾ;

        ਜੇ (isempty(*fancy_name))
                ਜਾਓ clear;

        /* We undo one level of C escapes on this */
        ssize_t n = cunescape(*fancy_name, /* flags= */ 0, &unescaped_fancy_name);
        ਜੇ (n < 0) {
                log_debug_errno((ਪੂਰਨ) n, "Failed to unescape FANCY_NAME= string, suppressing: %m");
                ਜਾਓ clear;
        }

        ਜੇ (!utf8_is_valid(unescaped_fancy_name)) {
                log_debug("Unescaped FANCY_NAME= string is not valid UTF-8, suppressing.");
                ਜਾਓ clear;
        }

        free_and_replace(*fancy_name, unescaped_fancy_name);
        ਵਾਪਸ *fancy_name;

clear:
        *fancy_name = mfree(*fancy_name);
        ਵਾਪਸ ਨਿਰਾ;
}

bool use_fancy_name(ਅਚਲ ਅੱਖਰ *fancy_name) {

        /* Decides whether to show the specified fancy name */

        ਜੇ (isempty(fancy_name))
                ਵਾਪਸ false;

        ਜੇ (!colors_enabled())
                ਵਾਪਸ false;

        ਵਾਪਸ emoji_enabled() || ascii_is_valid(fancy_name);
}
