/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <sys/stat.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "env-file.h"
#அந்தர்பாவ "env-util.h"
#அந்தர்பாவ "escape.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fileio.h"
#அந்தர்பாவ "fs-util.h"
#அந்தர்பாவ "label-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"
#அந்தர்பாவ "tmpfile-util.h"
#அந்தர்பாவ "utf8.h"

வகைவரை முழு (*தள்ளு_env_func_t)(
                மாறிலி எழுத்து *filename,
                unsigned வரி,
                மாறிலி எழுத்து *சாவி,
                எழுத்து *மதிப்பு,
                வெறுமை *userdata);

நிலையான முழு parse_env_file_internal(
                FILE *f,
                மாறிலி எழுத்து *fname,
                தள்ளு_env_func_t தள்ளு,
                வெறுமை *userdata) {

        size_t n_சாவி = 0, n_மதிப்பு = 0, last_மதிப்பு_whitespace = SIZE_MAX, last_சாவி_whitespace = SIZE_MAX;
        _cleanup_free_ எழுத்து *contents = வெற்று, *சாவி = வெற்று, *மதிப்பு = வெற்று;
        unsigned வரி = 1;
        முழு r;

        எண்ணிக்கை {
                PRE_சாவி,
                சாவி_2,
                PRE_மதிப்பு,
                மதிப்பு_2,
                மதிப்பு_ESCAPE,
                SINGLE_QUOTE_மதிப்பு,
                DOUBLE_QUOTE_மதிப்பு,
                DOUBLE_QUOTE_மதிப்பு_ESCAPE,
                COMMENT,
                COMMENT_ESCAPE
        } state = PRE_சாவி;

        ப்ரதிஜ்ஞா(f || fname);
        ப்ரதிஜ்ஞா(தள்ளு);

        எனில் (f)
                r = read_full_stream(f, &contents, வெற்று);
        இல்லையெனில்
                r = read_full_file(fname, &contents, வெற்று);
        எனில் (r < 0)
                திரும்பு r;

        க்கு (எழுத்து *p = contents; *p; p++) {
                எழுத்து c = *p;

                தேர்வு (state) {

                நிலை PRE_சாவி:
                        எனில் (strchr(COMMENTS, c))
                                state = COMMENT;
                        இல்லையெனில் எனில் (!strchr(WHITESPACE, c)) {
                                state = சாவி_2;
                                last_சாவி_whitespace = SIZE_MAX;

                                எனில் (!GREEDY_REALLOC(சாவி, n_சாவி+2))
                                        திரும்பு -ENOMEM;

                                சாவி[n_சாவி++] = c;
                        }
                        நிறுத்து;

                நிலை சாவி_2:
                        எனில் (strchr(NEWLINE, c)) {
                                state = PRE_சாவி;
                                வரி++;
                                n_சாவி = 0;
                        } இல்லையெனில் எனில் (c == '=') {
                                state = PRE_மதிப்பு;
                                last_மதிப்பு_whitespace = SIZE_MAX;
                        } இல்லையெனில் {
                                எனில் (!strchr(WHITESPACE, c))
                                        last_சாவி_whitespace = SIZE_MAX;
                                இல்லையெனில் எனில் (last_சாவி_whitespace == SIZE_MAX)
                                         last_சாவி_whitespace = n_சாவி;

                                எனில் (!GREEDY_REALLOC(சாவி, n_சாவி+2))
                                        திரும்பு -ENOMEM;

                                சாவி[n_சாவி++] = c;
                        }

                        நிறுத்து;

                நிலை PRE_மதிப்பு:
                        எனில் (strchr(NEWLINE, c)) {
                                state = PRE_சாவி;
                                வரி++;
                                சாவி[n_சாவி] = 0;

                                எனில் (மதிப்பு)
                                        மதிப்பு[n_மதிப்பு] = 0;

                                /* strip trailing whitespace from key */
                                எனில் (last_சாவி_whitespace != SIZE_MAX)
                                        சாவி[last_சாவி_whitespace] = 0;

                                r = தள்ளு(fname, வரி, சாவி, மதிப்பு, userdata);
                                எனில் (r < 0)
                                        திரும்பு r;

                                n_சாவி = 0;
                                மதிப்பு = வெற்று;
                                n_மதிப்பு = 0;

                        } இல்லையெனில் எனில் (c == '\'')
                                state = SINGLE_QUOTE_மதிப்பு;
                        இல்லையெனில் எனில் (c == '"')
                                state = DOUBLE_QUOTE_மதிப்பு;
                        இல்லையெனில் எனில் (c == '\\')
                                state = மதிப்பு_ESCAPE;
                        இல்லையெனில் எனில் (!strchr(WHITESPACE, c)) {
                                state = மதிப்பு_2;

                                எனில் (!GREEDY_REALLOC(மதிப்பு, n_மதிப்பு+2))
                                        திரும்பு -ENOMEM;

                                மதிப்பு[n_மதிப்பு++] = c;
                        }

                        நிறுத்து;

                நிலை மதிப்பு_2:
                        எனில் (strchr(NEWLINE, c)) {
                                state = PRE_சாவி;
                                வரி++;

                                சாவி[n_சாவி] = 0;

                                எனில் (மதிப்பு)
                                        மதிப்பு[n_மதிப்பு] = 0;

                                /* Chomp off trailing whitespace from value */
                                எனில் (last_மதிப்பு_whitespace != SIZE_MAX)
                                        மதிப்பு[last_மதிப்பு_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                எனில் (last_சாவி_whitespace != SIZE_MAX)
                                        சாவி[last_சாவி_whitespace] = 0;

                                r = தள்ளு(fname, வரி, சாவி, மதிப்பு, userdata);
                                எனில் (r < 0)
                                        திரும்பு r;

                                n_சாவி = 0;
                                மதிப்பு = வெற்று;
                                n_மதிப்பு = 0;

                        } இல்லையெனில் எனில் (c == '\\') {
                                state = மதிப்பு_ESCAPE;
                                last_மதிப்பு_whitespace = SIZE_MAX;
                        } இல்லையெனில் {
                                எனில் (!strchr(WHITESPACE, c))
                                        last_மதிப்பு_whitespace = SIZE_MAX;
                                இல்லையெனில் எனில் (last_மதிப்பு_whitespace == SIZE_MAX)
                                        last_மதிப்பு_whitespace = n_மதிப்பு;

                                எனில் (!GREEDY_REALLOC(மதிப்பு, n_மதிப்பு+2))
                                        திரும்பு -ENOMEM;

                                மதிப்பு[n_மதிப்பு++] = c;
                        }

                        நிறுத்து;

                நிலை மதிப்பு_ESCAPE:
                        state = மதிப்பு_2;

                        எனில் (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                எனில் (!GREEDY_REALLOC(மதிப்பு, n_மதிப்பு+2))
                                        திரும்பு -ENOMEM;

                                மதிப்பு[n_மதிப்பு++] = c;
                        }
                        நிறுத்து;

                நிலை SINGLE_QUOTE_மதிப்பு:
                        எனில் (c == '\'')
                                state = PRE_மதிப்பு;
                        இல்லையெனில் {
                                எனில் (!GREEDY_REALLOC(மதிப்பு, n_மதிப்பு+2))
                                        திரும்பு -ENOMEM;

                                மதிப்பு[n_மதிப்பு++] = c;
                        }

                        நிறுத்து;

                நிலை DOUBLE_QUOTE_மதிப்பு:
                        எனில் (c == '"')
                                state = PRE_மதிப்பு;
                        இல்லையெனில் எனில் (c == '\\')
                                state = DOUBLE_QUOTE_மதிப்பு_ESCAPE;
                        இல்லையெனில் {
                                எனில் (!GREEDY_REALLOC(மதிப்பு, n_மதிப்பு+2))
                                        திரும்பு -ENOMEM;

                                மதிப்பு[n_மதிப்பு++] = c;
                        }

                        நிறுத்து;

                நிலை DOUBLE_QUOTE_மதிப்பு_ESCAPE:
                        state = DOUBLE_QUOTE_மதிப்பு;

                        எனில் (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                எனில் (!GREEDY_REALLOC(மதிப்பு, n_மதிப்பு+2))
                                        திரும்பு -ENOMEM;
                                மதிப்பு[n_மதிப்பு++] = c;
                        } இல்லையெனில் எனில் (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                எனில் (!GREEDY_REALLOC(மதிப்பு, n_மதிப்பு+3))
                                        திரும்பு -ENOMEM;
                                மதிப்பு[n_மதிப்பு++] = '\\';
                                மதிப்பு[n_மதிப்பு++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        நிறுத்து;

                நிலை COMMENT:
                        எனில் (c == '\\')
                                state = COMMENT_ESCAPE;
                        இல்லையெனில் எனில் (strchr(NEWLINE, c)) {
                                state = PRE_சாவி;
                                வரி++;
                        }
                        நிறுத்து;

                நிலை COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        எனில் (strchr(NEWLINE, c)) {
                                state = PRE_சாவி;
                                வரி++;
                        } இல்லையெனில்
                                state = COMMENT;
                        நிறுத்து;
                }
        }

        எனில் (IN_SET(state,
                   PRE_மதிப்பு,
                   மதிப்பு_2,
                   மதிப்பு_ESCAPE,
                   SINGLE_QUOTE_மதிப்பு,
                   DOUBLE_QUOTE_மதிப்பு,
                   DOUBLE_QUOTE_மதிப்பு_ESCAPE)) {

                சாவி[n_சாவி] = 0;

                எனில் (மதிப்பு)
                        மதிப்பு[n_மதிப்பு] = 0;

                எனில் (state == மதிப்பு_2)
                        எனில் (last_மதிப்பு_whitespace != SIZE_MAX)
                                மதிப்பு[last_மதிப்பு_whitespace] = 0;

                /* strip trailing whitespace from key */
                எனில் (last_சாவி_whitespace != SIZE_MAX)
                        சாவி[last_சாவி_whitespace] = 0;

                r = தள்ளு(fname, வரி, சாவி, மதிப்பு, userdata);
                எனில் (r < 0)
                        திரும்பு r;

                மதிப்பு = வெற்று;
        }

        திரும்பு 0;
}

நிலையான முழு check_utf8ness_and_warn(
                மாறிலி எழுத்து *filename, unsigned வரி,
                மாறிலி எழுத்து *சாவி, எழுத்து *மதிப்பு) {

        ப்ரதிஜ்ஞா(சாவி);

        எனில் (!utf8_is_valid(சாவி)) {
                _cleanup_free_ எழுத்து *p = வெற்று;

                p = utf8_escape_invalid(சாவி);
                திரும்பு log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), வரி, p);
        }

        எனில் (மதிப்பு && !utf8_is_valid(மதிப்பு)) {
                _cleanup_free_ எழுத்து *p = வெற்று;

                p = utf8_escape_invalid(மதிப்பு);
                திரும்பு log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), வரி, சாவி, p);
        }

        திரும்பு 0;
}

நிலையான முழு parse_env_file_தள்ளு(
                மாறிலி எழுத்து *filename, unsigned வரி,
                மாறிலி எழுத்து *சாவி, எழுத்து *மதிப்பு,
                வெறுமை *userdata) {

        மாறிலி எழுத்து *k;
        va_list aq, *ap = userdata;
        முழு r;

        ப்ரதிஜ்ஞா(சாவி);

        r = check_utf8ness_and_warn(filename, வரி, சாவி, மதிப்பு);
        எனில் (r < 0)
                திரும்பு r;

        va_copy(aq, *ap);

        போது ((k = va_arg(aq, மாறிலி எழுத்து *))) {
                எழுத்து **v;

                v = va_arg(aq, எழுத்து **);

                எனில் (streq(சாவி, k)) {
                        va_end(aq);
                        free_and_replace(*v, மதிப்பு);

                        திரும்பு 1;
                }
        }

        va_end(aq);
        விடுவி(மதிப்பு);

        திரும்பு 0;
}

முழு parse_env_filev(
                FILE *f,
                மாறிலி எழுத்து *fname,
                va_list ap) {

        முழு r;
        va_list aq;

        ப்ரதிஜ்ஞா(f || fname);

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_தள்ளு, &aq);
        va_end(aq);
        திரும்பு r;
}

முழு parse_env_file_fdv(முழு fd, மாறிலி எழுத்து *fname, va_list ap) {
        _cleanup_fclose_ FILE *f = வெற்று;
        va_list aq;
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        எனில் (r < 0)
                திரும்பு r;

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_தள்ளு, &aq);
        va_end(aq);
        திரும்பு r;
}

முழு parse_env_file_sentinel(
                FILE *f,
                மாறிலி எழுத்து *fname,
                ...) {

        va_list ap;
        முழு r;

        ப்ரதிஜ்ஞா(f || fname);

        va_start(ap, fname);
        r = parse_env_filev(f, fname, ap);
        va_end(ap);

        திரும்பு r;
}

முழு parse_env_file_fd_sentinel(
                முழு fd,
                மாறிலி எழுத்து *fname, /* only used for logging */
                ...) {

        va_list ap;
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);

        va_start(ap, fname);
        r = parse_env_file_fdv(fd, fname, ap);
        va_end(ap);

        திரும்பு r;
}

முழு parse_env_datav(
                மாறிலி எழுத்து *data,
                size_t size,
                மாறிலி எழுத்து *fname, /* only used for logging */
                va_list ap) {

        ப்ரதிஜ்ஞா(data);

        எனில் (size == SIZE_MAX)
                size = strlen_ptr(data);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((வெறுமை*) data, size, "r");
        எனில் (!f)
                திரும்பு -ENOMEM;

        திரும்பு parse_env_filev(f, fname, ap);
}

முழு parse_env_data_sentinel(
                மாறிலி எழுத்து *data,
                size_t size,
                மாறிலி எழுத்து *fname, /* only used for logging */
                ...) {

        va_list ap;
        முழு r;

        va_start(ap, fname);
        r = parse_env_datav(data, size, fname, ap);
        va_end(ap);

        திரும்பு r;
}

நிலையான முழு load_env_file_தள்ளு(
                மாறிலி எழுத்து *filename, unsigned வரி,
                மாறிலி எழுத்து *சாவி, எழுத்து *மதிப்பு,
                வெறுமை *userdata) {

        எழுத்து ***m = userdata;
        எழுத்து *p;
        முழு r;

        ப்ரதிஜ்ஞா(சாவி);

        r = check_utf8ness_and_warn(filename, வரி, சாவி, மதிப்பு);
        எனில் (r < 0)
                திரும்பு r;

        p = strjoin(சாவி, "=", மதிப்பு);
        எனில் (!p)
                திரும்பு -ENOMEM;

        r = strv_env_replace_consume(m, p);
        எனில் (r < 0)
                திரும்பு r;

        விடுவி(மதிப்பு);
        திரும்பு 0;
}

முழு load_env_file(FILE *f, மாறிலி எழுத்து *fname, எழுத்து ***ret) {
        _cleanup_strv_free_ எழுத்து **m = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(f || fname);
        ப்ரதிஜ்ஞா(ret);

        r = parse_env_file_internal(f, fname, load_env_file_தள்ளு, &m);
        எனில் (r < 0)
                திரும்பு r;

        *ret = TAKE_PTR(m);
        திரும்பு 0;
}

நிலையான முழு load_env_file_தள்ளு_pairs(
                மாறிலி எழுத்து *filename, unsigned வரி,
                மாறிலி எழுத்து *சாவி, எழுத்து *மதிப்பு,
                வெறுமை *userdata) {

        எழுத்து ***m = ASSERT_PTR(userdata);
        முழு r;

        ப்ரதிஜ்ஞா(சாவி);

        r = check_utf8ness_and_warn(filename, வரி, சாவி, மதிப்பு);
        எனில் (r < 0)
                திரும்பு r;

        /* Check if the key is present */
        க்கு (எழுத்து **t = *m; t && *t; t += 2)
                எனில் (streq(t[0], சாவி)) {
                        எனில் (மதிப்பு)
                                திரும்பு free_and_replace(t[1], மதிப்பு);
                        இல்லையெனில்
                                திரும்பு free_and_strdup(t+1, "");
                }

        r = strv_extend(m, சாவி);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (மதிப்பு)
                திரும்பு strv_தள்ளு(m, மதிப்பு);
        இல்லையெனில்
                திரும்பு strv_extend(m, "");
}

முழு load_env_file_pairs(FILE *f, மாறிலி எழுத்து *fname, எழுத்து ***ret) {
        _cleanup_strv_free_ எழுத்து **m = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(f || fname);
        ப்ரதிஜ்ஞா(ret);

        r = parse_env_file_internal(f, fname, load_env_file_தள்ளு_pairs, &m);
        எனில் (r < 0)
                திரும்பு r;

        *ret = TAKE_PTR(m);
        திரும்பு 0;
}

முழு load_env_file_pairs_fd(முழு fd, மாறிலி எழுத்து *fname, எழுத்து ***ret) {
        _cleanup_fclose_ FILE *f = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு load_env_file_pairs(f, fname, ret);
}

நிலையான முழு merge_env_file_தள்ளு(
                மாறிலி எழுத்து *filename, unsigned வரி,
                மாறிலி எழுத்து *சாவி, எழுத்து *மதிப்பு,
                வெறுமை *userdata) {

        எழுத்து ***env = ASSERT_PTR(userdata);
        எழுத்து *expanded_மதிப்பு;
        முழு r;

        ப்ரதிஜ்ஞா(சாவி);

        எனில் (!மதிப்பு) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), வரி, சாவி);
                திரும்பு 0;
        }

        எனில் (!env_name_is_valid(சாவி)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), வரி, சாவி);
                விடுவி(மதிப்பு);
                திரும்பு 0;
        }

        r = replace_env(மதிப்பு,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_மதிப்பு);
        எனில் (r < 0)
                திரும்பு log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), வரி, மதிப்பு);

        free_and_replace(மதிப்பு, expanded_மதிப்பு);

        log_debug("%s:%u: setting %s=%s", filename, வரி, சாவி, மதிப்பு);

        திரும்பு load_env_file_தள்ளு(filename, வரி, சாவி, மதிப்பு, env);
}

முழு merge_env_file(
                எழுத்து ***env,
                FILE *f,
                மாறிலி எழுத்து *fname) {

        ப்ரதிஜ்ஞா(env);
        ப்ரதிஜ்ஞா(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        திரும்பு parse_env_file_internal(f, fname, merge_env_file_தள்ளு, env);
}

நிலையான வெறுமை env_file_fputs_escaped(FILE *f, மாறிலி எழுத்து *p) {
        ப்ரதிஜ்ஞா(f);
        ப்ரதிஜ்ஞா(p);

        flockfile(f);

        எனில் (string_has_cc(p, வெற்று) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                க்கு (; *p; p++) {
                        எனில் (strchr(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } இல்லையெனில்
                fputs_unlocked(p, f);

        funlockfile(f);
}

வெறுமை env_file_fputs_assignment(FILE *f, மாறிலி எழுத்து *k, மாறிலி எழுத்து *v) {
        ப்ரதிஜ்ஞா(f);
        ப்ரதிஜ்ஞா(k);

        எனில் (!v)
                திரும்பு;

        fputs(k, f);
        env_file_fputs_escaped(f, v);
        fputc('\n', f);
}

நிலையான வெறுமை write_env_var(FILE *f, மாறிலி எழுத்து *v) {
        மாறிலி எழுத்து *p;

        ப்ரதிஜ்ஞா(f);
        ப்ரதிஜ்ஞா(v);

        p = strchr(v, '=');
        எனில் (!p) {
                /* Fallback */
                fputs_unlocked(v, f);
                fputc_unlocked('\n', f);
                திரும்பு;
        }

        p++;
        fwrite_unlocked(v, 1, p-v, f);

        env_file_fputs_escaped(f, p);

        fputc_unlocked('\n', f);
}

முழு write_env_file_label(முழு dir_fd, மாறிலி எழுத்து *fname, எழுத்து **headers, எழுத்து **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        முழு r;

        ப்ரதிஜ்ஞா(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ப்ரதிஜ்ஞா(fname);

        எனில் (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                எனில் (r < 0)
                        திரும்பு r;
        }

        _cleanup_fclose_ FILE *f = வெற்று;
        _cleanup_free_ எழுத்து *p = வெற்று;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        எனில் (r < 0) {
                எனில் (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL))
                        (வெறுமை) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                திரும்பு r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        எனில் (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ வெற்று, /* created= */ true, label_context);
                எனில் (r < 0)
                        திரும்பு r;
        }

        r = fchmod_umask(fileno(f), 0644);
        எனில் (r < 0)
                திரும்பு r;

        STRV_FOREACH(i, headers) {
                ப்ரதிஜ்ஞா(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        எனில் (r < 0)
                திரும்பு r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        திரும்பு 0;
}

முழு write_vconsole_conf(முழு dir_fd, மாறிலி எழுத்து *fname, எழுத்து **l) {
        எழுத்து **headers = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        திரும்பு write_env_file_label(dir_fd, fname, headers, l, WRITE_ENV_FILE_LABEL, /* label_context= */ வெற்று);
}
