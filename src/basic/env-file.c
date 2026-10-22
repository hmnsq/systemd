/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "env-file.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "env-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "escape.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "label-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "tmpfile-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utf8.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*ਧੱਕੋ_env_func_t)(
                ਅਚਲ ਅੱਖਰ *filename,
                ਬਿਨਾਨਿਸ਼ਾਨੀ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ,
                ਅੱਖਰ *ਮੁੱਲ,
                ਖਾਲੀ *userdata);

ਸਥਿਰ ਪੂਰਨ parse_env_file_ਅੰਦਰੂਨੀ(
                FILE *f,
                ਅਚਲ ਅੱਖਰ *fname,
                ਧੱਕੋ_env_func_t ਧੱਕੋ,
                ਖਾਲੀ *userdata) {

        size_t n_ਕੁੰਜੀ = 0, n_ਮੁੱਲ = 0, ਆਖਰੀ_ਮੁੱਲ_whitespace = SIZE_MAX, ਆਖਰੀ_ਕੁੰਜੀ_whitespace = SIZE_MAX;
        _cleanup_free_ ਅੱਖਰ *contents = ਨਿਰਾ, *ਕੁੰਜੀ = ਨਿਰਾ, *ਮੁੱਲ = ਨਿਰਾ;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਸਤਰ = 1;
        ਪੂਰਨ r;

        ਗਿਣਤੀ {
                PRE_ਕੁੰਜੀ,
                ਕੁੰਜੀ_2,
                PRE_ਮੁੱਲ,
                ਮੁੱਲ_2,
                ਮੁੱਲ_ESCAPE,
                SINGLE_QUOTE_ਮੁੱਲ,
                DOUBLE_QUOTE_ਮੁੱਲ,
                DOUBLE_QUOTE_ਮੁੱਲ_ESCAPE,
                COMMENT,
                COMMENT_ESCAPE
        } state = PRE_ਕੁੰਜੀ;

        ਪ੍ਰਤਿਜ੍ਞਾ(f || fname);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਧੱਕੋ);

        ਜੇ (f)
                r = read_ਭਰਿਆ_ਧਾਰਾ(f, &contents, ਨਿਰਾ);
        ਨਹੀਂਤਾਂ
                r = read_ਭਰਿਆ_file(fname, &contents, ਨਿਰਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਲਈ (ਅੱਖਰ *p = contents; *p; p++) {
                ਅੱਖਰ c = *p;

                ਚੋਣ (state) {

                ਹਾਲਤ PRE_ਕੁੰਜੀ:
                        ਜੇ (strchr(COMMENTS, c))
                                state = COMMENT;
                        ਨਹੀਂਤਾਂ ਜੇ (!strchr(WHITESPACE, c)) {
                                state = ਕੁੰਜੀ_2;
                                ਆਖਰੀ_ਕੁੰਜੀ_whitespace = SIZE_MAX;

                                ਜੇ (!GREEDY_REALLOC(ਕੁੰਜੀ, n_ਕੁੰਜੀ+2))
                                        ਵਾਪਸ -ENOMEM;

                                ਕੁੰਜੀ[n_ਕੁੰਜੀ++] = c;
                        }
                        ਰੋਕੋ;

                ਹਾਲਤ ਕੁੰਜੀ_2:
                        ਜੇ (strchr(NEWLINE, c)) {
                                state = PRE_ਕੁੰਜੀ;
                                ਸਤਰ++;
                                n_ਕੁੰਜੀ = 0;
                        } ਨਹੀਂਤਾਂ ਜੇ (c == '=') {
                                state = PRE_ਮੁੱਲ;
                                ਆਖਰੀ_ਮੁੱਲ_whitespace = SIZE_MAX;
                        } ਨਹੀਂਤਾਂ {
                                ਜੇ (!strchr(WHITESPACE, c))
                                        ਆਖਰੀ_ਕੁੰਜੀ_whitespace = SIZE_MAX;
                                ਨਹੀਂਤਾਂ ਜੇ (ਆਖਰੀ_ਕੁੰਜੀ_whitespace == SIZE_MAX)
                                         ਆਖਰੀ_ਕੁੰਜੀ_whitespace = n_ਕੁੰਜੀ;

                                ਜੇ (!GREEDY_REALLOC(ਕੁੰਜੀ, n_ਕੁੰਜੀ+2))
                                        ਵਾਪਸ -ENOMEM;

                                ਕੁੰਜੀ[n_ਕੁੰਜੀ++] = c;
                        }

                        ਰੋਕੋ;

                ਹਾਲਤ PRE_ਮੁੱਲ:
                        ਜੇ (strchr(NEWLINE, c)) {
                                state = PRE_ਕੁੰਜੀ;
                                ਸਤਰ++;
                                ਕੁੰਜੀ[n_ਕੁੰਜੀ] = 0;

                                ਜੇ (ਮੁੱਲ)
                                        ਮੁੱਲ[n_ਮੁੱਲ] = 0;

                                /* strip trailing whitespace from key */
                                ਜੇ (ਆਖਰੀ_ਕੁੰਜੀ_whitespace != SIZE_MAX)
                                        ਕੁੰਜੀ[ਆਖਰੀ_ਕੁੰਜੀ_whitespace] = 0;

                                r = ਧੱਕੋ(fname, ਸਤਰ, ਕੁੰਜੀ, ਮੁੱਲ, userdata);
                                ਜੇ (r < 0)
                                        ਵਾਪਸ r;

                                n_ਕੁੰਜੀ = 0;
                                ਮੁੱਲ = ਨਿਰਾ;
                                n_ਮੁੱਲ = 0;

                        } ਨਹੀਂਤਾਂ ਜੇ (c == '\'')
                                state = SINGLE_QUOTE_ਮੁੱਲ;
                        ਨਹੀਂਤਾਂ ਜੇ (c == '"')
                                state = DOUBLE_QUOTE_ਮੁੱਲ;
                        ਨਹੀਂਤਾਂ ਜੇ (c == '\\')
                                state = ਮੁੱਲ_ESCAPE;
                        ਨਹੀਂਤਾਂ ਜੇ (!strchr(WHITESPACE, c)) {
                                state = ਮੁੱਲ_2;

                                ਜੇ (!GREEDY_REALLOC(ਮੁੱਲ, n_ਮੁੱਲ+2))
                                        ਵਾਪਸ -ENOMEM;

                                ਮੁੱਲ[n_ਮੁੱਲ++] = c;
                        }

                        ਰੋਕੋ;

                ਹਾਲਤ ਮੁੱਲ_2:
                        ਜੇ (strchr(NEWLINE, c)) {
                                state = PRE_ਕੁੰਜੀ;
                                ਸਤਰ++;

                                ਕੁੰਜੀ[n_ਕੁੰਜੀ] = 0;

                                ਜੇ (ਮੁੱਲ)
                                        ਮੁੱਲ[n_ਮੁੱਲ] = 0;

                                /* Chomp off trailing whitespace from value */
                                ਜੇ (ਆਖਰੀ_ਮੁੱਲ_whitespace != SIZE_MAX)
                                        ਮੁੱਲ[ਆਖਰੀ_ਮੁੱਲ_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                ਜੇ (ਆਖਰੀ_ਕੁੰਜੀ_whitespace != SIZE_MAX)
                                        ਕੁੰਜੀ[ਆਖਰੀ_ਕੁੰਜੀ_whitespace] = 0;

                                r = ਧੱਕੋ(fname, ਸਤਰ, ਕੁੰਜੀ, ਮੁੱਲ, userdata);
                                ਜੇ (r < 0)
                                        ਵਾਪਸ r;

                                n_ਕੁੰਜੀ = 0;
                                ਮੁੱਲ = ਨਿਰਾ;
                                n_ਮੁੱਲ = 0;

                        } ਨਹੀਂਤਾਂ ਜੇ (c == '\\') {
                                state = ਮੁੱਲ_ESCAPE;
                                ਆਖਰੀ_ਮੁੱਲ_whitespace = SIZE_MAX;
                        } ਨਹੀਂਤਾਂ {
                                ਜੇ (!strchr(WHITESPACE, c))
                                        ਆਖਰੀ_ਮੁੱਲ_whitespace = SIZE_MAX;
                                ਨਹੀਂਤਾਂ ਜੇ (ਆਖਰੀ_ਮੁੱਲ_whitespace == SIZE_MAX)
                                        ਆਖਰੀ_ਮੁੱਲ_whitespace = n_ਮੁੱਲ;

                                ਜੇ (!GREEDY_REALLOC(ਮੁੱਲ, n_ਮੁੱਲ+2))
                                        ਵਾਪਸ -ENOMEM;

                                ਮੁੱਲ[n_ਮੁੱਲ++] = c;
                        }

                        ਰੋਕੋ;

                ਹਾਲਤ ਮੁੱਲ_ESCAPE:
                        state = ਮੁੱਲ_2;

                        ਜੇ (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                ਜੇ (!GREEDY_REALLOC(ਮੁੱਲ, n_ਮੁੱਲ+2))
                                        ਵਾਪਸ -ENOMEM;

                                ਮੁੱਲ[n_ਮੁੱਲ++] = c;
                        }
                        ਰੋਕੋ;

                ਹਾਲਤ SINGLE_QUOTE_ਮੁੱਲ:
                        ਜੇ (c == '\'')
                                state = PRE_ਮੁੱਲ;
                        ਨਹੀਂਤਾਂ {
                                ਜੇ (!GREEDY_REALLOC(ਮੁੱਲ, n_ਮੁੱਲ+2))
                                        ਵਾਪਸ -ENOMEM;

                                ਮੁੱਲ[n_ਮੁੱਲ++] = c;
                        }

                        ਰੋਕੋ;

                ਹਾਲਤ DOUBLE_QUOTE_ਮੁੱਲ:
                        ਜੇ (c == '"')
                                state = PRE_ਮੁੱਲ;
                        ਨਹੀਂਤਾਂ ਜੇ (c == '\\')
                                state = DOUBLE_QUOTE_ਮੁੱਲ_ESCAPE;
                        ਨਹੀਂਤਾਂ {
                                ਜੇ (!GREEDY_REALLOC(ਮੁੱਲ, n_ਮੁੱਲ+2))
                                        ਵਾਪਸ -ENOMEM;

                                ਮੁੱਲ[n_ਮੁੱਲ++] = c;
                        }

                        ਰੋਕੋ;

                ਹਾਲਤ DOUBLE_QUOTE_ਮੁੱਲ_ESCAPE:
                        state = DOUBLE_QUOTE_ਮੁੱਲ;

                        ਜੇ (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                ਜੇ (!GREEDY_REALLOC(ਮੁੱਲ, n_ਮੁੱਲ+2))
                                        ਵਾਪਸ -ENOMEM;
                                ਮੁੱਲ[n_ਮੁੱਲ++] = c;
                        } ਨਹੀਂਤਾਂ ਜੇ (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                ਜੇ (!GREEDY_REALLOC(ਮੁੱਲ, n_ਮੁੱਲ+3))
                                        ਵਾਪਸ -ENOMEM;
                                ਮੁੱਲ[n_ਮੁੱਲ++] = '\\';
                                ਮੁੱਲ[n_ਮੁੱਲ++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        ਰੋਕੋ;

                ਹਾਲਤ COMMENT:
                        ਜੇ (c == '\\')
                                state = COMMENT_ESCAPE;
                        ਨਹੀਂਤਾਂ ਜੇ (strchr(NEWLINE, c)) {
                                state = PRE_ਕੁੰਜੀ;
                                ਸਤਰ++;
                        }
                        ਰੋਕੋ;

                ਹਾਲਤ COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        ਜੇ (strchr(NEWLINE, c)) {
                                state = PRE_ਕੁੰਜੀ;
                                ਸਤਰ++;
                        } ਨਹੀਂਤਾਂ
                                state = COMMENT;
                        ਰੋਕੋ;
                }
        }

        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(state,
                   PRE_ਮੁੱਲ,
                   ਮੁੱਲ_2,
                   ਮੁੱਲ_ESCAPE,
                   SINGLE_QUOTE_ਮੁੱਲ,
                   DOUBLE_QUOTE_ਮੁੱਲ,
                   DOUBLE_QUOTE_ਮੁੱਲ_ESCAPE)) {

                ਕੁੰਜੀ[n_ਕੁੰਜੀ] = 0;

                ਜੇ (ਮੁੱਲ)
                        ਮੁੱਲ[n_ਮੁੱਲ] = 0;

                ਜੇ (state == ਮੁੱਲ_2)
                        ਜੇ (ਆਖਰੀ_ਮੁੱਲ_whitespace != SIZE_MAX)
                                ਮੁੱਲ[ਆਖਰੀ_ਮੁੱਲ_whitespace] = 0;

                /* strip trailing whitespace from key */
                ਜੇ (ਆਖਰੀ_ਕੁੰਜੀ_whitespace != SIZE_MAX)
                        ਕੁੰਜੀ[ਆਖਰੀ_ਕੁੰਜੀ_whitespace] = 0;

                r = ਧੱਕੋ(fname, ਸਤਰ, ਕੁੰਜੀ, ਮੁੱਲ, userdata);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਮੁੱਲ = ਨਿਰਾ;
        }

        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ check_utf8ness_and_warn(
                ਅਚਲ ਅੱਖਰ *filename, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅੱਖਰ *ਮੁੱਲ) {

        ਪ੍ਰਤਿਜ੍ਞਾ(ਕੁੰਜੀ);

        ਜੇ (!utf8_is_valid(ਕੁੰਜੀ)) {
                _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;

                p = utf8_escape_invalid(ਕੁੰਜੀ);
                ਵਾਪਸ log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), ਸਤਰ, p);
        }

        ਜੇ (ਮੁੱਲ && !utf8_is_valid(ਮੁੱਲ)) {
                _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;

                p = utf8_escape_invalid(ਮੁੱਲ);
                ਵਾਪਸ log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), ਸਤਰ, ਕੁੰਜੀ, p);
        }

        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ parse_env_file_ਧੱਕੋ(
                ਅਚਲ ਅੱਖਰ *filename, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅੱਖਰ *ਮੁੱਲ,
                ਖਾਲੀ *userdata) {

        ਅਚਲ ਅੱਖਰ *k;
        va_list aq, *ap = userdata;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਕੁੰਜੀ);

        r = check_utf8ness_and_warn(filename, ਸਤਰ, ਕੁੰਜੀ, ਮੁੱਲ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        va_copy(aq, *ap);

        ਜਦੋਂ ((k = va_arg(aq, ਅਚਲ ਅੱਖਰ *))) {
                ਅੱਖਰ **v;

                v = va_arg(aq, ਅੱਖਰ **);

                ਜੇ (streq(ਕੁੰਜੀ, k)) {
                        va_end(aq);
                        free_and_replace(*v, ਮੁੱਲ);

                        ਵਾਪਸ 1;
                }
        }

        va_end(aq);
        ਮੁਕਤ(ਮੁੱਲ);

        ਵਾਪਸ 0;
}

ਪੂਰਨ parse_env_filev(
                FILE *f,
                ਅਚਲ ਅੱਖਰ *fname,
                va_list ap) {

        ਪੂਰਨ r;
        va_list aq;

        ਪ੍ਰਤਿਜ੍ਞਾ(f || fname);

        va_copy(aq, ap);
        r = parse_env_file_ਅੰਦਰੂਨੀ(f, fname, parse_env_file_ਧੱਕੋ, &aq);
        va_end(aq);
        ਵਾਪਸ r;
}

ਪੂਰਨ parse_env_file_fdv(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *fname, va_list ap) {
        _cleanup_fclose_ FILE *f = ਨਿਰਾ;
        va_list aq;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        va_copy(aq, ap);
        r = parse_env_file_ਅੰਦਰੂਨੀ(f, fname, parse_env_file_ਧੱਕੋ, &aq);
        va_end(aq);
        ਵਾਪਸ r;
}

ਪੂਰਨ parse_env_file_sentinel(
                FILE *f,
                ਅਚਲ ਅੱਖਰ *fname,
                ...) {

        va_list ap;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(f || fname);

        va_start(ap, fname);
        r = parse_env_filev(f, fname, ap);
        va_end(ap);

        ਵਾਪਸ r;
}

ਪੂਰਨ parse_env_file_fd_sentinel(
                ਪੂਰਨ fd,
                ਅਚਲ ਅੱਖਰ *fname, /* only used for logging */
                ...) {

        va_list ap;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        va_start(ap, fname);
        r = parse_env_file_fdv(fd, fname, ap);
        va_end(ap);

        ਵਾਪਸ r;
}

ਪੂਰਨ parse_env_datav(
                ਅਚਲ ਅੱਖਰ *ਅੰਕੜੇ,
                size_t ਮਾਪ,
                ਅਚਲ ਅੱਖਰ *fname, /* only used for logging */
                va_list ap) {

        ਪ੍ਰਤਿਜ੍ਞਾ(ਅੰਕੜੇ);

        ਜੇ (ਮਾਪ == SIZE_MAX)
                ਮਾਪ = strlen_ptr(ਅੰਕੜੇ);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((ਖਾਲੀ*) ਅੰਕੜੇ, ਮਾਪ, "r");
        ਜੇ (!f)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ parse_env_filev(f, fname, ap);
}

ਪੂਰਨ parse_env_ਅੰਕੜੇ_sentinel(
                ਅਚਲ ਅੱਖਰ *ਅੰਕੜੇ,
                size_t ਮਾਪ,
                ਅਚਲ ਅੱਖਰ *fname, /* only used for logging */
                ...) {

        va_list ap;
        ਪੂਰਨ r;

        va_start(ap, fname);
        r = parse_env_datav(ਅੰਕੜੇ, ਮਾਪ, fname, ap);
        va_end(ap);

        ਵਾਪਸ r;
}

ਸਥਿਰ ਪੂਰਨ ਲੱਦੋ_env_file_ਧੱਕੋ(
                ਅਚਲ ਅੱਖਰ *filename, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅੱਖਰ *ਮੁੱਲ,
                ਖਾਲੀ *userdata) {

        ਅੱਖਰ ***m = userdata;
        ਅੱਖਰ *p;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਕੁੰਜੀ);

        r = check_utf8ness_and_warn(filename, ਸਤਰ, ਕੁੰਜੀ, ਮੁੱਲ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        p = strjoin(ਕੁੰਜੀ, "=", ਮੁੱਲ);
        ਜੇ (!p)
                ਵਾਪਸ -ENOMEM;

        r = strv_env_replace_consume(m, p);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਮੁਕਤ(ਮੁੱਲ);
        ਵਾਪਸ 0;
}

ਪੂਰਨ ਲੱਦੋ_env_file(FILE *f, ਅਚਲ ਅੱਖਰ *fname, ਅੱਖਰ ***ret) {
        _cleanup_strv_free_ ਅੱਖਰ **m = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(f || fname);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = parse_env_file_ਅੰਦਰੂਨੀ(f, fname, ਲੱਦੋ_env_file_ਧੱਕੋ, &m);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        *ret = TAKE_PTR(m);
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ ਲੱਦੋ_env_file_ਧੱਕੋ_ਜੋੜੇ(
                ਅਚਲ ਅੱਖਰ *filename, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅੱਖਰ *ਮੁੱਲ,
                ਖਾਲੀ *userdata) {

        ਅੱਖਰ ***m = ASSERT_PTR(userdata);
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਕੁੰਜੀ);

        r = check_utf8ness_and_warn(filename, ਸਤਰ, ਕੁੰਜੀ, ਮੁੱਲ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* Check if the key is present */
        ਲਈ (ਅੱਖਰ **t = *m; t && *t; t += 2)
                ਜੇ (streq(t[0], ਕੁੰਜੀ)) {
                        ਜੇ (ਮੁੱਲ)
                                ਵਾਪਸ free_and_replace(t[1], ਮੁੱਲ);
                        ਨਹੀਂਤਾਂ
                                ਵਾਪਸ free_and_strdup(t+1, "");
                }

        r = strv_extend(m, ਕੁੰਜੀ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ਮੁੱਲ)
                ਵਾਪਸ strv_ਧੱਕੋ(m, ਮੁੱਲ);
        ਨਹੀਂਤਾਂ
                ਵਾਪਸ strv_extend(m, "");
}

ਪੂਰਨ ਲੱਦੋ_env_file_ਜੋੜੇ(FILE *f, ਅਚਲ ਅੱਖਰ *fname, ਅੱਖਰ ***ret) {
        _cleanup_strv_free_ ਅੱਖਰ **m = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(f || fname);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = parse_env_file_ਅੰਦਰੂਨੀ(f, fname, ਲੱਦੋ_env_file_ਧੱਕੋ_ਜੋੜੇ, &m);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        *ret = TAKE_PTR(m);
        ਵਾਪਸ 0;
}

ਪੂਰਨ ਲੱਦੋ_env_file_ਜੋੜੇ_fd(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *fname, ਅੱਖਰ ***ret) {
        _cleanup_fclose_ FILE *f = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ ਲੱਦੋ_env_file_ਜੋੜੇ(f, fname, ret);
}

ਸਥਿਰ ਪੂਰਨ ਮਿਲਾਓ_env_file_ਧੱਕੋ(
                ਅਚਲ ਅੱਖਰ *filename, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅੱਖਰ *ਮੁੱਲ,
                ਖਾਲੀ *userdata) {

        ਅੱਖਰ ***env = ASSERT_PTR(userdata);
        ਅੱਖਰ *expanded_ਮੁੱਲ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਕੁੰਜੀ);

        ਜੇ (!ਮੁੱਲ) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), ਸਤਰ, ਕੁੰਜੀ);
                ਵਾਪਸ 0;
        }

        ਜੇ (!env_name_is_valid(ਕੁੰਜੀ)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), ਸਤਰ, ਕੁੰਜੀ);
                ਮੁਕਤ(ਮੁੱਲ);
                ਵਾਪਸ 0;
        }

        r = replace_env(ਮੁੱਲ,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_ਮੁੱਲ);
        ਜੇ (r < 0)
                ਵਾਪਸ log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), ਸਤਰ, ਮੁੱਲ);

        free_and_replace(ਮੁੱਲ, expanded_ਮੁੱਲ);

        log_debug("%s:%u: setting %s=%s", filename, ਸਤਰ, ਕੁੰਜੀ, ਮੁੱਲ);

        ਵਾਪਸ ਲੱਦੋ_env_file_ਧੱਕੋ(filename, ਸਤਰ, ਕੁੰਜੀ, ਮੁੱਲ, env);
}

ਪੂਰਨ ਮਿਲਾਓ_env_file(
                ਅੱਖਰ ***env,
                FILE *f,
                ਅਚਲ ਅੱਖਰ *fname) {

        ਪ੍ਰਤਿਜ੍ਞਾ(env);
        ਪ੍ਰਤਿਜ੍ਞਾ(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        ਵਾਪਸ parse_env_file_ਅੰਦਰੂਨੀ(f, fname, ਮਿਲਾਓ_env_file_ਧੱਕੋ, env);
}

ਸਥਿਰ ਖਾਲੀ env_file_fputs_escaped(FILE *f, ਅਚਲ ਅੱਖਰ *p) {
        ਪ੍ਰਤਿਜ੍ਞਾ(f);
        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        flockfile(f);

        ਜੇ (string_has_cc(p, ਨਿਰਾ) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                ਲਈ (; *p; p++) {
                        ਜੇ (strchr(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } ਨਹੀਂਤਾਂ
                fputs_unlocked(p, f);

        funlockfile(f);
}

ਖਾਲੀ env_file_fputs_assignment(FILE *f, ਅਚਲ ਅੱਖਰ *k, ਅਚਲ ਅੱਖਰ *v) {
        ਪ੍ਰਤਿਜ੍ਞਾ(f);
        ਪ੍ਰਤਿਜ੍ਞਾ(k);

        ਜੇ (!v)
                ਵਾਪਸ;

        fputs(k, f);
        env_file_fputs_escaped(f, v);
        fputc('\n', f);
}

ਸਥਿਰ ਖਾਲੀ write_env_var(FILE *f, ਅਚਲ ਅੱਖਰ *v) {
        ਅਚਲ ਅੱਖਰ *p;

        ਪ੍ਰਤਿਜ੍ਞਾ(f);
        ਪ੍ਰਤਿਜ੍ਞਾ(v);

        p = strchr(v, '=');
        ਜੇ (!p) {
                /* Fallback */
                fputs_unlocked(v, f);
                fputc_unlocked('\n', f);
                ਵਾਪਸ;
        }

        p++;
        fwrite_unlocked(v, 1, p-v, f);

        env_file_fputs_escaped(f, p);

        fputc_unlocked('\n', f);
}

ਪੂਰਨ write_env_file_label(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *fname, ਅੱਖਰ **ਸਿਰਲੇਖਾਂ, ਅੱਖਰ **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ਪ੍ਰਤਿਜ੍ਞਾ(fname);

        ਜੇ (FLAGS_ਸੈੱਟ(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        _cleanup_fclose_ FILE *f = ਨਿਰਾ;
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        ਜੇ (r < 0) {
                ਜੇ (FLAGS_ਸੈੱਟ(flags, WRITE_ENV_FILE_LABEL))
                        (ਖਾਲੀ) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                ਵਾਪਸ r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        ਜੇ (FLAGS_ਸੈੱਟ(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ ਨਿਰਾ, /* created= */ true, label_context);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        r = fchmod_umask(fileno(f), 0644);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        STRV_FOREACH(i, ਸਿਰਲੇਖਾਂ) {
                ਪ੍ਰਤਿਜ੍ਞਾ(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        ਵਾਪਸ 0;
}

ਪੂਰਨ write_vconsole_conf(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *fname, ਅੱਖਰ **l) {
        ਅੱਖਰ **ਸਿਰਲੇਖਾਂ = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        ਵਾਪਸ write_env_file_label(dir_fd, fname, ਸਿਰਲੇਖਾਂ, l, WRITE_ENV_FILE_LABEL, /* label_context= */ ਨਿਰਾ);
}
