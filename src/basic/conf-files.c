/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <stdlib.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "chase.h"
#అన్తర్భావ "conf-files.h"
#అన్తర్భావ "dirent-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fileio.h"
#అన్తర్భావ "glyph-util.h"
#అన్తర్భావ "hashmap.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "nulstr-util.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "set.h"
#అన్తర్భావ "stat-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"

ConfFile* conf_file_free(ConfFile *c) {
        ఒకవేళ (!c)
                తిరిగి ఖాళీ;

        విడుదల(c->filename);
        విడుదల(c->ఫలితం);
        విడుదల(c->original_మార్గం);
        విడుదల(c->resolved_మార్గం);
        సురక్షిత_close(c->fd);

        తిరిగి mfree(c);
}

DEFINE_సూచిక_ARRAY_FREE_FUNC(ConfFile*, conf_file_free);

స్థిర పూర్ణాంకం conf_files_log_level(ConfFilesFlags flags) {
        తిరిగి FLAGS_సమితి(flags, CONF_FILES_WARN) ? LOG_WARNING : LOG_DEBUG;
}

స్థిర పూర్ణాంకం prepare_dirs(
                స్థిరరాశి అక్షరం *మూలం,
                ConfFilesFlags flags,
                అక్షరం * స్థిరరాశి *dirs,
                అక్షరం **ret_మూలం,
                పూర్ణాంకం *ret_rfd,
                అక్షరం ***ret_dirs) {

        _cleanup_free_ అక్షరం *మూలం_abs = ఖాళీ;
        _cleanup_strv_free_ అక్షరం **dirs_abs = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret_మూలం);
        ప్రతిజ్ఞా(ret_rfd);
        ప్రతిజ్ఞా(ret_dirs || strv_isempty(dirs));

        పూర్ణాంకం log_level = conf_files_log_level(flags);

        r = ఖాళీలు_or_మూలం_harder_గమ్యం_null(&మూలం);
        ఒకవేళ (r < 0)
                తిరిగి log_నిండిన_errno(log_level, r, "Failed to determine if '%s' points to the root directory: %m", strempty(మూలం));

        ఒకవేళ (ret_dirs) {
                dirs_abs = strv_copy(dirs);
                ఒకవేళ (!dirs_abs)
                        తిరిగి log_oom_నిండిన(log_level);
        }

        _cleanup_close_ పూర్ణాంకం rfd = XAT_FDROOT;
        ఒకవేళ (మూలం) {
                /* When a non-trivial root is specified, we will prefix the result later. Hence, it is not
                 * necessary to modify each config directories here. but needs to normalize the root directory. */
                r = మార్గం_make_absolute_cwd(మూలం, &మూలం_abs);
                ఒకవేళ (r < 0)
                        తిరిగి log_నిండిన_errno(log_level, r, "Failed to make '%s' absolute: %m", మూలం);

                మార్గం_simplify(మూలం_abs);

                rfd = open(మూలం, O_CLOEXEC|O_DIRECTORY|O_మార్గం);
                ఒకవేళ (rfd < 0)
                        తిరిగి log_నిండిన_errno(log_level, errno, "Failed to open '%s': %m", మూలం_abs);

        } లేకపోతే ఒకవేళ (ret_dirs) {
                /* When an empty root or "/" is specified, we will open "/" below, hence we need to make
                 * each config directory absolute if relative. */
                r = మార్గం_strv_make_absolute_cwd(dirs_abs);
                ఒకవేళ (r < 0)
                        తిరిగి log_నిండిన_errno(log_level, r, "Failed to make directories absolute: %m");
        }

        *ret_మూలం = TAKE_PTR(మూలం_abs);
        *ret_rfd = TAKE_FD(rfd);
        ఒకవేళ (ret_dirs)
                *ret_dirs = TAKE_PTR(dirs_abs);
        తిరిగి 0;
}

స్థిర పూర్ణాంకం conf_file_ఉపసర్గ_మూలం(ConfFile *c, స్థిరరాశి అక్షరం *మూలం, ConfFilesFlags flags) {
        అక్షరం *p;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(c);

        ఒకవేళ (FLAGS_సమితి(flags, CONF_FILES_DONT_ఉపసర్గ_మూలం))
                తిరిగి 0;

        పూర్ణాంకం log_level = conf_files_log_level(flags);

        r = chaseat_ఉపసర్గ_మూలం(c->ఫలితం, మూలం, &p);
        ఒకవేళ (r < 0)
                తిరిగి log_నిండిన_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->ఫలితం, మూలం);
        free_and_replace(c->ఫలితం, p);

        r = chaseat_ఉపసర్గ_మూలం(c->resolved_మార్గం, మూలం, &p);
        ఒకవేళ (r < 0)
                తిరిగి log_నిండిన_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->resolved_మార్గం, మూలం);
        free_and_replace(c->resolved_మార్గం, p);

        /* Do not use chaseat_prefix_root(), as it is for the result of chaseat(), but the path is not chased. */
        p = మార్గం_join(ఖాళీలు_గమ్యం_మూలం(మూలం), skip_leading_slash(c->original_మార్గం));
        ఒకవేళ (!p)
                తిరిగి log_oom_నిండిన(log_level);

        మార్గం_simplify(p);
        తిరిగి free_and_replace(c->original_మార్గం, p);
}

స్థిర bool conf_files_need_stat(ConfFilesFlags flags) {
        తిరిగి (flags & (CONF_FILES_FILTER_MASKED | CONF_FILES_REGULAR | CONF_FILES_DIRECTORY | CONF_FILES_EXECUTABLE)) != 0;
}

స్థిర ChaseFlags conf_files_chase_flags(ConfFilesFlags flags) {
        ChaseFlags chase_flags = 0;

        ఒకవేళ (!conf_files_need_stat(flags) || FLAGS_సమితి(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK))
                /* Even if no verification is requested, let's unconditionally call chaseat(),
                 * to drop unsafe symlinks. */
                chase_flags |= CHASE_NONEXISTENT;

        తిరిగి chase_flags;
}

స్థిర పూర్ణాంకం conf_file_chase_and_verify(
                స్థిరరాశి అక్షరం *మూలం,          /* for logging, can be NULL */
                పూర్ణాంకం rfd,
                స్థిరరాశి అక్షరం *original_మార్గం, /* for logging */
                స్థిరరాశి అక్షరం *మార్గం,
                స్థిరరాశి అక్షరం *name,
                సమితి_2 **masked,              /* optional */
                ConfFilesFlags flags,
                అక్షరం **ret_మార్గం,
                పూర్ణాంకం *ret_fd,
                నిర్మాణం stat *ret_stat) {

        _cleanup_free_ అక్షరం *resolved_మార్గం = ఖాళీ;
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        నిర్మాణం stat st = {};
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(wildcard_fd_is_valid(rfd));
        ప్రతిజ్ఞా(original_మార్గం);
        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(name);

        పూర్ణాంకం log_level = conf_files_log_level(flags);

        మూలం = ఖాళీలు_గమ్యం_మూలం(మూలం);

        r = chaseat(rfd, rfd, మార్గం, conf_files_chase_flags(flags), &resolved_మార్గం, &fd);
        ఒకవేళ (r < 0)
                తిరిగి log_నిండిన_errno(log_level, r, "Failed to chase '%s%s': %m",
                                      మూలం, skip_leading_slash(original_మార్గం));
        ఒకవేళ (r == 0) {
                ఒకవేళ (FLAGS_సమితి(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK)) {
                        /* If the path points to /dev/null in a image or so, then the device node may not exist. */
                        ఒకవేళ (మార్గం_equal(skip_leading_slash(resolved_మార్గం), "dev/null")) {
                                ఒకవేళ (masked) {
                                        /* Mark this one as masked */
                                        r = సమితి_put_strdup(masked, name);
                                        ఒకవేళ (r < 0)
                                                తిరిగి log_oom_నిండిన(log_level);
                                }

                                తిరిగి log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                                       మూలం, skip_leading_slash(original_మార్గం));
                        }
                }

                ఒకవేళ (conf_files_need_stat(flags))
                        /* If we need to have stat, skip the entry. */
                        తిరిగి log_నిండిన_errno(log_level, SYNTHETIC_ERRNO(ENOENT), "Failed to chase '%s%s': %m",
                                              మూలం, skip_leading_slash(original_మార్గం));
        }

        /* Even if we do not need stat, let's take stat now. The caller may use the info later. */
        ఒకవేళ (fd >= 0 && fstat(fd, &st) < 0)
                తిరిగి log_నిండిన_errno(log_level, errno, "Failed to stat '%s%s': %m",
                                      మూలం, skip_leading_slash(original_మార్గం));

        /* Is this a masking entry? */
        ఒకవేళ (FLAGS_సమితి(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK) && stat_may_be_dev_null(&st)) {
                ఒకవేళ (masked) {
                        /* Mark this one as masked */
                        r = సమితి_put_strdup(masked, name);
                        ఒకవేళ (r < 0)
                                తిరిగి log_oom_నిండిన(log_level);
                }

                తిరిగి log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                       మూలం, skip_leading_slash(original_మార్గం));
        }

        ఒకవేళ (FLAGS_సమితి(flags, CONF_FILES_FILTER_MASKED_BY_ఖాళీలు) && stat_is_ఖాళీలు(&st)) {
                ఒకవేళ (masked) {
                        /* Mark this one as masked */
                        r = సమితి_put_strdup(masked, name);
                        ఒకవేళ (r < 0)
                                తిరిగి log_oom_నిండిన(log_level);
                }

                తిరిగి log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (an empty file).",
                                       మూలం, skip_leading_slash(original_మార్గం));
        }

        ఒకవేళ (FLAGS_సమితి(flags, CONF_FILES_REGULAR|CONF_FILES_DIRECTORY)) {
                ఒకవేళ (!S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode))
                        తిరిగి log_debug_errno(SYNTHETIC_ERRNO(EBADFD),
                                               "File '%s%s' is neither a regular file or directory.",
                                               మూలం, skip_leading_slash(original_మార్గం));
        } లేకపోతే {
                /* Is this node a regular file? */
                ఒకవేళ (FLAGS_సమితి(flags, CONF_FILES_REGULAR)) {
                        r = stat_verify_regular(&st);
                        ఒకవేళ (r < 0)
                                తిరిగి log_debug_errno(r, "File '%s%s' is not a regular file: %m",
                                                       మూలం, skip_leading_slash(original_మార్గం));
                }

                /* Is this node a directory? */
                ఒకవేళ (FLAGS_సమితి(flags, CONF_FILES_DIRECTORY)) {
                        r = stat_verify_directory(&st);
                        ఒకవేళ (r < 0)
                                తిరిగి log_debug_errno(r, "File '%s%s' is not a directory: %m",
                                                       మూలం, skip_leading_slash(original_మార్గం));
                }
        }

        /* Does this node have the executable bit set?
         * As requested: check if the file is marked executable. Note that we don't check access(X_OK) here,
         * as we care about whether the file is marked executable at all, and not whether it is executable
         * for us, because if so, such errors are stuff we should log about. */
        ఒకవేళ (FLAGS_సమితి(flags, CONF_FILES_EXECUTABLE) && (st.st_mode & 0111) == 0)
                తిరిగి log_debug_errno(SYNTHETIC_ERRNO(ENOEXEC),
                                       "File '%s%s' is not marked executable.",
                                       మూలం, skip_leading_slash(original_మార్గం));

        ఒకవేళ (ret_మార్గం)
                *ret_మార్గం = TAKE_PTR(resolved_మార్గం);
        ఒకవేళ (ret_fd)
                *ret_fd = TAKE_FD(fd);
        ఒకవేళ (ret_stat)
                *ret_stat = st;

        తిరిగి 0;
}

పూర్ణాంకం conf_file_new_at(
                స్థిరరాశి అక్షరం *మార్గం,
                స్థిరరాశి అక్షరం *మూలం,
                పూర్ణాంకం rfd,
                ConfFilesFlags flags,
                ConfFile **ret) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(wildcard_fd_is_valid(rfd));
        ప్రతిజ్ఞా(ret);

        పూర్ణాంకం log_level = conf_files_log_level(flags);

        _cleanup_free_ అక్షరం *మూలం_2 = ఖాళీ;
        ఒకవేళ (DEBUG_LOGGING && !మూలం) {
                (శూన్య) fd_get_మార్గం(rfd, &మూలం_2);
                మూలం = మూలం_2;
        }

        _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
        ఒకవేళ (!c)
                తిరిగి log_oom_నిండిన(log_level);

        *c = (ConfFile) {
                .original_మార్గం = strdup(మార్గం),
                .fd = -EBADF,
        };

        ఒకవేళ (!c->original_మార్గం)
                తిరిగి log_oom_నిండిన(log_level);

        r = మార్గం_వెలికితీయి_filename(మార్గం, &c->filename);
        ఒకవేళ (r < 0)
                తిరిగి log_నిండిన_errno(log_level, r, "Failed to extract filename from '%s': %m", మార్గం);

        _cleanup_free_ అక్షరం *dirpath = ఖాళీ, *resolved_dirpath = ఖాళీ;
        r = మార్గం_వెలికితీయి_directory(మార్గం, &dirpath);
        ఒకవేళ (r < 0 && r != -EDESTADDRREQ)
                తిరిగి log_నిండిన_errno(log_level, r, "Failed to extract directory from '%s': %m", మార్గం);
        ఒకవేళ (r >= 0) {
                r = chaseat(rfd, rfd, dirpath,
                            CHASE_MUST_BE_DIRECTORY | conf_files_chase_flags(flags),
                            &resolved_dirpath, /* ret_fd= */ ఖాళీ);
                ఒకవేళ (r < 0)
                        తిరిగి log_నిండిన_errno(log_level, r, "Failed to chase '%s%s': %m", ఖాళీలు_గమ్యం_మూలం(మూలం), skip_leading_slash(dirpath));
        }

        c->ఫలితం = మార్గం_join(resolved_dirpath, c->filename);
        ఒకవేళ (!c->ఫలితం)
                తిరిగి log_oom_నిండిన(log_level);

        r = conf_file_chase_and_verify(
                        మూలం,
                        rfd,
                        c->original_మార్గం,
                        c->ఫలితం,
                        c->filename,
                        /* masked= */ ఖాళీ,
                        flags,
                        &c->resolved_మార్గం,
                        &c->fd,
                        &c->st);
        ఒకవేళ (r < 0)
                తిరిగి r;

        *ret = TAKE_PTR(c);
        తిరిగి 0;
}

పూర్ణాంకం conf_file_new(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *మూలం, ConfFilesFlags flags, ConfFile **ret) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(ret);

        _cleanup_free_ అక్షరం *మూలం_abs = ఖాళీ;
        _cleanup_close_ పూర్ణాంకం rfd = -EBADF;
        r = prepare_dirs(మూలం, flags, /* dirs= */ ఖాళీ, &మూలం_abs, &rfd, /* ret_dirs= */ ఖాళీ);
        ఒకవేళ (r < 0)
                తిరిగి r;

        _cleanup_free_ అక్షరం *మార్గం_abs = ఖాళీ;
        ఒకవేళ (!మూలం_abs) {
                r = మార్గం_make_absolute_cwd(మార్గం, &మార్గం_abs);
                ఒకవేళ (r < 0)
                        తిరిగి log_నిండిన_errno(conf_files_log_level(flags), r, "Failed to make '%s' absolute: %m", మార్గం);

                మార్గం = మార్గం_abs;
        }

        _cleanup_(conf_file_freep) ConfFile *c = ఖాళీ;
        r = conf_file_new_at(మార్గం, మూలం_abs, rfd, flags, &c);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = conf_file_ఉపసర్గ_మూలం(c, మూలం_abs, flags);
        ఒకవేళ (r < 0)
                తిరిగి r;

        *ret = TAKE_PTR(c);
        తిరిగి 0;
}

DEFINE_వ్యక్తిగత_HASH_OPS_WITH_విలువ_DESTRUCTOR(
                conf_file_hash_ops,
                అక్షరం, string_hash_func, string_compare_func,
                ConfFile, conf_file_free);

స్థిర పూర్ణాంకం files_కూడు(
                DIR *dir,
                స్థిరరాశి అక్షరం *original_dirpath,
                స్థిరరాశి అక్షరం *resolved_dirpath,
                స్థిరరాశి అక్షరం *మూలం, /* for logging, can be NULL */
                పూర్ణాంకం rfd,
                Hashmap **files,
                సమితి_2 **masked,
                స్థిరరాశి అక్షరం *suffix,
                ConfFilesFlags flags) {

        పూర్ణాంకం r;

        ప్రతిజ్ఞా(dir);
        ప్రతిజ్ఞా(original_dirpath);
        ప్రతిజ్ఞా(resolved_dirpath);
        ప్రతిజ్ఞా(wildcard_fd_is_valid(rfd));
        ప్రతిజ్ఞా(files);
        ప్రతిజ్ఞా(masked);

        పూర్ణాంకం log_level = conf_files_log_level(flags);

        మూలం = ఖాళీలు_గమ్యం_మూలం(మూలం);

        FOREACH_DIRENT(de, dir, తిరిగి log_నిండిన_errno(log_level, errno, "Failed to read directory '%s%s': %m",
                                                      మూలం, skip_leading_slash(original_dirpath))) {

                _cleanup_free_ అక్షరం *original_మార్గం = మార్గం_join(original_dirpath, de->d_name);
                ఒకవేళ (!original_మార్గం)
                        తిరిగి log_oom_నిండిన(log_level);

                /* Does this match the suffix? */
                ఒకవేళ (suffix && !endswith(de->d_name, suffix)) {
                        log_debug("Skipping file '%s%s', suffix is not '%s'.", మూలం, skip_leading_slash(original_మార్గం), suffix);
                        కొనసాగు;
                }

                /* Has this file already been found in an earlier directory? */
                ఒకవేళ (hashmap_కలిగియుంది(*files, de->d_name)) {
                        log_debug("Skipping overridden file '%s%s'.", మూలం, skip_leading_slash(original_మార్గం));
                        కొనసాగు;
                }

                /* Has this been masked in an earlier directory? */
                ఒకవేళ ((flags & CONF_FILES_FILTER_MASKED) != 0 && సమితి_కలిగియుంది(*masked, de->d_name)) {
                        log_debug("File '%s%s' is masked by previous entry.", మూలం, skip_leading_slash(original_మార్గం));
                        కొనసాగు;
                }

                _cleanup_free_ అక్షరం *p = మార్గం_join(resolved_dirpath, de->d_name);
                ఒకవేళ (!p)
                        తిరిగి log_oom_నిండిన(log_level);

                _cleanup_free_ అక్షరం *resolved_మార్గం = ఖాళీ;
                _cleanup_close_ పూర్ణాంకం fd = -EBADF;
                నిర్మాణం stat st;
                r = conf_file_chase_and_verify(
                                మూలం,
                                rfd,
                                original_మార్గం,
                                p,
                                de->d_name,
                                masked,
                                flags,
                                &resolved_మార్గం,
                                &fd,
                                &st);
                ఒకవేళ (r == -ENOMEM)
                        తిరిగి r;
                ఒకవేళ (r < 0)
                        కొనసాగు;

                _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
                ఒకవేళ (!c)
                        తిరిగి log_oom_నిండిన(log_level);

                *c = (ConfFile) {
                        .filename = strdup(de->d_name),
                        .ఫలితం = TAKE_PTR(p),
                        .original_మార్గం = TAKE_PTR(original_మార్గం),
                        .resolved_మార్గం = TAKE_PTR(resolved_మార్గం),
                        .fd = TAKE_FD(fd),
                        .st = st,
                };

                ఒకవేళ (!c->filename)
                        తిరిగి log_oom_నిండిన(log_level);

                r = hashmap_ensure_put(files, &conf_file_hash_ops, c->filename, c);
                ఒకవేళ (r < 0) {
                        ప్రతిజ్ఞా(r == -ENOMEM);
                        తిరిగి log_oom_నిండిన(log_level);
                }
                ప్రతిజ్ఞా(r > 0);

                TAKE_PTR(c);
        }

        తిరిగి 0;
}

స్థిర పూర్ణాంకం dump_files(Hashmap *fh, స్థిరరాశి అక్షరం *మూలం, ConfFilesFlags flags, ConfFile ***ret_files, size_t *ret_n_files) {
        ConfFile **files = ఖాళీ;
        size_t n_files = 0;
        పూర్ణాంకం r;

        CLEANUP_ARRAY(files, n_files, conf_file_free_array);

        ప్రతిజ్ఞా(ret_files);
        ప్రతిజ్ఞా(ret_n_files);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap. */
        r = hashmap_dump_sorted(fh, (శూన్య***) &files, &n_files);
        ఒకవేళ (r < 0)
                తిరిగి log_oom_నిండిన(conf_files_log_level(flags));

        /* Hence, we need to remove them from the hashmap. */
        FOREACH_ARRAY(i, files, n_files)
                assert_se(hashmap_remove(fh, (*i)->filename) == *i);

        ఒకవేళ (మూలం)
                FOREACH_ARRAY(i, files, n_files) {
                        r = conf_file_ఉపసర్గ_మూలం(*i, మూలం, flags);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                }

        *ret_files = TAKE_PTR(files);
        *ret_n_files = n_files;
        తిరిగి 0;
}

స్థిర పూర్ణాంకం copy_and_క్రమీకరించు_files_from_hashmap(
                Hashmap *fh,
                స్థిరరాశి అక్షరం *suffix,
                స్థిరరాశి అక్షరం *మూలం,
                ConfFilesFlags flags,
                అక్షరం ***ret) {

        _cleanup_strv_free_ అక్షరం **results = ఖాళీ;
        _cleanup_free_ ConfFile **files = ఖాళీ;
        size_t n_files = 0, n_results = 0;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        పూర్ణాంకం log_level = conf_files_log_level(flags);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap.
         * Hence, do not use conf_file_free_array() for 'entries' */
        r = hashmap_dump_sorted(fh, (శూన్య***) &files, &n_files);
        ఒకవేళ (r < 0)
                తిరిగి log_oom_నిండిన(log_level);

        FOREACH_ARRAY(i, files, n_files) {
                ConfFile *c = *i;
                స్థిరరాశి అక్షరం *కూడు = ఖాళీ;

                ఒకవేళ (FLAGS_సమితి(flags, CONF_FILES_BASENAME))
                        కూడు = c->filename;
                లేకపోతే ఒకవేళ (మూలం && !FLAGS_సమితి(flags, CONF_FILES_DONT_ఉపసర్గ_మూలం)) {
                        _cleanup_free_ అక్షరం *p = ఖాళీ;

                        r = chaseat_ఉపసర్గ_మూలం(c->ఫలితం, మూలం, &p);
                        ఒకవేళ (r < 0)
                                తిరిగి log_నిండిన_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->ఫలితం, మూలం);

                        ఒకవేళ (FLAGS_సమితి(flags, CONF_FILES_TRUNCATE_SUFFIX) && suffix) {
                                అక్షరం *e = endswith(p, suffix);
                                ఒకవేళ (!e)
                                        కొనసాగు;

                                *e = 0;
                        }

                        ఒకవేళ (strv_consume_with_కొలత(&results, &n_results, TAKE_PTR(p)) < 0)
                                తిరిగి log_oom_నిండిన(log_level);

                        కొనసాగు;
                } లేకపోతే
                        కూడు = c->ఫలితం;

                ఒకవేళ (FLAGS_సమితి(flags, CONF_FILES_TRUNCATE_SUFFIX)) {
                        స్థిరరాశి అక్షరం *e = endswith(కూడు, suffix);
                        ఒకవేళ (!e)
                                కొనసాగు;

                        _cleanup_free_ అక్షరం *n = strndup(కూడు, e - కూడు);
                        ఒకవేళ (!n)
                                తిరిగి log_oom_నిండిన(log_level);

                        r = strv_consume_with_కొలత(&results, &n_results, TAKE_PTR(n));
                } లేకపోతే
                        r = strv_extend_with_కొలత(&results, &n_results, కూడు);
                ఒకవేళ (r < 0)
                        తిరిగి log_oom_నిండిన(log_level);
        }

        *ret = TAKE_PTR(results);
        తిరిగి 0;
}

స్థిర పూర్ణాంకం చొప్పించు_replacement(Hashmap **fh, ConfFile *replacement, ConfFilesFlags flags, స్థిరరాశి ConfFile **ret) {
        _cleanup_(conf_file_freep) ConfFile *c = ASSERT_PTR(replacement);
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fh);
        ప్రతిజ్ఞా(ret);

        /* This consumes the input ConfFile. */

        ConfFile *existing = hashmap_get(*fh, c->filename);
        ఒకవేళ (existing) {
                log_debug("An entry with higher priority '%s' -> '%s' already exists, ignoring the replacement: %s",
                          existing->filename, existing->ఫలితం, c->original_మార్గం);
                *ret = ఖాళీ;
                తిరిగి 0;
        }

        r = hashmap_ensure_put(fh, &conf_file_hash_ops, c->filename, c);
        ఒకవేళ (r < 0) {
                ప్రతిజ్ఞా(r == -ENOMEM);
                తిరిగి log_oom_నిండిన(conf_files_log_level(flags));
        }
        ప్రతిజ్ఞా(r > 0);

        log_debug("Inserted replacement: '%s' -> '%s'", c->filename, c->ఫలితం);

        *ret = TAKE_PTR(c);
        తిరిగి 0;
}

స్థిర పూర్ణాంకం conf_files_list_impl(
                స్థిరరాశి అక్షరం *suffix,
                స్థిరరాశి అక్షరం *మూలం, /* for logging, can be NULL */
                పూర్ణాంకం rfd,
                ConfFilesFlags flags,
                స్థిరరాశి అక్షరం * స్థిరరాశి *dirs,
                స్థిరరాశి అక్షరం *replacement,
                Hashmap **ret,
                స్థిరరాశి ConfFile **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = ఖాళీ;
        cleanup_సమితి_free సమితి_2 *masked = ఖాళీ;
        _cleanup_(conf_file_freep) ConfFile *c = ఖాళీ;
        స్థిరరాశి ConfFile *inserted = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(wildcard_fd_is_valid(rfd));
        ప్రతిజ్ఞా(ret);

        మూలం = ఖాళీలు_గమ్యం_మూలం(మూలం);

        ఒకవేళ (replacement) {
                r = conf_file_new_at(replacement, మూలం, rfd, flags & CONF_FILES_WARN, &c);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        STRV_FOREACH(p, dirs) {
                _cleanup_closedir_ DIR *dir = ఖాళీ;
                _cleanup_free_ అక్షరం *మార్గం = ఖాళీ;

                r = chase_and_opendirat(rfd, rfd, *p, 0, &మార్గం, &dir);
                ఒకవేళ (r < 0) {
                        ఒకవేళ (r != -ENOENT)
                                log_నిండిన_errno(conf_files_log_level(flags), r,
                                               "Failed to chase and open directory '%s%s', ignoring: %m",
                                               మూలం, skip_leading_slash(*p));
                        కొనసాగు;
                }

                ఒకవేళ (c && streq_ptr(మార్గం_startswith(c->ఫలితం, మార్గం), c->filename)) {
                        r = చొప్పించు_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                }

                r = files_కూడు(dir, *p, మార్గం, మూలం, rfd, &fh, &masked, suffix, flags);
                ఒకవేళ (r == -ENOMEM)
                        తిరిగి r;
        }

        ఒకవేళ (c) {
                r = చొప్పించు_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        *ret = TAKE_PTR(fh);
        ఒకవేళ (ret_inserted)
                *ret_inserted = inserted;
        తిరిగి 0;
}

పూర్ణాంకం conf_files_list_strv(
                అక్షరం ***ret,
                స్థిరరాశి అక్షరం *suffix,
                స్థిరరాశి అక్షరం *మూలం,
                ConfFilesFlags flags,
                స్థిరరాశి అక్షరం * స్థిరరాశి *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = ఖాళీ;
        _cleanup_close_ పూర్ణాంకం rfd = -EBADF;
        _cleanup_free_ అక్షరం *మూలం_abs = ఖాళీ;
        _cleanup_strv_free_ అక్షరం **dirs_abs = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        r = prepare_dirs(మూలం, flags, (అక్షరం**) dirs, &మూలం_abs, &rfd, &dirs_abs);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = conf_files_list_impl(suffix, మూలం_abs, rfd, flags, (స్థిరరాశి అక్షరం * స్థిరరాశి *) dirs_abs,
                                 /* replacement= */ ఖాళీ, &fh, /* ret_inserted= */ ఖాళీ);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి copy_and_క్రమీకరించు_files_from_hashmap(fh, suffix, ఖాళీలు_గమ్యం_మూలం(మూలం_abs), flags, ret);
}

పూర్ణాంకం conf_files_list_strv_నిండిన(
                స్థిరరాశి అక్షరం *suffix,
                స్థిరరాశి అక్షరం *మూలం,
                ConfFilesFlags flags,
                స్థిరరాశి అక్షరం * స్థిరరాశి *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = ఖాళీ;
        _cleanup_close_ పూర్ణాంకం rfd = -EBADF;
        _cleanup_free_ అక్షరం *మూలం_abs = ఖాళీ;
        _cleanup_strv_free_ అక్షరం **dirs_abs = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret_files);
        ప్రతిజ్ఞా(ret_n_files);

        r = prepare_dirs(మూలం, flags, (అక్షరం**) dirs, &మూలం_abs, &rfd, &dirs_abs);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = conf_files_list_impl(suffix, మూలం_abs, rfd, flags, (స్థిరరాశి అక్షరం * స్థిరరాశి *) dirs_abs,
                                 /* replacement= */ ఖాళీ, &fh, /* ret_inserted= */ ఖాళీ);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి dump_files(fh, ఖాళీలు_గమ్యం_మూలం(మూలం_abs), flags, ret_files, ret_n_files);
}

పూర్ణాంకం conf_files_list_strv_at(
                అక్షరం ***ret,
                స్థిరరాశి అక్షరం *suffix,
                పూర్ణాంకం rfd,
                ConfFilesFlags flags,
                స్థిరరాశి అక్షరం * స్థిరరాశి *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = ఖాళీ;
        _cleanup_free_ అక్షరం *మూలం = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(wildcard_fd_is_valid(rfd));
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (DEBUG_LOGGING)
                (శూన్య) fd_get_మార్గం(rfd, &మూలం); /* for logging */

        r = conf_files_list_impl(suffix, మూలం, rfd, flags, dirs, /* replacement= */ ఖాళీ, &fh, /* ret_inserted= */ ఖాళీ);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి copy_and_క్రమీకరించు_files_from_hashmap(fh, suffix, /* root= */ ఖాళీ, flags, ret);
}

పూర్ణాంకం conf_files_list_strv_at_నిండిన(
                స్థిరరాశి అక్షరం *suffix,
                పూర్ణాంకం rfd,
                ConfFilesFlags flags,
                స్థిరరాశి అక్షరం * స్థిరరాశి *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = ఖాళీ;
        _cleanup_free_ అక్షరం *మూలం = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(wildcard_fd_is_valid(rfd));
        ప్రతిజ్ఞా(ret_files);
        ప్రతిజ్ఞా(ret_n_files);

        ఒకవేళ (DEBUG_LOGGING)
                (శూన్య) fd_get_మార్గం(rfd, &మూలం); /* for logging */

        r = conf_files_list_impl(suffix, మూలం, rfd, flags, dirs, /* replacement= */ ఖాళీ, &fh, /* ret_inserted= */ ఖాళీ);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి dump_files(fh, /* root= */ ఖాళీ, flags, ret_files, ret_n_files);
}

పూర్ణాంకం conf_files_list(అక్షరం ***ret, స్థిరరాశి అక్షరం *suffix, స్థిరరాశి అక్షరం *మూలం, ConfFilesFlags flags, స్థిరరాశి అక్షరం *dir) {
        తిరిగి conf_files_list_strv(ret, suffix, మూలం, flags, STRV_MAKE_CONST(dir));
}

పూర్ణాంకం conf_files_list_నిండిన(స్థిరరాశి అక్షరం *suffix, స్థిరరాశి అక్షరం *మూలం, ConfFilesFlags flags, స్థిరరాశి అక్షరం *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        తిరిగి conf_files_list_strv_నిండిన(suffix, మూలం, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

పూర్ణాంకం conf_files_list_at(అక్షరం ***ret, స్థిరరాశి అక్షరం *suffix, పూర్ణాంకం rfd, ConfFilesFlags flags, స్థిరరాశి అక్షరం *dir) {
        తిరిగి conf_files_list_strv_at(ret, suffix, rfd, flags, STRV_MAKE_CONST(dir));
}

పూర్ణాంకం conf_files_list_at_నిండిన(స్థిరరాశి అక్షరం *suffix, పూర్ణాంకం rfd, ConfFilesFlags flags, స్థిరరాశి అక్షరం *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        తిరిగి conf_files_list_strv_at_నిండిన(suffix, rfd, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

పూర్ణాంకం conf_files_list_nulstr(అక్షరం ***ret, స్థిరరాశి అక్షరం *suffix, స్థిరరాశి అక్షరం *మూలం, ConfFilesFlags flags, స్థిరరాశి అక్షరం *dirs) {
        _cleanup_strv_free_ అక్షరం **d = ఖాళీ;

        ప్రతిజ్ఞా(ret);

        d = strv_విభజించు_nulstr(dirs);
        ఒకవేళ (!d)
                తిరిగి log_oom_నిండిన(conf_files_log_level(flags));

        తిరిగి conf_files_list_strv(ret, suffix, మూలం, flags, (స్థిరరాశి అక్షరం**) d);
}

పూర్ణాంకం conf_files_list_nulstr_నిండిన(స్థిరరాశి అక్షరం *suffix, స్థిరరాశి అక్షరం *మూలం, ConfFilesFlags flags, స్థిరరాశి అక్షరం *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ అక్షరం **d = ఖాళీ;

        ప్రతిజ్ఞా(ret_files);
        ప్రతిజ్ఞా(ret_n_files);

        d = strv_విభజించు_nulstr(dirs);
        ఒకవేళ (!d)
                తిరిగి log_oom_నిండిన(conf_files_log_level(flags));

        తిరిగి conf_files_list_strv_నిండిన(suffix, మూలం, flags, (స్థిరరాశి అక్షరం**) d, ret_files, ret_n_files);
}

పూర్ణాంకం conf_files_list_nulstr_at(అక్షరం ***ret, స్థిరరాశి అక్షరం *suffix, పూర్ణాంకం rfd, ConfFilesFlags flags, స్థిరరాశి అక్షరం *dirs) {
        _cleanup_strv_free_ అక్షరం **d = ఖాళీ;

        ప్రతిజ్ఞా(ret);

        d = strv_విభజించు_nulstr(dirs);
        ఒకవేళ (!d)
                తిరిగి log_oom_నిండిన(conf_files_log_level(flags));

        తిరిగి conf_files_list_strv_at(ret, suffix, rfd, flags, (స్థిరరాశి అక్షరం**) d);
}

పూర్ణాంకం conf_files_list_nulstr_at_నిండిన(స్థిరరాశి అక్షరం *suffix, పూర్ణాంకం rfd, ConfFilesFlags flags, స్థిరరాశి అక్షరం *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ అక్షరం **d = ఖాళీ;

        ప్రతిజ్ఞా(ret_files);
        ప్రతిజ్ఞా(ret_n_files);

        d = strv_విభజించు_nulstr(dirs);
        ఒకవేళ (!d)
                తిరిగి log_oom_నిండిన(conf_files_log_level(flags));

        తిరిగి conf_files_list_strv_at_నిండిన(suffix, rfd, flags, (స్థిరరాశి అక్షరం**) d, ret_files, ret_n_files);
}

పూర్ణాంకం conf_files_list_with_replacement(
                స్థిరరాశి అక్షరం *మూలం,
                అక్షరం **config_dirs,
                స్థిరరాశి అక్షరం *replacement,
                అక్షరం ***ret_files,
                అక్షరం **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = ఖాళీ;
        _cleanup_free_ అక్షరం *inserted = ఖాళీ;
        ConfFilesFlags flags = CONF_FILES_REGULAR | CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_WARN;
        _cleanup_close_ పూర్ణాంకం rfd = -EBADF;
        _cleanup_free_ అక్షరం *మూలం_abs = ఖాళీ;
        _cleanup_strv_free_ అక్షరం **dirs_abs = ఖాళీ;
        స్థిరరాశి ConfFile *c = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret_files);

        r = prepare_dirs(మూలం, flags, config_dirs, &మూలం_abs, &rfd, &dirs_abs);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = conf_files_list_impl(".conf", మూలం_abs, rfd, flags, (స్థిరరాశి అక్షరం * స్థిరరాశి *) dirs_abs,
                                 replacement, &fh, ret_inserted ? &c : ఖాళీ);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (c) {
                r = chaseat_ఉపసర్గ_మూలం(c->ఫలితం, మూలం_abs, &inserted);
                ఒకవేళ (r < 0)
                        తిరిగి log_నిండిన_errno(conf_files_log_level(flags), r,
                                              "Failed to prefix '%s' with root '%s': %m",
                                              c->ఫలితం, ఖాళీలు_గమ్యం_మూలం(మూలం_abs));
        }

        r = copy_and_క్రమీకరించు_files_from_hashmap(fh, ".conf", ఖాళీలు_గమ్యం_మూలం(మూలం_abs), flags, ret_files);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret_inserted)
                *ret_inserted = TAKE_PTR(inserted);
        తిరిగి 0;
}

పూర్ణాంకం conf_files_list_dropins(
                అక్షరం ***ret,
                స్థిరరాశి అక్షరం *dropin_dirname,
                స్థిరరాశి అక్షరం *మూలం,
                పూర్ణాంకం మూలం_fd,
                ConfFilesFlags flags,
                స్థిరరాశి అక్షరం * స్థిరరాశి *dirs) {

        _cleanup_strv_free_ అక్షరం **dropin_dirs = ఖాళీ;
        స్థిరరాశి అక్షరం *suffix;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);
        ప్రతిజ్ఞా(dropin_dirname);
        ప్రతిజ్ఞా(dirs);

        suffix = strjoina("/", dropin_dirname);
        r = strv_extend_strv_concat(&dropin_dirs, dirs, suffix);
        ఒకవేళ (r < 0)
                తిరిగి log_oom_నిండిన(conf_files_log_level(flags));

        తిరిగి conf_files_list_strv_at(ret, ".conf", మూలం_fd, flags, (స్థిరరాశి అక్షరం* స్థిరరాశి*) dropin_dirs);
}

/**
 * Open and read a config file.
 *
 * The <fn> argument may be:
 * - '-', meaning stdin.
 * - a file name without a path. In this case <config_dirs> are searched.
 * - a path, either relative or absolute. In this case <fn> is opened directly.
 *
 * This method is only suitable for configuration files which have a flat layout without dropins.
 */
పూర్ణాంకం conf_file_read(
                స్థిరరాశి అక్షరం *మూలం,
                స్థిరరాశి అక్షరం **config_dirs,
                స్థిరరాశి అక్షరం *fn,
                parse_పంక్తి_t parse_పంక్తి,
                శూన్య *userdata,
                bool ignore_enoent,
                bool *invalid_config) {

        _cleanup_fclose_ FILE *_f = ఖాళీ;
        _cleanup_free_ అక్షరం *_fn = ఖాళీ;
        గుర్తులేని v = 0;
        FILE *f;
        పూర్ణాంకం r = 0;

        ప్రతిజ్ఞా(fn);

        ఒకవేళ (streq(fn, "-")) {
                f = stdin;
                fn = "<stdin>";

                log_debug("Reading config from stdin%s", glyph(GLYPH_ELLIPSIS));

        } లేకపోతే ఒకవేళ (is_మార్గం(fn)) {
                r = మార్గం_make_absolute_cwd(fn, &_fn);
                ఒకవేళ (r < 0)
                        తిరిగి log_error_errno(r, "Failed to make path absolute: %m");
                fn = _fn;

                f = _f = fopen(fn, "re");
                ఒకవేళ (!_f)
                        r = -errno;
                లేకపోతే
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));

        } లేకపోతే {
                r = శోధన_and_fopen(fn, "re", మూలం, config_dirs, &_f, &_fn);
                ఒకవేళ (r >= 0) {
                        f = _f;
                        fn = _fn;
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));
                }
        }

        ఒకవేళ (r == -ENOENT && ignore_enoent) {
                log_debug_errno(r, "Failed to open \"%s\", ignoring: %m", fn);
                తిరిగి 0; /* No error, but nothing happened. */
        }
        ఒకవేళ (r < 0)
                తిరిగి log_error_errno(r, "Failed to read '%s': %m", fn);

        r = 1;  /* We entered the part where we may modify state. */

        కొరకు (;;) {
                _cleanup_free_ అక్షరం *పంక్తి = ఖాళీ;
                bool invalid_పంక్తి = false;
                పూర్ణాంకం k;

                k = read_stripped_పంక్తి(f, LONG_పంక్తి_MAX, &పంక్తి);
                ఒకవేళ (k < 0)
                        తిరిగి log_error_errno(k, "Failed to read '%s': %m", fn);
                ఒకవేళ (k == 0)
                        ఆపు;

                v++;

                ఒకవేళ (లోపల_సమితి(పంక్తి[0], 0, '#'))
                        కొనసాగు;

                k = parse_పంక్తి(fn, v, పంక్తి, invalid_config ? &invalid_పంక్తి : ఖాళీ, userdata);
                ఒకవేళ (k < 0 && invalid_పంక్తి)
                        /* Allow reporting with a special code if the caller requested this. */
                        *invalid_config = true;
                లేకపోతే
                        /* The first error, if any, becomes our return value. */
                        RET_GATHER(r, k);
        }

        ఒకవేళ (ferror(f))
                RET_GATHER(r, log_error_errno(SYNTHETIC_ERRNO(EIO), "Failed to read from file %s.", fn));

        తిరిగి r;
}
