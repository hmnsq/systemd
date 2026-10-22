/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <stdio.h>
#समावेश <stdlib.h>

#समावेश "alloc-util.h"
#समावेश "chase.h"
#समावेश "conf-files.h"
#समावेश "dirent-util.h"
#समावेश "errno-util.h"
#समावेश "fd-util.h"
#समावेश "fileio.h"
#समावेश "glyph-util.h"
#समावेश "hashmap.h"
#समावेश "log.h"
#समावेश "nulstr-util.h"
#समावेश "path-util.h"
#समावेश "set.h"
#समावेश "stat-util.h"
#समावेश "string-util.h"
#समावेश "strv.h"

ConfFile* conf_file_free(ConfFile *c) {
        अगर (!c)
                वापस शून्य;

        मुक्त(c->filename);
        मुक्त(c->परिणाम);
        मुक्त(c->original_path);
        मुक्त(c->resolved_path);
        safe_close(c->fd);

        वापस mfree(c);
}

DEFINE_POINTER_ARRAY_FREE_FUNC(ConfFile*, conf_file_free);

अटल पूर्णांक conf_files_log_level(ConfFilesFlags flags) {
        वापस FLAGS_SET(flags, CONF_FILES_WARN) ? LOG_WARNING : LOG_DEBUG;
}

अटल पूर्णांक prepare_dirs(
                स्थिर अक्षर *मूल,
                ConfFilesFlags flags,
                अक्षर * स्थिर *dirs,
                अक्षर **ret_मूल,
                पूर्णांक *ret_rfd,
                अक्षर ***ret_dirs) {

        _cleanup_free_ अक्षर *मूल_abs = शून्य;
        _cleanup_strv_free_ अक्षर **dirs_abs = शून्य;
        पूर्णांक r;

        निश्चित(ret_मूल);
        निश्चित(ret_rfd);
        निश्चित(ret_dirs || strv_isempty(dirs));

        पूर्णांक log_level = conf_files_log_level(flags);

        r = empty_or_मूल_harder_to_null(&मूल);
        अगर (r < 0)
                वापस log_full_errno(log_level, r, "Failed to determine if '%s' points to the root directory: %m", strempty(मूल));

        अगर (ret_dirs) {
                dirs_abs = strv_copy(dirs);
                अगर (!dirs_abs)
                        वापस log_oom_full(log_level);
        }

        _cleanup_close_ पूर्णांक rfd = XAT_FDROOT;
        अगर (मूल) {
                /* When a non-trivial root is specified, we will prefix the result later. Hence, it is not
                 * necessary to modify each config directories here. but needs to normalize the root directory. */
                r = path_make_absolute_cwd(मूल, &मूल_abs);
                अगर (r < 0)
                        वापस log_full_errno(log_level, r, "Failed to make '%s' absolute: %m", मूल);

                path_simplify(मूल_abs);

                rfd = खोलो(मूल, O_CLOEXEC|O_DIRECTORY|O_PATH);
                अगर (rfd < 0)
                        वापस log_full_errno(log_level, त्रुटि_सं, "Failed to open '%s': %m", मूल_abs);

        } अन्यथा अगर (ret_dirs) {
                /* When an empty root or "/" is specified, we will open "/" below, hence we need to make
                 * each config directory absolute if relative. */
                r = path_strv_make_absolute_cwd(dirs_abs);
                अगर (r < 0)
                        वापस log_full_errno(log_level, r, "Failed to make directories absolute: %m");
        }

        *ret_मूल = TAKE_PTR(मूल_abs);
        *ret_rfd = TAKE_FD(rfd);
        अगर (ret_dirs)
                *ret_dirs = TAKE_PTR(dirs_abs);
        वापस 0;
}

अटल पूर्णांक conf_file_prefix_मूल(ConfFile *c, स्थिर अक्षर *मूल, ConfFilesFlags flags) {
        अक्षर *p;
        पूर्णांक r;

        निश्चित(c);

        अगर (FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_मूल))
                वापस 0;

        पूर्णांक log_level = conf_files_log_level(flags);

        r = chaseat_prefix_मूल(c->परिणाम, मूल, &p);
        अगर (r < 0)
                वापस log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->परिणाम, मूल);
        free_and_replace(c->परिणाम, p);

        r = chaseat_prefix_मूल(c->resolved_path, मूल, &p);
        अगर (r < 0)
                वापस log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->resolved_path, मूल);
        free_and_replace(c->resolved_path, p);

        /* Do not use chaseat_prefix_root(), as it is for the result of chaseat(), but the path is not chased. */
        p = path_join(empty_to_मूल(मूल), skip_leading_slash(c->original_path));
        अगर (!p)
                वापस log_oom_full(log_level);

        path_simplify(p);
        वापस free_and_replace(c->original_path, p);
}

अटल bool conf_files_need_stat(ConfFilesFlags flags) {
        वापस (flags & (CONF_FILES_FILTER_MASKED | CONF_FILES_REGULAR | CONF_FILES_DIRECTORY | CONF_FILES_EXECUTABLE)) != 0;
}

अटल ChaseFlags conf_files_chase_flags(ConfFilesFlags flags) {
        ChaseFlags chase_flags = 0;

        अगर (!conf_files_need_stat(flags) || FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK))
                /* Even if no verification is requested, let's unconditionally call chaseat(),
                 * to drop unsafe symlinks. */
                chase_flags |= CHASE_NONEXISTENT;

        वापस chase_flags;
}

अटल पूर्णांक conf_file_chase_and_verify(
                स्थिर अक्षर *मूल,          /* for logging, can be NULL */
                पूर्णांक rfd,
                स्थिर अक्षर *original_path, /* for logging */
                स्थिर अक्षर *path,
                स्थिर अक्षर *name,
                Set **masked,              /* optional */
                ConfFilesFlags flags,
                अक्षर **ret_path,
                पूर्णांक *ret_fd,
                काष्ठा stat *ret_stat) {

        _cleanup_free_ अक्षर *resolved_path = शून्य;
        _cleanup_close_ पूर्णांक fd = -EBADF;
        काष्ठा stat st = {};
        पूर्णांक r;

        निश्चित(wildcard_fd_is_valid(rfd));
        निश्चित(original_path);
        निश्चित(path);
        निश्चित(name);

        पूर्णांक log_level = conf_files_log_level(flags);

        मूल = empty_to_मूल(मूल);

        r = chaseat(rfd, rfd, path, conf_files_chase_flags(flags), &resolved_path, &fd);
        अगर (r < 0)
                वापस log_full_errno(log_level, r, "Failed to chase '%s%s': %m",
                                      मूल, skip_leading_slash(original_path));
        अगर (r == 0) {
                अगर (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK)) {
                        /* If the path points to /dev/null in a image or so, then the device node may not exist. */
                        अगर (path_equal(skip_leading_slash(resolved_path), "dev/null")) {
                                अगर (masked) {
                                        /* Mark this one as masked */
                                        r = set_put_strdup(masked, name);
                                        अगर (r < 0)
                                                वापस log_oom_full(log_level);
                                }

                                वापस log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                                       मूल, skip_leading_slash(original_path));
                        }
                }

                अगर (conf_files_need_stat(flags))
                        /* If we need to have stat, skip the entry. */
                        वापस log_full_errno(log_level, SYNTHETIC_ERRNO(ENOENT), "Failed to chase '%s%s': %m",
                                              मूल, skip_leading_slash(original_path));
        }

        /* Even if we do not need stat, let's take stat now. The caller may use the info later. */
        अगर (fd >= 0 && ख_स्थिति(fd, &st) < 0)
                वापस log_full_errno(log_level, त्रुटि_सं, "Failed to stat '%s%s': %m",
                                      मूल, skip_leading_slash(original_path));

        /* Is this a masking entry? */
        अगर (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK) && stat_may_be_dev_null(&st)) {
                अगर (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        अगर (r < 0)
                                वापस log_oom_full(log_level);
                }

                वापस log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                       मूल, skip_leading_slash(original_path));
        }

        अगर (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_EMPTY) && stat_is_empty(&st)) {
                अगर (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        अगर (r < 0)
                                वापस log_oom_full(log_level);
                }

                वापस log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (an empty file).",
                                       मूल, skip_leading_slash(original_path));
        }

        अगर (FLAGS_SET(flags, CONF_FILES_REGULAR|CONF_FILES_DIRECTORY)) {
                अगर (!S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode))
                        वापस log_debug_errno(SYNTHETIC_ERRNO(EBADFD),
                                               "File '%s%s' is neither a regular file or directory.",
                                               मूल, skip_leading_slash(original_path));
        } अन्यथा {
                /* Is this node a regular file? */
                अगर (FLAGS_SET(flags, CONF_FILES_REGULAR)) {
                        r = stat_verify_regular(&st);
                        अगर (r < 0)
                                वापस log_debug_errno(r, "File '%s%s' is not a regular file: %m",
                                                       मूल, skip_leading_slash(original_path));
                }

                /* Is this node a directory? */
                अगर (FLAGS_SET(flags, CONF_FILES_DIRECTORY)) {
                        r = stat_verify_directory(&st);
                        अगर (r < 0)
                                वापस log_debug_errno(r, "File '%s%s' is not a directory: %m",
                                                       मूल, skip_leading_slash(original_path));
                }
        }

        /* Does this node have the executable bit set?
         * As requested: check if the file is marked executable. Note that we don't check access(X_OK) here,
         * as we care about whether the file is marked executable at all, and not whether it is executable
         * for us, because if so, such errors are stuff we should log about. */
        अगर (FLAGS_SET(flags, CONF_FILES_EXECUTABLE) && (st.st_mode & 0111) == 0)
                वापस log_debug_errno(SYNTHETIC_ERRNO(ENOEXEC),
                                       "File '%s%s' is not marked executable.",
                                       मूल, skip_leading_slash(original_path));

        अगर (ret_path)
                *ret_path = TAKE_PTR(resolved_path);
        अगर (ret_fd)
                *ret_fd = TAKE_FD(fd);
        अगर (ret_stat)
                *ret_stat = st;

        वापस 0;
}

पूर्णांक conf_file_new_at(
                स्थिर अक्षर *path,
                स्थिर अक्षर *मूल,
                पूर्णांक rfd,
                ConfFilesFlags flags,
                ConfFile **ret) {
        पूर्णांक r;

        निश्चित(path);
        निश्चित(wildcard_fd_is_valid(rfd));
        निश्चित(ret);

        पूर्णांक log_level = conf_files_log_level(flags);

        _cleanup_free_ अक्षर *मूल_2 = शून्य;
        अगर (DEBUG_LOGGING && !मूल) {
                (व्योम) fd_get_path(rfd, &मूल_2);
                मूल = मूल_2;
        }

        _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
        अगर (!c)
                वापस log_oom_full(log_level);

        *c = (ConfFile) {
                .original_path = strdup(path),
                .fd = -EBADF,
        };

        अगर (!c->original_path)
                वापस log_oom_full(log_level);

        r = path_extract_filename(path, &c->filename);
        अगर (r < 0)
                वापस log_full_errno(log_level, r, "Failed to extract filename from '%s': %m", path);

        _cleanup_free_ अक्षर *dirpath = शून्य, *resolved_dirpath = शून्य;
        r = path_extract_directory(path, &dirpath);
        अगर (r < 0 && r != -EDESTADDRREQ)
                वापस log_full_errno(log_level, r, "Failed to extract directory from '%s': %m", path);
        अगर (r >= 0) {
                r = chaseat(rfd, rfd, dirpath,
                            CHASE_MUST_BE_DIRECTORY | conf_files_chase_flags(flags),
                            &resolved_dirpath, /* ret_fd= */ शून्य);
                अगर (r < 0)
                        वापस log_full_errno(log_level, r, "Failed to chase '%s%s': %m", empty_to_मूल(मूल), skip_leading_slash(dirpath));
        }

        c->परिणाम = path_join(resolved_dirpath, c->filename);
        अगर (!c->परिणाम)
                वापस log_oom_full(log_level);

        r = conf_file_chase_and_verify(
                        मूल,
                        rfd,
                        c->original_path,
                        c->परिणाम,
                        c->filename,
                        /* masked= */ शून्य,
                        flags,
                        &c->resolved_path,
                        &c->fd,
                        &c->st);
        अगर (r < 0)
                वापस r;

        *ret = TAKE_PTR(c);
        वापस 0;
}

पूर्णांक conf_file_new(स्थिर अक्षर *path, स्थिर अक्षर *मूल, ConfFilesFlags flags, ConfFile **ret) {
        पूर्णांक r;

        निश्चित(path);
        निश्चित(ret);

        _cleanup_free_ अक्षर *मूल_abs = शून्य;
        _cleanup_close_ पूर्णांक rfd = -EBADF;
        r = prepare_dirs(मूल, flags, /* dirs= */ शून्य, &मूल_abs, &rfd, /* ret_dirs= */ शून्य);
        अगर (r < 0)
                वापस r;

        _cleanup_free_ अक्षर *path_abs = शून्य;
        अगर (!मूल_abs) {
                r = path_make_absolute_cwd(path, &path_abs);
                अगर (r < 0)
                        वापस log_full_errno(conf_files_log_level(flags), r, "Failed to make '%s' absolute: %m", path);

                path = path_abs;
        }

        _cleanup_(conf_file_freep) ConfFile *c = शून्य;
        r = conf_file_new_at(path, मूल_abs, rfd, flags, &c);
        अगर (r < 0)
                वापस r;

        r = conf_file_prefix_मूल(c, मूल_abs, flags);
        अगर (r < 0)
                वापस r;

        *ret = TAKE_PTR(c);
        वापस 0;
}

DEFINE_PRIVATE_HASH_OPS_WITH_मूल्य_DESTRUCTOR(
                conf_file_hash_ops,
                अक्षर, string_hash_func, string_compare_func,
                ConfFile, conf_file_free);

अटल पूर्णांक files_add(
                सूची *dir,
                स्थिर अक्षर *original_dirpath,
                स्थिर अक्षर *resolved_dirpath,
                स्थिर अक्षर *मूल, /* for logging, can be NULL */
                पूर्णांक rfd,
                Hashmap **files,
                Set **masked,
                स्थिर अक्षर *suffix,
                ConfFilesFlags flags) {

        पूर्णांक r;

        निश्चित(dir);
        निश्चित(original_dirpath);
        निश्चित(resolved_dirpath);
        निश्चित(wildcard_fd_is_valid(rfd));
        निश्चित(files);
        निश्चित(masked);

        पूर्णांक log_level = conf_files_log_level(flags);

        मूल = empty_to_मूल(मूल);

        FOREACH_DIRENT(de, dir, वापस log_full_errno(log_level, त्रुटि_सं, "Failed to read directory '%s%s': %m",
                                                      मूल, skip_leading_slash(original_dirpath))) {

                _cleanup_free_ अक्षर *original_path = path_join(original_dirpath, de->d_name);
                अगर (!original_path)
                        वापस log_oom_full(log_level);

                /* Does this match the suffix? */
                अगर (suffix && !endswith(de->d_name, suffix)) {
                        log_debug("Skipping file '%s%s', suffix is not '%s'.", मूल, skip_leading_slash(original_path), suffix);
                        जारी;
                }

                /* Has this file already been found in an earlier directory? */
                अगर (hashmap_contains(*files, de->d_name)) {
                        log_debug("Skipping overridden file '%s%s'.", मूल, skip_leading_slash(original_path));
                        जारी;
                }

                /* Has this been masked in an earlier directory? */
                अगर ((flags & CONF_FILES_FILTER_MASKED) != 0 && set_contains(*masked, de->d_name)) {
                        log_debug("File '%s%s' is masked by previous entry.", मूल, skip_leading_slash(original_path));
                        जारी;
                }

                _cleanup_free_ अक्षर *p = path_join(resolved_dirpath, de->d_name);
                अगर (!p)
                        वापस log_oom_full(log_level);

                _cleanup_free_ अक्षर *resolved_path = शून्य;
                _cleanup_close_ पूर्णांक fd = -EBADF;
                काष्ठा stat st;
                r = conf_file_chase_and_verify(
                                मूल,
                                rfd,
                                original_path,
                                p,
                                de->d_name,
                                masked,
                                flags,
                                &resolved_path,
                                &fd,
                                &st);
                अगर (r == -ENOMEM)
                        वापस r;
                अगर (r < 0)
                        जारी;

                _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
                अगर (!c)
                        वापस log_oom_full(log_level);

                *c = (ConfFile) {
                        .filename = strdup(de->d_name),
                        .परिणाम = TAKE_PTR(p),
                        .original_path = TAKE_PTR(original_path),
                        .resolved_path = TAKE_PTR(resolved_path),
                        .fd = TAKE_FD(fd),
                        .st = st,
                };

                अगर (!c->filename)
                        वापस log_oom_full(log_level);

                r = hashmap_ensure_put(files, &conf_file_hash_ops, c->filename, c);
                अगर (r < 0) {
                        निश्चित(r == -ENOMEM);
                        वापस log_oom_full(log_level);
                }
                निश्चित(r > 0);

                TAKE_PTR(c);
        }

        वापस 0;
}

अटल पूर्णांक dump_files(Hashmap *fh, स्थिर अक्षर *मूल, ConfFilesFlags flags, ConfFile ***ret_files, माप_प्रकार *ret_n_files) {
        ConfFile **files = शून्य;
        माप_प्रकार n_files = 0;
        पूर्णांक r;

        CLEANUP_ARRAY(files, n_files, conf_file_free_array);

        निश्चित(ret_files);
        निश्चित(ret_n_files);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap. */
        r = hashmap_dump_sorted(fh, (व्योम***) &files, &n_files);
        अगर (r < 0)
                वापस log_oom_full(conf_files_log_level(flags));

        /* Hence, we need to remove them from the hashmap. */
        FOREACH_ARRAY(i, files, n_files)
                assert_se(hashmap_remove(fh, (*i)->filename) == *i);

        अगर (मूल)
                FOREACH_ARRAY(i, files, n_files) {
                        r = conf_file_prefix_मूल(*i, मूल, flags);
                        अगर (r < 0)
                                वापस r;
                }

        *ret_files = TAKE_PTR(files);
        *ret_n_files = n_files;
        वापस 0;
}

अटल पूर्णांक copy_and_sort_files_from_hashmap(
                Hashmap *fh,
                स्थिर अक्षर *suffix,
                स्थिर अक्षर *मूल,
                ConfFilesFlags flags,
                अक्षर ***ret) {

        _cleanup_strv_free_ अक्षर **results = शून्य;
        _cleanup_free_ ConfFile **files = शून्य;
        माप_प्रकार n_files = 0, n_results = 0;
        पूर्णांक r;

        निश्चित(ret);

        पूर्णांक log_level = conf_files_log_level(flags);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap.
         * Hence, do not use conf_file_free_array() for 'entries' */
        r = hashmap_dump_sorted(fh, (व्योम***) &files, &n_files);
        अगर (r < 0)
                वापस log_oom_full(log_level);

        FOREACH_ARRAY(i, files, n_files) {
                ConfFile *c = *i;
                स्थिर अक्षर *add = शून्य;

                अगर (FLAGS_SET(flags, CONF_FILES_BASENAME))
                        add = c->filename;
                अन्यथा अगर (मूल && !FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_मूल)) {
                        _cleanup_free_ अक्षर *p = शून्य;

                        r = chaseat_prefix_मूल(c->परिणाम, मूल, &p);
                        अगर (r < 0)
                                वापस log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->परिणाम, मूल);

                        अगर (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX) && suffix) {
                                अक्षर *e = endswith(p, suffix);
                                अगर (!e)
                                        जारी;

                                *e = 0;
                        }

                        अगर (strv_consume_with_आकार(&results, &n_results, TAKE_PTR(p)) < 0)
                                वापस log_oom_full(log_level);

                        जारी;
                } अन्यथा
                        add = c->परिणाम;

                अगर (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX)) {
                        स्थिर अक्षर *e = endswith(add, suffix);
                        अगर (!e)
                                जारी;

                        _cleanup_free_ अक्षर *n = strndup(add, e - add);
                        अगर (!n)
                                वापस log_oom_full(log_level);

                        r = strv_consume_with_आकार(&results, &n_results, TAKE_PTR(n));
                } अन्यथा
                        r = strv_extend_with_आकार(&results, &n_results, add);
                अगर (r < 0)
                        वापस log_oom_full(log_level);
        }

        *ret = TAKE_PTR(results);
        वापस 0;
}

अटल पूर्णांक जोड़ो_replacement(Hashmap **fh, ConfFile *replacement, ConfFilesFlags flags, स्थिर ConfFile **ret) {
        _cleanup_(conf_file_freep) ConfFile *c = ASSERT_PTR(replacement);
        पूर्णांक r;

        निश्चित(fh);
        निश्चित(ret);

        /* This consumes the input ConfFile. */

        ConfFile *existing = hashmap_get(*fh, c->filename);
        अगर (existing) {
                log_debug("An entry with higher priority '%s' -> '%s' already exists, ignoring the replacement: %s",
                          existing->filename, existing->परिणाम, c->original_path);
                *ret = शून्य;
                वापस 0;
        }

        r = hashmap_ensure_put(fh, &conf_file_hash_ops, c->filename, c);
        अगर (r < 0) {
                निश्चित(r == -ENOMEM);
                वापस log_oom_full(conf_files_log_level(flags));
        }
        निश्चित(r > 0);

        log_debug("Inserted replacement: '%s' -> '%s'", c->filename, c->परिणाम);

        *ret = TAKE_PTR(c);
        वापस 0;
}

अटल पूर्णांक conf_files_list_impl(
                स्थिर अक्षर *suffix,
                स्थिर अक्षर *मूल, /* for logging, can be NULL */
                पूर्णांक rfd,
                ConfFilesFlags flags,
                स्थिर अक्षर * स्थिर *dirs,
                स्थिर अक्षर *replacement,
                Hashmap **ret,
                स्थिर ConfFile **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = शून्य;
        _cleanup_set_free_ Set *masked = शून्य;
        _cleanup_(conf_file_freep) ConfFile *c = शून्य;
        स्थिर ConfFile *inserted = शून्य;
        पूर्णांक r;

        निश्चित(wildcard_fd_is_valid(rfd));
        निश्चित(ret);

        मूल = empty_to_मूल(मूल);

        अगर (replacement) {
                r = conf_file_new_at(replacement, मूल, rfd, flags & CONF_FILES_WARN, &c);
                अगर (r < 0)
                        वापस r;
        }

        STRV_FOREACH(p, dirs) {
                _cleanup_closedir_ सूची *dir = शून्य;
                _cleanup_free_ अक्षर *path = शून्य;

                r = chase_and_opendirat(rfd, rfd, *p, 0, &path, &dir);
                अगर (r < 0) {
                        अगर (r != -ENOENT)
                                log_full_errno(conf_files_log_level(flags), r,
                                               "Failed to chase and open directory '%s%s', ignoring: %m",
                                               मूल, skip_leading_slash(*p));
                        जारी;
                }

                अगर (c && streq_ptr(path_startswith(c->परिणाम, path), c->filename)) {
                        r = जोड़ो_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                        अगर (r < 0)
                                वापस r;
                }

                r = files_add(dir, *p, path, मूल, rfd, &fh, &masked, suffix, flags);
                अगर (r == -ENOMEM)
                        वापस r;
        }

        अगर (c) {
                r = जोड़ो_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                अगर (r < 0)
                        वापस r;
        }

        *ret = TAKE_PTR(fh);
        अगर (ret_inserted)
                *ret_inserted = inserted;
        वापस 0;
}

पूर्णांक conf_files_list_strv(
                अक्षर ***ret,
                स्थिर अक्षर *suffix,
                स्थिर अक्षर *मूल,
                ConfFilesFlags flags,
                स्थिर अक्षर * स्थिर *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = शून्य;
        _cleanup_close_ पूर्णांक rfd = -EBADF;
        _cleanup_free_ अक्षर *मूल_abs = शून्य;
        _cleanup_strv_free_ अक्षर **dirs_abs = शून्य;
        पूर्णांक r;

        निश्चित(ret);

        r = prepare_dirs(मूल, flags, (अक्षर**) dirs, &मूल_abs, &rfd, &dirs_abs);
        अगर (r < 0)
                वापस r;

        r = conf_files_list_impl(suffix, मूल_abs, rfd, flags, (स्थिर अक्षर * स्थिर *) dirs_abs,
                                 /* replacement= */ शून्य, &fh, /* ret_inserted= */ शून्य);
        अगर (r < 0)
                वापस r;

        वापस copy_and_sort_files_from_hashmap(fh, suffix, empty_to_मूल(मूल_abs), flags, ret);
}

पूर्णांक conf_files_list_strv_full(
                स्थिर अक्षर *suffix,
                स्थिर अक्षर *मूल,
                ConfFilesFlags flags,
                स्थिर अक्षर * स्थिर *dirs,
                ConfFile ***ret_files,
                माप_प्रकार *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = शून्य;
        _cleanup_close_ पूर्णांक rfd = -EBADF;
        _cleanup_free_ अक्षर *मूल_abs = शून्य;
        _cleanup_strv_free_ अक्षर **dirs_abs = शून्य;
        पूर्णांक r;

        निश्चित(ret_files);
        निश्चित(ret_n_files);

        r = prepare_dirs(मूल, flags, (अक्षर**) dirs, &मूल_abs, &rfd, &dirs_abs);
        अगर (r < 0)
                वापस r;

        r = conf_files_list_impl(suffix, मूल_abs, rfd, flags, (स्थिर अक्षर * स्थिर *) dirs_abs,
                                 /* replacement= */ शून्य, &fh, /* ret_inserted= */ शून्य);
        अगर (r < 0)
                वापस r;

        वापस dump_files(fh, empty_to_मूल(मूल_abs), flags, ret_files, ret_n_files);
}

पूर्णांक conf_files_list_strv_at(
                अक्षर ***ret,
                स्थिर अक्षर *suffix,
                पूर्णांक rfd,
                ConfFilesFlags flags,
                स्थिर अक्षर * स्थिर *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = शून्य;
        _cleanup_free_ अक्षर *मूल = शून्य;
        पूर्णांक r;

        निश्चित(wildcard_fd_is_valid(rfd));
        निश्चित(ret);

        अगर (DEBUG_LOGGING)
                (व्योम) fd_get_path(rfd, &मूल); /* for logging */

        r = conf_files_list_impl(suffix, मूल, rfd, flags, dirs, /* replacement= */ शून्य, &fh, /* ret_inserted= */ शून्य);
        अगर (r < 0)
                वापस r;

        वापस copy_and_sort_files_from_hashmap(fh, suffix, /* root= */ शून्य, flags, ret);
}

पूर्णांक conf_files_list_strv_at_full(
                स्थिर अक्षर *suffix,
                पूर्णांक rfd,
                ConfFilesFlags flags,
                स्थिर अक्षर * स्थिर *dirs,
                ConfFile ***ret_files,
                माप_प्रकार *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = शून्य;
        _cleanup_free_ अक्षर *मूल = शून्य;
        पूर्णांक r;

        निश्चित(wildcard_fd_is_valid(rfd));
        निश्चित(ret_files);
        निश्चित(ret_n_files);

        अगर (DEBUG_LOGGING)
                (व्योम) fd_get_path(rfd, &मूल); /* for logging */

        r = conf_files_list_impl(suffix, मूल, rfd, flags, dirs, /* replacement= */ शून्य, &fh, /* ret_inserted= */ शून्य);
        अगर (r < 0)
                वापस r;

        वापस dump_files(fh, /* root= */ शून्य, flags, ret_files, ret_n_files);
}

पूर्णांक conf_files_list(अक्षर ***ret, स्थिर अक्षर *suffix, स्थिर अक्षर *मूल, ConfFilesFlags flags, स्थिर अक्षर *dir) {
        वापस conf_files_list_strv(ret, suffix, मूल, flags, STRV_MAKE_CONST(dir));
}

पूर्णांक conf_files_list_full(स्थिर अक्षर *suffix, स्थिर अक्षर *मूल, ConfFilesFlags flags, स्थिर अक्षर *dir, ConfFile ***ret_files, माप_प्रकार *ret_n_files) {
        वापस conf_files_list_strv_full(suffix, मूल, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

पूर्णांक conf_files_list_at(अक्षर ***ret, स्थिर अक्षर *suffix, पूर्णांक rfd, ConfFilesFlags flags, स्थिर अक्षर *dir) {
        वापस conf_files_list_strv_at(ret, suffix, rfd, flags, STRV_MAKE_CONST(dir));
}

पूर्णांक conf_files_list_at_full(स्थिर अक्षर *suffix, पूर्णांक rfd, ConfFilesFlags flags, स्थिर अक्षर *dir, ConfFile ***ret_files, माप_प्रकार *ret_n_files) {
        वापस conf_files_list_strv_at_full(suffix, rfd, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

पूर्णांक conf_files_list_nulstr(अक्षर ***ret, स्थिर अक्षर *suffix, स्थिर अक्षर *मूल, ConfFilesFlags flags, स्थिर अक्षर *dirs) {
        _cleanup_strv_free_ अक्षर **d = शून्य;

        निश्चित(ret);

        d = strv_split_nulstr(dirs);
        अगर (!d)
                वापस log_oom_full(conf_files_log_level(flags));

        वापस conf_files_list_strv(ret, suffix, मूल, flags, (स्थिर अक्षर**) d);
}

पूर्णांक conf_files_list_nulstr_full(स्थिर अक्षर *suffix, स्थिर अक्षर *मूल, ConfFilesFlags flags, स्थिर अक्षर *dirs, ConfFile ***ret_files, माप_प्रकार *ret_n_files) {
        _cleanup_strv_free_ अक्षर **d = शून्य;

        निश्चित(ret_files);
        निश्चित(ret_n_files);

        d = strv_split_nulstr(dirs);
        अगर (!d)
                वापस log_oom_full(conf_files_log_level(flags));

        वापस conf_files_list_strv_full(suffix, मूल, flags, (स्थिर अक्षर**) d, ret_files, ret_n_files);
}

पूर्णांक conf_files_list_nulstr_at(अक्षर ***ret, स्थिर अक्षर *suffix, पूर्णांक rfd, ConfFilesFlags flags, स्थिर अक्षर *dirs) {
        _cleanup_strv_free_ अक्षर **d = शून्य;

        निश्चित(ret);

        d = strv_split_nulstr(dirs);
        अगर (!d)
                वापस log_oom_full(conf_files_log_level(flags));

        वापस conf_files_list_strv_at(ret, suffix, rfd, flags, (स्थिर अक्षर**) d);
}

पूर्णांक conf_files_list_nulstr_at_full(स्थिर अक्षर *suffix, पूर्णांक rfd, ConfFilesFlags flags, स्थिर अक्षर *dirs, ConfFile ***ret_files, माप_प्रकार *ret_n_files) {
        _cleanup_strv_free_ अक्षर **d = शून्य;

        निश्चित(ret_files);
        निश्चित(ret_n_files);

        d = strv_split_nulstr(dirs);
        अगर (!d)
                वापस log_oom_full(conf_files_log_level(flags));

        वापस conf_files_list_strv_at_full(suffix, rfd, flags, (स्थिर अक्षर**) d, ret_files, ret_n_files);
}

पूर्णांक conf_files_list_with_replacement(
                स्थिर अक्षर *मूल,
                अक्षर **config_dirs,
                स्थिर अक्षर *replacement,
                अक्षर ***ret_files,
                अक्षर **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = शून्य;
        _cleanup_free_ अक्षर *inserted = शून्य;
        ConfFilesFlags flags = CONF_FILES_REGULAR | CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_WARN;
        _cleanup_close_ पूर्णांक rfd = -EBADF;
        _cleanup_free_ अक्षर *मूल_abs = शून्य;
        _cleanup_strv_free_ अक्षर **dirs_abs = शून्य;
        स्थिर ConfFile *c = शून्य;
        पूर्णांक r;

        निश्चित(ret_files);

        r = prepare_dirs(मूल, flags, config_dirs, &मूल_abs, &rfd, &dirs_abs);
        अगर (r < 0)
                वापस r;

        r = conf_files_list_impl(".conf", मूल_abs, rfd, flags, (स्थिर अक्षर * स्थिर *) dirs_abs,
                                 replacement, &fh, ret_inserted ? &c : शून्य);
        अगर (r < 0)
                वापस r;

        अगर (c) {
                r = chaseat_prefix_मूल(c->परिणाम, मूल_abs, &inserted);
                अगर (r < 0)
                        वापस log_full_errno(conf_files_log_level(flags), r,
                                              "Failed to prefix '%s' with root '%s': %m",
                                              c->परिणाम, empty_to_मूल(मूल_abs));
        }

        r = copy_and_sort_files_from_hashmap(fh, ".conf", empty_to_मूल(मूल_abs), flags, ret_files);
        अगर (r < 0)
                वापस r;

        अगर (ret_inserted)
                *ret_inserted = TAKE_PTR(inserted);
        वापस 0;
}

पूर्णांक conf_files_list_dropins(
                अक्षर ***ret,
                स्थिर अक्षर *dropin_dirname,
                स्थिर अक्षर *मूल,
                पूर्णांक मूल_fd,
                ConfFilesFlags flags,
                स्थिर अक्षर * स्थिर *dirs) {

        _cleanup_strv_free_ अक्षर **dropin_dirs = शून्य;
        स्थिर अक्षर *suffix;
        पूर्णांक r;

        निश्चित(ret);
        निश्चित(dropin_dirname);
        निश्चित(dirs);

        suffix = strjoina("/", dropin_dirname);
        r = strv_extend_strv_concat(&dropin_dirs, dirs, suffix);
        अगर (r < 0)
                वापस log_oom_full(conf_files_log_level(flags));

        वापस conf_files_list_strv_at(ret, ".conf", मूल_fd, flags, (स्थिर अक्षर* स्थिर*) dropin_dirs);
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
                स्थिर अक्षर *मूल,
                स्थिर अक्षर **config_dirs,
                स्थिर अक्षर *fn,
                parse_पंक्ति_t parse_पंक्ति,
                व्योम *userdata,
                bool ignore_enoent,
                bool *invalid_config) {

        _cleanup_fclose_ खाता *_f = शून्य;
        _cleanup_free_ अक्षर *_fn = शून्य;
        अचिन्हित v = 0;
        खाता *f;
        पूर्णांक r = 0;

        निश्चित(fn);

        अगर (streq(fn, "-")) {
                f = मानक_निवेश;
                fn = "<stdin>";

                log_debug("Reading config from stdin%s", glyph(GLYPH_ELLIPSIS));

        } अन्यथा अगर (is_path(fn)) {
                r = path_make_absolute_cwd(fn, &_fn);
                अगर (r < 0)
                        वापस log_error_errno(r, "Failed to make path absolute: %m");
                fn = _fn;

                f = _f = ख_खोलो(fn, "re");
                अगर (!_f)
                        r = -त्रुटि_सं;
                अन्यथा
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));

        } अन्यथा {
                r = खोज_and_fopen(fn, "re", मूल, config_dirs, &_f, &_fn);
                अगर (r >= 0) {
                        f = _f;
                        fn = _fn;
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));
                }
        }

        अगर (r == -ENOENT && ignore_enoent) {
                log_debug_errno(r, "Failed to open \"%s\", ignoring: %m", fn);
                वापस 0; /* No error, but nothing happened. */
        }
        अगर (r < 0)
                वापस log_error_errno(r, "Failed to read '%s': %m", fn);

        r = 1;  /* We entered the part where we may modify state. */

        क्रम (;;) {
                _cleanup_free_ अक्षर *पंक्ति = शून्य;
                bool invalid_पंक्ति = false;
                पूर्णांक k;

                k = read_stripped_पंक्ति(f, LONG_पंक्ति_MAX, &पंक्ति);
                अगर (k < 0)
                        वापस log_error_errno(k, "Failed to read '%s': %m", fn);
                अगर (k == 0)
                        अवरोध;

                v++;

                अगर (IN_SET(पंक्ति[0], 0, '#'))
                        जारी;

                k = parse_पंक्ति(fn, v, पंक्ति, invalid_config ? &invalid_पंक्ति : शून्य, userdata);
                अगर (k < 0 && invalid_पंक्ति)
                        /* Allow reporting with a special code if the caller requested this. */
                        *invalid_config = true;
                अन्यथा
                        /* The first error, if any, becomes our return value. */
                        RET_GATHER(r, k);
        }

        अगर (ख_त्रुटि(f))
                RET_GATHER(r, log_error_errno(SYNTHETIC_ERRNO(EIO), "Failed to read from file %s.", fn));

        वापस r;
}
