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
        जर (!c)
                परत रिक्त;

        मुक्त(c->filename);
        मुक्त(c->result);
        मुक्त(c->original_path);
        मुक्त(c->resolved_path);
        safe_close(c->fd);

        परत mfree(c);
}

DEFINE_POINTER_ARRAY_FREE_FUNC(ConfFile*, conf_file_free);

स्थिर पूर्णांक conf_files_log_level(ConfFilesFlags flags) {
        परत FLAGS_SET(flags, CONF_FILES_WARN) ? LOG_WARNING : LOG_DEBUG;
}

स्थिर पूर्णांक prepare_dirs(
                स्थिरांक अक्षर *मूळ,
                ConfFilesFlags flags,
                अक्षर * स्थिरांक *dirs,
                अक्षर **ret_मूळ,
                पूर्णांक *ret_rfd,
                अक्षर ***ret_dirs) {

        _cleanup_free_ अक्षर *मूळ_abs = रिक्त;
        _cleanup_strv_free_ अक्षर **dirs_abs = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(ret_मूळ);
        प्रतिज्ञा(ret_rfd);
        प्रतिज्ञा(ret_dirs || strv_isempty(dirs));

        पूर्णांक log_level = conf_files_log_level(flags);

        r = empty_or_मूळ_harder_to_null(&मूळ);
        जर (r < 0)
                परत log_full_errno(log_level, r, "Failed to determine if '%s' points to the root directory: %m", strempty(मूळ));

        जर (ret_dirs) {
                dirs_abs = strv_copy(dirs);
                जर (!dirs_abs)
                        परत log_oom_full(log_level);
        }

        _cleanup_close_ पूर्णांक rfd = XAT_FDROOT;
        जर (मूळ) {
                /* When a non-trivial root is specified, we will prefix the result later. Hence, it is not
                 * necessary to modify each config directories here. but needs to normalize the root directory. */
                r = path_make_absolute_cwd(मूळ, &मूळ_abs);
                जर (r < 0)
                        परत log_full_errno(log_level, r, "Failed to make '%s' absolute: %m", मूळ);

                path_simplify(मूळ_abs);

                rfd = खोलो(मूळ, O_CLOEXEC|O_DIRECTORY|O_PATH);
                जर (rfd < 0)
                        परत log_full_errno(log_level, त्रुटि_सं, "Failed to open '%s': %m", मूळ_abs);

        } अन्यथा जर (ret_dirs) {
                /* When an empty root or "/" is specified, we will open "/" below, hence we need to make
                 * each config directory absolute if relative. */
                r = path_strv_make_absolute_cwd(dirs_abs);
                जर (r < 0)
                        परत log_full_errno(log_level, r, "Failed to make directories absolute: %m");
        }

        *ret_मूळ = TAKE_PTR(मूळ_abs);
        *ret_rfd = TAKE_FD(rfd);
        जर (ret_dirs)
                *ret_dirs = TAKE_PTR(dirs_abs);
        परत 0;
}

स्थिर पूर्णांक conf_file_prefix_मूळ(ConfFile *c, स्थिरांक अक्षर *मूळ, ConfFilesFlags flags) {
        अक्षर *p;
        पूर्णांक r;

        प्रतिज्ञा(c);

        जर (FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_मूळ))
                परत 0;

        पूर्णांक log_level = conf_files_log_level(flags);

        r = chaseat_prefix_मूळ(c->result, मूळ, &p);
        जर (r < 0)
                परत log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->result, मूळ);
        free_and_replace(c->result, p);

        r = chaseat_prefix_मूळ(c->resolved_path, मूळ, &p);
        जर (r < 0)
                परत log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->resolved_path, मूळ);
        free_and_replace(c->resolved_path, p);

        /* Do not use chaseat_prefix_root(), as it is for the result of chaseat(), but the path is not chased. */
        p = path_join(empty_to_मूळ(मूळ), skip_leading_slash(c->original_path));
        जर (!p)
                परत log_oom_full(log_level);

        path_simplify(p);
        परत free_and_replace(c->original_path, p);
}

स्थिर bool conf_files_need_stat(ConfFilesFlags flags) {
        परत (flags & (CONF_FILES_FILTER_MASKED | CONF_FILES_REGULAR | CONF_FILES_DIRECTORY | CONF_FILES_EXECUTABLE)) != 0;
}

स्थिर ChaseFlags conf_files_chase_flags(ConfFilesFlags flags) {
        ChaseFlags chase_flags = 0;

        जर (!conf_files_need_stat(flags) || FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK))
                /* Even if no verification is requested, let's unconditionally call chaseat(),
                 * to drop unsafe symlinks. */
                chase_flags |= CHASE_NONEXISTENT;

        परत chase_flags;
}

स्थिर पूर्णांक conf_file_chase_and_verify(
                स्थिरांक अक्षर *मूळ,          /* for logging, can be NULL */
                पूर्णांक rfd,
                स्थिरांक अक्षर *original_path, /* for logging */
                स्थिरांक अक्षर *path,
                स्थिरांक अक्षर *name,
                Set **masked,              /* optional */
                ConfFilesFlags flags,
                अक्षर **ret_path,
                पूर्णांक *ret_fd,
                संरचना stat *ret_stat) {

        _cleanup_free_ अक्षर *resolved_path = रिक्त;
        _cleanup_close_ पूर्णांक fd = -EBADF;
        संरचना stat st = {};
        पूर्णांक r;

        प्रतिज्ञा(wildcard_fd_is_valid(rfd));
        प्रतिज्ञा(original_path);
        प्रतिज्ञा(path);
        प्रतिज्ञा(name);

        पूर्णांक log_level = conf_files_log_level(flags);

        मूळ = empty_to_मूळ(मूळ);

        r = chaseat(rfd, rfd, path, conf_files_chase_flags(flags), &resolved_path, &fd);
        जर (r < 0)
                परत log_full_errno(log_level, r, "Failed to chase '%s%s': %m",
                                      मूळ, skip_leading_slash(original_path));
        जर (r == 0) {
                जर (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK)) {
                        /* If the path points to /dev/null in a image or so, then the device node may not exist. */
                        जर (path_equal(skip_leading_slash(resolved_path), "dev/null")) {
                                जर (masked) {
                                        /* Mark this one as masked */
                                        r = set_put_strdup(masked, name);
                                        जर (r < 0)
                                                परत log_oom_full(log_level);
                                }

                                परत log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                                       मूळ, skip_leading_slash(original_path));
                        }
                }

                जर (conf_files_need_stat(flags))
                        /* If we need to have stat, skip the entry. */
                        परत log_full_errno(log_level, SYNTHETIC_ERRNO(ENOENT), "Failed to chase '%s%s': %m",
                                              मूळ, skip_leading_slash(original_path));
        }

        /* Even if we do not need stat, let's take stat now. The caller may use the info later. */
        जर (fd >= 0 && ख_स्थिति(fd, &st) < 0)
                परत log_full_errno(log_level, त्रुटि_सं, "Failed to stat '%s%s': %m",
                                      मूळ, skip_leading_slash(original_path));

        /* Is this a masking entry? */
        जर (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK) && stat_may_be_dev_null(&st)) {
                जर (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        जर (r < 0)
                                परत log_oom_full(log_level);
                }

                परत log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                       मूळ, skip_leading_slash(original_path));
        }

        जर (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_EMPTY) && stat_is_empty(&st)) {
                जर (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        जर (r < 0)
                                परत log_oom_full(log_level);
                }

                परत log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (an empty file).",
                                       मूळ, skip_leading_slash(original_path));
        }

        जर (FLAGS_SET(flags, CONF_FILES_REGULAR|CONF_FILES_DIRECTORY)) {
                जर (!S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode))
                        परत log_debug_errno(SYNTHETIC_ERRNO(EBADFD),
                                               "File '%s%s' is neither a regular file or directory.",
                                               मूळ, skip_leading_slash(original_path));
        } अन्यथा {
                /* Is this node a regular file? */
                जर (FLAGS_SET(flags, CONF_FILES_REGULAR)) {
                        r = stat_verify_regular(&st);
                        जर (r < 0)
                                परत log_debug_errno(r, "File '%s%s' is not a regular file: %m",
                                                       मूळ, skip_leading_slash(original_path));
                }

                /* Is this node a directory? */
                जर (FLAGS_SET(flags, CONF_FILES_DIRECTORY)) {
                        r = stat_verify_directory(&st);
                        जर (r < 0)
                                परत log_debug_errno(r, "File '%s%s' is not a directory: %m",
                                                       मूळ, skip_leading_slash(original_path));
                }
        }

        /* Does this node have the executable bit set?
         * As requested: check if the file is marked executable. Note that we don't check access(X_OK) here,
         * as we care about whether the file is marked executable at all, and not whether it is executable
         * for us, because if so, such errors are stuff we should log about. */
        जर (FLAGS_SET(flags, CONF_FILES_EXECUTABLE) && (st.st_mode & 0111) == 0)
                परत log_debug_errno(SYNTHETIC_ERRNO(ENOEXEC),
                                       "File '%s%s' is not marked executable.",
                                       मूळ, skip_leading_slash(original_path));

        जर (ret_path)
                *ret_path = TAKE_PTR(resolved_path);
        जर (ret_fd)
                *ret_fd = TAKE_FD(fd);
        जर (ret_stat)
                *ret_stat = st;

        परत 0;
}

पूर्णांक conf_file_new_at(
                स्थिरांक अक्षर *path,
                स्थिरांक अक्षर *मूळ,
                पूर्णांक rfd,
                ConfFilesFlags flags,
                ConfFile **ret) {
        पूर्णांक r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(wildcard_fd_is_valid(rfd));
        प्रतिज्ञा(ret);

        पूर्णांक log_level = conf_files_log_level(flags);

        _cleanup_free_ अक्षर *मूळ_2 = रिक्त;
        जर (DEBUG_LOGGING && !मूळ) {
                (शून्य) fd_get_path(rfd, &मूळ_2);
                मूळ = मूळ_2;
        }

        _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
        जर (!c)
                परत log_oom_full(log_level);

        *c = (ConfFile) {
                .original_path = strdup(path),
                .fd = -EBADF,
        };

        जर (!c->original_path)
                परत log_oom_full(log_level);

        r = path_extract_filename(path, &c->filename);
        जर (r < 0)
                परत log_full_errno(log_level, r, "Failed to extract filename from '%s': %m", path);

        _cleanup_free_ अक्षर *dirpath = रिक्त, *resolved_dirpath = रिक्त;
        r = path_extract_directory(path, &dirpath);
        जर (r < 0 && r != -EDESTADDRREQ)
                परत log_full_errno(log_level, r, "Failed to extract directory from '%s': %m", path);
        जर (r >= 0) {
                r = chaseat(rfd, rfd, dirpath,
                            CHASE_MUST_BE_DIRECTORY | conf_files_chase_flags(flags),
                            &resolved_dirpath, /* ret_fd= */ रिक्त);
                जर (r < 0)
                        परत log_full_errno(log_level, r, "Failed to chase '%s%s': %m", empty_to_मूळ(मूळ), skip_leading_slash(dirpath));
        }

        c->result = path_join(resolved_dirpath, c->filename);
        जर (!c->result)
                परत log_oom_full(log_level);

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
        जर (r < 0)
                परत r;

        *ret = TAKE_PTR(c);
        परत 0;
}

पूर्णांक conf_file_new(स्थिरांक अक्षर *path, स्थिरांक अक्षर *मूळ, ConfFilesFlags flags, ConfFile **ret) {
        पूर्णांक r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(ret);

        _cleanup_free_ अक्षर *मूळ_abs = रिक्त;
        _cleanup_close_ पूर्णांक rfd = -EBADF;
        r = prepare_dirs(मूळ, flags, /* dirs= */ रिक्त, &मूळ_abs, &rfd, /* ret_dirs= */ रिक्त);
        जर (r < 0)
                परत r;

        _cleanup_free_ अक्षर *path_abs = रिक्त;
        जर (!मूळ_abs) {
                r = path_make_absolute_cwd(path, &path_abs);
                जर (r < 0)
                        परत log_full_errno(conf_files_log_level(flags), r, "Failed to make '%s' absolute: %m", path);

                path = path_abs;
        }

        _cleanup_(conf_file_freep) ConfFile *c = रिक्त;
        r = conf_file_new_at(path, मूळ_abs, rfd, flags, &c);
        जर (r < 0)
                परत r;

        r = conf_file_prefix_मूळ(c, मूळ_abs, flags);
        जर (r < 0)
                परत r;

        *ret = TAKE_PTR(c);
        परत 0;
}

DEFINE_PRIVATE_HASH_OPS_WITH_मूल्य_DESTRUCTOR(
                conf_file_hash_ops,
                अक्षर, string_hash_func, string_compare_func,
                ConfFile, conf_file_free);

स्थिर पूर्णांक files_add(
                सूची *dir,
                स्थिरांक अक्षर *original_dirpath,
                स्थिरांक अक्षर *resolved_dirpath,
                स्थिरांक अक्षर *मूळ, /* for logging, can be NULL */
                पूर्णांक rfd,
                Hashmap **files,
                Set **masked,
                स्थिरांक अक्षर *suffix,
                ConfFilesFlags flags) {

        पूर्णांक r;

        प्रतिज्ञा(dir);
        प्रतिज्ञा(original_dirpath);
        प्रतिज्ञा(resolved_dirpath);
        प्रतिज्ञा(wildcard_fd_is_valid(rfd));
        प्रतिज्ञा(files);
        प्रतिज्ञा(masked);

        पूर्णांक log_level = conf_files_log_level(flags);

        मूळ = empty_to_मूळ(मूळ);

        FOREACH_DIRENT(de, dir, परत log_full_errno(log_level, त्रुटि_सं, "Failed to read directory '%s%s': %m",
                                                      मूळ, skip_leading_slash(original_dirpath))) {

                _cleanup_free_ अक्षर *original_path = path_join(original_dirpath, de->d_name);
                जर (!original_path)
                        परत log_oom_full(log_level);

                /* Does this match the suffix? */
                जर (suffix && !endswith(de->d_name, suffix)) {
                        log_debug("Skipping file '%s%s', suffix is not '%s'.", मूळ, skip_leading_slash(original_path), suffix);
                        सुरूठेवा;
                }

                /* Has this file already been found in an earlier directory? */
                जर (hashmap_contains(*files, de->d_name)) {
                        log_debug("Skipping overridden file '%s%s'.", मूळ, skip_leading_slash(original_path));
                        सुरूठेवा;
                }

                /* Has this been masked in an earlier directory? */
                जर ((flags & CONF_FILES_FILTER_MASKED) != 0 && set_contains(*masked, de->d_name)) {
                        log_debug("File '%s%s' is masked by previous entry.", मूळ, skip_leading_slash(original_path));
                        सुरूठेवा;
                }

                _cleanup_free_ अक्षर *p = path_join(resolved_dirpath, de->d_name);
                जर (!p)
                        परत log_oom_full(log_level);

                _cleanup_free_ अक्षर *resolved_path = रिक्त;
                _cleanup_close_ पूर्णांक fd = -EBADF;
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
                जर (r == -ENOMEM)
                        परत r;
                जर (r < 0)
                        सुरूठेवा;

                _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
                जर (!c)
                        परत log_oom_full(log_level);

                *c = (ConfFile) {
                        .filename = strdup(de->d_name),
                        .result = TAKE_PTR(p),
                        .original_path = TAKE_PTR(original_path),
                        .resolved_path = TAKE_PTR(resolved_path),
                        .fd = TAKE_FD(fd),
                        .st = st,
                };

                जर (!c->filename)
                        परत log_oom_full(log_level);

                r = hashmap_ensure_put(files, &conf_file_hash_ops, c->filename, c);
                जर (r < 0) {
                        प्रतिज्ञा(r == -ENOMEM);
                        परत log_oom_full(log_level);
                }
                प्रतिज्ञा(r > 0);

                TAKE_PTR(c);
        }

        परत 0;
}

स्थिर पूर्णांक dump_files(Hashmap *fh, स्थिरांक अक्षर *मूळ, ConfFilesFlags flags, ConfFile ***ret_files, माप_प्रकार *ret_n_files) {
        ConfFile **files = रिक्त;
        माप_प्रकार n_files = 0;
        पूर्णांक r;

        CLEANUP_ARRAY(files, n_files, conf_file_free_array);

        प्रतिज्ञा(ret_files);
        प्रतिज्ञा(ret_n_files);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap. */
        r = hashmap_dump_sorted(fh, (शून्य***) &files, &n_files);
        जर (r < 0)
                परत log_oom_full(conf_files_log_level(flags));

        /* Hence, we need to remove them from the hashmap. */
        FOREACH_ARRAY(i, files, n_files)
                assert_se(hashmap_remove(fh, (*i)->filename) == *i);

        जर (मूळ)
                FOREACH_ARRAY(i, files, n_files) {
                        r = conf_file_prefix_मूळ(*i, मूळ, flags);
                        जर (r < 0)
                                परत r;
                }

        *ret_files = TAKE_PTR(files);
        *ret_n_files = n_files;
        परत 0;
}

स्थिर पूर्णांक copy_and_sort_files_from_hashmap(
                Hashmap *fh,
                स्थिरांक अक्षर *suffix,
                स्थिरांक अक्षर *मूळ,
                ConfFilesFlags flags,
                अक्षर ***ret) {

        _cleanup_strv_free_ अक्षर **results = रिक्त;
        _cleanup_free_ ConfFile **files = रिक्त;
        माप_प्रकार n_files = 0, n_results = 0;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        पूर्णांक log_level = conf_files_log_level(flags);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap.
         * Hence, do not use conf_file_free_array() for 'entries' */
        r = hashmap_dump_sorted(fh, (शून्य***) &files, &n_files);
        जर (r < 0)
                परत log_oom_full(log_level);

        FOREACH_ARRAY(i, files, n_files) {
                ConfFile *c = *i;
                स्थिरांक अक्षर *add = रिक्त;

                जर (FLAGS_SET(flags, CONF_FILES_BASENAME))
                        add = c->filename;
                अन्यथा जर (मूळ && !FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_मूळ)) {
                        _cleanup_free_ अक्षर *p = रिक्त;

                        r = chaseat_prefix_मूळ(c->result, मूळ, &p);
                        जर (r < 0)
                                परत log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->result, मूळ);

                        जर (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX) && suffix) {
                                अक्षर *e = endswith(p, suffix);
                                जर (!e)
                                        सुरूठेवा;

                                *e = 0;
                        }

                        जर (strv_consume_with_size(&results, &n_results, TAKE_PTR(p)) < 0)
                                परत log_oom_full(log_level);

                        सुरूठेवा;
                } अन्यथा
                        add = c->result;

                जर (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX)) {
                        स्थिरांक अक्षर *e = endswith(add, suffix);
                        जर (!e)
                                सुरूठेवा;

                        _cleanup_free_ अक्षर *n = strndup(add, e - add);
                        जर (!n)
                                परत log_oom_full(log_level);

                        r = strv_consume_with_size(&results, &n_results, TAKE_PTR(n));
                } अन्यथा
                        r = strv_extend_with_size(&results, &n_results, add);
                जर (r < 0)
                        परत log_oom_full(log_level);
        }

        *ret = TAKE_PTR(results);
        परत 0;
}

स्थिर पूर्णांक घाला_replacement(Hashmap **fh, ConfFile *replacement, ConfFilesFlags flags, स्थिरांक ConfFile **ret) {
        _cleanup_(conf_file_freep) ConfFile *c = ASSERT_PTR(replacement);
        पूर्णांक r;

        प्रतिज्ञा(fh);
        प्रतिज्ञा(ret);

        /* This consumes the input ConfFile. */

        ConfFile *existing = hashmap_get(*fh, c->filename);
        जर (existing) {
                log_debug("An entry with higher priority '%s' -> '%s' already exists, ignoring the replacement: %s",
                          existing->filename, existing->result, c->original_path);
                *ret = रिक्त;
                परत 0;
        }

        r = hashmap_ensure_put(fh, &conf_file_hash_ops, c->filename, c);
        जर (r < 0) {
                प्रतिज्ञा(r == -ENOMEM);
                परत log_oom_full(conf_files_log_level(flags));
        }
        प्रतिज्ञा(r > 0);

        log_debug("Inserted replacement: '%s' -> '%s'", c->filename, c->result);

        *ret = TAKE_PTR(c);
        परत 0;
}

स्थिर पूर्णांक conf_files_list_impl(
                स्थिरांक अक्षर *suffix,
                स्थिरांक अक्षर *मूळ, /* for logging, can be NULL */
                पूर्णांक rfd,
                ConfFilesFlags flags,
                स्थिरांक अक्षर * स्थिरांक *dirs,
                स्थिरांक अक्षर *replacement,
                Hashmap **ret,
                स्थिरांक ConfFile **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = रिक्त;
        _cleanup_set_free_ Set *masked = रिक्त;
        _cleanup_(conf_file_freep) ConfFile *c = रिक्त;
        स्थिरांक ConfFile *inserted = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(wildcard_fd_is_valid(rfd));
        प्रतिज्ञा(ret);

        मूळ = empty_to_मूळ(मूळ);

        जर (replacement) {
                r = conf_file_new_at(replacement, मूळ, rfd, flags & CONF_FILES_WARN, &c);
                जर (r < 0)
                        परत r;
        }

        STRV_FOREACH(p, dirs) {
                _cleanup_closedir_ सूची *dir = रिक्त;
                _cleanup_free_ अक्षर *path = रिक्त;

                r = chase_and_opendirat(rfd, rfd, *p, 0, &path, &dir);
                जर (r < 0) {
                        जर (r != -ENOENT)
                                log_full_errno(conf_files_log_level(flags), r,
                                               "Failed to chase and open directory '%s%s', ignoring: %m",
                                               मूळ, skip_leading_slash(*p));
                        सुरूठेवा;
                }

                जर (c && streq_ptr(path_startswith(c->result, path), c->filename)) {
                        r = घाला_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                        जर (r < 0)
                                परत r;
                }

                r = files_add(dir, *p, path, मूळ, rfd, &fh, &masked, suffix, flags);
                जर (r == -ENOMEM)
                        परत r;
        }

        जर (c) {
                r = घाला_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                जर (r < 0)
                        परत r;
        }

        *ret = TAKE_PTR(fh);
        जर (ret_inserted)
                *ret_inserted = inserted;
        परत 0;
}

पूर्णांक conf_files_list_strv(
                अक्षर ***ret,
                स्थिरांक अक्षर *suffix,
                स्थिरांक अक्षर *मूळ,
                ConfFilesFlags flags,
                स्थिरांक अक्षर * स्थिरांक *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = रिक्त;
        _cleanup_close_ पूर्णांक rfd = -EBADF;
        _cleanup_free_ अक्षर *मूळ_abs = रिक्त;
        _cleanup_strv_free_ अक्षर **dirs_abs = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        r = prepare_dirs(मूळ, flags, (अक्षर**) dirs, &मूळ_abs, &rfd, &dirs_abs);
        जर (r < 0)
                परत r;

        r = conf_files_list_impl(suffix, मूळ_abs, rfd, flags, (स्थिरांक अक्षर * स्थिरांक *) dirs_abs,
                                 /* replacement= */ रिक्त, &fh, /* ret_inserted= */ रिक्त);
        जर (r < 0)
                परत r;

        परत copy_and_sort_files_from_hashmap(fh, suffix, empty_to_मूळ(मूळ_abs), flags, ret);
}

पूर्णांक conf_files_list_strv_full(
                स्थिरांक अक्षर *suffix,
                स्थिरांक अक्षर *मूळ,
                ConfFilesFlags flags,
                स्थिरांक अक्षर * स्थिरांक *dirs,
                ConfFile ***ret_files,
                माप_प्रकार *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = रिक्त;
        _cleanup_close_ पूर्णांक rfd = -EBADF;
        _cleanup_free_ अक्षर *मूळ_abs = रिक्त;
        _cleanup_strv_free_ अक्षर **dirs_abs = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(ret_files);
        प्रतिज्ञा(ret_n_files);

        r = prepare_dirs(मूळ, flags, (अक्षर**) dirs, &मूळ_abs, &rfd, &dirs_abs);
        जर (r < 0)
                परत r;

        r = conf_files_list_impl(suffix, मूळ_abs, rfd, flags, (स्थिरांक अक्षर * स्थिरांक *) dirs_abs,
                                 /* replacement= */ रिक्त, &fh, /* ret_inserted= */ रिक्त);
        जर (r < 0)
                परत r;

        परत dump_files(fh, empty_to_मूळ(मूळ_abs), flags, ret_files, ret_n_files);
}

पूर्णांक conf_files_list_strv_at(
                अक्षर ***ret,
                स्थिरांक अक्षर *suffix,
                पूर्णांक rfd,
                ConfFilesFlags flags,
                स्थिरांक अक्षर * स्थिरांक *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = रिक्त;
        _cleanup_free_ अक्षर *मूळ = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(wildcard_fd_is_valid(rfd));
        प्रतिज्ञा(ret);

        जर (DEBUG_LOGGING)
                (शून्य) fd_get_path(rfd, &मूळ); /* for logging */

        r = conf_files_list_impl(suffix, मूळ, rfd, flags, dirs, /* replacement= */ रिक्त, &fh, /* ret_inserted= */ रिक्त);
        जर (r < 0)
                परत r;

        परत copy_and_sort_files_from_hashmap(fh, suffix, /* root= */ रिक्त, flags, ret);
}

पूर्णांक conf_files_list_strv_at_full(
                स्थिरांक अक्षर *suffix,
                पूर्णांक rfd,
                ConfFilesFlags flags,
                स्थिरांक अक्षर * स्थिरांक *dirs,
                ConfFile ***ret_files,
                माप_प्रकार *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = रिक्त;
        _cleanup_free_ अक्षर *मूळ = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(wildcard_fd_is_valid(rfd));
        प्रतिज्ञा(ret_files);
        प्रतिज्ञा(ret_n_files);

        जर (DEBUG_LOGGING)
                (शून्य) fd_get_path(rfd, &मूळ); /* for logging */

        r = conf_files_list_impl(suffix, मूळ, rfd, flags, dirs, /* replacement= */ रिक्त, &fh, /* ret_inserted= */ रिक्त);
        जर (r < 0)
                परत r;

        परत dump_files(fh, /* root= */ रिक्त, flags, ret_files, ret_n_files);
}

पूर्णांक conf_files_list(अक्षर ***ret, स्थिरांक अक्षर *suffix, स्थिरांक अक्षर *मूळ, ConfFilesFlags flags, स्थिरांक अक्षर *dir) {
        परत conf_files_list_strv(ret, suffix, मूळ, flags, STRV_MAKE_CONST(dir));
}

पूर्णांक conf_files_list_full(स्थिरांक अक्षर *suffix, स्थिरांक अक्षर *मूळ, ConfFilesFlags flags, स्थिरांक अक्षर *dir, ConfFile ***ret_files, माप_प्रकार *ret_n_files) {
        परत conf_files_list_strv_full(suffix, मूळ, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

पूर्णांक conf_files_list_at(अक्षर ***ret, स्थिरांक अक्षर *suffix, पूर्णांक rfd, ConfFilesFlags flags, स्थिरांक अक्षर *dir) {
        परत conf_files_list_strv_at(ret, suffix, rfd, flags, STRV_MAKE_CONST(dir));
}

पूर्णांक conf_files_list_at_full(स्थिरांक अक्षर *suffix, पूर्णांक rfd, ConfFilesFlags flags, स्थिरांक अक्षर *dir, ConfFile ***ret_files, माप_प्रकार *ret_n_files) {
        परत conf_files_list_strv_at_full(suffix, rfd, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

पूर्णांक conf_files_list_nulstr(अक्षर ***ret, स्थिरांक अक्षर *suffix, स्थिरांक अक्षर *मूळ, ConfFilesFlags flags, स्थिरांक अक्षर *dirs) {
        _cleanup_strv_free_ अक्षर **d = रिक्त;

        प्रतिज्ञा(ret);

        d = strv_split_nulstr(dirs);
        जर (!d)
                परत log_oom_full(conf_files_log_level(flags));

        परत conf_files_list_strv(ret, suffix, मूळ, flags, (स्थिरांक अक्षर**) d);
}

पूर्णांक conf_files_list_nulstr_full(स्थिरांक अक्षर *suffix, स्थिरांक अक्षर *मूळ, ConfFilesFlags flags, स्थिरांक अक्षर *dirs, ConfFile ***ret_files, माप_प्रकार *ret_n_files) {
        _cleanup_strv_free_ अक्षर **d = रिक्त;

        प्रतिज्ञा(ret_files);
        प्रतिज्ञा(ret_n_files);

        d = strv_split_nulstr(dirs);
        जर (!d)
                परत log_oom_full(conf_files_log_level(flags));

        परत conf_files_list_strv_full(suffix, मूळ, flags, (स्थिरांक अक्षर**) d, ret_files, ret_n_files);
}

पूर्णांक conf_files_list_nulstr_at(अक्षर ***ret, स्थिरांक अक्षर *suffix, पूर्णांक rfd, ConfFilesFlags flags, स्थिरांक अक्षर *dirs) {
        _cleanup_strv_free_ अक्षर **d = रिक्त;

        प्रतिज्ञा(ret);

        d = strv_split_nulstr(dirs);
        जर (!d)
                परत log_oom_full(conf_files_log_level(flags));

        परत conf_files_list_strv_at(ret, suffix, rfd, flags, (स्थिरांक अक्षर**) d);
}

पूर्णांक conf_files_list_nulstr_at_full(स्थिरांक अक्षर *suffix, पूर्णांक rfd, ConfFilesFlags flags, स्थिरांक अक्षर *dirs, ConfFile ***ret_files, माप_प्रकार *ret_n_files) {
        _cleanup_strv_free_ अक्षर **d = रिक्त;

        प्रतिज्ञा(ret_files);
        प्रतिज्ञा(ret_n_files);

        d = strv_split_nulstr(dirs);
        जर (!d)
                परत log_oom_full(conf_files_log_level(flags));

        परत conf_files_list_strv_at_full(suffix, rfd, flags, (स्थिरांक अक्षर**) d, ret_files, ret_n_files);
}

पूर्णांक conf_files_list_with_replacement(
                स्थिरांक अक्षर *मूळ,
                अक्षर **config_dirs,
                स्थिरांक अक्षर *replacement,
                अक्षर ***ret_files,
                अक्षर **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = रिक्त;
        _cleanup_free_ अक्षर *inserted = रिक्त;
        ConfFilesFlags flags = CONF_FILES_REGULAR | CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_WARN;
        _cleanup_close_ पूर्णांक rfd = -EBADF;
        _cleanup_free_ अक्षर *मूळ_abs = रिक्त;
        _cleanup_strv_free_ अक्षर **dirs_abs = रिक्त;
        स्थिरांक ConfFile *c = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(ret_files);

        r = prepare_dirs(मूळ, flags, config_dirs, &मूळ_abs, &rfd, &dirs_abs);
        जर (r < 0)
                परत r;

        r = conf_files_list_impl(".conf", मूळ_abs, rfd, flags, (स्थिरांक अक्षर * स्थिरांक *) dirs_abs,
                                 replacement, &fh, ret_inserted ? &c : रिक्त);
        जर (r < 0)
                परत r;

        जर (c) {
                r = chaseat_prefix_मूळ(c->result, मूळ_abs, &inserted);
                जर (r < 0)
                        परत log_full_errno(conf_files_log_level(flags), r,
                                              "Failed to prefix '%s' with root '%s': %m",
                                              c->result, empty_to_मूळ(मूळ_abs));
        }

        r = copy_and_sort_files_from_hashmap(fh, ".conf", empty_to_मूळ(मूळ_abs), flags, ret_files);
        जर (r < 0)
                परत r;

        जर (ret_inserted)
                *ret_inserted = TAKE_PTR(inserted);
        परत 0;
}

पूर्णांक conf_files_list_dropins(
                अक्षर ***ret,
                स्थिरांक अक्षर *dropin_dirname,
                स्थिरांक अक्षर *मूळ,
                पूर्णांक मूळ_fd,
                ConfFilesFlags flags,
                स्थिरांक अक्षर * स्थिरांक *dirs) {

        _cleanup_strv_free_ अक्षर **dropin_dirs = रिक्त;
        स्थिरांक अक्षर *suffix;
        पूर्णांक r;

        प्रतिज्ञा(ret);
        प्रतिज्ञा(dropin_dirname);
        प्रतिज्ञा(dirs);

        suffix = strjoina("/", dropin_dirname);
        r = strv_extend_strv_concat(&dropin_dirs, dirs, suffix);
        जर (r < 0)
                परत log_oom_full(conf_files_log_level(flags));

        परत conf_files_list_strv_at(ret, ".conf", मूळ_fd, flags, (स्थिरांक अक्षर* स्थिरांक*) dropin_dirs);
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
पूर्णांक conf_file_read(
                स्थिरांक अक्षर *मूळ,
                स्थिरांक अक्षर **config_dirs,
                स्थिरांक अक्षर *fn,
                parse_ओळ_t parse_ओळ,
                शून्य *userdata,
                bool ignore_enoent,
                bool *invalid_config) {

        _cleanup_fclose_ खाता *_f = रिक्त;
        _cleanup_free_ अक्षर *_fn = रिक्त;
        अचिन्हित v = 0;
        खाता *f;
        पूर्णांक r = 0;

        प्रतिज्ञा(fn);

        जर (streq(fn, "-")) {
                f = मानक_निवेश;
                fn = "<stdin>";

                log_debug("Reading config from stdin%s", glyph(GLYPH_ELLIPSIS));

        } अन्यथा जर (is_path(fn)) {
                r = path_make_absolute_cwd(fn, &_fn);
                जर (r < 0)
                        परत log_error_errno(r, "Failed to make path absolute: %m");
                fn = _fn;

                f = _f = ख_खोलो(fn, "re");
                जर (!_f)
                        r = -त्रुटि_सं;
                अन्यथा
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));

        } अन्यथा {
                r = शोध_and_fopen(fn, "re", मूळ, config_dirs, &_f, &_fn);
                जर (r >= 0) {
                        f = _f;
                        fn = _fn;
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));
                }
        }

        जर (r == -ENOENT && ignore_enoent) {
                log_debug_errno(r, "Failed to open \"%s\", ignoring: %m", fn);
                परत 0; /* No error, but nothing happened. */
        }
        जर (r < 0)
                परत log_error_errno(r, "Failed to read '%s': %m", fn);

        r = 1;  /* We entered the part where we may modify state. */

        साठी (;;) {
                _cleanup_free_ अक्षर *ओळ = रिक्त;
                bool invalid_ओळ = false;
                पूर्णांक k;

                k = read_stripped_ओळ(f, LONG_ओळ_MAX, &ओळ);
                जर (k < 0)
                        परत log_error_errno(k, "Failed to read '%s': %m", fn);
                जर (k == 0)
                        थांबा;

                v++;

                जर (IN_SET(ओळ[0], 0, '#'))
                        सुरूठेवा;

                k = parse_ओळ(fn, v, ओळ, invalid_config ? &invalid_ओळ : रिक्त, userdata);
                जर (k < 0 && invalid_ओळ)
                        /* Allow reporting with a special code if the caller requested this. */
                        *invalid_config = true;
                अन्यथा
                        /* The first error, if any, becomes our return value. */
                        RET_GATHER(r, k);
        }

        जर (ख_त्रुटि(f))
                RET_GATHER(r, log_error_errno(SYNTHETIC_ERRNO(EIO), "Failed to read from file %s.", fn));

        परत r;
}
