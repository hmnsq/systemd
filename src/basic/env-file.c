/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <sys/stat.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "env-file.h"
#అన్తర్భావ "env-util.h"
#అన్తర్భావ "escape.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fileio.h"
#అన్తర్భావ "fs-util.h"
#అన్తర్భావ "label-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"
#అన్తర్భావ "tmpfile-util.h"
#అన్తర్భావ "utf8.h"

రకనిర్వచనం పూర్ణాంకం (*నెట్టు_env_func_t)(
                స్థిరరాశి అక్షరం *filename,
                గుర్తులేని పంక్తి,
                స్థిరరాశి అక్షరం *కీలకం,
                అక్షరం *విలువ,
                శూన్య *userdata);

స్థిర పూర్ణాంకం parse_env_file_అంతర్గత(
                FILE *f,
                స్థిరరాశి అక్షరం *fname,
                నెట్టు_env_func_t నెట్టు,
                శూన్య *userdata) {

        size_t n_కీలకం = 0, n_విలువ = 0, చివరి_విలువ_whitespace = SIZE_MAX, చివరి_కీలకం_whitespace = SIZE_MAX;
        _cleanup_free_ అక్షరం *contents = ఖాళీ, *కీలకం = ఖాళీ, *విలువ = ఖాళీ;
        గుర్తులేని పంక్తి = 1;
        పూర్ణాంకం r;

        గణన {
                PRE_కీలకం,
                కీలకం_2,
                PRE_విలువ,
                విలువ_2,
                విలువ_ESCAPE,
                SINGLE_QUOTE_విలువ,
                DOUBLE_QUOTE_విలువ,
                DOUBLE_QUOTE_విలువ_ESCAPE,
                COMMENT,
                COMMENT_ESCAPE
        } state = PRE_కీలకం;

        ప్రతిజ్ఞా(f || fname);
        ప్రతిజ్ఞా(నెట్టు);

        ఒకవేళ (f)
                r = read_నిండిన_ప్రవాహం(f, &contents, ఖాళీ);
        లేకపోతే
                r = read_నిండిన_file(fname, &contents, ఖాళీ);
        ఒకవేళ (r < 0)
                తిరిగి r;

        కొరకు (అక్షరం *p = contents; *p; p++) {
                అక్షరం c = *p;

                ఎంపిక (state) {

                స్థితి PRE_కీలకం:
                        ఒకవేళ (strchr(COMMENTS, c))
                                state = COMMENT;
                        లేకపోతే ఒకవేళ (!strchr(WHITESPACE, c)) {
                                state = కీలకం_2;
                                చివరి_కీలకం_whitespace = SIZE_MAX;

                                ఒకవేళ (!GREEDY_REALLOC(కీలకం, n_కీలకం+2))
                                        తిరిగి -ENOMEM;

                                కీలకం[n_కీలకం++] = c;
                        }
                        ఆపు;

                స్థితి కీలకం_2:
                        ఒకవేళ (strchr(NEWLINE, c)) {
                                state = PRE_కీలకం;
                                పంక్తి++;
                                n_కీలకం = 0;
                        } లేకపోతే ఒకవేళ (c == '=') {
                                state = PRE_విలువ;
                                చివరి_విలువ_whitespace = SIZE_MAX;
                        } లేకపోతే {
                                ఒకవేళ (!strchr(WHITESPACE, c))
                                        చివరి_కీలకం_whitespace = SIZE_MAX;
                                లేకపోతే ఒకవేళ (చివరి_కీలకం_whitespace == SIZE_MAX)
                                         చివరి_కీలకం_whitespace = n_కీలకం;

                                ఒకవేళ (!GREEDY_REALLOC(కీలకం, n_కీలకం+2))
                                        తిరిగి -ENOMEM;

                                కీలకం[n_కీలకం++] = c;
                        }

                        ఆపు;

                స్థితి PRE_విలువ:
                        ఒకవేళ (strchr(NEWLINE, c)) {
                                state = PRE_కీలకం;
                                పంక్తి++;
                                కీలకం[n_కీలకం] = 0;

                                ఒకవేళ (విలువ)
                                        విలువ[n_విలువ] = 0;

                                /* strip trailing whitespace from key */
                                ఒకవేళ (చివరి_కీలకం_whitespace != SIZE_MAX)
                                        కీలకం[చివరి_కీలకం_whitespace] = 0;

                                r = నెట్టు(fname, పంక్తి, కీలకం, విలువ, userdata);
                                ఒకవేళ (r < 0)
                                        తిరిగి r;

                                n_కీలకం = 0;
                                విలువ = ఖాళీ;
                                n_విలువ = 0;

                        } లేకపోతే ఒకవేళ (c == '\'')
                                state = SINGLE_QUOTE_విలువ;
                        లేకపోతే ఒకవేళ (c == '"')
                                state = DOUBLE_QUOTE_విలువ;
                        లేకపోతే ఒకవేళ (c == '\\')
                                state = విలువ_ESCAPE;
                        లేకపోతే ఒకవేళ (!strchr(WHITESPACE, c)) {
                                state = విలువ_2;

                                ఒకవేళ (!GREEDY_REALLOC(విలువ, n_విలువ+2))
                                        తిరిగి -ENOMEM;

                                విలువ[n_విలువ++] = c;
                        }

                        ఆపు;

                స్థితి విలువ_2:
                        ఒకవేళ (strchr(NEWLINE, c)) {
                                state = PRE_కీలకం;
                                పంక్తి++;

                                కీలకం[n_కీలకం] = 0;

                                ఒకవేళ (విలువ)
                                        విలువ[n_విలువ] = 0;

                                /* Chomp off trailing whitespace from value */
                                ఒకవేళ (చివరి_విలువ_whitespace != SIZE_MAX)
                                        విలువ[చివరి_విలువ_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                ఒకవేళ (చివరి_కీలకం_whitespace != SIZE_MAX)
                                        కీలకం[చివరి_కీలకం_whitespace] = 0;

                                r = నెట్టు(fname, పంక్తి, కీలకం, విలువ, userdata);
                                ఒకవేళ (r < 0)
                                        తిరిగి r;

                                n_కీలకం = 0;
                                విలువ = ఖాళీ;
                                n_విలువ = 0;

                        } లేకపోతే ఒకవేళ (c == '\\') {
                                state = విలువ_ESCAPE;
                                చివరి_విలువ_whitespace = SIZE_MAX;
                        } లేకపోతే {
                                ఒకవేళ (!strchr(WHITESPACE, c))
                                        చివరి_విలువ_whitespace = SIZE_MAX;
                                లేకపోతే ఒకవేళ (చివరి_విలువ_whitespace == SIZE_MAX)
                                        చివరి_విలువ_whitespace = n_విలువ;

                                ఒకవేళ (!GREEDY_REALLOC(విలువ, n_విలువ+2))
                                        తిరిగి -ENOMEM;

                                విలువ[n_విలువ++] = c;
                        }

                        ఆపు;

                స్థితి విలువ_ESCAPE:
                        state = విలువ_2;

                        ఒకవేళ (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                ఒకవేళ (!GREEDY_REALLOC(విలువ, n_విలువ+2))
                                        తిరిగి -ENOMEM;

                                విలువ[n_విలువ++] = c;
                        }
                        ఆపు;

                స్థితి SINGLE_QUOTE_విలువ:
                        ఒకవేళ (c == '\'')
                                state = PRE_విలువ;
                        లేకపోతే {
                                ఒకవేళ (!GREEDY_REALLOC(విలువ, n_విలువ+2))
                                        తిరిగి -ENOMEM;

                                విలువ[n_విలువ++] = c;
                        }

                        ఆపు;

                స్థితి DOUBLE_QUOTE_విలువ:
                        ఒకవేళ (c == '"')
                                state = PRE_విలువ;
                        లేకపోతే ఒకవేళ (c == '\\')
                                state = DOUBLE_QUOTE_విలువ_ESCAPE;
                        లేకపోతే {
                                ఒకవేళ (!GREEDY_REALLOC(విలువ, n_విలువ+2))
                                        తిరిగి -ENOMEM;

                                విలువ[n_విలువ++] = c;
                        }

                        ఆపు;

                స్థితి DOUBLE_QUOTE_విలువ_ESCAPE:
                        state = DOUBLE_QUOTE_విలువ;

                        ఒకవేళ (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                ఒకవేళ (!GREEDY_REALLOC(విలువ, n_విలువ+2))
                                        తిరిగి -ENOMEM;
                                విలువ[n_విలువ++] = c;
                        } లేకపోతే ఒకవేళ (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                ఒకవేళ (!GREEDY_REALLOC(విలువ, n_విలువ+3))
                                        తిరిగి -ENOMEM;
                                విలువ[n_విలువ++] = '\\';
                                విలువ[n_విలువ++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        ఆపు;

                స్థితి COMMENT:
                        ఒకవేళ (c == '\\')
                                state = COMMENT_ESCAPE;
                        లేకపోతే ఒకవేళ (strchr(NEWLINE, c)) {
                                state = PRE_కీలకం;
                                పంక్తి++;
                        }
                        ఆపు;

                స్థితి COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        ఒకవేళ (strchr(NEWLINE, c)) {
                                state = PRE_కీలకం;
                                పంక్తి++;
                        } లేకపోతే
                                state = COMMENT;
                        ఆపు;
                }
        }

        ఒకవేళ (లోపల_సమితి(state,
                   PRE_విలువ,
                   విలువ_2,
                   విలువ_ESCAPE,
                   SINGLE_QUOTE_విలువ,
                   DOUBLE_QUOTE_విలువ,
                   DOUBLE_QUOTE_విలువ_ESCAPE)) {

                కీలకం[n_కీలకం] = 0;

                ఒకవేళ (విలువ)
                        విలువ[n_విలువ] = 0;

                ఒకవేళ (state == విలువ_2)
                        ఒకవేళ (చివరి_విలువ_whitespace != SIZE_MAX)
                                విలువ[చివరి_విలువ_whitespace] = 0;

                /* strip trailing whitespace from key */
                ఒకవేళ (చివరి_కీలకం_whitespace != SIZE_MAX)
                        కీలకం[చివరి_కీలకం_whitespace] = 0;

                r = నెట్టు(fname, పంక్తి, కీలకం, విలువ, userdata);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                విలువ = ఖాళీ;
        }

        తిరిగి 0;
}

స్థిర పూర్ణాంకం check_utf8ness_and_warn(
                స్థిరరాశి అక్షరం *filename, గుర్తులేని పంక్తి,
                స్థిరరాశి అక్షరం *కీలకం, అక్షరం *విలువ) {

        ప్రతిజ్ఞా(కీలకం);

        ఒకవేళ (!utf8_is_valid(కీలకం)) {
                _cleanup_free_ అక్షరం *p = ఖాళీ;

                p = utf8_escape_invalid(కీలకం);
                తిరిగి log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), పంక్తి, p);
        }

        ఒకవేళ (విలువ && !utf8_is_valid(విలువ)) {
                _cleanup_free_ అక్షరం *p = ఖాళీ;

                p = utf8_escape_invalid(విలువ);
                తిరిగి log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), పంక్తి, కీలకం, p);
        }

        తిరిగి 0;
}

స్థిర పూర్ణాంకం parse_env_file_నెట్టు(
                స్థిరరాశి అక్షరం *filename, గుర్తులేని పంక్తి,
                స్థిరరాశి అక్షరం *కీలకం, అక్షరం *విలువ,
                శూన్య *userdata) {

        స్థిరరాశి అక్షరం *k;
        va_list aq, *ap = userdata;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(కీలకం);

        r = check_utf8ness_and_warn(filename, పంక్తి, కీలకం, విలువ);
        ఒకవేళ (r < 0)
                తిరిగి r;

        va_copy(aq, *ap);

        అయితే ((k = va_arg(aq, స్థిరరాశి అక్షరం *))) {
                అక్షరం **v;

                v = va_arg(aq, అక్షరం **);

                ఒకవేళ (streq(కీలకం, k)) {
                        va_end(aq);
                        free_and_replace(*v, విలువ);

                        తిరిగి 1;
                }
        }

        va_end(aq);
        విడుదల(విలువ);

        తిరిగి 0;
}

పూర్ణాంకం parse_env_filev(
                FILE *f,
                స్థిరరాశి అక్షరం *fname,
                va_list ap) {

        పూర్ణాంకం r;
        va_list aq;

        ప్రతిజ్ఞా(f || fname);

        va_copy(aq, ap);
        r = parse_env_file_అంతర్గత(f, fname, parse_env_file_నెట్టు, &aq);
        va_end(aq);
        తిరిగి r;
}

పూర్ణాంకం parse_env_file_fdv(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *fname, va_list ap) {
        _cleanup_fclose_ FILE *f = ఖాళీ;
        va_list aq;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        ఒకవేళ (r < 0)
                తిరిగి r;

        va_copy(aq, ap);
        r = parse_env_file_అంతర్గత(f, fname, parse_env_file_నెట్టు, &aq);
        va_end(aq);
        తిరిగి r;
}

పూర్ణాంకం parse_env_file_sentinel(
                FILE *f,
                స్థిరరాశి అక్షరం *fname,
                ...) {

        va_list ap;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(f || fname);

        va_start(ap, fname);
        r = parse_env_filev(f, fname, ap);
        va_end(ap);

        తిరిగి r;
}

పూర్ణాంకం parse_env_file_fd_sentinel(
                పూర్ణాంకం fd,
                స్థిరరాశి అక్షరం *fname, /* only used for logging */
                ...) {

        va_list ap;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);

        va_start(ap, fname);
        r = parse_env_file_fdv(fd, fname, ap);
        va_end(ap);

        తిరిగి r;
}

పూర్ణాంకం parse_env_datav(
                స్థిరరాశి అక్షరం *దత్తాంశం,
                size_t కొలత,
                స్థిరరాశి అక్షరం *fname, /* only used for logging */
                va_list ap) {

        ప్రతిజ్ఞా(దత్తాంశం);

        ఒకవేళ (కొలత == SIZE_MAX)
                కొలత = strlen_ptr(దత్తాంశం);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((శూన్య*) దత్తాంశం, కొలత, "r");
        ఒకవేళ (!f)
                తిరిగి -ENOMEM;

        తిరిగి parse_env_filev(f, fname, ap);
}

పూర్ణాంకం parse_env_దత్తాంశం_sentinel(
                స్థిరరాశి అక్షరం *దత్తాంశం,
                size_t కొలత,
                స్థిరరాశి అక్షరం *fname, /* only used for logging */
                ...) {

        va_list ap;
        పూర్ణాంకం r;

        va_start(ap, fname);
        r = parse_env_datav(దత్తాంశం, కొలత, fname, ap);
        va_end(ap);

        తిరిగి r;
}

స్థిర పూర్ణాంకం ఎక్కించు_env_file_నెట్టు(
                స్థిరరాశి అక్షరం *filename, గుర్తులేని పంక్తి,
                స్థిరరాశి అక్షరం *కీలకం, అక్షరం *విలువ,
                శూన్య *userdata) {

        అక్షరం ***m = userdata;
        అక్షరం *p;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(కీలకం);

        r = check_utf8ness_and_warn(filename, పంక్తి, కీలకం, విలువ);
        ఒకవేళ (r < 0)
                తిరిగి r;

        p = strjoin(కీలకం, "=", విలువ);
        ఒకవేళ (!p)
                తిరిగి -ENOMEM;

        r = strv_env_replace_consume(m, p);
        ఒకవేళ (r < 0)
                తిరిగి r;

        విడుదల(విలువ);
        తిరిగి 0;
}

పూర్ణాంకం ఎక్కించు_env_file(FILE *f, స్థిరరాశి అక్షరం *fname, అక్షరం ***ret) {
        _cleanup_strv_free_ అక్షరం **m = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(f || fname);
        ప్రతిజ్ఞా(ret);

        r = parse_env_file_అంతర్గత(f, fname, ఎక్కించు_env_file_నెట్టు, &m);
        ఒకవేళ (r < 0)
                తిరిగి r;

        *ret = TAKE_PTR(m);
        తిరిగి 0;
}

స్థిర పూర్ణాంకం ఎక్కించు_env_file_నెట్టు_జతలు(
                స్థిరరాశి అక్షరం *filename, గుర్తులేని పంక్తి,
                స్థిరరాశి అక్షరం *కీలకం, అక్షరం *విలువ,
                శూన్య *userdata) {

        అక్షరం ***m = ASSERT_PTR(userdata);
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(కీలకం);

        r = check_utf8ness_and_warn(filename, పంక్తి, కీలకం, విలువ);
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* Check if the key is present */
        కొరకు (అక్షరం **t = *m; t && *t; t += 2)
                ఒకవేళ (streq(t[0], కీలకం)) {
                        ఒకవేళ (విలువ)
                                తిరిగి free_and_replace(t[1], విలువ);
                        లేకపోతే
                                తిరిగి free_and_strdup(t+1, "");
                }

        r = strv_extend(m, కీలకం);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (విలువ)
                తిరిగి strv_నెట్టు(m, విలువ);
        లేకపోతే
                తిరిగి strv_extend(m, "");
}

పూర్ణాంకం ఎక్కించు_env_file_జతలు(FILE *f, స్థిరరాశి అక్షరం *fname, అక్షరం ***ret) {
        _cleanup_strv_free_ అక్షరం **m = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(f || fname);
        ప్రతిజ్ఞా(ret);

        r = parse_env_file_అంతర్గత(f, fname, ఎక్కించు_env_file_నెట్టు_జతలు, &m);
        ఒకవేళ (r < 0)
                తిరిగి r;

        *ret = TAKE_PTR(m);
        తిరిగి 0;
}

పూర్ణాంకం ఎక్కించు_env_file_జతలు_fd(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *fname, అక్షరం ***ret) {
        _cleanup_fclose_ FILE *f = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి ఎక్కించు_env_file_జతలు(f, fname, ret);
}

స్థిర పూర్ణాంకం విలీనం_env_file_నెట్టు(
                స్థిరరాశి అక్షరం *filename, గుర్తులేని పంక్తి,
                స్థిరరాశి అక్షరం *కీలకం, అక్షరం *విలువ,
                శూన్య *userdata) {

        అక్షరం ***env = ASSERT_PTR(userdata);
        అక్షరం *expanded_విలువ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(కీలకం);

        ఒకవేళ (!విలువ) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), పంక్తి, కీలకం);
                తిరిగి 0;
        }

        ఒకవేళ (!env_name_is_valid(కీలకం)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), పంక్తి, కీలకం);
                విడుదల(విలువ);
                తిరిగి 0;
        }

        r = replace_env(విలువ,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_విలువ);
        ఒకవేళ (r < 0)
                తిరిగి log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), పంక్తి, విలువ);

        free_and_replace(విలువ, expanded_విలువ);

        log_debug("%s:%u: setting %s=%s", filename, పంక్తి, కీలకం, విలువ);

        తిరిగి ఎక్కించు_env_file_నెట్టు(filename, పంక్తి, కీలకం, విలువ, env);
}

పూర్ణాంకం విలీనం_env_file(
                అక్షరం ***env,
                FILE *f,
                స్థిరరాశి అక్షరం *fname) {

        ప్రతిజ్ఞా(env);
        ప్రతిజ్ఞా(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        తిరిగి parse_env_file_అంతర్గత(f, fname, విలీనం_env_file_నెట్టు, env);
}

స్థిర శూన్య env_file_fputs_escaped(FILE *f, స్థిరరాశి అక్షరం *p) {
        ప్రతిజ్ఞా(f);
        ప్రతిజ్ఞా(p);

        flockfile(f);

        ఒకవేళ (string_has_cc(p, ఖాళీ) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                కొరకు (; *p; p++) {
                        ఒకవేళ (strchr(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } లేకపోతే
                fputs_unlocked(p, f);

        funlockfile(f);
}

శూన్య env_file_fputs_assignment(FILE *f, స్థిరరాశి అక్షరం *k, స్థిరరాశి అక్షరం *v) {
        ప్రతిజ్ఞా(f);
        ప్రతిజ్ఞా(k);

        ఒకవేళ (!v)
                తిరిగి;

        fputs(k, f);
        env_file_fputs_escaped(f, v);
        fputc('\n', f);
}

స్థిర శూన్య write_env_var(FILE *f, స్థిరరాశి అక్షరం *v) {
        స్థిరరాశి అక్షరం *p;

        ప్రతిజ్ఞా(f);
        ప్రతిజ్ఞా(v);

        p = strchr(v, '=');
        ఒకవేళ (!p) {
                /* Fallback */
                fputs_unlocked(v, f);
                fputc_unlocked('\n', f);
                తిరిగి;
        }

        p++;
        fwrite_unlocked(v, 1, p-v, f);

        env_file_fputs_escaped(f, p);

        fputc_unlocked('\n', f);
}

పూర్ణాంకం write_env_file_label(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *fname, అక్షరం **శీర్షికలు, అక్షరం **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ప్రతిజ్ఞా(fname);

        ఒకవేళ (FLAGS_సమితి(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        _cleanup_fclose_ FILE *f = ఖాళీ;
        _cleanup_free_ అక్షరం *p = ఖాళీ;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        ఒకవేళ (r < 0) {
                ఒకవేళ (FLAGS_సమితి(flags, WRITE_ENV_FILE_LABEL))
                        (శూన్య) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                తిరిగి r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        ఒకవేళ (FLAGS_సమితి(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ ఖాళీ, /* created= */ true, label_context);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        r = fchmod_umask(fileno(f), 0644);
        ఒకవేళ (r < 0)
                తిరిగి r;

        STRV_FOREACH(i, శీర్షికలు) {
                ప్రతిజ్ఞా(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        ఒకవేళ (r < 0)
                తిరిగి r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        తిరిగి 0;
}

పూర్ణాంకం write_vconsole_conf(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *fname, అక్షరం **l) {
        అక్షరం **శీర్షికలు = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        తిరిగి write_env_file_label(dir_fd, fname, శీర్షికలు, l, WRITE_ENV_FILE_LABEL, /* label_context= */ ఖాళీ);
}
