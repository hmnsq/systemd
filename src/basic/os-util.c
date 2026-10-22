/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <stdlib.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "ansi-color.h"
#ଅନ୍ତର୍ଭାଵ "chase.h"
#ଅନ୍ତର୍ଭାଵ "dirent-util.h"
#ଅନ୍ତର୍ଭାଵ "env-file.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "escape.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fs-util.h"
#ଅନ୍ତର୍ଭାଵ "glyph-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "os-util.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "stat-util.h"
#ଅନ୍ତର୍ଭାଵ "string-table.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"
#ଅନ୍ତର୍ଭାଵ "time-util.h"
#ଅନ୍ତର୍ଭାଵ "utf8.h"
#ଅନ୍ତର୍ଭାଵ "xattr-util.h"

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* ଅଚଳ image_class_table[_IMAGE_CLASS_MAX] = {
        [IMAGE_MACHINE]  = "machine",
        [IMAGE_PORTABLE] = "portable",
        [IMAGE_SYSEXT]   = "sysext",
        [IMAGE_CONFEXT]  = "confext",
};

DEFINE_STRING_TABLE_LOOKUP(image_class, ImageClass);

/* Helper struct for naming simplicity and reusability */
ସ୍ଥିର ଅଚଳ ସଂରଚନା {
        ଅଚଳ ଅକ୍ଷର *release_file_directory;
        ଅଚଳ ଅକ୍ଷର *release_file_path_prefix;
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

bool image_name_is_valid(ଅଚଳ ଅକ୍ଷର *s) {
        ଯଦି (!filename_is_valid(s))
                ଫେରନ୍ତୁ false;

        ଯଦି (string_has_cc(s, ଖାଲି))
                ଫେରନ୍ତୁ false;

        ଯଦି (!utf8_is_valid(s))
                ଫେରନ୍ତୁ false;

        /* Temporary files for atomically creating new files */
        ଯଦି (startswith(s, ".#"))
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ true;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_extract_image_name(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *fn = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(ret);

        /* Extract last component from path, without any "/" suffixes. */
        r = path_extract_filename(path, &fn);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r != O_DIRECTORY) {
                /* Chop off any image suffixes we recognize (unless we already know this must refer to some dir) */
                ଅକ୍ଷର *m = ENDSWITH_SET(fn, ".sysext.raw", ".confext.raw", ".raw");
                ଯଦି (m)
                        *m = 0;
        }

        /* Truncate the version/counting suffixes */
        fn[strcspn(fn, "_+")] = 0;

        ଯଦି (!image_name_is_valid(fn))
                ଫେରନ୍ତୁ -EINVAL;

        *ret = TAKE_PTR(fn);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_is_extension_ବୃକ୍ଷ(ImageClass image_class, ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *extension, bool relax_extension_release_check) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);

        /* Does the path exist at all? If not, generate an error immediately. This is useful so that a missing root dir
         * always results in -ENOENT, and we can properly distinguish the case where the whole root doesn't exist from
         * the case where just the os-release file is missing. */
        r = access_nofollow(path, F_OK);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* We use /usr/lib/extension-release.d/extension-release[.NAME] as flag for something being a system extension,
         * /etc/extension-release.d/extension-release[.NAME] as flag for something being a system configuration, and finally,
         * and {/etc|/usr/lib}/os-release as a flag for something being an OS (when not an extension). */
        r = open_extension_release(path, image_class, extension, relax_extension_release_check, ଖାଲି, ଖାଲି);
        ଯଦି (r == -ENOENT) /* We got nothing */
                ଫେରନ୍ତୁ 0;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ 1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_is_os_ବୃକ୍ଷ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        r = open_extension_release_at(
                        fd,
                        IMAGE_MACHINE,
                        /* extension= */ ଖାଲି,
                        /* relax_extension_release_check= */ false,
                        /* ret_path= */ ଖାଲି,
                        /* ret_fd= */ ଖାଲି);
        ଯଦି (r == -ENOENT)
                ଫେରନ୍ତୁ false;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ true;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା extension_release_strict_xattr_ମୂଲ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା extension_release_fd, ଅଚଳ ଅକ୍ଷର *extension_release_dir_path, ଅଚଳ ଅକ୍ଷର *filename) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(extension_release_fd >= 0);
        ପ୍ରତିଜ୍ଞା(extension_release_dir_path);
        ପ୍ରତିଜ୍ଞା(filename);

        /* No xattr or cannot parse it? Then skip this. */
        r = getxattr_at_bool(extension_release_fd, /* path= */ ଖାଲି, "user.extension-release.strict", /* at_flags= */ 0);
        ଯଦି (ERRNO_IS_NEG_XATTR_ABSENT(r))
                ଫେରନ୍ତୁ log_debug_errno(r, "%s/%s does not have user.extension-release.strict xattr, ignoring.",
                                       extension_release_dir_path, filename);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "%s/%s: Failed to read 'user.extension-release.strict' extended attribute from file, ignoring: %m",
                                       extension_release_dir_path, filename);

        /* Explicitly set to request strict matching? Skip it. */
        ଯଦି (r > 0) {
                log_debug("%s/%s: 'user.extension-release.strict' attribute is true, ignoring file.",
                          extension_release_dir_path, filename);
                ଫେରନ୍ତୁ true;
        }

        log_debug("%s/%s: 'user.extension-release.strict' attribute is false%s",
                  extension_release_dir_path, filename,
                  glyph(GLYPH_ELLIPSIS));

        ଫେରନ୍ତୁ false;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_os_release_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd, ଅକ୍ଷର **ret_path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_fd) {
        ଅଚଳ ଅକ୍ଷର *e;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(rfd));

        e = secure_getenv("SYSTEMD_OS_RELEASE");
        ଯଦି (e)
                ଫେରନ୍ତୁ chaseat(rfd, rfd, e, /* flags= */ 0, ret_path, ret_fd);

        FOREACH_STRING(path, "/etc/os-release", "/usr/lib/os-release") {
                r = chaseat(rfd, rfd, path, /* flags= */ 0, ret_path, ret_fd);
                ଯଦି (r != -ENOENT)
                        ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ -ENOENT;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_os_release(ଅଚଳ ଅକ୍ଷର *ମୂଳ, ଅକ୍ଷର **ret_path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_fd) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (!empty_or_ମୂଳ(ମୂଳ)) {
                rfd = open(ମୂଳ, O_CLOEXEC | O_DIRECTORY | O_PATH);
                ଯଦି (rfd < 0)
                        ଫେରନ୍ତୁ -errno;
        }

        r = open_os_release_at(rfd, ret_path ? &p : ଖାଲି, ret_fd ? &fd : ଖାଲି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret_path) {
                r = chaseat_prefix_ମୂଳ(p, ମୂଳ, ret_path);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (ret_fd)
                *ret_fd = TAKE_FD(fd);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_extension_release_at(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd,
                ImageClass image_class,
                ଅଚଳ ଅକ୍ଷର *extension,
                bool relax_extension_release_check,
                ଅକ୍ଷର **ret_path,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_fd) {

        _cleanup_free_ ଅକ୍ଷର *dir_path = ଖାଲି, *path_found = ଖାଲି;
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_found = -EBADF;
        _cleanup_closedir_ DIR *dir = ଖାଲି;
        bool found = false;
        ଅଚଳ ଅକ୍ଷର *p;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(rfd));
        ପ୍ରତିଜ୍ଞା(!extension || (image_class >= 0 && image_class < _IMAGE_CLASS_MAX));

        ଯଦି (!extension)
                ଫେରନ୍ତୁ open_os_release_at(rfd, ret_path, ret_fd);

        ଯଦି (!IN_SET(image_class, IMAGE_SYSEXT, IMAGE_CONFEXT))
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (!image_name_is_valid(extension))
                ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The extension name %s is invalid.", extension);

        p = strjoina(image_class_release_info[image_class].release_file_path_prefix, extension);
        r = chaseat(rfd, rfd, p, /* flags= */ 0, ret_path, ret_fd);
        log_full_errno_zerook(LOG_DEBUG, MIN(r, 0), "Checking for %s: %m", p);
        ଯଦି (r != -ENOENT)
                ଫେରନ୍ତୁ r;

        /* Cannot find the expected extension-release file? The image filename might have been mangled on
         * deployment, so fallback to checking for any file in the extension-release.d directory, and return
         * the first one with a user.extension-release xattr instead. The user.extension-release.strict
         * xattr is checked to ensure the author of the image considers it OK if names do not match. */

        p = image_class_release_info[image_class].release_file_directory;
        r = chase_and_opendirat(rfd, rfd, p, /* chase_flags= */ 0, &dir_path, &dir);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Cannot open %s, ignoring: %m", p);

        FOREACH_DIRENT(de, dir, ଫେରନ୍ତୁ -errno) {
                _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
                ଅଚଳ ଅକ୍ଷର *image_name;

                ଯଦି (!IN_SET(de->d_type, DT_REG, DT_UNKNOWN))
                        ଜାରି;

                image_name = startswith(de->d_name, "extension-release.");
                ଯଦି (!image_name)
                        ଜାରି;

                ଯଦି (!image_name_is_valid(image_name)) {
                        log_debug("%s/%s is not a valid release file name, ignoring.", dir_path, de->d_name);
                        ଜାରି;
                }

                /* We already chased the directory, and checked that this is a real file, so we shouldn't
                 * fail to open it. */
                fd = openat(dirfd(dir), de->d_name, O_PATH|O_CLOEXEC|O_NOFOLLOW);
                ଯଦି (fd < 0)
                        ଫେରନ୍ତୁ log_debug_errno(errno, "Failed to open release file %s/%s: %m", dir_path, de->d_name);

                /* Really ensure it is a regular file after we open it. */
                r = fd_verify_regular(fd);
                ଯଦି (r < 0) {
                        log_debug_errno(r, "%s/%s is not a regular file, ignoring: %m", dir_path, de->d_name);
                        ଜାରି;
                }

                ଯଦି (!relax_extension_release_check) {
                        _cleanup_free_ ଅକ୍ଷର *base_extension = ଖାଲି;

                        r = path_extract_image_name(extension, &base_extension);
                        ଯଦି (r < 0) {
                                log_debug_errno(r, "Failed to extract image name from %s, ignoring: %m", extension);
                                ଜାରି;
                        }

                        ଯଦି (!streq(image_name, base_extension) &&
                            extension_release_strict_xattr_ମୂଲ୍ୟ(fd, dir_path, image_name) != 0)
                                ଜାରି;
                }

                /* We already found what we were looking for, but there's another candidate? We treat this as
                 * an error, as we want to enforce that there are no ambiguities in case we are in the
                 * fallback path. */
                ଯଦି (found)
                        ଫେରନ୍ତୁ -ENOTUNIQ;

                found = true;

                ଯଦି (ret_fd)
                        fd_found = TAKE_FD(fd);

                ଯଦି (ret_path) {
                        path_found = path_join(dir_path, de->d_name);
                        ଯଦି (!path_found)
                                ଫେରନ୍ତୁ -ENOMEM;
                }
        }
        ଯଦି (!found)
                ଫେରନ୍ତୁ -ENOENT;

        ଯଦି (ret_fd)
                *ret_fd = TAKE_FD(fd_found);
        ଯଦି (ret_path)
                *ret_path = TAKE_PTR(path_found);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_extension_release(
                ଅଚଳ ଅକ୍ଷର *ମୂଳ,
                ImageClass image_class,
                ଅଚଳ ଅକ୍ଷର *extension,
                bool relax_extension_release_check,
                ଅକ୍ଷର **ret_path,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_fd) {

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (!empty_or_ମୂଳ(ମୂଳ)) {
                rfd = open(ମୂଳ, O_CLOEXEC | O_DIRECTORY | O_PATH);
                ଯଦି (rfd < 0)
                        ଫେରନ୍ତୁ -errno;
        }

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check,
                                      ret_path ? &p : ଖାଲି, ret_fd ? &fd : ଖାଲି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret_path) {
                r = chaseat_prefix_ମୂଳ(p, ମୂଳ, ret_path);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (ret_fd)
                *ret_fd = TAKE_FD(fd);

        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_extension_release_atv(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd,
                ImageClass image_class,
                ଅଚଳ ଅକ୍ଷର *extension,
                bool relax_extension_release_check,
                va_list ap) {

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(rfd));

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check, &p, &fd);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ parse_env_file_fdv(fd, p, ap);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_extension_release_at_sentinel(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd,
                ImageClass image_class,
                bool relax_extension_release_check,
                ଅଚଳ ଅକ୍ଷର *extension,
                ...) {

        va_list ap;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(rfd));

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);
        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_extension_release_sentinel(
                ଅଚଳ ଅକ୍ଷର *ମୂଳ,
                ImageClass image_class,
                bool relax_extension_release_check,
                ଅଚଳ ଅକ୍ଷର *extension,
                ...) {

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd = XAT_FDROOT;
        va_list ap;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (!empty_or_ମୂଳ(ମୂଳ)) {
                rfd = open(ମୂଳ, O_CLOEXEC | O_DIRECTORY | O_PATH);
                ଯଦି (rfd < 0)
                        ଫେରନ୍ତୁ -errno;
        }

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା load_os_release_pairs_with_prefix(ଅଚଳ ଅକ୍ଷର *ମୂଳ, ଅଚଳ ଅକ୍ଷର *prefix, ଅକ୍ଷର ***ret) {
        _cleanup_strv_free_ ଅକ୍ଷର **os_release_pairs = ଖାଲି, **os_release_pairs_prefixed = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        r = load_os_release_pairs(ମୂଳ, &os_release_pairs);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        STRV_FOREACH_PAIR(p, q, os_release_pairs) {
                ଅକ୍ଷର *ଧାଡ଼ି;

                /* We strictly return only the four main ID fields and ignore the rest */
                ଯଦି (!STR_IN_SET(*p, "ID", "VERSION_ID", "BUILD_ID", "VARIANT_ID"))
                        ଜାରି;

                ascii_strlower(*p);
                ଧାଡ଼ି = strjoin(prefix, *p, "=", *q);
                ଯଦି (!ଧାଡ଼ି)
                        ଫେରନ୍ତୁ -ENOMEM;
                r = strv_consume(&os_release_pairs_prefixed, ଧାଡ଼ି);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        *ret = TAKE_PTR(os_release_pairs_prefixed);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା load_extension_release_pairs(ଅଚଳ ଅକ୍ଷର *ମୂଳ, ImageClass image_class, ଅଚଳ ଅକ୍ଷର *extension, bool relax_extension_release_check, ଅକ୍ଷର ***ret) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = open_extension_release(ମୂଳ, image_class, extension, relax_extension_release_check, &p, &fd);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ load_env_file_pairs_fd(fd, p, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା os_release_support_ended(ଅଚଳ ଅକ୍ଷର *support_end, bool quiet, usec_t *ret_eol) {
        _cleanup_free_ ଅକ୍ଷର *_support_end_alloc = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (!support_end) {
                /* If the caller has the variably handy, they can pass it in. If not, we'll read it
                 * ourselves. */

                r = parse_os_release(ଖାଲି,
                                     "SUPPORT_END", &_support_end_alloc);
                ଯଦି (r < 0 && r != -ENOENT)
                        ଫେରନ୍ତୁ log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                              "Failed to read os-release file, ignoring: %m");

                support_end = _support_end_alloc;
        }

        ଯଦି (isempty(support_end)) { /* An empty string is a explicit way to say "no EOL exists" */
                ଯଦି (ret_eol)
                        *ret_eol = USEC_INFINITY;

                ଫେରନ୍ତୁ false;  /* no end date defined */
        }

        ସଂରଚନା tm tm = {};
        ଅଚଳ ଅକ୍ଷର *k = strptime(support_end, "%Y-%m-%d", &tm);
        ଯଦି (!k || *k)
                ଫେରନ୍ତୁ log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, SYNTHETIC_ERRNO(EINVAL),
                                      "Failed to parse SUPPORT_END= from os-release file, ignoring: %s", support_end);

        usec_t eol;
        r = mktime_or_timegm_usec(&tm, /* utc= */ true, &eol);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                      "Failed to convert SUPPORT_END= time from os-release file, ignoring: %m");

        ଯଦି (ret_eol)
                *ret_eol = eol;

        ଫେରନ୍ତୁ now(CLOCK_REALTIME) > eol;
}

ଅଚଳ ଅକ୍ଷର* os_release_pretty_name(ଅଚଳ ଅକ୍ଷର *pretty_name, ଅଚଳ ଅକ୍ଷର *name) {
        /* Distills a "pretty" name to show from os-release data. First argument is supposed to be the
         * PRETTY_NAME= field, the second one the NAME= field. This function is trivial, of course, and
         * exists mostly to ensure we use the same logic wherever possible. */

        ଫେରନ୍ତୁ empty_to_null(pretty_name) ?:
                empty_to_null(name) ?: "Linux";
}

ଅକ୍ଷର *unescape_fancy_name(ଅକ୍ଷର **fancy_name) {
        ପ୍ରତିଜ୍ଞା(fancy_name);

        /* Checks if the fancy name is valid, unescapes if it is, nullifies it if not */

        _cleanup_free_ ଅକ୍ଷର *unescaped_fancy_name = ଖାଲି;

        ଯଦି (isempty(*fancy_name))
                ଯାଅ clear;

        /* We undo one level of C escapes on this */
        ssize_t n = cunescape(*fancy_name, /* flags= */ 0, &unescaped_fancy_name);
        ଯଦି (n < 0) {
                log_debug_errno((ପୂର୍ଣ୍ଣସଂଖ୍ୟା) n, "Failed to unescape FANCY_NAME= string, suppressing: %m");
                ଯାଅ clear;
        }

        ଯଦି (!utf8_is_valid(unescaped_fancy_name)) {
                log_debug("Unescaped FANCY_NAME= string is not valid UTF-8, suppressing.");
                ଯାଅ clear;
        }

        free_and_replace(*fancy_name, unescaped_fancy_name);
        ଫେରନ୍ତୁ *fancy_name;

clear:
        *fancy_name = mfree(*fancy_name);
        ଫେରନ୍ତୁ ଖାଲି;
}

bool use_fancy_name(ଅଚଳ ଅକ୍ଷର *fancy_name) {

        /* Decides whether to show the specified fancy name */

        ଯଦି (isempty(fancy_name))
                ଫେରନ୍ତୁ false;

        ଯଦି (!colors_enabled())
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ emoji_enabled() || ascii_is_valid(fancy_name);
}
