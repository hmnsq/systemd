/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <stdlib.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "ansi-color.h"
#అన్తర్భావ "chase.h"
#అన్తర్భావ "dirent-util.h"
#అన్తర్భావ "env-file.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "escape.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fs-util.h"
#అన్తర్భావ "glyph-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "os-util.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "stat-util.h"
#అన్తర్భావ "string-table.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"
#అన్తర్భావ "time-util.h"
#అన్తర్భావ "utf8.h"
#అన్తర్భావ "xattr-util.h"

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి image_class_పట్టిక[_IMAGE_CLASS_MAX] = {
        [IMAGE_MACHINE]  = "machine",
        [IMAGE_PORTABLE] = "portable",
        [IMAGE_SYSEXT]   = "sysext",
        [IMAGE_CONFEXT]  = "confext",
};

DEFINE_STRING_పట్టిక_LOOKUP(image_class, ImageClass);

/* Helper struct for naming simplicity and reusability */
స్థిర స్థిరరాశి నిర్మాణం {
        స్థిరరాశి అక్షరం *release_file_directory;
        స్థిరరాశి అక్షరం *release_file_మార్గం_ఉపసర్గ;
} image_class_release_info[_IMAGE_CLASS_MAX] = {
        [IMAGE_SYSEXT] = {
                .release_file_directory = "/usr/lib/extension-release.d/",
                .release_file_మార్గం_ఉపసర్గ = "/usr/lib/extension-release.d/extension-release.",
        },
        [IMAGE_CONFEXT] = {
                .release_file_directory = "/etc/extension-release.d/",
                .release_file_మార్గం_ఉపసర్గ = "/etc/extension-release.d/extension-release.",
        }
};

bool image_name_is_valid(స్థిరరాశి అక్షరం *s) {
        ఒకవేళ (!filename_is_valid(s))
                తిరిగి false;

        ఒకవేళ (string_has_cc(s, ఖాళీ))
                తిరిగి false;

        ఒకవేళ (!utf8_is_valid(s))
                తిరిగి false;

        /* Temporary files for atomically creating new files */
        ఒకవేళ (startswith(s, ".#"))
                తిరిగి false;

        తిరిగి true;
}

పూర్ణాంకం మార్గం_వెలికితీయి_image_name(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *fn = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(ret);

        /* Extract last component from path, without any "/" suffixes. */
        r = మార్గం_వెలికితీయి_filename(మార్గం, &fn);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r != O_DIRECTORY) {
                /* Chop off any image suffixes we recognize (unless we already know this must refer to some dir) */
                అక్షరం *m = ENDSWITH_సమితి(fn, ".sysext.raw", ".confext.raw", ".raw");
                ఒకవేళ (m)
                        *m = 0;
        }

        /* Truncate the version/counting suffixes */
        fn[strcspn(fn, "_+")] = 0;

        ఒకవేళ (!image_name_is_valid(fn))
                తిరిగి -EINVAL;

        *ret = TAKE_PTR(fn);
        తిరిగి 0;
}

పూర్ణాంకం మార్గం_is_extension_వృక్షం(ImageClass image_class, స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *extension, bool relax_extension_release_check) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);

        /* Does the path exist at all? If not, generate an error immediately. This is useful so that a missing root dir
         * always results in -ENOENT, and we can properly distinguish the case where the whole root doesn't exist from
         * the case where just the os-release file is missing. */
        r = access_nofollow(మార్గం, F_OK);
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* We use /usr/lib/extension-release.d/extension-release[.NAME] as flag for something being a system extension,
         * /etc/extension-release.d/extension-release[.NAME] as flag for something being a system configuration, and finally,
         * and {/etc|/usr/lib}/os-release as a flag for something being an OS (when not an extension). */
        r = open_extension_release(మార్గం, image_class, extension, relax_extension_release_check, ఖాళీ, ఖాళీ);
        ఒకవేళ (r == -ENOENT) /* We got nothing */
                తిరిగి 0;
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి 1;
}

పూర్ణాంకం fd_is_os_వృక్షం(పూర్ణాంకం fd) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);

        r = open_extension_release_at(
                        fd,
                        IMAGE_MACHINE,
                        /* extension= */ ఖాళీ,
                        /* relax_extension_release_check= */ false,
                        /* ret_path= */ ఖాళీ,
                        /* ret_fd= */ ఖాళీ);
        ఒకవేళ (r == -ENOENT)
                తిరిగి false;
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి true;
}

స్థిర పూర్ణాంకం extension_release_strict_xattr_విలువ(పూర్ణాంకం extension_release_fd, స్థిరరాశి అక్షరం *extension_release_dir_మార్గం, స్థిరరాశి అక్షరం *filename) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(extension_release_fd >= 0);
        ప్రతిజ్ఞా(extension_release_dir_మార్గం);
        ప్రతిజ్ఞా(filename);

        /* No xattr or cannot parse it? Then skip this. */
        r = getxattr_at_bool(extension_release_fd, /* path= */ ఖాళీ, "user.extension-release.strict", /* at_flags= */ 0);
        ఒకవేళ (ERRNO_IS_NEG_XATTR_ABSENT(r))
                తిరిగి log_debug_errno(r, "%s/%s does not have user.extension-release.strict xattr, ignoring.",
                                       extension_release_dir_మార్గం, filename);
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "%s/%s: Failed to read 'user.extension-release.strict' extended attribute from file, ignoring: %m",
                                       extension_release_dir_మార్గం, filename);

        /* Explicitly set to request strict matching? Skip it. */
        ఒకవేళ (r > 0) {
                log_debug("%s/%s: 'user.extension-release.strict' attribute is true, ignoring file.",
                          extension_release_dir_మార్గం, filename);
                తిరిగి true;
        }

        log_debug("%s/%s: 'user.extension-release.strict' attribute is false%s",
                  extension_release_dir_మార్గం, filename,
                  glyph(GLYPH_ELLIPSIS));

        తిరిగి false;
}

పూర్ణాంకం open_os_release_at(పూర్ణాంకం rfd, అక్షరం **ret_మార్గం, పూర్ణాంకం *ret_fd) {
        స్థిరరాశి అక్షరం *e;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(wildcard_fd_is_valid(rfd));

        e = secure_getenv("SYSTEMD_OS_RELEASE");
        ఒకవేళ (e)
                తిరిగి chaseat(rfd, rfd, e, /* flags= */ 0, ret_మార్గం, ret_fd);

        FOREACH_STRING(మార్గం, "/etc/os-release", "/usr/lib/os-release") {
                r = chaseat(rfd, rfd, మార్గం, /* flags= */ 0, ret_మార్గం, ret_fd);
                ఒకవేళ (r != -ENOENT)
                        తిరిగి r;
        }

        తిరిగి -ENOENT;
}

పూర్ణాంకం open_os_release(స్థిరరాశి అక్షరం *మూలం, అక్షరం **ret_మార్గం, పూర్ణాంకం *ret_fd) {
        _cleanup_close_ పూర్ణాంకం rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ అక్షరం *p = ఖాళీ;
        పూర్ణాంకం r;

        ఒకవేళ (!ఖాళీలు_or_మూలం(మూలం)) {
                rfd = open(మూలం, O_CLOEXEC | O_DIRECTORY | O_మార్గం);
                ఒకవేళ (rfd < 0)
                        తిరిగి -errno;
        }

        r = open_os_release_at(rfd, ret_మార్గం ? &p : ఖాళీ, ret_fd ? &fd : ఖాళీ);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret_మార్గం) {
                r = chaseat_ఉపసర్గ_మూలం(p, మూలం, ret_మార్గం);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        ఒకవేళ (ret_fd)
                *ret_fd = TAKE_FD(fd);

        తిరిగి 0;
}

పూర్ణాంకం open_extension_release_at(
                పూర్ణాంకం rfd,
                ImageClass image_class,
                స్థిరరాశి అక్షరం *extension,
                bool relax_extension_release_check,
                అక్షరం **ret_మార్గం,
                పూర్ణాంకం *ret_fd) {

        _cleanup_free_ అక్షరం *dir_మార్గం = ఖాళీ, *మార్గం_దొరికింది = ఖాళీ;
        _cleanup_close_ పూర్ణాంకం fd_దొరికింది = -EBADF;
        _cleanup_closedir_ DIR *dir = ఖాళీ;
        bool దొరికింది = false;
        స్థిరరాశి అక్షరం *p;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(wildcard_fd_is_valid(rfd));
        ప్రతిజ్ఞా(!extension || (image_class >= 0 && image_class < _IMAGE_CLASS_MAX));

        ఒకవేళ (!extension)
                తిరిగి open_os_release_at(rfd, ret_మార్గం, ret_fd);

        ఒకవేళ (!లోపల_సమితి(image_class, IMAGE_SYSEXT, IMAGE_CONFEXT))
                తిరిగి -EINVAL;

        ఒకవేళ (!image_name_is_valid(extension))
                తిరిగి log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The extension name %s is invalid.", extension);

        p = strjoina(image_class_release_info[image_class].release_file_మార్గం_ఉపసర్గ, extension);
        r = chaseat(rfd, rfd, p, /* flags= */ 0, ret_మార్గం, ret_fd);
        log_నిండిన_errno_zerook(LOG_DEBUG, MIN(r, 0), "Checking for %s: %m", p);
        ఒకవేళ (r != -ENOENT)
                తిరిగి r;

        /* Cannot find the expected extension-release file? The image filename might have been mangled on
         * deployment, so fallback to checking for any file in the extension-release.d directory, and return
         * the first one with a user.extension-release xattr instead. The user.extension-release.strict
         * xattr is checked to ensure the author of the image considers it OK if names do not match. */

        p = image_class_release_info[image_class].release_file_directory;
        r = chase_and_opendirat(rfd, rfd, p, /* chase_flags= */ 0, &dir_మార్గం, &dir);
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Cannot open %s, ignoring: %m", p);

        FOREACH_DIRENT(de, dir, తిరిగి -errno) {
                _cleanup_close_ పూర్ణాంకం fd = -EBADF;
                స్థిరరాశి అక్షరం *image_name;

                ఒకవేళ (!లోపల_సమితి(de->d_type, DT_REG, DT_UNKNOWN))
                        కొనసాగు;

                image_name = startswith(de->d_name, "extension-release.");
                ఒకవేళ (!image_name)
                        కొనసాగు;

                ఒకవేళ (!image_name_is_valid(image_name)) {
                        log_debug("%s/%s is not a valid release file name, ignoring.", dir_మార్గం, de->d_name);
                        కొనసాగు;
                }

                /* We already chased the directory, and checked that this is a real file, so we shouldn't
                 * fail to open it. */
                fd = openat(dirfd(dir), de->d_name, O_మార్గం|O_CLOEXEC|O_NOFOLLOW);
                ఒకవేళ (fd < 0)
                        తిరిగి log_debug_errno(errno, "Failed to open release file %s/%s: %m", dir_మార్గం, de->d_name);

                /* Really ensure it is a regular file after we open it. */
                r = fd_verify_regular(fd);
                ఒకవేళ (r < 0) {
                        log_debug_errno(r, "%s/%s is not a regular file, ignoring: %m", dir_మార్గం, de->d_name);
                        కొనసాగు;
                }

                ఒకవేళ (!relax_extension_release_check) {
                        _cleanup_free_ అక్షరం *పునాది_extension = ఖాళీ;

                        r = మార్గం_వెలికితీయి_image_name(extension, &పునాది_extension);
                        ఒకవేళ (r < 0) {
                                log_debug_errno(r, "Failed to extract image name from %s, ignoring: %m", extension);
                                కొనసాగు;
                        }

                        ఒకవేళ (!streq(image_name, పునాది_extension) &&
                            extension_release_strict_xattr_విలువ(fd, dir_మార్గం, image_name) != 0)
                                కొనసాగు;
                }

                /* We already found what we were looking for, but there's another candidate? We treat this as
                 * an error, as we want to enforce that there are no ambiguities in case we are in the
                 * fallback path. */
                ఒకవేళ (దొరికింది)
                        తిరిగి -ENOTUNIQ;

                దొరికింది = true;

                ఒకవేళ (ret_fd)
                        fd_దొరికింది = TAKE_FD(fd);

                ఒకవేళ (ret_మార్గం) {
                        మార్గం_దొరికింది = మార్గం_join(dir_మార్గం, de->d_name);
                        ఒకవేళ (!మార్గం_దొరికింది)
                                తిరిగి -ENOMEM;
                }
        }
        ఒకవేళ (!దొరికింది)
                తిరిగి -ENOENT;

        ఒకవేళ (ret_fd)
                *ret_fd = TAKE_FD(fd_దొరికింది);
        ఒకవేళ (ret_మార్గం)
                *ret_మార్గం = TAKE_PTR(మార్గం_దొరికింది);

        తిరిగి 0;
}

పూర్ణాంకం open_extension_release(
                స్థిరరాశి అక్షరం *మూలం,
                ImageClass image_class,
                స్థిరరాశి అక్షరం *extension,
                bool relax_extension_release_check,
                అక్షరం **ret_మార్గం,
                పూర్ణాంకం *ret_fd) {

        _cleanup_close_ పూర్ణాంకం rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ అక్షరం *p = ఖాళీ;
        పూర్ణాంకం r;

        ఒకవేళ (!ఖాళీలు_or_మూలం(మూలం)) {
                rfd = open(మూలం, O_CLOEXEC | O_DIRECTORY | O_మార్గం);
                ఒకవేళ (rfd < 0)
                        తిరిగి -errno;
        }

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check,
                                      ret_మార్గం ? &p : ఖాళీ, ret_fd ? &fd : ఖాళీ);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret_మార్గం) {
                r = chaseat_ఉపసర్గ_మూలం(p, మూలం, ret_మార్గం);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        ఒకవేళ (ret_fd)
                *ret_fd = TAKE_FD(fd);

        తిరిగి 0;
}

స్థిర పూర్ణాంకం parse_extension_release_atv(
                పూర్ణాంకం rfd,
                ImageClass image_class,
                స్థిరరాశి అక్షరం *extension,
                bool relax_extension_release_check,
                va_list ap) {

        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        _cleanup_free_ అక్షరం *p = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(wildcard_fd_is_valid(rfd));

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check, &p, &fd);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి parse_env_file_fdv(fd, p, ap);
}

పూర్ణాంకం parse_extension_release_at_sentinel(
                పూర్ణాంకం rfd,
                ImageClass image_class,
                bool relax_extension_release_check,
                స్థిరరాశి అక్షరం *extension,
                ...) {

        va_list ap;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(wildcard_fd_is_valid(rfd));

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);
        తిరిగి r;
}

పూర్ణాంకం parse_extension_release_sentinel(
                స్థిరరాశి అక్షరం *మూలం,
                ImageClass image_class,
                bool relax_extension_release_check,
                స్థిరరాశి అక్షరం *extension,
                ...) {

        _cleanup_close_ పూర్ణాంకం rfd = XAT_FDROOT;
        va_list ap;
        పూర్ణాంకం r;

        ఒకవేళ (!ఖాళీలు_or_మూలం(మూలం)) {
                rfd = open(మూలం, O_CLOEXEC | O_DIRECTORY | O_మార్గం);
                ఒకవేళ (rfd < 0)
                        తిరిగి -errno;
        }

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);

        తిరిగి r;
}

పూర్ణాంకం ఎక్కించు_os_release_జతలు_with_ఉపసర్గ(స్థిరరాశి అక్షరం *మూలం, స్థిరరాశి అక్షరం *ఉపసర్గ, అక్షరం ***ret) {
        _cleanup_strv_free_ అక్షరం **os_release_జతలు = ఖాళీ, **os_release_జతలు_prefixed = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        r = ఎక్కించు_os_release_జతలు(మూలం, &os_release_జతలు);
        ఒకవేళ (r < 0)
                తిరిగి r;

        STRV_FOREACH_PAIR(p, q, os_release_జతలు) {
                అక్షరం *పంక్తి;

                /* We strictly return only the four main ID fields and ignore the rest */
                ఒకవేళ (!STR_లోపల_సమితి(*p, "ID", "VERSION_ID", "BUILD_ID", "VARIANT_ID"))
                        కొనసాగు;

                ascii_strlower(*p);
                పంక్తి = strjoin(ఉపసర్గ, *p, "=", *q);
                ఒకవేళ (!పంక్తి)
                        తిరిగి -ENOMEM;
                r = strv_consume(&os_release_జతలు_prefixed, పంక్తి);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        *ret = TAKE_PTR(os_release_జతలు_prefixed);

        తిరిగి 0;
}

పూర్ణాంకం ఎక్కించు_extension_release_జతలు(స్థిరరాశి అక్షరం *మూలం, ImageClass image_class, స్థిరరాశి అక్షరం *extension, bool relax_extension_release_check, అక్షరం ***ret) {
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        _cleanup_free_ అక్షరం *p = ఖాళీ;
        పూర్ణాంకం r;

        r = open_extension_release(మూలం, image_class, extension, relax_extension_release_check, &p, &fd);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి ఎక్కించు_env_file_జతలు_fd(fd, p, ret);
}

పూర్ణాంకం os_release_support_ended(స్థిరరాశి అక్షరం *support_చివర, bool quiet, usec_t *ret_eol) {
        _cleanup_free_ అక్షరం *support_చివర_alloc = ఖాళీ;
        పూర్ణాంకం r;

        ఒకవేళ (!support_చివర) {
                /* If the caller has the variably handy, they can pass it in. If not, we'll read it
                 * ourselves. */

                r = parse_os_release(ఖాళీ,
                                     "SUPPORT_END", &support_చివర_alloc);
                ఒకవేళ (r < 0 && r != -ENOENT)
                        తిరిగి log_నిండిన_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                              "Failed to read os-release file, ignoring: %m");

                support_చివర = support_చివర_alloc;
        }

        ఒకవేళ (isempty(support_చివర)) { /* An empty string is a explicit way to say "no EOL exists" */
                ఒకవేళ (ret_eol)
                        *ret_eol = USEC_INFINITY;

                తిరిగి false;  /* no end date defined */
        }

        నిర్మాణం tm tm = {};
        స్థిరరాశి అక్షరం *k = strptime(support_చివర, "%Y-%m-%d", &tm);
        ఒకవేళ (!k || *k)
                తిరిగి log_నిండిన_errno(quiet ? LOG_DEBUG : LOG_WARNING, SYNTHETIC_ERRNO(EINVAL),
                                      "Failed to parse SUPPORT_END= from os-release file, ignoring: %s", support_చివర);

        usec_t eol;
        r = mktime_or_timegm_usec(&tm, /* utc= */ true, &eol);
        ఒకవేళ (r < 0)
                తిరిగి log_నిండిన_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                      "Failed to convert SUPPORT_END= time from os-release file, ignoring: %m");

        ఒకవేళ (ret_eol)
                *ret_eol = eol;

        తిరిగి now(CLOCK_REALTIME) > eol;
}

స్థిరరాశి అక్షరం* os_release_pretty_name(స్థిరరాశి అక్షరం *pretty_name, స్థిరరాశి అక్షరం *name) {
        /* Distills a "pretty" name to show from os-release data. First argument is supposed to be the
         * PRETTY_NAME= field, the second one the NAME= field. This function is trivial, of course, and
         * exists mostly to ensure we use the same logic wherever possible. */

        తిరిగి ఖాళీలు_గమ్యం_null(pretty_name) ?:
                ఖాళీలు_గమ్యం_null(name) ?: "Linux";
}

అక్షరం *unescape_fancy_name(అక్షరం **fancy_name) {
        ప్రతిజ్ఞా(fancy_name);

        /* Checks if the fancy name is valid, unescapes if it is, nullifies it if not */

        _cleanup_free_ అక్షరం *unescaped_fancy_name = ఖాళీ;

        ఒకవేళ (isempty(*fancy_name))
                వెళ్ళు clear;

        /* We undo one level of C escapes on this */
        ssize_t n = cunescape(*fancy_name, /* flags= */ 0, &unescaped_fancy_name);
        ఒకవేళ (n < 0) {
                log_debug_errno((పూర్ణాంకం) n, "Failed to unescape FANCY_NAME= string, suppressing: %m");
                వెళ్ళు clear;
        }

        ఒకవేళ (!utf8_is_valid(unescaped_fancy_name)) {
                log_debug("Unescaped FANCY_NAME= string is not valid UTF-8, suppressing.");
                వెళ్ళు clear;
        }

        free_and_replace(*fancy_name, unescaped_fancy_name);
        తిరిగి *fancy_name;

clear:
        *fancy_name = mfree(*fancy_name);
        తిరిగి ఖాళీ;
}

bool use_fancy_name(స్థిరరాశి అక్షరం *fancy_name) {

        /* Decides whether to show the specified fancy name */

        ఒకవేళ (isempty(fancy_name))
                తిరిగి false;

        ఒకవేళ (!colors_enabled())
                తిరిగి false;

        తిరిగి emoji_enabled() || ascii_is_valid(fancy_name);
}
