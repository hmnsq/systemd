/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "chase.h"
#अन्तर्भाव "conf-files.h"
#अन्तर्भाव "dirent-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "glyph-util.h"
#अन्तर्भाव "hashmap.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "nulstr-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "set.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"

ConfFile* conf_file_free(ConfFile *c) {
        यदि (!c)
                प्रत्यागम रिक्त;

        मुक्त(c->filename);
        मुक्त(c->result);
        मुक्त(c->original_path);
        मुक्त(c->resolved_path);
        safe_close(c->fd);

        प्रत्यागम mfree(c);
}

DEFINE_POINTER_ARRAY_FREE_FUNC(ConfFile*, conf_file_free);

स्थिर पूर्णाङ्क conf_files_log_level(ConfFilesFlags flags) {
        प्रत्यागम FLAGS_SET(flags, CONF_FILES_WARN) ? LOG_WARNING : LOG_DEBUG;
}

स्थिर पूर्णाङ्क prepare_dirs(
                ध्रुव वर्ण *मूळ,
                ConfFilesFlags flags,
                वर्ण * ध्रुव *dirs,
                वर्ण **ret_मूळ,
                पूर्णाङ्क *ret_rfd,
                वर्ण ***ret_dirs) {

        _cleanup_free_ वर्ण *मूळ_abs = रिक्त;
        _cleanup_strv_free_ वर्ण **dirs_abs = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret_मूळ);
        प्रतिज्ञा(ret_rfd);
        प्रतिज्ञा(ret_dirs || strv_isempty(dirs));

        पूर्णाङ्क log_level = conf_files_log_level(flags);

        r = empty_or_मूळ_harder_to_null(&मूळ);
        यदि (r < 0)
                प्रत्यागम log_full_errno(log_level, r, "Failed to determine if '%s' points to the root directory: %m", strempty(मूळ));

        यदि (ret_dirs) {
                dirs_abs = strv_copy(dirs);
                यदि (!dirs_abs)
                        प्रत्यागम log_oom_full(log_level);
        }

        _cleanup_close_ पूर्णाङ्क rfd = XAT_FDROOT;
        यदि (मूळ) {
                /* When a non-trivial root is specified, we will prefix the result later. Hence, it is not
                 * necessary to modify each config directories here. but needs to normalize the root directory. */
                r = path_make_absolute_cwd(मूळ, &मूळ_abs);
                यदि (r < 0)
                        प्रत्यागम log_full_errno(log_level, r, "Failed to make '%s' absolute: %m", मूळ);

                path_simplify(मूळ_abs);

                rfd = open(मूळ, O_CLOEXEC|O_DIRECTORY|O_PATH);
                यदि (rfd < 0)
                        प्रत्यागम log_full_errno(log_level, errno, "Failed to open '%s': %m", मूळ_abs);

        } अन्यथा यदि (ret_dirs) {
                /* When an empty root or "/" is specified, we will open "/" below, hence we need to make
                 * each config directory absolute if relative. */
                r = path_strv_make_absolute_cwd(dirs_abs);
                यदि (r < 0)
                        प्रत्यागम log_full_errno(log_level, r, "Failed to make directories absolute: %m");
        }

        *ret_मूळ = TAKE_PTR(मूळ_abs);
        *ret_rfd = TAKE_FD(rfd);
        यदि (ret_dirs)
                *ret_dirs = TAKE_PTR(dirs_abs);
        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क conf_file_prefix_मूळ(ConfFile *c, ध्रुव वर्ण *मूळ, ConfFilesFlags flags) {
        वर्ण *p;
        पूर्णाङ्क r;

        प्रतिज्ञा(c);

        यदि (FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_मूळ))
                प्रत्यागम 0;

        पूर्णाङ्क log_level = conf_files_log_level(flags);

        r = chaseat_prefix_मूळ(c->result, मूळ, &p);
        यदि (r < 0)
                प्रत्यागम log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->result, मूळ);
        free_and_replace(c->result, p);

        r = chaseat_prefix_मूळ(c->resolved_path, मूळ, &p);
        यदि (r < 0)
                प्रत्यागम log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->resolved_path, मूळ);
        free_and_replace(c->resolved_path, p);

        /* Do not use chaseat_prefix_root(), as it is for the result of chaseat(), but the path is not chased. */
        p = path_join(empty_to_मूळ(मूळ), skip_leading_slash(c->original_path));
        यदि (!p)
                प्रत्यागम log_oom_full(log_level);

        path_simplify(p);
        प्रत्यागम free_and_replace(c->original_path, p);
}

स्थिर bool conf_files_need_stat(ConfFilesFlags flags) {
        प्रत्यागम (flags & (CONF_FILES_FILTER_MASKED | CONF_FILES_REGULAR | CONF_FILES_DIRECTORY | CONF_FILES_EXECUTABLE)) != 0;
}

स्थिर ChaseFlags conf_files_chase_flags(ConfFilesFlags flags) {
        ChaseFlags chase_flags = 0;

        यदि (!conf_files_need_stat(flags) || FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK))
                /* Even if no verification is requested, let's unconditionally call chaseat(),
                 * to drop unsafe symlinks. */
                chase_flags |= CHASE_NONEXISTENT;

        प्रत्यागम chase_flags;
}

स्थिर पूर्णाङ्क conf_file_chase_and_verify(
                ध्रुव वर्ण *मूळ,          /* for logging, can be NULL */
                पूर्णाङ्क rfd,
                ध्रुव वर्ण *original_path, /* for logging */
                ध्रुव वर्ण *path,
                ध्रुव वर्ण *name,
                Set **masked,              /* optional */
                ConfFilesFlags flags,
                वर्ण **ret_path,
                पूर्णाङ्क *ret_fd,
                संरचना stat *ret_stat) {

        _cleanup_free_ वर्ण *resolved_path = रिक्त;
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        संरचना stat st = {};
        पूर्णाङ्क r;

        प्रतिज्ञा(wildcard_fd_is_valid(rfd));
        प्रतिज्ञा(original_path);
        प्रतिज्ञा(path);
        प्रतिज्ञा(name);

        पूर्णाङ्क log_level = conf_files_log_level(flags);

        मूळ = empty_to_मूळ(मूळ);

        r = chaseat(rfd, rfd, path, conf_files_chase_flags(flags), &resolved_path, &fd);
        यदि (r < 0)
                प्रत्यागम log_full_errno(log_level, r, "Failed to chase '%s%s': %m",
                                      मूळ, skip_leading_slash(original_path));
        यदि (r == 0) {
                यदि (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK)) {
                        /* If the path points to /dev/null in a image or so, then the device node may not exist. */
                        यदि (path_equal(skip_leading_slash(resolved_path), "dev/null")) {
                                यदि (masked) {
                                        /* Mark this one as masked */
                                        r = set_put_strdup(masked, name);
                                        यदि (r < 0)
                                                प्रत्यागम log_oom_full(log_level);
                                }

                                प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                                       मूळ, skip_leading_slash(original_path));
                        }
                }

                यदि (conf_files_need_stat(flags))
                        /* If we need to have stat, skip the entry. */
                        प्रत्यागम log_full_errno(log_level, SYNTHETIC_ERRNO(ENOENT), "Failed to chase '%s%s': %m",
                                              मूळ, skip_leading_slash(original_path));
        }

        /* Even if we do not need stat, let's take stat now. The caller may use the info later. */
        यदि (fd >= 0 && fstat(fd, &st) < 0)
                प्रत्यागम log_full_errno(log_level, errno, "Failed to stat '%s%s': %m",
                                      मूळ, skip_leading_slash(original_path));

        /* Is this a masking entry? */
        यदि (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK) && stat_may_be_dev_null(&st)) {
                यदि (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        यदि (r < 0)
                                प्रत्यागम log_oom_full(log_level);
                }

                प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                       मूळ, skip_leading_slash(original_path));
        }

        यदि (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_EMPTY) && stat_is_empty(&st)) {
                यदि (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        यदि (r < 0)
                                प्रत्यागम log_oom_full(log_level);
                }

                प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (an empty file).",
                                       मूळ, skip_leading_slash(original_path));
        }

        यदि (FLAGS_SET(flags, CONF_FILES_REGULAR|CONF_FILES_DIRECTORY)) {
                यदि (!S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode))
                        प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(EBADFD),
                                               "File '%s%s' is neither a regular file or directory.",
                                               मूळ, skip_leading_slash(original_path));
        } अन्यथा {
                /* Is this node a regular file? */
                यदि (FLAGS_SET(flags, CONF_FILES_REGULAR)) {
                        r = stat_verify_regular(&st);
                        यदि (r < 0)
                                प्रत्यागम log_debug_errno(r, "File '%s%s' is not a regular file: %m",
                                                       मूळ, skip_leading_slash(original_path));
                }

                /* Is this node a directory? */
                यदि (FLAGS_SET(flags, CONF_FILES_DIRECTORY)) {
                        r = stat_verify_directory(&st);
                        यदि (r < 0)
                                प्रत्यागम log_debug_errno(r, "File '%s%s' is not a directory: %m",
                                                       मूळ, skip_leading_slash(original_path));
                }
        }

        /* Does this node have the executable bit set?
         * As requested: check if the file is marked executable. Note that we don't check access(X_OK) here,
         * as we care about whether the file is marked executable at all, and not whether it is executable
         * for us, because if so, such errors are stuff we should log about. */
        यदि (FLAGS_SET(flags, CONF_FILES_EXECUTABLE) && (st.st_mode & 0111) == 0)
                प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(ENOEXEC),
                                       "File '%s%s' is not marked executable.",
                                       मूळ, skip_leading_slash(original_path));

        यदि (ret_path)
                *ret_path = TAKE_PTR(resolved_path);
        यदि (ret_fd)
                *ret_fd = TAKE_FD(fd);
        यदि (ret_stat)
                *ret_stat = st;

        प्रत्यागम 0;
}

पूर्णाङ्क conf_file_new_at(
                ध्रुव वर्ण *path,
                ध्रुव वर्ण *मूळ,
                पूर्णाङ्क rfd,
                ConfFilesFlags flags,
                ConfFile **ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(wildcard_fd_is_valid(rfd));
        प्रतिज्ञा(ret);

        पूर्णाङ्क log_level = conf_files_log_level(flags);

        _cleanup_free_ वर्ण *मूळ_2 = रिक्त;
        यदि (DEBUG_LOGGING && !मूळ) {
                (शून्य) fd_get_path(rfd, &मूळ_2);
                मूळ = मूळ_2;
        }

        _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
        यदि (!c)
                प्रत्यागम log_oom_full(log_level);

        *c = (ConfFile) {
                .original_path = strdup(path),
                .fd = -EBADF,
        };

        यदि (!c->original_path)
                प्रत्यागम log_oom_full(log_level);

        r = path_extract_filename(path, &c->filename);
        यदि (r < 0)
                प्रत्यागम log_full_errno(log_level, r, "Failed to extract filename from '%s': %m", path);

        _cleanup_free_ वर्ण *dirpath = रिक्त, *resolved_dirpath = रिक्त;
        r = path_extract_directory(path, &dirpath);
        यदि (r < 0 && r != -EDESTADDRREQ)
                प्रत्यागम log_full_errno(log_level, r, "Failed to extract directory from '%s': %m", path);
        यदि (r >= 0) {
                r = chaseat(rfd, rfd, dirpath,
                            CHASE_MUST_BE_DIRECTORY | conf_files_chase_flags(flags),
                            &resolved_dirpath, /* ret_fd= */ रिक्त);
                यदि (r < 0)
                        प्रत्यागम log_full_errno(log_level, r, "Failed to chase '%s%s': %m", empty_to_मूळ(मूळ), skip_leading_slash(dirpath));
        }

        c->result = path_join(resolved_dirpath, c->filename);
        यदि (!c->result)
                प्रत्यागम log_oom_full(log_level);

        r = conf_file_chase_and_verify(
                        मूळ,
                        rfd,
                        c->original_path,
                        c->result,
                        c->filename,
                        /* masked= */ रिक्त,
                        flags,
                        &c->resolved_path,
                        &c->fd,
                        &c->st);
        यदि (r < 0)
                प्रत्यागम r;

        *ret = TAKE_PTR(c);
        प्रत्यागम 0;
}

पूर्णाङ्क conf_file_new(ध्रुव वर्ण *path, ध्रुव वर्ण *मूळ, ConfFilesFlags flags, ConfFile **ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(ret);

        _cleanup_free_ वर्ण *मूळ_abs = रिक्त;
        _cleanup_close_ पूर्णाङ्क rfd = -EBADF;
        r = prepare_dirs(मूळ, flags, /* dirs= */ रिक्त, &मूळ_abs, &rfd, /* ret_dirs= */ रिक्त);
        यदि (r < 0)
                प्रत्यागम r;

        _cleanup_free_ वर्ण *path_abs = रिक्त;
        यदि (!मूळ_abs) {
                r = path_make_absolute_cwd(path, &path_abs);
                यदि (r < 0)
                        प्रत्यागम log_full_errno(conf_files_log_level(flags), r, "Failed to make '%s' absolute: %m", path);

                path = path_abs;
        }

        _cleanup_(conf_file_freep) ConfFile *c = रिक्त;
        r = conf_file_new_at(path, मूळ_abs, rfd, flags, &c);
        यदि (r < 0)
                प्रत्यागम r;

        r = conf_file_prefix_मूळ(c, मूळ_abs, flags);
        यदि (r < 0)
                प्रत्यागम r;

        *ret = TAKE_PTR(c);
        प्रत्यागम 0;
}

DEFINE_PRIVATE_HASH_OPS_WITH_मूल्य_DESTRUCTOR(
                conf_file_hash_ops,
                वर्ण, string_hash_func, string_compare_func,
                ConfFile, conf_file_free);

स्थिर पूर्णाङ्क files_add(
                DIR *dir,
                ध्रुव वर्ण *original_dirpath,
                ध्रुव वर्ण *resolved_dirpath,
                ध्रुव वर्ण *मूळ, /* for logging, can be NULL */
                पूर्णाङ्क rfd,
                Hashmap **files,
                Set **masked,
                ध्रुव वर्ण *suffix,
                ConfFilesFlags flags) {

        पूर्णाङ्क r;

        प्रतिज्ञा(dir);
        प्रतिज्ञा(original_dirpath);
        प्रतिज्ञा(resolved_dirpath);
        प्रतिज्ञा(wildcard_fd_is_valid(rfd));
        प्रतिज्ञा(files);
        प्रतिज्ञा(masked);

        पूर्णाङ्क log_level = conf_files_log_level(flags);

        मूळ = empty_to_मूळ(मूळ);

        FOREACH_DIRENT(de, dir, प्रत्यागम log_full_errno(log_level, errno, "Failed to read directory '%s%s': %m",
                                                      मूळ, skip_leading_slash(original_dirpath))) {

                _cleanup_free_ वर्ण *original_path = path_join(original_dirpath, de->d_name);
                यदि (!original_path)
                        प्रत्यागम log_oom_full(log_level);

                /* Does this match the suffix? */
                यदि (suffix && !endswith(de->d_name, suffix)) {
                        log_debug("Skipping file '%s%s', suffix is not '%s'.", मूळ, skip_leading_slash(original_path), suffix);
                        अनुवर्त;
                }

                /* Has this file already been found in an earlier directory? */
                यदि (hashmap_contains(*files, de->d_name)) {
                        log_debug("Skipping overridden file '%s%s'.", मूळ, skip_leading_slash(original_path));
                        अनुवर्त;
                }

                /* Has this been masked in an earlier directory? */
                यदि ((flags & CONF_FILES_FILTER_MASKED) != 0 && set_contains(*masked, de->d_name)) {
                        log_debug("File '%s%s' is masked by previous entry.", मूळ, skip_leading_slash(original_path));
                        अनुवर्त;
                }

                _cleanup_free_ वर्ण *p = path_join(resolved_dirpath, de->d_name);
                यदि (!p)
                        प्रत्यागम log_oom_full(log_level);

                _cleanup_free_ वर्ण *resolved_path = रिक्त;
                _cleanup_close_ पूर्णाङ्क fd = -EBADF;
                संरचना stat st;
                r = conf_file_chase_and_verify(
                                मूळ,
                                rfd,
                                original_path,
                                p,
                                de->d_name,
                                masked,
                                flags,
                                &resolved_path,
                                &fd,
                                &st);
                यदि (r == -ENOMEM)
                        प्रत्यागम r;
                यदि (r < 0)
                        अनुवर्त;

                _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
                यदि (!c)
                        प्रत्यागम log_oom_full(log_level);

                *c = (ConfFile) {
                        .filename = strdup(de->d_name),
                        .result = TAKE_PTR(p),
                        .original_path = TAKE_PTR(original_path),
                        .resolved_path = TAKE_PTR(resolved_path),
                        .fd = TAKE_FD(fd),
                        .st = st,
                };

                यदि (!c->filename)
                        प्रत्यागम log_oom_full(log_level);

                r = hashmap_ensure_put(files, &conf_file_hash_ops, c->filename, c);
                यदि (r < 0) {
                        प्रतिज्ञा(r == -ENOMEM);
                        प्रत्यागम log_oom_full(log_level);
                }
                प्रतिज्ञा(r > 0);

                TAKE_PTR(c);
        }

        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क dump_files(Hashmap *fh, ध्रुव वर्ण *मूळ, ConfFilesFlags flags, ConfFile ***ret_files, size_t *ret_n_files) {
        ConfFile **files = रिक्त;
        size_t n_files = 0;
        पूर्णाङ्क r;

        CLEANUP_ARRAY(files, n_files, conf_file_free_array);

        प्रतिज्ञा(ret_files);
        प्रतिज्ञा(ret_n_files);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap. */
        r = hashmap_dump_sorted(fh, (शून्य***) &files, &n_files);
        यदि (r < 0)
                प्रत्यागम log_oom_full(conf_files_log_level(flags));

        /* Hence, we need to remove them from the hashmap. */
        FOREACH_ARRAY(i, files, n_files)
                assert_se(hashmap_remove(fh, (*i)->filename) == *i);

        यदि (मूळ)
                FOREACH_ARRAY(i, files, n_files) {
                        r = conf_file_prefix_मूळ(*i, मूळ, flags);
                        यदि (r < 0)
                                प्रत्यागम r;
                }

        *ret_files = TAKE_PTR(files);
        *ret_n_files = n_files;
        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क copy_and_sort_files_from_hashmap(
                Hashmap *fh,
                ध्रुव वर्ण *suffix,
                ध्रुव वर्ण *मूळ,
                ConfFilesFlags flags,
                वर्ण ***ret) {

        _cleanup_strv_free_ वर्ण **results = रिक्त;
        _cleanup_free_ ConfFile **files = रिक्त;
        size_t n_files = 0, n_results = 0;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        पूर्णाङ्क log_level = conf_files_log_level(flags);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap.
         * Hence, do not use conf_file_free_array() for 'entries' */
        r = hashmap_dump_sorted(fh, (शून्य***) &files, &n_files);
        यदि (r < 0)
                प्रत्यागम log_oom_full(log_level);

        FOREACH_ARRAY(i, files, n_files) {
                ConfFile *c = *i;
                ध्रुव वर्ण *add = रिक्त;

                यदि (FLAGS_SET(flags, CONF_FILES_BASENAME))
                        add = c->filename;
                अन्यथा यदि (मूळ && !FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_मूळ)) {
                        _cleanup_free_ वर्ण *p = रिक्त;

                        r = chaseat_prefix_मूळ(c->result, मूळ, &p);
                        यदि (r < 0)
                                प्रत्यागम log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->result, मूळ);

                        यदि (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX) && suffix) {
                                वर्ण *e = endswith(p, suffix);
                                यदि (!e)
                                        अनुवर्त;

                                *e = 0;
                        }

                        यदि (strv_consume_with_size(&results, &n_results, TAKE_PTR(p)) < 0)
                                प्रत्यागम log_oom_full(log_level);

                        अनुवर्त;
                } अन्यथा
                        add = c->result;

                यदि (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX)) {
                        ध्रुव वर्ण *e = endswith(add, suffix);
                        यदि (!e)
                                अनुवर्त;

                        _cleanup_free_ वर्ण *n = strndup(add, e - add);
                        यदि (!n)
                                प्रत्यागम log_oom_full(log_level);

                        r = strv_consume_with_size(&results, &n_results, TAKE_PTR(n));
                } अन्यथा
                        r = strv_extend_with_size(&results, &n_results, add);
                यदि (r < 0)
                        प्रत्यागम log_oom_full(log_level);
        }

        *ret = TAKE_PTR(results);
        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क घाला_replacement(Hashmap **fh, ConfFile *replacement, ConfFilesFlags flags, ध्रुव ConfFile **ret) {
        _cleanup_(conf_file_freep) ConfFile *c = ASSERT_PTR(replacement);
        पूर्णाङ्क r;

        प्रतिज्ञा(fh);
        प्रतिज्ञा(ret);

        /* This consumes the input ConfFile. */

        ConfFile *existing = hashmap_get(*fh, c->filename);
        यदि (existing) {
                log_debug("An entry with higher priority '%s' -> '%s' already exists, ignoring the replacement: %s",
                          existing->filename, existing->result, c->original_path);
                *ret = रिक्त;
                प्रत्यागम 0;
        }

        r = hashmap_ensure_put(fh, &conf_file_hash_ops, c->filename, c);
        यदि (r < 0) {
                प्रतिज्ञा(r == -ENOMEM);
                प्रत्यागम log_oom_full(conf_files_log_level(flags));
        }
        प्रतिज्ञा(r > 0);

        log_debug("Inserted replacement: '%s' -> '%s'", c->filename, c->result);

        *ret = TAKE_PTR(c);
        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क conf_files_list_impl(
                ध्रुव वर्ण *suffix,
                ध्रुव वर्ण *मूळ, /* for logging, can be NULL */
                पूर्णाङ्क rfd,
                ConfFilesFlags flags,
                ध्रुव वर्ण * ध्रुव *dirs,
                ध्रुव वर्ण *replacement,
                Hashmap **ret,
                ध्रुव ConfFile **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = रिक्त;
        _cleanup_set_free_ Set *masked = रिक्त;
        _cleanup_(conf_file_freep) ConfFile *c = रिक्त;
        ध्रुव ConfFile *inserted = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(wildcard_fd_is_valid(rfd));
        प्रतिज्ञा(ret);

        मूळ = empty_to_मूळ(मूळ);

        यदि (replacement) {
                r = conf_file_new_at(replacement, मूळ, rfd, flags & CONF_FILES_WARN, &c);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        STRV_FOREACH(p, dirs) {
                _cleanup_closedir_ DIR *dir = रिक्त;
                _cleanup_free_ वर्ण *path = रिक्त;

                r = chase_and_opendirat(rfd, rfd, *p, 0, &path, &dir);
                यदि (r < 0) {
                        यदि (r != -ENOENT)
                                log_full_errno(conf_files_log_level(flags), r,
                                               "Failed to chase and open directory '%s%s', ignoring: %m",
                                               मूळ, skip_leading_slash(*p));
                        अनुवर्त;
                }

                यदि (c && streq_ptr(path_startswith(c->result, path), c->filename)) {
                        r = घाला_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                        यदि (r < 0)
                                प्रत्यागम r;
                }

                r = files_add(dir, *p, path, मूळ, rfd, &fh, &masked, suffix, flags);
                यदि (r == -ENOMEM)
                        प्रत्यागम r;
        }

        यदि (c) {
                r = घाला_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        *ret = TAKE_PTR(fh);
        यदि (ret_inserted)
                *ret_inserted = inserted;
        प्रत्यागम 0;
}

पूर्णाङ्क conf_files_list_strv(
                वर्ण ***ret,
                ध्रुव वर्ण *suffix,
                ध्रुव वर्ण *मूळ,
                ConfFilesFlags flags,
                ध्रुव वर्ण * ध्रुव *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = रिक्त;
        _cleanup_close_ पूर्णाङ्क rfd = -EBADF;
        _cleanup_free_ वर्ण *मूळ_abs = रिक्त;
        _cleanup_strv_free_ वर्ण **dirs_abs = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = prepare_dirs(मूळ, flags, (वर्ण**) dirs, &मूळ_abs, &rfd, &dirs_abs);
        यदि (r < 0)
                प्रत्यागम r;

        r = conf_files_list_impl(suffix, मूळ_abs, rfd, flags, (ध्रुव वर्ण * ध्रुव *) dirs_abs,
                                 /* replacement= */ रिक्त, &fh, /* ret_inserted= */ रिक्त);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम copy_and_sort_files_from_hashmap(fh, suffix, empty_to_मूळ(मूळ_abs), flags, ret);
}

पूर्णाङ्क conf_files_list_strv_full(
                ध्रुव वर्ण *suffix,
                ध्रुव वर्ण *मूळ,
                ConfFilesFlags flags,
                ध्रुव वर्ण * ध्रुव *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = रिक्त;
        _cleanup_close_ पूर्णाङ्क rfd = -EBADF;
        _cleanup_free_ वर्ण *मूळ_abs = रिक्त;
        _cleanup_strv_free_ वर्ण **dirs_abs = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret_files);
        प्रतिज्ञा(ret_n_files);

        r = prepare_dirs(मूळ, flags, (वर्ण**) dirs, &मूळ_abs, &rfd, &dirs_abs);
        यदि (r < 0)
                प्रत्यागम r;

        r = conf_files_list_impl(suffix, मूळ_abs, rfd, flags, (ध्रुव वर्ण * ध्रुव *) dirs_abs,
                                 /* replacement= */ रिक्त, &fh, /* ret_inserted= */ रिक्त);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम dump_files(fh, empty_to_मूळ(मूळ_abs), flags, ret_files, ret_n_files);
}

पूर्णाङ्क conf_files_list_strv_at(
                वर्ण ***ret,
                ध्रुव वर्ण *suffix,
                पूर्णाङ्क rfd,
                ConfFilesFlags flags,
                ध्रुव वर्ण * ध्रुव *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = रिक्त;
        _cleanup_free_ वर्ण *मूळ = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(wildcard_fd_is_valid(rfd));
        प्रतिज्ञा(ret);

        यदि (DEBUG_LOGGING)
                (शून्य) fd_get_path(rfd, &मूळ); /* for logging */

        r = conf_files_list_impl(suffix, मूळ, rfd, flags, dirs, /* replacement= */ रिक्त, &fh, /* ret_inserted= */ रिक्त);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम copy_and_sort_files_from_hashmap(fh, suffix, /* root= */ रिक्त, flags, ret);
}

पूर्णाङ्क conf_files_list_strv_at_full(
                ध्रुव वर्ण *suffix,
                पूर्णाङ्क rfd,
                ConfFilesFlags flags,
                ध्रुव वर्ण * ध्रुव *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = रिक्त;
        _cleanup_free_ वर्ण *मूळ = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(wildcard_fd_is_valid(rfd));
        प्रतिज्ञा(ret_files);
        प्रतिज्ञा(ret_n_files);

        यदि (DEBUG_LOGGING)
                (शून्य) fd_get_path(rfd, &मूळ); /* for logging */

        r = conf_files_list_impl(suffix, मूळ, rfd, flags, dirs, /* replacement= */ रिक्त, &fh, /* ret_inserted= */ रिक्त);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम dump_files(fh, /* root= */ रिक्त, flags, ret_files, ret_n_files);
}

पूर्णाङ्क conf_files_list(वर्ण ***ret, ध्रुव वर्ण *suffix, ध्रुव वर्ण *मूळ, ConfFilesFlags flags, ध्रुव वर्ण *dir) {
        प्रत्यागम conf_files_list_strv(ret, suffix, मूळ, flags, STRV_MAKE_CONST(dir));
}

पूर्णाङ्क conf_files_list_full(ध्रुव वर्ण *suffix, ध्रुव वर्ण *मूळ, ConfFilesFlags flags, ध्रुव वर्ण *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        प्रत्यागम conf_files_list_strv_full(suffix, मूळ, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

पूर्णाङ्क conf_files_list_at(वर्ण ***ret, ध्रुव वर्ण *suffix, पूर्णाङ्क rfd, ConfFilesFlags flags, ध्रुव वर्ण *dir) {
        प्रत्यागम conf_files_list_strv_at(ret, suffix, rfd, flags, STRV_MAKE_CONST(dir));
}

पूर्णाङ्क conf_files_list_at_full(ध्रुव वर्ण *suffix, पूर्णाङ्क rfd, ConfFilesFlags flags, ध्रुव वर्ण *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        प्रत्यागम conf_files_list_strv_at_full(suffix, rfd, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

पूर्णाङ्क conf_files_list_nulstr(वर्ण ***ret, ध्रुव वर्ण *suffix, ध्रुव वर्ण *मूळ, ConfFilesFlags flags, ध्रुव वर्ण *dirs) {
        _cleanup_strv_free_ वर्ण **d = रिक्त;

        प्रतिज्ञा(ret);

        d = strv_split_nulstr(dirs);
        यदि (!d)
                प्रत्यागम log_oom_full(conf_files_log_level(flags));

        प्रत्यागम conf_files_list_strv(ret, suffix, मूळ, flags, (ध्रुव वर्ण**) d);
}

पूर्णाङ्क conf_files_list_nulstr_full(ध्रुव वर्ण *suffix, ध्रुव वर्ण *मूळ, ConfFilesFlags flags, ध्रुव वर्ण *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ वर्ण **d = रिक्त;

        प्रतिज्ञा(ret_files);
        प्रतिज्ञा(ret_n_files);

        d = strv_split_nulstr(dirs);
        यदि (!d)
                प्रत्यागम log_oom_full(conf_files_log_level(flags));

        प्रत्यागम conf_files_list_strv_full(suffix, मूळ, flags, (ध्रुव वर्ण**) d, ret_files, ret_n_files);
}

पूर्णाङ्क conf_files_list_nulstr_at(वर्ण ***ret, ध्रुव वर्ण *suffix, पूर्णाङ्क rfd, ConfFilesFlags flags, ध्रुव वर्ण *dirs) {
        _cleanup_strv_free_ वर्ण **d = रिक्त;

        प्रतिज्ञा(ret);

        d = strv_split_nulstr(dirs);
        यदि (!d)
                प्रत्यागम log_oom_full(conf_files_log_level(flags));

        प्रत्यागम conf_files_list_strv_at(ret, suffix, rfd, flags, (ध्रुव वर्ण**) d);
}

पूर्णाङ्क conf_files_list_nulstr_at_full(ध्रुव वर्ण *suffix, पूर्णाङ्क rfd, ConfFilesFlags flags, ध्रुव वर्ण *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ वर्ण **d = रिक्त;

        प्रतिज्ञा(ret_files);
        प्रतिज्ञा(ret_n_files);

        d = strv_split_nulstr(dirs);
        यदि (!d)
                प्रत्यागम log_oom_full(conf_files_log_level(flags));

        प्रत्यागम conf_files_list_strv_at_full(suffix, rfd, flags, (ध्रुव वर्ण**) d, ret_files, ret_n_files);
}

पूर्णाङ्क conf_files_list_with_replacement(
                ध्रुव वर्ण *मूळ,
                वर्ण **config_dirs,
                ध्रुव वर्ण *replacement,
                वर्ण ***ret_files,
                वर्ण **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = रिक्त;
        _cleanup_free_ वर्ण *inserted = रिक्त;
        ConfFilesFlags flags = CONF_FILES_REGULAR | CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_WARN;
        _cleanup_close_ पूर्णाङ्क rfd = -EBADF;
        _cleanup_free_ वर्ण *मूळ_abs = रिक्त;
        _cleanup_strv_free_ वर्ण **dirs_abs = रिक्त;
        ध्रुव ConfFile *c = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret_files);

        r = prepare_dirs(मूळ, flags, config_dirs, &मूळ_abs, &rfd, &dirs_abs);
        यदि (r < 0)
                प्रत्यागम r;

        r = conf_files_list_impl(".conf", मूळ_abs, rfd, flags, (ध्रुव वर्ण * ध्रुव *) dirs_abs,
                                 replacement, &fh, ret_inserted ? &c : रिक्त);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (c) {
                r = chaseat_prefix_मूळ(c->result, मूळ_abs, &inserted);
                यदि (r < 0)
                        प्रत्यागम log_full_errno(conf_files_log_level(flags), r,
                                              "Failed to prefix '%s' with root '%s': %m",
                                              c->result, empty_to_मूळ(मूळ_abs));
        }

        r = copy_and_sort_files_from_hashmap(fh, ".conf", empty_to_मूळ(मूळ_abs), flags, ret_files);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (ret_inserted)
                *ret_inserted = TAKE_PTR(inserted);
        प्रत्यागम 0;
}

पूर्णाङ्क conf_files_list_dropins(
                वर्ण ***ret,
                ध्रुव वर्ण *dropin_dirname,
                ध्रुव वर्ण *मूळ,
                पूर्णाङ्क मूळ_fd,
                ConfFilesFlags flags,
                ध्रुव वर्ण * ध्रुव *dirs) {

        _cleanup_strv_free_ वर्ण **dropin_dirs = रिक्त;
        ध्रुव वर्ण *suffix;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);
        प्रतिज्ञा(dropin_dirname);
        प्रतिज्ञा(dirs);

        suffix = strjoina("/", dropin_dirname);
        r = strv_extend_strv_concat(&dropin_dirs, dirs, suffix);
        यदि (r < 0)
                प्रत्यागम log_oom_full(conf_files_log_level(flags));

        प्रत्यागम conf_files_list_strv_at(ret, ".conf", मूळ_fd, flags, (ध्रुव वर्ण* ध्रुव*) dropin_dirs);
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
पूर्णाङ्क conf_file_read(
                ध्रुव वर्ण *मूळ,
                ध्रुव वर्ण **config_dirs,
                ध्रुव वर्ण *fn,
                parse_ओळ_t parse_ओळ,
                शून्य *userdata,
                bool ignore_enoent,
                bool *invalid_config) {

        _cleanup_fclose_ FILE *_f = रिक्त;
        _cleanup_free_ वर्ण *_fn = रिक्त;
        unsigned v = 0;
        FILE *f;
        पूर्णाङ्क r = 0;

        प्रतिज्ञा(fn);

        यदि (streq(fn, "-")) {
                f = stdin;
                fn = "<stdin>";

                log_debug("Reading config from stdin%s", glyph(GLYPH_ELLIPSIS));

        } अन्यथा यदि (is_path(fn)) {
                r = path_make_absolute_cwd(fn, &_fn);
                यदि (r < 0)
                        प्रत्यागम log_error_errno(r, "Failed to make path absolute: %m");
                fn = _fn;

                f = _f = fopen(fn, "re");
                यदि (!_f)
                        r = -errno;
                अन्यथा
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));

        } अन्यथा {
                r = शोध_and_fopen(fn, "re", मूळ, config_dirs, &_f, &_fn);
                यदि (r >= 0) {
                        f = _f;
                        fn = _fn;
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));
                }
        }

        यदि (r == -ENOENT && ignore_enoent) {
                log_debug_errno(r, "Failed to open \"%s\", ignoring: %m", fn);
                प्रत्यागम 0; /* No error, but nothing happened. */
        }
        यदि (r < 0)
                प्रत्यागम log_error_errno(r, "Failed to read '%s': %m", fn);

        r = 1;  /* We entered the part where we may modify state. */

        प्रति (;;) {
                _cleanup_free_ वर्ण *ओळ = रिक्त;
                bool invalid_ओळ = false;
                पूर्णाङ्क k;

                k = read_stripped_ओळ(f, LONG_ओळ_MAX, &ओळ);
                यदि (k < 0)
                        प्रत्यागम log_error_errno(k, "Failed to read '%s': %m", fn);
                यदि (k == 0)
                        भङ्ग;

                v++;

                यदि (IN_SET(ओळ[0], 0, '#'))
                        अनुवर्त;

                k = parse_ओळ(fn, v, ओळ, invalid_config ? &invalid_ओळ : रिक्त, userdata);
                यदि (k < 0 && invalid_ओळ)
                        /* Allow reporting with a special code if the caller requested this. */
                        *invalid_config = true;
                अन्यथा
                        /* The first error, if any, becomes our return value. */
                        RET_GATHER(r, k);
        }

        यदि (ferror(f))
                RET_GATHER(r, log_error_errno(SYNTHETIC_ERRNO(EIO), "Failed to read from file %s.", fn));

        प्रत्यागम r;
}
