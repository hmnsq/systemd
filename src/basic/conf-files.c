/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <stdlib.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "chase.h"
#அந்தர்பாவ "conf-files.h"
#அந்தர்பாவ "dirent-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fileio.h"
#அந்தர்பாவ "glyph-util.h"
#அந்தர்பாவ "hashmap.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "nulstr-util.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "set.h"
#அந்தர்பாவ "stat-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"

ConfFile* conf_file_free(ConfFile *c) {
        எனில் (!c)
                திரும்பு வெற்று;

        விடுவி(c->filename);
        விடுவி(c->result);
        விடுவி(c->original_path);
        விடுவி(c->resolved_path);
        safe_close(c->fd);

        திரும்பு mfree(c);
}

DEFINE_POINTER_ARRAY_FREE_FUNC(ConfFile*, conf_file_free);

நிலையான முழு conf_files_log_level(ConfFilesFlags flags) {
        திரும்பு FLAGS_SET(flags, CONF_FILES_WARN) ? LOG_WARNING : LOG_DEBUG;
}

நிலையான முழு prepare_dirs(
                மாறிலி எழுத்து *வேர்,
                ConfFilesFlags flags,
                எழுத்து * மாறிலி *dirs,
                எழுத்து **ret_வேர்,
                முழு *ret_rfd,
                எழுத்து ***ret_dirs) {

        _cleanup_free_ எழுத்து *வேர்_abs = வெற்று;
        _cleanup_strv_free_ எழுத்து **dirs_abs = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(ret_வேர்);
        ப்ரதிஜ்ஞா(ret_rfd);
        ப்ரதிஜ்ஞா(ret_dirs || strv_isempty(dirs));

        முழு log_level = conf_files_log_level(flags);

        r = empty_or_வேர்_harder_to_null(&வேர்);
        எனில் (r < 0)
                திரும்பு log_full_errno(log_level, r, "Failed to determine if '%s' points to the root directory: %m", strempty(வேர்));

        எனில் (ret_dirs) {
                dirs_abs = strv_copy(dirs);
                எனில் (!dirs_abs)
                        திரும்பு log_oom_full(log_level);
        }

        _cleanup_close_ முழு rfd = XAT_FDROOT;
        எனில் (வேர்) {
                /* When a non-trivial root is specified, we will prefix the result later. Hence, it is not
                 * necessary to modify each config directories here. but needs to normalize the root directory. */
                r = path_make_absolute_cwd(வேர், &வேர்_abs);
                எனில் (r < 0)
                        திரும்பு log_full_errno(log_level, r, "Failed to make '%s' absolute: %m", வேர்);

                path_simplify(வேர்_abs);

                rfd = open(வேர், O_CLOEXEC|O_DIRECTORY|O_PATH);
                எனில் (rfd < 0)
                        திரும்பு log_full_errno(log_level, errno, "Failed to open '%s': %m", வேர்_abs);

        } இல்லையெனில் எனில் (ret_dirs) {
                /* When an empty root or "/" is specified, we will open "/" below, hence we need to make
                 * each config directory absolute if relative. */
                r = path_strv_make_absolute_cwd(dirs_abs);
                எனில் (r < 0)
                        திரும்பு log_full_errno(log_level, r, "Failed to make directories absolute: %m");
        }

        *ret_வேர் = TAKE_PTR(வேர்_abs);
        *ret_rfd = TAKE_FD(rfd);
        எனில் (ret_dirs)
                *ret_dirs = TAKE_PTR(dirs_abs);
        திரும்பு 0;
}

நிலையான முழு conf_file_prefix_வேர்(ConfFile *c, மாறிலி எழுத்து *வேர், ConfFilesFlags flags) {
        எழுத்து *p;
        முழு r;

        ப்ரதிஜ்ஞா(c);

        எனில் (FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_வேர்))
                திரும்பு 0;

        முழு log_level = conf_files_log_level(flags);

        r = chaseat_prefix_வேர்(c->result, வேர், &p);
        எனில் (r < 0)
                திரும்பு log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->result, வேர்);
        free_and_replace(c->result, p);

        r = chaseat_prefix_வேர்(c->resolved_path, வேர், &p);
        எனில் (r < 0)
                திரும்பு log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->resolved_path, வேர்);
        free_and_replace(c->resolved_path, p);

        /* Do not use chaseat_prefix_root(), as it is for the result of chaseat(), but the path is not chased. */
        p = path_join(empty_to_வேர்(வேர்), skip_leading_slash(c->original_path));
        எனில் (!p)
                திரும்பு log_oom_full(log_level);

        path_simplify(p);
        திரும்பு free_and_replace(c->original_path, p);
}

நிலையான bool conf_files_need_stat(ConfFilesFlags flags) {
        திரும்பு (flags & (CONF_FILES_FILTER_MASKED | CONF_FILES_REGULAR | CONF_FILES_DIRECTORY | CONF_FILES_EXECUTABLE)) != 0;
}

நிலையான ChaseFlags conf_files_chase_flags(ConfFilesFlags flags) {
        ChaseFlags chase_flags = 0;

        எனில் (!conf_files_need_stat(flags) || FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK))
                /* Even if no verification is requested, let's unconditionally call chaseat(),
                 * to drop unsafe symlinks. */
                chase_flags |= CHASE_NONEXISTENT;

        திரும்பு chase_flags;
}

நிலையான முழு conf_file_chase_and_verify(
                மாறிலி எழுத்து *வேர்,          /* for logging, can be NULL */
                முழு rfd,
                மாறிலி எழுத்து *original_path, /* for logging */
                மாறிலி எழுத்து *path,
                மாறிலி எழுத்து *name,
                Set **masked,              /* optional */
                ConfFilesFlags flags,
                எழுத்து **ret_path,
                முழு *ret_fd,
                கட்டமைப்பு stat *ret_stat) {

        _cleanup_free_ எழுத்து *resolved_path = வெற்று;
        _cleanup_close_ முழு fd = -EBADF;
        கட்டமைப்பு stat st = {};
        முழு r;

        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(rfd));
        ப்ரதிஜ்ஞா(original_path);
        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(name);

        முழு log_level = conf_files_log_level(flags);

        வேர் = empty_to_வேர்(வேர்);

        r = chaseat(rfd, rfd, path, conf_files_chase_flags(flags), &resolved_path, &fd);
        எனில் (r < 0)
                திரும்பு log_full_errno(log_level, r, "Failed to chase '%s%s': %m",
                                      வேர், skip_leading_slash(original_path));
        எனில் (r == 0) {
                எனில் (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK)) {
                        /* If the path points to /dev/null in a image or so, then the device node may not exist. */
                        எனில் (path_equal(skip_leading_slash(resolved_path), "dev/null")) {
                                எனில் (masked) {
                                        /* Mark this one as masked */
                                        r = set_put_strdup(masked, name);
                                        எனில் (r < 0)
                                                திரும்பு log_oom_full(log_level);
                                }

                                திரும்பு log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                                       வேர், skip_leading_slash(original_path));
                        }
                }

                எனில் (conf_files_need_stat(flags))
                        /* If we need to have stat, skip the entry. */
                        திரும்பு log_full_errno(log_level, SYNTHETIC_ERRNO(ENOENT), "Failed to chase '%s%s': %m",
                                              வேர், skip_leading_slash(original_path));
        }

        /* Even if we do not need stat, let's take stat now. The caller may use the info later. */
        எனில் (fd >= 0 && fstat(fd, &st) < 0)
                திரும்பு log_full_errno(log_level, errno, "Failed to stat '%s%s': %m",
                                      வேர், skip_leading_slash(original_path));

        /* Is this a masking entry? */
        எனில் (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK) && stat_may_be_dev_null(&st)) {
                எனில் (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        எனில் (r < 0)
                                திரும்பு log_oom_full(log_level);
                }

                திரும்பு log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                       வேர், skip_leading_slash(original_path));
        }

        எனில் (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_EMPTY) && stat_is_empty(&st)) {
                எனில் (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        எனில் (r < 0)
                                திரும்பு log_oom_full(log_level);
                }

                திரும்பு log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (an empty file).",
                                       வேர், skip_leading_slash(original_path));
        }

        எனில் (FLAGS_SET(flags, CONF_FILES_REGULAR|CONF_FILES_DIRECTORY)) {
                எனில் (!S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode))
                        திரும்பு log_debug_errno(SYNTHETIC_ERRNO(EBADFD),
                                               "File '%s%s' is neither a regular file or directory.",
                                               வேர், skip_leading_slash(original_path));
        } இல்லையெனில் {
                /* Is this node a regular file? */
                எனில் (FLAGS_SET(flags, CONF_FILES_REGULAR)) {
                        r = stat_verify_regular(&st);
                        எனில் (r < 0)
                                திரும்பு log_debug_errno(r, "File '%s%s' is not a regular file: %m",
                                                       வேர், skip_leading_slash(original_path));
                }

                /* Is this node a directory? */
                எனில் (FLAGS_SET(flags, CONF_FILES_DIRECTORY)) {
                        r = stat_verify_directory(&st);
                        எனில் (r < 0)
                                திரும்பு log_debug_errno(r, "File '%s%s' is not a directory: %m",
                                                       வேர், skip_leading_slash(original_path));
                }
        }

        /* Does this node have the executable bit set?
         * As requested: check if the file is marked executable. Note that we don't check access(X_OK) here,
         * as we care about whether the file is marked executable at all, and not whether it is executable
         * for us, because if so, such errors are stuff we should log about. */
        எனில் (FLAGS_SET(flags, CONF_FILES_EXECUTABLE) && (st.st_mode & 0111) == 0)
                திரும்பு log_debug_errno(SYNTHETIC_ERRNO(ENOEXEC),
                                       "File '%s%s' is not marked executable.",
                                       வேர், skip_leading_slash(original_path));

        எனில் (ret_path)
                *ret_path = TAKE_PTR(resolved_path);
        எனில் (ret_fd)
                *ret_fd = TAKE_FD(fd);
        எனில் (ret_stat)
                *ret_stat = st;

        திரும்பு 0;
}

முழு conf_file_new_at(
                மாறிலி எழுத்து *path,
                மாறிலி எழுத்து *வேர்,
                முழு rfd,
                ConfFilesFlags flags,
                ConfFile **ret) {
        முழு r;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(rfd));
        ப்ரதிஜ்ஞா(ret);

        முழு log_level = conf_files_log_level(flags);

        _cleanup_free_ எழுத்து *வேர்_2 = வெற்று;
        எனில் (DEBUG_LOGGING && !வேர்) {
                (வெறுமை) fd_get_path(rfd, &வேர்_2);
                வேர் = வேர்_2;
        }

        _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
        எனில் (!c)
                திரும்பு log_oom_full(log_level);

        *c = (ConfFile) {
                .original_path = strdup(path),
                .fd = -EBADF,
        };

        எனில் (!c->original_path)
                திரும்பு log_oom_full(log_level);

        r = path_extract_filename(path, &c->filename);
        எனில் (r < 0)
                திரும்பு log_full_errno(log_level, r, "Failed to extract filename from '%s': %m", path);

        _cleanup_free_ எழுத்து *dirpath = வெற்று, *resolved_dirpath = வெற்று;
        r = path_extract_directory(path, &dirpath);
        எனில் (r < 0 && r != -EDESTADDRREQ)
                திரும்பு log_full_errno(log_level, r, "Failed to extract directory from '%s': %m", path);
        எனில் (r >= 0) {
                r = chaseat(rfd, rfd, dirpath,
                            CHASE_MUST_BE_DIRECTORY | conf_files_chase_flags(flags),
                            &resolved_dirpath, /* ret_fd= */ வெற்று);
                எனில் (r < 0)
                        திரும்பு log_full_errno(log_level, r, "Failed to chase '%s%s': %m", empty_to_வேர்(வேர்), skip_leading_slash(dirpath));
        }

        c->result = path_join(resolved_dirpath, c->filename);
        எனில் (!c->result)
                திரும்பு log_oom_full(log_level);

        r = conf_file_chase_and_verify(
                        வேர்,
                        rfd,
                        c->original_path,
                        c->result,
                        c->filename,
                        /* masked= */ வெற்று,
                        flags,
                        &c->resolved_path,
                        &c->fd,
                        &c->st);
        எனில் (r < 0)
                திரும்பு r;

        *ret = TAKE_PTR(c);
        திரும்பு 0;
}

முழு conf_file_new(மாறிலி எழுத்து *path, மாறிலி எழுத்து *வேர், ConfFilesFlags flags, ConfFile **ret) {
        முழு r;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(ret);

        _cleanup_free_ எழுத்து *வேர்_abs = வெற்று;
        _cleanup_close_ முழு rfd = -EBADF;
        r = prepare_dirs(வேர், flags, /* dirs= */ வெற்று, &வேர்_abs, &rfd, /* ret_dirs= */ வெற்று);
        எனில் (r < 0)
                திரும்பு r;

        _cleanup_free_ எழுத்து *path_abs = வெற்று;
        எனில் (!வேர்_abs) {
                r = path_make_absolute_cwd(path, &path_abs);
                எனில் (r < 0)
                        திரும்பு log_full_errno(conf_files_log_level(flags), r, "Failed to make '%s' absolute: %m", path);

                path = path_abs;
        }

        _cleanup_(conf_file_freep) ConfFile *c = வெற்று;
        r = conf_file_new_at(path, வேர்_abs, rfd, flags, &c);
        எனில் (r < 0)
                திரும்பு r;

        r = conf_file_prefix_வேர்(c, வேர்_abs, flags);
        எனில் (r < 0)
                திரும்பு r;

        *ret = TAKE_PTR(c);
        திரும்பு 0;
}

DEFINE_PRIVATE_HASH_OPS_WITH_மதிப்பு_DESTRUCTOR(
                conf_file_hash_ops,
                எழுத்து, string_hash_func, string_compare_func,
                ConfFile, conf_file_free);

நிலையான முழு files_add(
                DIR *dir,
                மாறிலி எழுத்து *original_dirpath,
                மாறிலி எழுத்து *resolved_dirpath,
                மாறிலி எழுத்து *வேர், /* for logging, can be NULL */
                முழு rfd,
                Hashmap **files,
                Set **masked,
                மாறிலி எழுத்து *suffix,
                ConfFilesFlags flags) {

        முழு r;

        ப்ரதிஜ்ஞா(dir);
        ப்ரதிஜ்ஞா(original_dirpath);
        ப்ரதிஜ்ஞா(resolved_dirpath);
        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(rfd));
        ப்ரதிஜ்ஞா(files);
        ப்ரதிஜ்ஞா(masked);

        முழு log_level = conf_files_log_level(flags);

        வேர் = empty_to_வேர்(வேர்);

        FOREACH_DIRENT(de, dir, திரும்பு log_full_errno(log_level, errno, "Failed to read directory '%s%s': %m",
                                                      வேர், skip_leading_slash(original_dirpath))) {

                _cleanup_free_ எழுத்து *original_path = path_join(original_dirpath, de->d_name);
                எனில் (!original_path)
                        திரும்பு log_oom_full(log_level);

                /* Does this match the suffix? */
                எனில் (suffix && !endswith(de->d_name, suffix)) {
                        log_debug("Skipping file '%s%s', suffix is not '%s'.", வேர், skip_leading_slash(original_path), suffix);
                        தொடர்;
                }

                /* Has this file already been found in an earlier directory? */
                எனில் (hashmap_contains(*files, de->d_name)) {
                        log_debug("Skipping overridden file '%s%s'.", வேர், skip_leading_slash(original_path));
                        தொடர்;
                }

                /* Has this been masked in an earlier directory? */
                எனில் ((flags & CONF_FILES_FILTER_MASKED) != 0 && set_contains(*masked, de->d_name)) {
                        log_debug("File '%s%s' is masked by previous entry.", வேர், skip_leading_slash(original_path));
                        தொடர்;
                }

                _cleanup_free_ எழுத்து *p = path_join(resolved_dirpath, de->d_name);
                எனில் (!p)
                        திரும்பு log_oom_full(log_level);

                _cleanup_free_ எழுத்து *resolved_path = வெற்று;
                _cleanup_close_ முழு fd = -EBADF;
                கட்டமைப்பு stat st;
                r = conf_file_chase_and_verify(
                                வேர்,
                                rfd,
                                original_path,
                                p,
                                de->d_name,
                                masked,
                                flags,
                                &resolved_path,
                                &fd,
                                &st);
                எனில் (r == -ENOMEM)
                        திரும்பு r;
                எனில் (r < 0)
                        தொடர்;

                _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
                எனில் (!c)
                        திரும்பு log_oom_full(log_level);

                *c = (ConfFile) {
                        .filename = strdup(de->d_name),
                        .result = TAKE_PTR(p),
                        .original_path = TAKE_PTR(original_path),
                        .resolved_path = TAKE_PTR(resolved_path),
                        .fd = TAKE_FD(fd),
                        .st = st,
                };

                எனில் (!c->filename)
                        திரும்பு log_oom_full(log_level);

                r = hashmap_ensure_put(files, &conf_file_hash_ops, c->filename, c);
                எனில் (r < 0) {
                        ப்ரதிஜ்ஞா(r == -ENOMEM);
                        திரும்பு log_oom_full(log_level);
                }
                ப்ரதிஜ்ஞா(r > 0);

                TAKE_PTR(c);
        }

        திரும்பு 0;
}

நிலையான முழு dump_files(Hashmap *fh, மாறிலி எழுத்து *வேர், ConfFilesFlags flags, ConfFile ***ret_files, size_t *ret_n_files) {
        ConfFile **files = வெற்று;
        size_t n_files = 0;
        முழு r;

        CLEANUP_ARRAY(files, n_files, conf_file_free_array);

        ப்ரதிஜ்ஞா(ret_files);
        ப்ரதிஜ்ஞா(ret_n_files);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap. */
        r = hashmap_dump_sorted(fh, (வெறுமை***) &files, &n_files);
        எனில் (r < 0)
                திரும்பு log_oom_full(conf_files_log_level(flags));

        /* Hence, we need to remove them from the hashmap. */
        FOREACH_ARRAY(i, files, n_files)
                assert_se(hashmap_remove(fh, (*i)->filename) == *i);

        எனில் (வேர்)
                FOREACH_ARRAY(i, files, n_files) {
                        r = conf_file_prefix_வேர்(*i, வேர், flags);
                        எனில் (r < 0)
                                திரும்பு r;
                }

        *ret_files = TAKE_PTR(files);
        *ret_n_files = n_files;
        திரும்பு 0;
}

நிலையான முழு copy_and_sort_files_from_hashmap(
                Hashmap *fh,
                மாறிலி எழுத்து *suffix,
                மாறிலி எழுத்து *வேர்,
                ConfFilesFlags flags,
                எழுத்து ***ret) {

        _cleanup_strv_free_ எழுத்து **results = வெற்று;
        _cleanup_free_ ConfFile **files = வெற்று;
        size_t n_files = 0, n_results = 0;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        முழு log_level = conf_files_log_level(flags);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap.
         * Hence, do not use conf_file_free_array() for 'entries' */
        r = hashmap_dump_sorted(fh, (வெறுமை***) &files, &n_files);
        எனில் (r < 0)
                திரும்பு log_oom_full(log_level);

        FOREACH_ARRAY(i, files, n_files) {
                ConfFile *c = *i;
                மாறிலி எழுத்து *add = வெற்று;

                எனில் (FLAGS_SET(flags, CONF_FILES_BASENAME))
                        add = c->filename;
                இல்லையெனில் எனில் (வேர் && !FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_வேர்)) {
                        _cleanup_free_ எழுத்து *p = வெற்று;

                        r = chaseat_prefix_வேர்(c->result, வேர், &p);
                        எனில் (r < 0)
                                திரும்பு log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->result, வேர்);

                        எனில் (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX) && suffix) {
                                எழுத்து *e = endswith(p, suffix);
                                எனில் (!e)
                                        தொடர்;

                                *e = 0;
                        }

                        எனில் (strv_consume_with_size(&results, &n_results, TAKE_PTR(p)) < 0)
                                திரும்பு log_oom_full(log_level);

                        தொடர்;
                } இல்லையெனில்
                        add = c->result;

                எனில் (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX)) {
                        மாறிலி எழுத்து *e = endswith(add, suffix);
                        எனில் (!e)
                                தொடர்;

                        _cleanup_free_ எழுத்து *n = strndup(add, e - add);
                        எனில் (!n)
                                திரும்பு log_oom_full(log_level);

                        r = strv_consume_with_size(&results, &n_results, TAKE_PTR(n));
                } இல்லையெனில்
                        r = strv_extend_with_size(&results, &n_results, add);
                எனில் (r < 0)
                        திரும்பு log_oom_full(log_level);
        }

        *ret = TAKE_PTR(results);
        திரும்பு 0;
}

நிலையான முழு செருகு_replacement(Hashmap **fh, ConfFile *replacement, ConfFilesFlags flags, மாறிலி ConfFile **ret) {
        _cleanup_(conf_file_freep) ConfFile *c = ASSERT_PTR(replacement);
        முழு r;

        ப்ரதிஜ்ஞா(fh);
        ப்ரதிஜ்ஞா(ret);

        /* This consumes the input ConfFile. */

        ConfFile *existing = hashmap_get(*fh, c->filename);
        எனில் (existing) {
                log_debug("An entry with higher priority '%s' -> '%s' already exists, ignoring the replacement: %s",
                          existing->filename, existing->result, c->original_path);
                *ret = வெற்று;
                திரும்பு 0;
        }

        r = hashmap_ensure_put(fh, &conf_file_hash_ops, c->filename, c);
        எனில் (r < 0) {
                ப்ரதிஜ்ஞா(r == -ENOMEM);
                திரும்பு log_oom_full(conf_files_log_level(flags));
        }
        ப்ரதிஜ்ஞா(r > 0);

        log_debug("Inserted replacement: '%s' -> '%s'", c->filename, c->result);

        *ret = TAKE_PTR(c);
        திரும்பு 0;
}

நிலையான முழு conf_files_list_impl(
                மாறிலி எழுத்து *suffix,
                மாறிலி எழுத்து *வேர், /* for logging, can be NULL */
                முழு rfd,
                ConfFilesFlags flags,
                மாறிலி எழுத்து * மாறிலி *dirs,
                மாறிலி எழுத்து *replacement,
                Hashmap **ret,
                மாறிலி ConfFile **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = வெற்று;
        _cleanup_set_free_ Set *masked = வெற்று;
        _cleanup_(conf_file_freep) ConfFile *c = வெற்று;
        மாறிலி ConfFile *inserted = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(rfd));
        ப்ரதிஜ்ஞா(ret);

        வேர் = empty_to_வேர்(வேர்);

        எனில் (replacement) {
                r = conf_file_new_at(replacement, வேர், rfd, flags & CONF_FILES_WARN, &c);
                எனில் (r < 0)
                        திரும்பு r;
        }

        STRV_FOREACH(p, dirs) {
                _cleanup_closedir_ DIR *dir = வெற்று;
                _cleanup_free_ எழுத்து *path = வெற்று;

                r = chase_and_opendirat(rfd, rfd, *p, 0, &path, &dir);
                எனில் (r < 0) {
                        எனில் (r != -ENOENT)
                                log_full_errno(conf_files_log_level(flags), r,
                                               "Failed to chase and open directory '%s%s', ignoring: %m",
                                               வேர், skip_leading_slash(*p));
                        தொடர்;
                }

                எனில் (c && streq_ptr(path_startswith(c->result, path), c->filename)) {
                        r = செருகு_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                        எனில் (r < 0)
                                திரும்பு r;
                }

                r = files_add(dir, *p, path, வேர், rfd, &fh, &masked, suffix, flags);
                எனில் (r == -ENOMEM)
                        திரும்பு r;
        }

        எனில் (c) {
                r = செருகு_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                எனில் (r < 0)
                        திரும்பு r;
        }

        *ret = TAKE_PTR(fh);
        எனில் (ret_inserted)
                *ret_inserted = inserted;
        திரும்பு 0;
}

முழு conf_files_list_strv(
                எழுத்து ***ret,
                மாறிலி எழுத்து *suffix,
                மாறிலி எழுத்து *வேர்,
                ConfFilesFlags flags,
                மாறிலி எழுத்து * மாறிலி *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = வெற்று;
        _cleanup_close_ முழு rfd = -EBADF;
        _cleanup_free_ எழுத்து *வேர்_abs = வெற்று;
        _cleanup_strv_free_ எழுத்து **dirs_abs = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        r = prepare_dirs(வேர், flags, (எழுத்து**) dirs, &வேர்_abs, &rfd, &dirs_abs);
        எனில் (r < 0)
                திரும்பு r;

        r = conf_files_list_impl(suffix, வேர்_abs, rfd, flags, (மாறிலி எழுத்து * மாறிலி *) dirs_abs,
                                 /* replacement= */ வெற்று, &fh, /* ret_inserted= */ வெற்று);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு copy_and_sort_files_from_hashmap(fh, suffix, empty_to_வேர்(வேர்_abs), flags, ret);
}

முழு conf_files_list_strv_full(
                மாறிலி எழுத்து *suffix,
                மாறிலி எழுத்து *வேர்,
                ConfFilesFlags flags,
                மாறிலி எழுத்து * மாறிலி *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = வெற்று;
        _cleanup_close_ முழு rfd = -EBADF;
        _cleanup_free_ எழுத்து *வேர்_abs = வெற்று;
        _cleanup_strv_free_ எழுத்து **dirs_abs = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(ret_files);
        ப்ரதிஜ்ஞா(ret_n_files);

        r = prepare_dirs(வேர், flags, (எழுத்து**) dirs, &வேர்_abs, &rfd, &dirs_abs);
        எனில் (r < 0)
                திரும்பு r;

        r = conf_files_list_impl(suffix, வேர்_abs, rfd, flags, (மாறிலி எழுத்து * மாறிலி *) dirs_abs,
                                 /* replacement= */ வெற்று, &fh, /* ret_inserted= */ வெற்று);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு dump_files(fh, empty_to_வேர்(வேர்_abs), flags, ret_files, ret_n_files);
}

முழு conf_files_list_strv_at(
                எழுத்து ***ret,
                மாறிலி எழுத்து *suffix,
                முழு rfd,
                ConfFilesFlags flags,
                மாறிலி எழுத்து * மாறிலி *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = வெற்று;
        _cleanup_free_ எழுத்து *வேர் = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(rfd));
        ப்ரதிஜ்ஞா(ret);

        எனில் (DEBUG_LOGGING)
                (வெறுமை) fd_get_path(rfd, &வேர்); /* for logging */

        r = conf_files_list_impl(suffix, வேர், rfd, flags, dirs, /* replacement= */ வெற்று, &fh, /* ret_inserted= */ வெற்று);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு copy_and_sort_files_from_hashmap(fh, suffix, /* root= */ வெற்று, flags, ret);
}

முழு conf_files_list_strv_at_full(
                மாறிலி எழுத்து *suffix,
                முழு rfd,
                ConfFilesFlags flags,
                மாறிலி எழுத்து * மாறிலி *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = வெற்று;
        _cleanup_free_ எழுத்து *வேர் = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(rfd));
        ப்ரதிஜ்ஞா(ret_files);
        ப்ரதிஜ்ஞா(ret_n_files);

        எனில் (DEBUG_LOGGING)
                (வெறுமை) fd_get_path(rfd, &வேர்); /* for logging */

        r = conf_files_list_impl(suffix, வேர், rfd, flags, dirs, /* replacement= */ வெற்று, &fh, /* ret_inserted= */ வெற்று);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு dump_files(fh, /* root= */ வெற்று, flags, ret_files, ret_n_files);
}

முழு conf_files_list(எழுத்து ***ret, மாறிலி எழுத்து *suffix, மாறிலி எழுத்து *வேர், ConfFilesFlags flags, மாறிலி எழுத்து *dir) {
        திரும்பு conf_files_list_strv(ret, suffix, வேர், flags, STRV_MAKE_CONST(dir));
}

முழு conf_files_list_full(மாறிலி எழுத்து *suffix, மாறிலி எழுத்து *வேர், ConfFilesFlags flags, மாறிலி எழுத்து *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        திரும்பு conf_files_list_strv_full(suffix, வேர், flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

முழு conf_files_list_at(எழுத்து ***ret, மாறிலி எழுத்து *suffix, முழு rfd, ConfFilesFlags flags, மாறிலி எழுத்து *dir) {
        திரும்பு conf_files_list_strv_at(ret, suffix, rfd, flags, STRV_MAKE_CONST(dir));
}

முழு conf_files_list_at_full(மாறிலி எழுத்து *suffix, முழு rfd, ConfFilesFlags flags, மாறிலி எழுத்து *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        திரும்பு conf_files_list_strv_at_full(suffix, rfd, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

முழு conf_files_list_nulstr(எழுத்து ***ret, மாறிலி எழுத்து *suffix, மாறிலி எழுத்து *வேர், ConfFilesFlags flags, மாறிலி எழுத்து *dirs) {
        _cleanup_strv_free_ எழுத்து **d = வெற்று;

        ப்ரதிஜ்ஞா(ret);

        d = strv_split_nulstr(dirs);
        எனில் (!d)
                திரும்பு log_oom_full(conf_files_log_level(flags));

        திரும்பு conf_files_list_strv(ret, suffix, வேர், flags, (மாறிலி எழுத்து**) d);
}

முழு conf_files_list_nulstr_full(மாறிலி எழுத்து *suffix, மாறிலி எழுத்து *வேர், ConfFilesFlags flags, மாறிலி எழுத்து *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ எழுத்து **d = வெற்று;

        ப்ரதிஜ்ஞா(ret_files);
        ப்ரதிஜ்ஞா(ret_n_files);

        d = strv_split_nulstr(dirs);
        எனில் (!d)
                திரும்பு log_oom_full(conf_files_log_level(flags));

        திரும்பு conf_files_list_strv_full(suffix, வேர், flags, (மாறிலி எழுத்து**) d, ret_files, ret_n_files);
}

முழு conf_files_list_nulstr_at(எழுத்து ***ret, மாறிலி எழுத்து *suffix, முழு rfd, ConfFilesFlags flags, மாறிலி எழுத்து *dirs) {
        _cleanup_strv_free_ எழுத்து **d = வெற்று;

        ப்ரதிஜ்ஞா(ret);

        d = strv_split_nulstr(dirs);
        எனில் (!d)
                திரும்பு log_oom_full(conf_files_log_level(flags));

        திரும்பு conf_files_list_strv_at(ret, suffix, rfd, flags, (மாறிலி எழுத்து**) d);
}

முழு conf_files_list_nulstr_at_full(மாறிலி எழுத்து *suffix, முழு rfd, ConfFilesFlags flags, மாறிலி எழுத்து *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ எழுத்து **d = வெற்று;

        ப்ரதிஜ்ஞா(ret_files);
        ப்ரதிஜ்ஞா(ret_n_files);

        d = strv_split_nulstr(dirs);
        எனில் (!d)
                திரும்பு log_oom_full(conf_files_log_level(flags));

        திரும்பு conf_files_list_strv_at_full(suffix, rfd, flags, (மாறிலி எழுத்து**) d, ret_files, ret_n_files);
}

முழு conf_files_list_with_replacement(
                மாறிலி எழுத்து *வேர்,
                எழுத்து **config_dirs,
                மாறிலி எழுத்து *replacement,
                எழுத்து ***ret_files,
                எழுத்து **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = வெற்று;
        _cleanup_free_ எழுத்து *inserted = வெற்று;
        ConfFilesFlags flags = CONF_FILES_REGULAR | CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_WARN;
        _cleanup_close_ முழு rfd = -EBADF;
        _cleanup_free_ எழுத்து *வேர்_abs = வெற்று;
        _cleanup_strv_free_ எழுத்து **dirs_abs = வெற்று;
        மாறிலி ConfFile *c = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(ret_files);

        r = prepare_dirs(வேர், flags, config_dirs, &வேர்_abs, &rfd, &dirs_abs);
        எனில் (r < 0)
                திரும்பு r;

        r = conf_files_list_impl(".conf", வேர்_abs, rfd, flags, (மாறிலி எழுத்து * மாறிலி *) dirs_abs,
                                 replacement, &fh, ret_inserted ? &c : வெற்று);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (c) {
                r = chaseat_prefix_வேர்(c->result, வேர்_abs, &inserted);
                எனில் (r < 0)
                        திரும்பு log_full_errno(conf_files_log_level(flags), r,
                                              "Failed to prefix '%s' with root '%s': %m",
                                              c->result, empty_to_வேர்(வேர்_abs));
        }

        r = copy_and_sort_files_from_hashmap(fh, ".conf", empty_to_வேர்(வேர்_abs), flags, ret_files);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret_inserted)
                *ret_inserted = TAKE_PTR(inserted);
        திரும்பு 0;
}

முழு conf_files_list_dropins(
                எழுத்து ***ret,
                மாறிலி எழுத்து *dropin_dirname,
                மாறிலி எழுத்து *வேர்,
                முழு வேர்_fd,
                ConfFilesFlags flags,
                மாறிலி எழுத்து * மாறிலி *dirs) {

        _cleanup_strv_free_ எழுத்து **dropin_dirs = வெற்று;
        மாறிலி எழுத்து *suffix;
        முழு r;

        ப்ரதிஜ்ஞா(ret);
        ப்ரதிஜ்ஞா(dropin_dirname);
        ப்ரதிஜ்ஞா(dirs);

        suffix = strjoina("/", dropin_dirname);
        r = strv_extend_strv_concat(&dropin_dirs, dirs, suffix);
        எனில் (r < 0)
                திரும்பு log_oom_full(conf_files_log_level(flags));

        திரும்பு conf_files_list_strv_at(ret, ".conf", வேர்_fd, flags, (மாறிலி எழுத்து* மாறிலி*) dropin_dirs);
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
முழு conf_file_read(
                மாறிலி எழுத்து *வேர்,
                மாறிலி எழுத்து **config_dirs,
                மாறிலி எழுத்து *fn,
                parse_வரி_t parse_வரி,
                வெறுமை *userdata,
                bool ignore_enoent,
                bool *invalid_config) {

        _cleanup_fclose_ FILE *_f = வெற்று;
        _cleanup_free_ எழுத்து *_fn = வெற்று;
        unsigned v = 0;
        FILE *f;
        முழு r = 0;

        ப்ரதிஜ்ஞா(fn);

        எனில் (streq(fn, "-")) {
                f = stdin;
                fn = "<stdin>";

                log_debug("Reading config from stdin%s", glyph(GLYPH_ELLIPSIS));

        } இல்லையெனில் எனில் (is_path(fn)) {
                r = path_make_absolute_cwd(fn, &_fn);
                எனில் (r < 0)
                        திரும்பு log_error_errno(r, "Failed to make path absolute: %m");
                fn = _fn;

                f = _f = fopen(fn, "re");
                எனில் (!_f)
                        r = -errno;
                இல்லையெனில்
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));

        } இல்லையெனில் {
                r = தேடல்_and_fopen(fn, "re", வேர், config_dirs, &_f, &_fn);
                எனில் (r >= 0) {
                        f = _f;
                        fn = _fn;
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));
                }
        }

        எனில் (r == -ENOENT && ignore_enoent) {
                log_debug_errno(r, "Failed to open \"%s\", ignoring: %m", fn);
                திரும்பு 0; /* No error, but nothing happened. */
        }
        எனில் (r < 0)
                திரும்பு log_error_errno(r, "Failed to read '%s': %m", fn);

        r = 1;  /* We entered the part where we may modify state. */

        க்கு (;;) {
                _cleanup_free_ எழுத்து *வரி = வெற்று;
                bool invalid_வரி = false;
                முழு k;

                k = read_stripped_வரி(f, LONG_வரி_MAX, &வரி);
                எனில் (k < 0)
                        திரும்பு log_error_errno(k, "Failed to read '%s': %m", fn);
                எனில் (k == 0)
                        நிறுத்து;

                v++;

                எனில் (IN_SET(வரி[0], 0, '#'))
                        தொடர்;

                k = parse_வரி(fn, v, வரி, invalid_config ? &invalid_வரி : வெற்று, userdata);
                எனில் (k < 0 && invalid_வரி)
                        /* Allow reporting with a special code if the caller requested this. */
                        *invalid_config = true;
                இல்லையெனில்
                        /* The first error, if any, becomes our return value. */
                        RET_GATHER(r, k);
        }

        எனில் (ferror(f))
                RET_GATHER(r, log_error_errno(SYNTHETIC_ERRNO(EIO), "Failed to read from file %s.", fn));

        திரும்பு r;
}
