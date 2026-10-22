/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <stdlib.h>

#समावेश "alloc-util.h"
#समावेश "ansi-color.h"
#समावेश "chase.h"
#समावेश "dirent-util.h"
#समावेश "env-file.h"
#समावेश "errno-util.h"
#समावेश "escape.h"
#समावेश "fd-util.h"
#समावेश "fs-util.h"
#समावेश "glyph-util.h"
#समावेश "log.h"
#समावेश "os-util.h"
#समावेश "path-util.h"
#समावेश "stat-util.h"
#समावेश "string-table.h"
#समावेश "string-util.h"
#समावेश "strv.h"
#समावेश "time-util.h"
#समावेश "utf8.h"
#समावेश "xattr-util.h"

अटल स्थिर अक्षर* स्थिर image_class_table[_IMAGE_CLASS_MAX] = {
        [IMAGE_MACHINE]  = "machine",
        [IMAGE_PORTABLE] = "portable",
        [IMAGE_SYSEXT]   = "sysext",
        [IMAGE_CONFEXT]  = "confext",
};

DEFINE_STRING_TABLE_LOOKUP(image_class, ImageClass);

/* Helper struct for naming simplicity and reusability */
अटल स्थिर काष्ठा {
        स्थिर अक्षर *release_file_directory;
        स्थिर अक्षर *release_file_path_prefix;
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

bool image_name_is_valid(स्थिर अक्षर *s) {
        अगर (!filename_is_valid(s))
                वापस false;

        अगर (string_has_cc(s, शून्य))
                वापस false;

        अगर (!utf8_is_valid(s))
                वापस false;

        /* Temporary files for atomically creating new files */
        अगर (startswith(s, ".#"))
                वापस false;

        वापस true;
}

पूर्णांक path_extract_image_name(स्थिर अक्षर *path, अक्षर **ret) {
        _cleanup_free_ अक्षर *fn = शून्य;
        पूर्णांक r;

        निश्चित(path);
        निश्चित(ret);

        /* Extract last component from path, without any "/" suffixes. */
        r = path_extract_filename(path, &fn);
        अगर (r < 0)
                वापस r;
        अगर (r != O_DIRECTORY) {
                /* Chop off any image suffixes we recognize (unless we already know this must refer to some dir) */
                अक्षर *m = ENDSWITH_SET(fn, ".sysext.raw", ".confext.raw", ".raw");
                अगर (m)
                        *m = 0;
        }

        /* Truncate the version/counting suffixes */
        fn[म_खोज(fn, "_+")] = 0;

        अगर (!image_name_is_valid(fn))
                वापस -EINVAL;

        *ret = TAKE_PTR(fn);
        वापस 0;
}

पूर्णांक path_is_extension_वृक्ष(ImageClass image_class, स्थिर अक्षर *path, स्थिर अक्षर *extension, bool relax_extension_release_check) {
        पूर्णांक r;

        निश्चित(path);

        /* Does the path exist at all? If not, generate an error immediately. This is useful so that a missing root dir
         * always results in -ENOENT, and we can properly distinguish the case where the whole root doesn't exist from
         * the case where just the os-release file is missing. */
        r = access_nofollow(path, F_OK);
        अगर (r < 0)
                वापस r;

        /* We use /usr/lib/extension-release.d/extension-release[.NAME] as flag for something being a system extension,
         * /etc/extension-release.d/extension-release[.NAME] as flag for something being a system configuration, and finally,
         * and {/etc|/usr/lib}/os-release as a flag for something being an OS (when not an extension). */
        r = open_extension_release(path, image_class, extension, relax_extension_release_check, शून्य, शून्य);
        अगर (r == -ENOENT) /* We got nothing */
                वापस 0;
        अगर (r < 0)
                वापस r;

        वापस 1;
}

पूर्णांक fd_is_os_वृक्ष(पूर्णांक fd) {
        पूर्णांक r;

        निश्चित(fd >= 0);

        r = open_extension_release_at(
                        fd,
                        IMAGE_MACHINE,
                        /* extension= */ शून्य,
                        /* relax_extension_release_check= */ false,
                        /* ret_path= */ शून्य,
                        /* ret_fd= */ शून्य);
        अगर (r == -ENOENT)
                वापस false;
        अगर (r < 0)
                वापस r;

        वापस true;
}

अटल पूर्णांक extension_release_strict_xattr_मूल्य(पूर्णांक extension_release_fd, स्थिर अक्षर *extension_release_dir_path, स्थिर अक्षर *filename) {
        पूर्णांक r;

        निश्चित(extension_release_fd >= 0);
        निश्चित(extension_release_dir_path);
        निश्चित(filename);

        /* No xattr or cannot parse it? Then skip this. */
        r = getxattr_at_bool(extension_release_fd, /* path= */ शून्य, "user.extension-release.strict", /* at_flags= */ 0);
        अगर (ERRNO_IS_NEG_XATTR_ABSENT(r))
                वापस log_debug_errno(r, "%s/%s does not have user.extension-release.strict xattr, ignoring.",
                                       extension_release_dir_path, filename);
        अगर (r < 0)
                वापस log_debug_errno(r, "%s/%s: Failed to read 'user.extension-release.strict' extended attribute from file, ignoring: %m",
                                       extension_release_dir_path, filename);

        /* Explicitly set to request strict matching? Skip it. */
        अगर (r > 0) {
                log_debug("%s/%s: 'user.extension-release.strict' attribute is true, ignoring file.",
                          extension_release_dir_path, filename);
                वापस true;
        }

        log_debug("%s/%s: 'user.extension-release.strict' attribute is false%s",
                  extension_release_dir_path, filename,
                  glyph(GLYPH_ELLIPSIS));

        वापस false;
}

पूर्णांक open_os_release_at(पूर्णांक rfd, अक्षर **ret_path, पूर्णांक *ret_fd) {
        स्थिर अक्षर *e;
        पूर्णांक r;

        निश्चित(wildcard_fd_is_valid(rfd));

        e = secure_getenv("SYSTEMD_OS_RELEASE");
        अगर (e)
                वापस chaseat(rfd, rfd, e, /* flags= */ 0, ret_path, ret_fd);

        FOREACH_STRING(path, "/etc/os-release", "/usr/lib/os-release") {
                r = chaseat(rfd, rfd, path, /* flags= */ 0, ret_path, ret_fd);
                अगर (r != -ENOENT)
                        वापस r;
        }

        वापस -ENOENT;
}

पूर्णांक open_os_release(स्थिर अक्षर *मूल, अक्षर **ret_path, पूर्णांक *ret_fd) {
        _cleanup_close_ पूर्णांक rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ अक्षर *p = शून्य;
        पूर्णांक r;

        अगर (!empty_or_मूल(मूल)) {
                rfd = खोलो(मूल, O_CLOEXEC | O_DIRECTORY | O_PATH);
                अगर (rfd < 0)
                        वापस -त्रुटि_सं;
        }

        r = open_os_release_at(rfd, ret_path ? &p : शून्य, ret_fd ? &fd : शून्य);
        अगर (r < 0)
                वापस r;

        अगर (ret_path) {
                r = chaseat_prefix_मूल(p, मूल, ret_path);
                अगर (r < 0)
                        वापस r;
        }

        अगर (ret_fd)
                *ret_fd = TAKE_FD(fd);

        वापस 0;
}

पूर्णांक open_extension_release_at(
                पूर्णांक rfd,
                ImageClass image_class,
                स्थिर अक्षर *extension,
                bool relax_extension_release_check,
                अक्षर **ret_path,
                पूर्णांक *ret_fd) {

        _cleanup_free_ अक्षर *dir_path = शून्य, *path_found = शून्य;
        _cleanup_close_ पूर्णांक fd_found = -EBADF;
        _cleanup_closedir_ सूची *dir = शून्य;
        bool found = false;
        स्थिर अक्षर *p;
        पूर्णांक r;

        निश्चित(wildcard_fd_is_valid(rfd));
        निश्चित(!extension || (image_class >= 0 && image_class < _IMAGE_CLASS_MAX));

        अगर (!extension)
                वापस open_os_release_at(rfd, ret_path, ret_fd);

        अगर (!IN_SET(image_class, IMAGE_SYSEXT, IMAGE_CONFEXT))
                वापस -EINVAL;

        अगर (!image_name_is_valid(extension))
                वापस log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The extension name %s is invalid.", extension);

        p = strjoina(image_class_release_info[image_class].release_file_path_prefix, extension);
        r = chaseat(rfd, rfd, p, /* flags= */ 0, ret_path, ret_fd);
        log_full_errno_zerook(LOG_DEBUG, MIN(r, 0), "Checking for %s: %m", p);
        अगर (r != -ENOENT)
                वापस r;

        /* Cannot find the expected extension-release file? The image filename might have been mangled on
         * deployment, so fallback to checking for any file in the extension-release.d directory, and return
         * the first one with a user.extension-release xattr instead. The user.extension-release.strict
         * xattr is checked to ensure the author of the image considers it OK if names do not match. */

        p = image_class_release_info[image_class].release_file_directory;
        r = chase_and_opendirat(rfd, rfd, p, /* chase_flags= */ 0, &dir_path, &dir);
        अगर (r < 0)
                वापस log_debug_errno(r, "Cannot open %s, ignoring: %m", p);

        FOREACH_DIRENT(de, dir, वापस -त्रुटि_सं) {
                _cleanup_close_ पूर्णांक fd = -EBADF;
                स्थिर अक्षर *image_name;

                अगर (!IN_SET(de->d_type, DT_REG, DT_UNKNOWN))
                        जारी;

                image_name = startswith(de->d_name, "extension-release.");
                अगर (!image_name)
                        जारी;

                अगर (!image_name_is_valid(image_name)) {
                        log_debug("%s/%s is not a valid release file name, ignoring.", dir_path, de->d_name);
                        जारी;
                }

                /* We already chased the directory, and checked that this is a real file, so we shouldn't
                 * fail to open it. */
                fd = openat(dirfd(dir), de->d_name, O_PATH|O_CLOEXEC|O_NOFOLLOW);
                अगर (fd < 0)
                        वापस log_debug_errno(त्रुटि_सं, "Failed to open release file %s/%s: %m", dir_path, de->d_name);

                /* Really ensure it is a regular file after we open it. */
                r = fd_verify_regular(fd);
                अगर (r < 0) {
                        log_debug_errno(r, "%s/%s is not a regular file, ignoring: %m", dir_path, de->d_name);
                        जारी;
                }

                अगर (!relax_extension_release_check) {
                        _cleanup_free_ अक्षर *base_extension = शून्य;

                        r = path_extract_image_name(extension, &base_extension);
                        अगर (r < 0) {
                                log_debug_errno(r, "Failed to extract image name from %s, ignoring: %m", extension);
                                जारी;
                        }

                        अगर (!streq(image_name, base_extension) &&
                            extension_release_strict_xattr_मूल्य(fd, dir_path, image_name) != 0)
                                जारी;
                }

                /* We already found what we were looking for, but there's another candidate? We treat this as
                 * an error, as we want to enforce that there are no ambiguities in case we are in the
                 * fallback path. */
                अगर (found)
                        वापस -ENOTUNIQ;

                found = true;

                अगर (ret_fd)
                        fd_found = TAKE_FD(fd);

                अगर (ret_path) {
                        path_found = path_join(dir_path, de->d_name);
                        अगर (!path_found)
                                वापस -ENOMEM;
                }
        }
        अगर (!found)
                वापस -ENOENT;

        अगर (ret_fd)
                *ret_fd = TAKE_FD(fd_found);
        अगर (ret_path)
                *ret_path = TAKE_PTR(path_found);

        वापस 0;
}

पूर्णांक open_extension_release(
                स्थिर अक्षर *मूल,
                ImageClass image_class,
                स्थिर अक्षर *extension,
                bool relax_extension_release_check,
                अक्षर **ret_path,
                पूर्णांक *ret_fd) {

        _cleanup_close_ पूर्णांक rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ अक्षर *p = शून्य;
        पूर्णांक r;

        अगर (!empty_or_मूल(मूल)) {
                rfd = खोलो(मूल, O_CLOEXEC | O_DIRECTORY | O_PATH);
                अगर (rfd < 0)
                        वापस -त्रुटि_सं;
        }

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check,
                                      ret_path ? &p : शून्य, ret_fd ? &fd : शून्य);
        अगर (r < 0)
                वापस r;

        अगर (ret_path) {
                r = chaseat_prefix_मूल(p, मूल, ret_path);
                अगर (r < 0)
                        वापस r;
        }

        अगर (ret_fd)
                *ret_fd = TAKE_FD(fd);

        वापस 0;
}

अटल पूर्णांक parse_extension_release_atv(
                पूर्णांक rfd,
                ImageClass image_class,
                स्थिर अक्षर *extension,
                bool relax_extension_release_check,
                बहु_सूची ap) {

        _cleanup_close_ पूर्णांक fd = -EBADF;
        _cleanup_free_ अक्षर *p = शून्य;
        पूर्णांक r;

        निश्चित(wildcard_fd_is_valid(rfd));

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check, &p, &fd);
        अगर (r < 0)
                वापस r;

        वापस parse_env_file_fdv(fd, p, ap);
}

पूर्णांक parse_extension_release_at_sentinel(
                पूर्णांक rfd,
                ImageClass image_class,
                bool relax_extension_release_check,
                स्थिर अक्षर *extension,
                ...) {

        बहु_सूची ap;
        पूर्णांक r;

        निश्चित(wildcard_fd_is_valid(rfd));

        बहु_शुरू(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        बहु_पूर्ण(ap);
        वापस r;
}

पूर्णांक parse_extension_release_sentinel(
                स्थिर अक्षर *मूल,
                ImageClass image_class,
                bool relax_extension_release_check,
                स्थिर अक्षर *extension,
                ...) {

        _cleanup_close_ पूर्णांक rfd = XAT_FDROOT;
        बहु_सूची ap;
        पूर्णांक r;

        अगर (!empty_or_मूल(मूल)) {
                rfd = खोलो(मूल, O_CLOEXEC | O_DIRECTORY | O_PATH);
                अगर (rfd < 0)
                        वापस -त्रुटि_सं;
        }

        बहु_शुरू(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        बहु_पूर्ण(ap);

        वापस r;
}

पूर्णांक load_os_release_pairs_with_prefix(स्थिर अक्षर *मूल, स्थिर अक्षर *prefix, अक्षर ***ret) {
        _cleanup_strv_free_ अक्षर **os_release_pairs = शून्य, **os_release_pairs_prefixed = शून्य;
        पूर्णांक r;

        निश्चित(ret);

        r = load_os_release_pairs(मूल, &os_release_pairs);
        अगर (r < 0)
                वापस r;

        STRV_FOREACH_PAIR(p, q, os_release_pairs) {
                अक्षर *पंक्ति;

                /* We strictly return only the four main ID fields and ignore the rest */
                अगर (!STR_IN_SET(*p, "ID", "VERSION_ID", "BUILD_ID", "VARIANT_ID"))
                        जारी;

                ascii_strlower(*p);
                पंक्ति = strjoin(prefix, *p, "=", *q);
                अगर (!पंक्ति)
                        वापस -ENOMEM;
                r = strv_consume(&os_release_pairs_prefixed, पंक्ति);
                अगर (r < 0)
                        वापस r;
        }

        *ret = TAKE_PTR(os_release_pairs_prefixed);

        वापस 0;
}

पूर्णांक load_extension_release_pairs(स्थिर अक्षर *मूल, ImageClass image_class, स्थिर अक्षर *extension, bool relax_extension_release_check, अक्षर ***ret) {
        _cleanup_close_ पूर्णांक fd = -EBADF;
        _cleanup_free_ अक्षर *p = शून्य;
        पूर्णांक r;

        r = open_extension_release(मूल, image_class, extension, relax_extension_release_check, &p, &fd);
        अगर (r < 0)
                वापस r;

        वापस load_env_file_pairs_fd(fd, p, ret);
}

पूर्णांक os_release_support_ended(स्थिर अक्षर *support_end, bool quiet, usec_t *ret_eol) {
        _cleanup_free_ अक्षर *_support_end_alloc = शून्य;
        पूर्णांक r;

        अगर (!support_end) {
                /* If the caller has the variably handy, they can pass it in. If not, we'll read it
                 * ourselves. */

                r = parse_os_release(शून्य,
                                     "SUPPORT_END", &_support_end_alloc);
                अगर (r < 0 && r != -ENOENT)
                        वापस log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                              "Failed to read os-release file, ignoring: %m");

                support_end = _support_end_alloc;
        }

        अगर (isempty(support_end)) { /* An empty string is a explicit way to say "no EOL exists" */
                अगर (ret_eol)
                        *ret_eol = USEC_INFINITY;

                वापस false;  /* no end date defined */
        }

        काष्ठा पंचांग पंचांग = {};
        स्थिर अक्षर *k = strptime(support_end, "%Y-%m-%d", &पंचांग);
        अगर (!k || *k)
                वापस log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, SYNTHETIC_ERRNO(EINVAL),
                                      "Failed to parse SUPPORT_END= from os-release file, ignoring: %s", support_end);

        usec_t eol;
        r = mktime_or_timegm_usec(&पंचांग, /* utc= */ true, &eol);
        अगर (r < 0)
                वापस log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                      "Failed to convert SUPPORT_END= time from os-release file, ignoring: %m");

        अगर (ret_eol)
                *ret_eol = eol;

        वापस now(CLOCK_REALTIME) > eol;
}

स्थिर अक्षर* os_release_pretty_name(स्थिर अक्षर *pretty_name, स्थिर अक्षर *name) {
        /* Distills a "pretty" name to show from os-release data. First argument is supposed to be the
         * PRETTY_NAME= field, the second one the NAME= field. This function is trivial, of course, and
         * exists mostly to ensure we use the same logic wherever possible. */

        वापस empty_to_null(pretty_name) ?:
                empty_to_null(name) ?: "Linux";
}

अक्षर *unescape_fancy_name(अक्षर **fancy_name) {
        निश्चित(fancy_name);

        /* Checks if the fancy name is valid, unescapes if it is, nullifies it if not */

        _cleanup_free_ अक्षर *unescaped_fancy_name = शून्य;

        अगर (isempty(*fancy_name))
                जाओ clear;

        /* We undo one level of C escapes on this */
        ssize_t n = cunescape(*fancy_name, /* flags= */ 0, &unescaped_fancy_name);
        अगर (n < 0) {
                log_debug_errno((पूर्णांक) n, "Failed to unescape FANCY_NAME= string, suppressing: %m");
                जाओ clear;
        }

        अगर (!utf8_is_valid(unescaped_fancy_name)) {
                log_debug("Unescaped FANCY_NAME= string is not valid UTF-8, suppressing.");
                जाओ clear;
        }

        free_and_replace(*fancy_name, unescaped_fancy_name);
        वापस *fancy_name;

clear:
        *fancy_name = mfree(*fancy_name);
        वापस शून्य;
}

bool use_fancy_name(स्थिर अक्षर *fancy_name) {

        /* Decides whether to show the specified fancy name */

        अगर (isempty(fancy_name))
                वापस false;

        अगर (!colors_enabled())
                वापस false;

        वापस emoji_enabled() || ascii_is_valid(fancy_name);
}
