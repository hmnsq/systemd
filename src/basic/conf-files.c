/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <stdlib.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "chase.h"
#അന്തര്ഭാവ "conf-files.h"
#അന്തര്ഭാവ "dirent-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fileio.h"
#അന്തര്ഭാവ "glyph-util.h"
#അന്തര്ഭാവ "hashmap.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "nulstr-util.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "set.h"
#അന്തര്ഭാവ "stat-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"

ConfFile* conf_file_free(ConfFile *c) {
        എങ്കിൽ (!c)
                തിരികെ ശൂന്യ;

        സ്വതന്ത്രം(c->filename);
        സ്വതന്ത്രം(c->result);
        സ്വതന്ത്രം(c->original_path);
        സ്വതന്ത്രം(c->resolved_path);
        safe_close(c->fd);

        തിരികെ mfree(c);
}

DEFINE_POINTER_ARRAY_FREE_FUNC(ConfFile*, conf_file_free);

സ്ഥിതം പൂർണ്ണസംഖ്യ conf_files_log_level(ConfFilesFlags flags) {
        തിരികെ FLAGS_SET(flags, CONF_FILES_WARN) ? LOG_WARNING : LOG_DEBUG;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ prepare_dirs(
                സ്ഥിരം അക്ഷരം *വേര്,
                ConfFilesFlags flags,
                അക്ഷരം * സ്ഥിരം *dirs,
                അക്ഷരം **ret_വേര്,
                പൂർണ്ണസംഖ്യ *ret_rfd,
                അക്ഷരം ***ret_dirs) {

        _cleanup_free_ അക്ഷരം *വേര്_abs = ശൂന്യ;
        _cleanup_strv_free_ അക്ഷരം **dirs_abs = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret_വേര്);
        പ്രതിജ്ഞാ(ret_rfd);
        പ്രതിജ്ഞാ(ret_dirs || strv_isempty(dirs));

        പൂർണ്ണസംഖ്യ log_level = conf_files_log_level(flags);

        r = empty_or_വേര്_harder_to_null(&വേര്);
        എങ്കിൽ (r < 0)
                തിരികെ log_full_errno(log_level, r, "Failed to determine if '%s' points to the root directory: %m", strempty(വേര്));

        എങ്കിൽ (ret_dirs) {
                dirs_abs = strv_copy(dirs);
                എങ്കിൽ (!dirs_abs)
                        തിരികെ log_oom_full(log_level);
        }

        _cleanup_close_ പൂർണ്ണസംഖ്യ rfd = XAT_FDROOT;
        എങ്കിൽ (വേര്) {
                /* When a non-trivial root is specified, we will prefix the result later. Hence, it is not
                 * necessary to modify each config directories here. but needs to normalize the root directory. */
                r = path_make_absolute_cwd(വേര്, &വേര്_abs);
                എങ്കിൽ (r < 0)
                        തിരികെ log_full_errno(log_level, r, "Failed to make '%s' absolute: %m", വേര്);

                path_simplify(വേര്_abs);

                rfd = open(വേര്, O_CLOEXEC|O_DIRECTORY|O_PATH);
                എങ്കിൽ (rfd < 0)
                        തിരികെ log_full_errno(log_level, errno, "Failed to open '%s': %m", വേര്_abs);

        } അല്ലെങ്കിൽ എങ്കിൽ (ret_dirs) {
                /* When an empty root or "/" is specified, we will open "/" below, hence we need to make
                 * each config directory absolute if relative. */
                r = path_strv_make_absolute_cwd(dirs_abs);
                എങ്കിൽ (r < 0)
                        തിരികെ log_full_errno(log_level, r, "Failed to make directories absolute: %m");
        }

        *ret_വേര് = TAKE_PTR(വേര്_abs);
        *ret_rfd = TAKE_FD(rfd);
        എങ്കിൽ (ret_dirs)
                *ret_dirs = TAKE_PTR(dirs_abs);
        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ conf_file_prefix_വേര്(ConfFile *c, സ്ഥിരം അക്ഷരം *വേര്, ConfFilesFlags flags) {
        അക്ഷരം *p;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(c);

        എങ്കിൽ (FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_വേര്))
                തിരികെ 0;

        പൂർണ്ണസംഖ്യ log_level = conf_files_log_level(flags);

        r = chaseat_prefix_വേര്(c->result, വേര്, &p);
        എങ്കിൽ (r < 0)
                തിരികെ log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->result, വേര്);
        free_and_replace(c->result, p);

        r = chaseat_prefix_വേര്(c->resolved_path, വേര്, &p);
        എങ്കിൽ (r < 0)
                തിരികെ log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->resolved_path, വേര്);
        free_and_replace(c->resolved_path, p);

        /* Do not use chaseat_prefix_root(), as it is for the result of chaseat(), but the path is not chased. */
        p = path_join(empty_to_വേര്(വേര്), skip_leading_slash(c->original_path));
        എങ്കിൽ (!p)
                തിരികെ log_oom_full(log_level);

        path_simplify(p);
        തിരികെ free_and_replace(c->original_path, p);
}

സ്ഥിതം bool conf_files_need_stat(ConfFilesFlags flags) {
        തിരികെ (flags & (CONF_FILES_FILTER_MASKED | CONF_FILES_REGULAR | CONF_FILES_DIRECTORY | CONF_FILES_EXECUTABLE)) != 0;
}

സ്ഥിതം ChaseFlags conf_files_chase_flags(ConfFilesFlags flags) {
        ChaseFlags chase_flags = 0;

        എങ്കിൽ (!conf_files_need_stat(flags) || FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK))
                /* Even if no verification is requested, let's unconditionally call chaseat(),
                 * to drop unsafe symlinks. */
                chase_flags |= CHASE_NONEXISTENT;

        തിരികെ chase_flags;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ conf_file_chase_and_verify(
                സ്ഥിരം അക്ഷരം *വേര്,          /* for logging, can be NULL */
                പൂർണ്ണസംഖ്യ rfd,
                സ്ഥിരം അക്ഷരം *original_path, /* for logging */
                സ്ഥിരം അക്ഷരം *path,
                സ്ഥിരം അക്ഷരം *name,
                Set **masked,              /* optional */
                ConfFilesFlags flags,
                അക്ഷരം **ret_path,
                പൂർണ്ണസംഖ്യ *ret_fd,
                ഘടന stat *ret_stat) {

        _cleanup_free_ അക്ഷരം *resolved_path = ശൂന്യ;
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
        ഘടന stat st = {};
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(wildcard_fd_is_valid(rfd));
        പ്രതിജ്ഞാ(original_path);
        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(name);

        പൂർണ്ണസംഖ്യ log_level = conf_files_log_level(flags);

        വേര് = empty_to_വേര്(വേര്);

        r = chaseat(rfd, rfd, path, conf_files_chase_flags(flags), &resolved_path, &fd);
        എങ്കിൽ (r < 0)
                തിരികെ log_full_errno(log_level, r, "Failed to chase '%s%s': %m",
                                      വേര്, skip_leading_slash(original_path));
        എങ്കിൽ (r == 0) {
                എങ്കിൽ (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK)) {
                        /* If the path points to /dev/null in a image or so, then the device node may not exist. */
                        എങ്കിൽ (path_equal(skip_leading_slash(resolved_path), "dev/null")) {
                                എങ്കിൽ (masked) {
                                        /* Mark this one as masked */
                                        r = set_put_strdup(masked, name);
                                        എങ്കിൽ (r < 0)
                                                തിരികെ log_oom_full(log_level);
                                }

                                തിരികെ log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                                       വേര്, skip_leading_slash(original_path));
                        }
                }

                എങ്കിൽ (conf_files_need_stat(flags))
                        /* If we need to have stat, skip the entry. */
                        തിരികെ log_full_errno(log_level, SYNTHETIC_ERRNO(ENOENT), "Failed to chase '%s%s': %m",
                                              വേര്, skip_leading_slash(original_path));
        }

        /* Even if we do not need stat, let's take stat now. The caller may use the info later. */
        എങ്കിൽ (fd >= 0 && fstat(fd, &st) < 0)
                തിരികെ log_full_errno(log_level, errno, "Failed to stat '%s%s': %m",
                                      വേര്, skip_leading_slash(original_path));

        /* Is this a masking entry? */
        എങ്കിൽ (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK) && stat_may_be_dev_null(&st)) {
                എങ്കിൽ (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        എങ്കിൽ (r < 0)
                                തിരികെ log_oom_full(log_level);
                }

                തിരികെ log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                       വേര്, skip_leading_slash(original_path));
        }

        എങ്കിൽ (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_EMPTY) && stat_is_empty(&st)) {
                എങ്കിൽ (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        എങ്കിൽ (r < 0)
                                തിരികെ log_oom_full(log_level);
                }

                തിരികെ log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (an empty file).",
                                       വേര്, skip_leading_slash(original_path));
        }

        എങ്കിൽ (FLAGS_SET(flags, CONF_FILES_REGULAR|CONF_FILES_DIRECTORY)) {
                എങ്കിൽ (!S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode))
                        തിരികെ log_debug_errno(SYNTHETIC_ERRNO(EBADFD),
                                               "File '%s%s' is neither a regular file or directory.",
                                               വേര്, skip_leading_slash(original_path));
        } അല്ലെങ്കിൽ {
                /* Is this node a regular file? */
                എങ്കിൽ (FLAGS_SET(flags, CONF_FILES_REGULAR)) {
                        r = stat_verify_regular(&st);
                        എങ്കിൽ (r < 0)
                                തിരികെ log_debug_errno(r, "File '%s%s' is not a regular file: %m",
                                                       വേര്, skip_leading_slash(original_path));
                }

                /* Is this node a directory? */
                എങ്കിൽ (FLAGS_SET(flags, CONF_FILES_DIRECTORY)) {
                        r = stat_verify_directory(&st);
                        എങ്കിൽ (r < 0)
                                തിരികെ log_debug_errno(r, "File '%s%s' is not a directory: %m",
                                                       വേര്, skip_leading_slash(original_path));
                }
        }

        /* Does this node have the executable bit set?
         * As requested: check if the file is marked executable. Note that we don't check access(X_OK) here,
         * as we care about whether the file is marked executable at all, and not whether it is executable
         * for us, because if so, such errors are stuff we should log about. */
        എങ്കിൽ (FLAGS_SET(flags, CONF_FILES_EXECUTABLE) && (st.st_mode & 0111) == 0)
                തിരികെ log_debug_errno(SYNTHETIC_ERRNO(ENOEXEC),
                                       "File '%s%s' is not marked executable.",
                                       വേര്, skip_leading_slash(original_path));

        എങ്കിൽ (ret_path)
                *ret_path = TAKE_PTR(resolved_path);
        എങ്കിൽ (ret_fd)
                *ret_fd = TAKE_FD(fd);
        എങ്കിൽ (ret_stat)
                *ret_stat = st;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ conf_file_new_at(
                സ്ഥിരം അക്ഷരം *path,
                സ്ഥിരം അക്ഷരം *വേര്,
                പൂർണ്ണസംഖ്യ rfd,
                ConfFilesFlags flags,
                ConfFile **ret) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(wildcard_fd_is_valid(rfd));
        പ്രതിജ്ഞാ(ret);

        പൂർണ്ണസംഖ്യ log_level = conf_files_log_level(flags);

        _cleanup_free_ അക്ഷരം *വേര്_2 = ശൂന്യ;
        എങ്കിൽ (DEBUG_LOGGING && !വേര്) {
                (ശൂന്യം) fd_get_path(rfd, &വേര്_2);
                വേര് = വേര്_2;
        }

        _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
        എങ്കിൽ (!c)
                തിരികെ log_oom_full(log_level);

        *c = (ConfFile) {
                .original_path = strdup(path),
                .fd = -EBADF,
        };

        എങ്കിൽ (!c->original_path)
                തിരികെ log_oom_full(log_level);

        r = path_extract_filename(path, &c->filename);
        എങ്കിൽ (r < 0)
                തിരികെ log_full_errno(log_level, r, "Failed to extract filename from '%s': %m", path);

        _cleanup_free_ അക്ഷരം *dirpath = ശൂന്യ, *resolved_dirpath = ശൂന്യ;
        r = path_extract_directory(path, &dirpath);
        എങ്കിൽ (r < 0 && r != -EDESTADDRREQ)
                തിരികെ log_full_errno(log_level, r, "Failed to extract directory from '%s': %m", path);
        എങ്കിൽ (r >= 0) {
                r = chaseat(rfd, rfd, dirpath,
                            CHASE_MUST_BE_DIRECTORY | conf_files_chase_flags(flags),
                            &resolved_dirpath, /* ret_fd= */ ശൂന്യ);
                എങ്കിൽ (r < 0)
                        തിരികെ log_full_errno(log_level, r, "Failed to chase '%s%s': %m", empty_to_വേര്(വേര്), skip_leading_slash(dirpath));
        }

        c->result = path_join(resolved_dirpath, c->filename);
        എങ്കിൽ (!c->result)
                തിരികെ log_oom_full(log_level);

        r = conf_file_chase_and_verify(
                        വേര്,
                        rfd,
                        c->original_path,
                        c->result,
                        c->filename,
                        /* masked= */ ശൂന്യ,
                        flags,
                        &c->resolved_path,
                        &c->fd,
                        &c->st);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        *ret = TAKE_PTR(c);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ conf_file_new(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *വേര്, ConfFilesFlags flags, ConfFile **ret) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(ret);

        _cleanup_free_ അക്ഷരം *വേര്_abs = ശൂന്യ;
        _cleanup_close_ പൂർണ്ണസംഖ്യ rfd = -EBADF;
        r = prepare_dirs(വേര്, flags, /* dirs= */ ശൂന്യ, &വേര്_abs, &rfd, /* ret_dirs= */ ശൂന്യ);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        _cleanup_free_ അക്ഷരം *path_abs = ശൂന്യ;
        എങ്കിൽ (!വേര്_abs) {
                r = path_make_absolute_cwd(path, &path_abs);
                എങ്കിൽ (r < 0)
                        തിരികെ log_full_errno(conf_files_log_level(flags), r, "Failed to make '%s' absolute: %m", path);

                path = path_abs;
        }

        _cleanup_(conf_file_freep) ConfFile *c = ശൂന്യ;
        r = conf_file_new_at(path, വേര്_abs, rfd, flags, &c);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = conf_file_prefix_വേര്(c, വേര്_abs, flags);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        *ret = TAKE_PTR(c);
        തിരികെ 0;
}

DEFINE_PRIVATE_HASH_OPS_WITH_മൂല്യം_DESTRUCTOR(
                conf_file_hash_ops,
                അക്ഷരം, string_hash_func, string_compare_func,
                ConfFile, conf_file_free);

സ്ഥിതം പൂർണ്ണസംഖ്യ files_add(
                DIR *dir,
                സ്ഥിരം അക്ഷരം *original_dirpath,
                സ്ഥിരം അക്ഷരം *resolved_dirpath,
                സ്ഥിരം അക്ഷരം *വേര്, /* for logging, can be NULL */
                പൂർണ്ണസംഖ്യ rfd,
                Hashmap **files,
                Set **masked,
                സ്ഥിരം അക്ഷരം *suffix,
                ConfFilesFlags flags) {

        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(dir);
        പ്രതിജ്ഞാ(original_dirpath);
        പ്രതിജ്ഞാ(resolved_dirpath);
        പ്രതിജ്ഞാ(wildcard_fd_is_valid(rfd));
        പ്രതിജ്ഞാ(files);
        പ്രതിജ്ഞാ(masked);

        പൂർണ്ണസംഖ്യ log_level = conf_files_log_level(flags);

        വേര് = empty_to_വേര്(വേര്);

        FOREACH_DIRENT(de, dir, തിരികെ log_full_errno(log_level, errno, "Failed to read directory '%s%s': %m",
                                                      വേര്, skip_leading_slash(original_dirpath))) {

                _cleanup_free_ അക്ഷരം *original_path = path_join(original_dirpath, de->d_name);
                എങ്കിൽ (!original_path)
                        തിരികെ log_oom_full(log_level);

                /* Does this match the suffix? */
                എങ്കിൽ (suffix && !endswith(de->d_name, suffix)) {
                        log_debug("Skipping file '%s%s', suffix is not '%s'.", വേര്, skip_leading_slash(original_path), suffix);
                        തുടരുക;
                }

                /* Has this file already been found in an earlier directory? */
                എങ്കിൽ (hashmap_contains(*files, de->d_name)) {
                        log_debug("Skipping overridden file '%s%s'.", വേര്, skip_leading_slash(original_path));
                        തുടരുക;
                }

                /* Has this been masked in an earlier directory? */
                എങ്കിൽ ((flags & CONF_FILES_FILTER_MASKED) != 0 && set_contains(*masked, de->d_name)) {
                        log_debug("File '%s%s' is masked by previous entry.", വേര്, skip_leading_slash(original_path));
                        തുടരുക;
                }

                _cleanup_free_ അക്ഷരം *p = path_join(resolved_dirpath, de->d_name);
                എങ്കിൽ (!p)
                        തിരികെ log_oom_full(log_level);

                _cleanup_free_ അക്ഷരം *resolved_path = ശൂന്യ;
                _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
                ഘടന stat st;
                r = conf_file_chase_and_verify(
                                വേര്,
                                rfd,
                                original_path,
                                p,
                                de->d_name,
                                masked,
                                flags,
                                &resolved_path,
                                &fd,
                                &st);
                എങ്കിൽ (r == -ENOMEM)
                        തിരികെ r;
                എങ്കിൽ (r < 0)
                        തുടരുക;

                _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
                എങ്കിൽ (!c)
                        തിരികെ log_oom_full(log_level);

                *c = (ConfFile) {
                        .filename = strdup(de->d_name),
                        .result = TAKE_PTR(p),
                        .original_path = TAKE_PTR(original_path),
                        .resolved_path = TAKE_PTR(resolved_path),
                        .fd = TAKE_FD(fd),
                        .st = st,
                };

                എങ്കിൽ (!c->filename)
                        തിരികെ log_oom_full(log_level);

                r = hashmap_ensure_put(files, &conf_file_hash_ops, c->filename, c);
                എങ്കിൽ (r < 0) {
                        പ്രതിജ്ഞാ(r == -ENOMEM);
                        തിരികെ log_oom_full(log_level);
                }
                പ്രതിജ്ഞാ(r > 0);

                TAKE_PTR(c);
        }

        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ dump_files(Hashmap *fh, സ്ഥിരം അക്ഷരം *വേര്, ConfFilesFlags flags, ConfFile ***ret_files, size_t *ret_n_files) {
        ConfFile **files = ശൂന്യ;
        size_t n_files = 0;
        പൂർണ്ണസംഖ്യ r;

        CLEANUP_ARRAY(files, n_files, conf_file_free_array);

        പ്രതിജ്ഞാ(ret_files);
        പ്രതിജ്ഞാ(ret_n_files);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap. */
        r = hashmap_dump_sorted(fh, (ശൂന്യം***) &files, &n_files);
        എങ്കിൽ (r < 0)
                തിരികെ log_oom_full(conf_files_log_level(flags));

        /* Hence, we need to remove them from the hashmap. */
        FOREACH_ARRAY(i, files, n_files)
                assert_se(hashmap_remove(fh, (*i)->filename) == *i);

        എങ്കിൽ (വേര്)
                FOREACH_ARRAY(i, files, n_files) {
                        r = conf_file_prefix_വേര്(*i, വേര്, flags);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                }

        *ret_files = TAKE_PTR(files);
        *ret_n_files = n_files;
        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ copy_and_sort_files_from_hashmap(
                Hashmap *fh,
                സ്ഥിരം അക്ഷരം *suffix,
                സ്ഥിരം അക്ഷരം *വേര്,
                ConfFilesFlags flags,
                അക്ഷരം ***ret) {

        _cleanup_strv_free_ അക്ഷരം **results = ശൂന്യ;
        _cleanup_free_ ConfFile **files = ശൂന്യ;
        size_t n_files = 0, n_results = 0;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        പൂർണ്ണസംഖ്യ log_level = conf_files_log_level(flags);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap.
         * Hence, do not use conf_file_free_array() for 'entries' */
        r = hashmap_dump_sorted(fh, (ശൂന്യം***) &files, &n_files);
        എങ്കിൽ (r < 0)
                തിരികെ log_oom_full(log_level);

        FOREACH_ARRAY(i, files, n_files) {
                ConfFile *c = *i;
                സ്ഥിരം അക്ഷരം *add = ശൂന്യ;

                എങ്കിൽ (FLAGS_SET(flags, CONF_FILES_BASENAME))
                        add = c->filename;
                അല്ലെങ്കിൽ എങ്കിൽ (വേര് && !FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_വേര്)) {
                        _cleanup_free_ അക്ഷരം *p = ശൂന്യ;

                        r = chaseat_prefix_വേര്(c->result, വേര്, &p);
                        എങ്കിൽ (r < 0)
                                തിരികെ log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->result, വേര്);

                        എങ്കിൽ (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX) && suffix) {
                                അക്ഷരം *e = endswith(p, suffix);
                                എങ്കിൽ (!e)
                                        തുടരുക;

                                *e = 0;
                        }

                        എങ്കിൽ (strv_consume_with_size(&results, &n_results, TAKE_PTR(p)) < 0)
                                തിരികെ log_oom_full(log_level);

                        തുടരുക;
                } അല്ലെങ്കിൽ
                        add = c->result;

                എങ്കിൽ (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX)) {
                        സ്ഥിരം അക്ഷരം *e = endswith(add, suffix);
                        എങ്കിൽ (!e)
                                തുടരുക;

                        _cleanup_free_ അക്ഷരം *n = strndup(add, e - add);
                        എങ്കിൽ (!n)
                                തിരികെ log_oom_full(log_level);

                        r = strv_consume_with_size(&results, &n_results, TAKE_PTR(n));
                } അല്ലെങ്കിൽ
                        r = strv_extend_with_size(&results, &n_results, add);
                എങ്കിൽ (r < 0)
                        തിരികെ log_oom_full(log_level);
        }

        *ret = TAKE_PTR(results);
        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ തിരുകുക_replacement(Hashmap **fh, ConfFile *replacement, ConfFilesFlags flags, സ്ഥിരം ConfFile **ret) {
        _cleanup_(conf_file_freep) ConfFile *c = ASSERT_PTR(replacement);
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fh);
        പ്രതിജ്ഞാ(ret);

        /* This consumes the input ConfFile. */

        ConfFile *existing = hashmap_get(*fh, c->filename);
        എങ്കിൽ (existing) {
                log_debug("An entry with higher priority '%s' -> '%s' already exists, ignoring the replacement: %s",
                          existing->filename, existing->result, c->original_path);
                *ret = ശൂന്യ;
                തിരികെ 0;
        }

        r = hashmap_ensure_put(fh, &conf_file_hash_ops, c->filename, c);
        എങ്കിൽ (r < 0) {
                പ്രതിജ്ഞാ(r == -ENOMEM);
                തിരികെ log_oom_full(conf_files_log_level(flags));
        }
        പ്രതിജ്ഞാ(r > 0);

        log_debug("Inserted replacement: '%s' -> '%s'", c->filename, c->result);

        *ret = TAKE_PTR(c);
        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ conf_files_list_impl(
                സ്ഥിരം അക്ഷരം *suffix,
                സ്ഥിരം അക്ഷരം *വേര്, /* for logging, can be NULL */
                പൂർണ്ണസംഖ്യ rfd,
                ConfFilesFlags flags,
                സ്ഥിരം അക്ഷരം * സ്ഥിരം *dirs,
                സ്ഥിരം അക്ഷരം *replacement,
                Hashmap **ret,
                സ്ഥിരം ConfFile **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = ശൂന്യ;
        _cleanup_set_free_ Set *masked = ശൂന്യ;
        _cleanup_(conf_file_freep) ConfFile *c = ശൂന്യ;
        സ്ഥിരം ConfFile *inserted = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(wildcard_fd_is_valid(rfd));
        പ്രതിജ്ഞാ(ret);

        വേര് = empty_to_വേര്(വേര്);

        എങ്കിൽ (replacement) {
                r = conf_file_new_at(replacement, വേര്, rfd, flags & CONF_FILES_WARN, &c);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        STRV_FOREACH(p, dirs) {
                _cleanup_closedir_ DIR *dir = ശൂന്യ;
                _cleanup_free_ അക്ഷരം *path = ശൂന്യ;

                r = chase_and_opendirat(rfd, rfd, *p, 0, &path, &dir);
                എങ്കിൽ (r < 0) {
                        എങ്കിൽ (r != -ENOENT)
                                log_full_errno(conf_files_log_level(flags), r,
                                               "Failed to chase and open directory '%s%s', ignoring: %m",
                                               വേര്, skip_leading_slash(*p));
                        തുടരുക;
                }

                എങ്കിൽ (c && streq_ptr(path_startswith(c->result, path), c->filename)) {
                        r = തിരുകുക_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                }

                r = files_add(dir, *p, path, വേര്, rfd, &fh, &masked, suffix, flags);
                എങ്കിൽ (r == -ENOMEM)
                        തിരികെ r;
        }

        എങ്കിൽ (c) {
                r = തിരുകുക_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        *ret = TAKE_PTR(fh);
        എങ്കിൽ (ret_inserted)
                *ret_inserted = inserted;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ conf_files_list_strv(
                അക്ഷരം ***ret,
                സ്ഥിരം അക്ഷരം *suffix,
                സ്ഥിരം അക്ഷരം *വേര്,
                ConfFilesFlags flags,
                സ്ഥിരം അക്ഷരം * സ്ഥിരം *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = ശൂന്യ;
        _cleanup_close_ പൂർണ്ണസംഖ്യ rfd = -EBADF;
        _cleanup_free_ അക്ഷരം *വേര്_abs = ശൂന്യ;
        _cleanup_strv_free_ അക്ഷരം **dirs_abs = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        r = prepare_dirs(വേര്, flags, (അക്ഷരം**) dirs, &വേര്_abs, &rfd, &dirs_abs);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = conf_files_list_impl(suffix, വേര്_abs, rfd, flags, (സ്ഥിരം അക്ഷരം * സ്ഥിരം *) dirs_abs,
                                 /* replacement= */ ശൂന്യ, &fh, /* ret_inserted= */ ശൂന്യ);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ copy_and_sort_files_from_hashmap(fh, suffix, empty_to_വേര്(വേര്_abs), flags, ret);
}

പൂർണ്ണസംഖ്യ conf_files_list_strv_full(
                സ്ഥിരം അക്ഷരം *suffix,
                സ്ഥിരം അക്ഷരം *വേര്,
                ConfFilesFlags flags,
                സ്ഥിരം അക്ഷരം * സ്ഥിരം *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = ശൂന്യ;
        _cleanup_close_ പൂർണ്ണസംഖ്യ rfd = -EBADF;
        _cleanup_free_ അക്ഷരം *വേര്_abs = ശൂന്യ;
        _cleanup_strv_free_ അക്ഷരം **dirs_abs = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret_files);
        പ്രതിജ്ഞാ(ret_n_files);

        r = prepare_dirs(വേര്, flags, (അക്ഷരം**) dirs, &വേര്_abs, &rfd, &dirs_abs);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = conf_files_list_impl(suffix, വേര്_abs, rfd, flags, (സ്ഥിരം അക്ഷരം * സ്ഥിരം *) dirs_abs,
                                 /* replacement= */ ശൂന്യ, &fh, /* ret_inserted= */ ശൂന്യ);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ dump_files(fh, empty_to_വേര്(വേര്_abs), flags, ret_files, ret_n_files);
}

പൂർണ്ണസംഖ്യ conf_files_list_strv_at(
                അക്ഷരം ***ret,
                സ്ഥിരം അക്ഷരം *suffix,
                പൂർണ്ണസംഖ്യ rfd,
                ConfFilesFlags flags,
                സ്ഥിരം അക്ഷരം * സ്ഥിരം *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = ശൂന്യ;
        _cleanup_free_ അക്ഷരം *വേര് = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(wildcard_fd_is_valid(rfd));
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (DEBUG_LOGGING)
                (ശൂന്യം) fd_get_path(rfd, &വേര്); /* for logging */

        r = conf_files_list_impl(suffix, വേര്, rfd, flags, dirs, /* replacement= */ ശൂന്യ, &fh, /* ret_inserted= */ ശൂന്യ);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ copy_and_sort_files_from_hashmap(fh, suffix, /* root= */ ശൂന്യ, flags, ret);
}

പൂർണ്ണസംഖ്യ conf_files_list_strv_at_full(
                സ്ഥിരം അക്ഷരം *suffix,
                പൂർണ്ണസംഖ്യ rfd,
                ConfFilesFlags flags,
                സ്ഥിരം അക്ഷരം * സ്ഥിരം *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = ശൂന്യ;
        _cleanup_free_ അക്ഷരം *വേര് = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(wildcard_fd_is_valid(rfd));
        പ്രതിജ്ഞാ(ret_files);
        പ്രതിജ്ഞാ(ret_n_files);

        എങ്കിൽ (DEBUG_LOGGING)
                (ശൂന്യം) fd_get_path(rfd, &വേര്); /* for logging */

        r = conf_files_list_impl(suffix, വേര്, rfd, flags, dirs, /* replacement= */ ശൂന്യ, &fh, /* ret_inserted= */ ശൂന്യ);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ dump_files(fh, /* root= */ ശൂന്യ, flags, ret_files, ret_n_files);
}

പൂർണ്ണസംഖ്യ conf_files_list(അക്ഷരം ***ret, സ്ഥിരം അക്ഷരം *suffix, സ്ഥിരം അക്ഷരം *വേര്, ConfFilesFlags flags, സ്ഥിരം അക്ഷരം *dir) {
        തിരികെ conf_files_list_strv(ret, suffix, വേര്, flags, STRV_MAKE_CONST(dir));
}

പൂർണ്ണസംഖ്യ conf_files_list_full(സ്ഥിരം അക്ഷരം *suffix, സ്ഥിരം അക്ഷരം *വേര്, ConfFilesFlags flags, സ്ഥിരം അക്ഷരം *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        തിരികെ conf_files_list_strv_full(suffix, വേര്, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

പൂർണ്ണസംഖ്യ conf_files_list_at(അക്ഷരം ***ret, സ്ഥിരം അക്ഷരം *suffix, പൂർണ്ണസംഖ്യ rfd, ConfFilesFlags flags, സ്ഥിരം അക്ഷരം *dir) {
        തിരികെ conf_files_list_strv_at(ret, suffix, rfd, flags, STRV_MAKE_CONST(dir));
}

പൂർണ്ണസംഖ്യ conf_files_list_at_full(സ്ഥിരം അക്ഷരം *suffix, പൂർണ്ണസംഖ്യ rfd, ConfFilesFlags flags, സ്ഥിരം അക്ഷരം *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        തിരികെ conf_files_list_strv_at_full(suffix, rfd, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

പൂർണ്ണസംഖ്യ conf_files_list_nulstr(അക്ഷരം ***ret, സ്ഥിരം അക്ഷരം *suffix, സ്ഥിരം അക്ഷരം *വേര്, ConfFilesFlags flags, സ്ഥിരം അക്ഷരം *dirs) {
        _cleanup_strv_free_ അക്ഷരം **d = ശൂന്യ;

        പ്രതിജ്ഞാ(ret);

        d = strv_split_nulstr(dirs);
        എങ്കിൽ (!d)
                തിരികെ log_oom_full(conf_files_log_level(flags));

        തിരികെ conf_files_list_strv(ret, suffix, വേര്, flags, (സ്ഥിരം അക്ഷരം**) d);
}

പൂർണ്ണസംഖ്യ conf_files_list_nulstr_full(സ്ഥിരം അക്ഷരം *suffix, സ്ഥിരം അക്ഷരം *വേര്, ConfFilesFlags flags, സ്ഥിരം അക്ഷരം *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ അക്ഷരം **d = ശൂന്യ;

        പ്രതിജ്ഞാ(ret_files);
        പ്രതിജ്ഞാ(ret_n_files);

        d = strv_split_nulstr(dirs);
        എങ്കിൽ (!d)
                തിരികെ log_oom_full(conf_files_log_level(flags));

        തിരികെ conf_files_list_strv_full(suffix, വേര്, flags, (സ്ഥിരം അക്ഷരം**) d, ret_files, ret_n_files);
}

പൂർണ്ണസംഖ്യ conf_files_list_nulstr_at(അക്ഷരം ***ret, സ്ഥിരം അക്ഷരം *suffix, പൂർണ്ണസംഖ്യ rfd, ConfFilesFlags flags, സ്ഥിരം അക്ഷരം *dirs) {
        _cleanup_strv_free_ അക്ഷരം **d = ശൂന്യ;

        പ്രതിജ്ഞാ(ret);

        d = strv_split_nulstr(dirs);
        എങ്കിൽ (!d)
                തിരികെ log_oom_full(conf_files_log_level(flags));

        തിരികെ conf_files_list_strv_at(ret, suffix, rfd, flags, (സ്ഥിരം അക്ഷരം**) d);
}

പൂർണ്ണസംഖ്യ conf_files_list_nulstr_at_full(സ്ഥിരം അക്ഷരം *suffix, പൂർണ്ണസംഖ്യ rfd, ConfFilesFlags flags, സ്ഥിരം അക്ഷരം *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ അക്ഷരം **d = ശൂന്യ;

        പ്രതിജ്ഞാ(ret_files);
        പ്രതിജ്ഞാ(ret_n_files);

        d = strv_split_nulstr(dirs);
        എങ്കിൽ (!d)
                തിരികെ log_oom_full(conf_files_log_level(flags));

        തിരികെ conf_files_list_strv_at_full(suffix, rfd, flags, (സ്ഥിരം അക്ഷരം**) d, ret_files, ret_n_files);
}

പൂർണ്ണസംഖ്യ conf_files_list_with_replacement(
                സ്ഥിരം അക്ഷരം *വേര്,
                അക്ഷരം **config_dirs,
                സ്ഥിരം അക്ഷരം *replacement,
                അക്ഷരം ***ret_files,
                അക്ഷരം **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = ശൂന്യ;
        _cleanup_free_ അക്ഷരം *inserted = ശൂന്യ;
        ConfFilesFlags flags = CONF_FILES_REGULAR | CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_WARN;
        _cleanup_close_ പൂർണ്ണസംഖ്യ rfd = -EBADF;
        _cleanup_free_ അക്ഷരം *വേര്_abs = ശൂന്യ;
        _cleanup_strv_free_ അക്ഷരം **dirs_abs = ശൂന്യ;
        സ്ഥിരം ConfFile *c = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret_files);

        r = prepare_dirs(വേര്, flags, config_dirs, &വേര്_abs, &rfd, &dirs_abs);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = conf_files_list_impl(".conf", വേര്_abs, rfd, flags, (സ്ഥിരം അക്ഷരം * സ്ഥിരം *) dirs_abs,
                                 replacement, &fh, ret_inserted ? &c : ശൂന്യ);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (c) {
                r = chaseat_prefix_വേര്(c->result, വേര്_abs, &inserted);
                എങ്കിൽ (r < 0)
                        തിരികെ log_full_errno(conf_files_log_level(flags), r,
                                              "Failed to prefix '%s' with root '%s': %m",
                                              c->result, empty_to_വേര്(വേര്_abs));
        }

        r = copy_and_sort_files_from_hashmap(fh, ".conf", empty_to_വേര്(വേര്_abs), flags, ret_files);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret_inserted)
                *ret_inserted = TAKE_PTR(inserted);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ conf_files_list_dropins(
                അക്ഷരം ***ret,
                സ്ഥിരം അക്ഷരം *dropin_dirname,
                സ്ഥിരം അക്ഷരം *വേര്,
                പൂർണ്ണസംഖ്യ വേര്_fd,
                ConfFilesFlags flags,
                സ്ഥിരം അക്ഷരം * സ്ഥിരം *dirs) {

        _cleanup_strv_free_ അക്ഷരം **dropin_dirs = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *suffix;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);
        പ്രതിജ്ഞാ(dropin_dirname);
        പ്രതിജ്ഞാ(dirs);

        suffix = strjoina("/", dropin_dirname);
        r = strv_extend_strv_concat(&dropin_dirs, dirs, suffix);
        എങ്കിൽ (r < 0)
                തിരികെ log_oom_full(conf_files_log_level(flags));

        തിരികെ conf_files_list_strv_at(ret, ".conf", വേര്_fd, flags, (സ്ഥിരം അക്ഷരം* സ്ഥിരം*) dropin_dirs);
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
പൂർണ്ണസംഖ്യ conf_file_read(
                സ്ഥിരം അക്ഷരം *വേര്,
                സ്ഥിരം അക്ഷരം **config_dirs,
                സ്ഥിരം അക്ഷരം *fn,
                parse_വരി_t parse_വരി,
                ശൂന്യം *userdata,
                bool ignore_enoent,
                bool *invalid_config) {

        _cleanup_fclose_ FILE *_f = ശൂന്യ;
        _cleanup_free_ അക്ഷരം *_fn = ശൂന്യ;
        unsigned v = 0;
        FILE *f;
        പൂർണ്ണസംഖ്യ r = 0;

        പ്രതിജ്ഞാ(fn);

        എങ്കിൽ (streq(fn, "-")) {
                f = stdin;
                fn = "<stdin>";

                log_debug("Reading config from stdin%s", glyph(GLYPH_ELLIPSIS));

        } അല്ലെങ്കിൽ എങ്കിൽ (is_path(fn)) {
                r = path_make_absolute_cwd(fn, &_fn);
                എങ്കിൽ (r < 0)
                        തിരികെ log_error_errno(r, "Failed to make path absolute: %m");
                fn = _fn;

                f = _f = fopen(fn, "re");
                എങ്കിൽ (!_f)
                        r = -errno;
                അല്ലെങ്കിൽ
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));

        } അല്ലെങ്കിൽ {
                r = തിരയൽ_and_fopen(fn, "re", വേര്, config_dirs, &_f, &_fn);
                എങ്കിൽ (r >= 0) {
                        f = _f;
                        fn = _fn;
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));
                }
        }

        എങ്കിൽ (r == -ENOENT && ignore_enoent) {
                log_debug_errno(r, "Failed to open \"%s\", ignoring: %m", fn);
                തിരികെ 0; /* No error, but nothing happened. */
        }
        എങ്കിൽ (r < 0)
                തിരികെ log_error_errno(r, "Failed to read '%s': %m", fn);

        r = 1;  /* We entered the part where we may modify state. */

        വേണ്ടി (;;) {
                _cleanup_free_ അക്ഷരം *വരി = ശൂന്യ;
                bool invalid_വരി = false;
                പൂർണ്ണസംഖ്യ k;

                k = read_stripped_വരി(f, LONG_വരി_MAX, &വരി);
                എങ്കിൽ (k < 0)
                        തിരികെ log_error_errno(k, "Failed to read '%s': %m", fn);
                എങ്കിൽ (k == 0)
                        നിർത്തുക;

                v++;

                എങ്കിൽ (IN_SET(വരി[0], 0, '#'))
                        തുടരുക;

                k = parse_വരി(fn, v, വരി, invalid_config ? &invalid_വരി : ശൂന്യ, userdata);
                എങ്കിൽ (k < 0 && invalid_വരി)
                        /* Allow reporting with a special code if the caller requested this. */
                        *invalid_config = true;
                അല്ലെങ്കിൽ
                        /* The first error, if any, becomes our return value. */
                        RET_GATHER(r, k);
        }

        എങ്കിൽ (ferror(f))
                RET_GATHER(r, log_error_errno(SYNTHETIC_ERRNO(EIO), "Failed to read from file %s.", fn));

        തിരികെ r;
}
