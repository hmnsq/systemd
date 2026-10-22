/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <sys/stat.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "env-file.h"
#ଅନ୍ତର୍ଭାଵ "env-util.h"
#ଅନ୍ତର୍ଭାଵ "escape.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fileio.h"
#ଅନ୍ତର୍ଭାଵ "fs-util.h"
#ଅନ୍ତର୍ଭାଵ "label-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"
#ଅନ୍ତର୍ଭାଵ "tmpfile-util.h"
#ଅନ୍ତର୍ଭାଵ "utf8.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*ଠେଲ_env_func_t)(
                ଅଚଳ ଅକ୍ଷର *filename,
                unsigned ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *ଚାବି,
                ଅକ୍ଷର *ମୂଲ୍ୟ,
                ଶୂନ୍ୟ *userdata);

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_env_file_internal(
                FILE *f,
                ଅଚଳ ଅକ୍ଷର *fname,
                ଠେଲ_env_func_t ଠେଲ,
                ଶୂନ୍ୟ *userdata) {

        size_t n_ଚାବି = 0, n_ମୂଲ୍ୟ = 0, last_ମୂଲ୍ୟ_whitespace = SIZE_MAX, last_ଚାବି_whitespace = SIZE_MAX;
        _cleanup_free_ ଅକ୍ଷର *contents = ଖାଲି, *ଚାବି = ଖାଲି, *ମୂଲ୍ୟ = ଖାଲି;
        unsigned ଧାଡ଼ି = 1;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଗଣନା {
                PRE_ଚାବି,
                ଚାବି_2,
                PRE_ମୂଲ୍ୟ,
                ମୂଲ୍ୟ_2,
                ମୂଲ୍ୟ_ESCAPE,
                SINGLE_QUOTE_ମୂଲ୍ୟ,
                DOUBLE_QUOTE_ମୂଲ୍ୟ,
                DOUBLE_QUOTE_ମୂଲ୍ୟ_ESCAPE,
                COMMENT,
                COMMENT_ESCAPE
        } state = PRE_ଚାବି;

        ପ୍ରତିଜ୍ଞା(f || fname);
        ପ୍ରତିଜ୍ଞା(ଠେଲ);

        ଯଦି (f)
                r = read_full_stream(f, &contents, ଖାଲି);
        ଅନ୍ୟଥା
                r = read_full_file(fname, &contents, ଖାଲି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ପାଇଁ (ଅକ୍ଷର *p = contents; *p; p++) {
                ଅକ୍ଷର c = *p;

                ବାଛ (state) {

                ସ୍ଥିତି PRE_ଚାବି:
                        ଯଦି (strchr(COMMENTS, c))
                                state = COMMENT;
                        ଅନ୍ୟଥା ଯଦି (!strchr(WHITESPACE, c)) {
                                state = ଚାବି_2;
                                last_ଚାବି_whitespace = SIZE_MAX;

                                ଯଦି (!GREEDY_REALLOC(ଚାବି, n_ଚାବି+2))
                                        ଫେରନ୍ତୁ -ENOMEM;

                                ଚାବି[n_ଚାବି++] = c;
                        }
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି ଚାବି_2:
                        ଯଦି (strchr(NEWLINE, c)) {
                                state = PRE_ଚାବି;
                                ଧାଡ଼ି++;
                                n_ଚାବି = 0;
                        } ଅନ୍ୟଥା ଯଦି (c == '=') {
                                state = PRE_ମୂଲ୍ୟ;
                                last_ମୂଲ୍ୟ_whitespace = SIZE_MAX;
                        } ଅନ୍ୟଥା {
                                ଯଦି (!strchr(WHITESPACE, c))
                                        last_ଚାବି_whitespace = SIZE_MAX;
                                ଅନ୍ୟଥା ଯଦି (last_ଚାବି_whitespace == SIZE_MAX)
                                         last_ଚାବି_whitespace = n_ଚାବି;

                                ଯଦି (!GREEDY_REALLOC(ଚାବି, n_ଚାବି+2))
                                        ଫେରନ୍ତୁ -ENOMEM;

                                ଚାବି[n_ଚାବି++] = c;
                        }

                        ଭାଙ୍ଗ;

                ସ୍ଥିତି PRE_ମୂଲ୍ୟ:
                        ଯଦି (strchr(NEWLINE, c)) {
                                state = PRE_ଚାବି;
                                ଧାଡ଼ି++;
                                ଚାବି[n_ଚାବି] = 0;

                                ଯଦି (ମୂଲ୍ୟ)
                                        ମୂଲ୍ୟ[n_ମୂଲ୍ୟ] = 0;

                                /* strip trailing whitespace from key */
                                ଯଦି (last_ଚାବି_whitespace != SIZE_MAX)
                                        ଚାବି[last_ଚାବି_whitespace] = 0;

                                r = ଠେଲ(fname, ଧାଡ଼ି, ଚାବି, ମୂଲ୍ୟ, userdata);
                                ଯଦି (r < 0)
                                        ଫେରନ୍ତୁ r;

                                n_ଚାବି = 0;
                                ମୂଲ୍ୟ = ଖାଲି;
                                n_ମୂଲ୍ୟ = 0;

                        } ଅନ୍ୟଥା ଯଦି (c == '\'')
                                state = SINGLE_QUOTE_ମୂଲ୍ୟ;
                        ଅନ୍ୟଥା ଯଦି (c == '"')
                                state = DOUBLE_QUOTE_ମୂଲ୍ୟ;
                        ଅନ୍ୟଥା ଯଦି (c == '\\')
                                state = ମୂଲ୍ୟ_ESCAPE;
                        ଅନ୍ୟଥା ଯଦି (!strchr(WHITESPACE, c)) {
                                state = ମୂଲ୍ୟ_2;

                                ଯଦି (!GREEDY_REALLOC(ମୂଲ୍ୟ, n_ମୂଲ୍ୟ+2))
                                        ଫେରନ୍ତୁ -ENOMEM;

                                ମୂଲ୍ୟ[n_ମୂଲ୍ୟ++] = c;
                        }

                        ଭାଙ୍ଗ;

                ସ୍ଥିତି ମୂଲ୍ୟ_2:
                        ଯଦି (strchr(NEWLINE, c)) {
                                state = PRE_ଚାବି;
                                ଧାଡ଼ି++;

                                ଚାବି[n_ଚାବି] = 0;

                                ଯଦି (ମୂଲ୍ୟ)
                                        ମୂଲ୍ୟ[n_ମୂଲ୍ୟ] = 0;

                                /* Chomp off trailing whitespace from value */
                                ଯଦି (last_ମୂଲ୍ୟ_whitespace != SIZE_MAX)
                                        ମୂଲ୍ୟ[last_ମୂଲ୍ୟ_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                ଯଦି (last_ଚାବି_whitespace != SIZE_MAX)
                                        ଚାବି[last_ଚାବି_whitespace] = 0;

                                r = ଠେଲ(fname, ଧାଡ଼ି, ଚାବି, ମୂଲ୍ୟ, userdata);
                                ଯଦି (r < 0)
                                        ଫେରନ୍ତୁ r;

                                n_ଚାବି = 0;
                                ମୂଲ୍ୟ = ଖାଲି;
                                n_ମୂଲ୍ୟ = 0;

                        } ଅନ୍ୟଥା ଯଦି (c == '\\') {
                                state = ମୂଲ୍ୟ_ESCAPE;
                                last_ମୂଲ୍ୟ_whitespace = SIZE_MAX;
                        } ଅନ୍ୟଥା {
                                ଯଦି (!strchr(WHITESPACE, c))
                                        last_ମୂଲ୍ୟ_whitespace = SIZE_MAX;
                                ଅନ୍ୟଥା ଯଦି (last_ମୂଲ୍ୟ_whitespace == SIZE_MAX)
                                        last_ମୂଲ୍ୟ_whitespace = n_ମୂଲ୍ୟ;

                                ଯଦି (!GREEDY_REALLOC(ମୂଲ୍ୟ, n_ମୂଲ୍ୟ+2))
                                        ଫେରନ୍ତୁ -ENOMEM;

                                ମୂଲ୍ୟ[n_ମୂଲ୍ୟ++] = c;
                        }

                        ଭାଙ୍ଗ;

                ସ୍ଥିତି ମୂଲ୍ୟ_ESCAPE:
                        state = ମୂଲ୍ୟ_2;

                        ଯଦି (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                ଯଦି (!GREEDY_REALLOC(ମୂଲ୍ୟ, n_ମୂଲ୍ୟ+2))
                                        ଫେରନ୍ତୁ -ENOMEM;

                                ମୂଲ୍ୟ[n_ମୂଲ୍ୟ++] = c;
                        }
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି SINGLE_QUOTE_ମୂଲ୍ୟ:
                        ଯଦି (c == '\'')
                                state = PRE_ମୂଲ୍ୟ;
                        ଅନ୍ୟଥା {
                                ଯଦି (!GREEDY_REALLOC(ମୂଲ୍ୟ, n_ମୂଲ୍ୟ+2))
                                        ଫେରନ୍ତୁ -ENOMEM;

                                ମୂଲ୍ୟ[n_ମୂଲ୍ୟ++] = c;
                        }

                        ଭାଙ୍ଗ;

                ସ୍ଥିତି DOUBLE_QUOTE_ମୂଲ୍ୟ:
                        ଯଦି (c == '"')
                                state = PRE_ମୂଲ୍ୟ;
                        ଅନ୍ୟଥା ଯଦି (c == '\\')
                                state = DOUBLE_QUOTE_ମୂଲ୍ୟ_ESCAPE;
                        ଅନ୍ୟଥା {
                                ଯଦି (!GREEDY_REALLOC(ମୂଲ୍ୟ, n_ମୂଲ୍ୟ+2))
                                        ଫେରନ୍ତୁ -ENOMEM;

                                ମୂଲ୍ୟ[n_ମୂଲ୍ୟ++] = c;
                        }

                        ଭାଙ୍ଗ;

                ସ୍ଥିତି DOUBLE_QUOTE_ମୂଲ୍ୟ_ESCAPE:
                        state = DOUBLE_QUOTE_ମୂଲ୍ୟ;

                        ଯଦି (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                ଯଦି (!GREEDY_REALLOC(ମୂଲ୍ୟ, n_ମୂଲ୍ୟ+2))
                                        ଫେରନ୍ତୁ -ENOMEM;
                                ମୂଲ୍ୟ[n_ମୂଲ୍ୟ++] = c;
                        } ଅନ୍ୟଥା ଯଦି (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                ଯଦି (!GREEDY_REALLOC(ମୂଲ୍ୟ, n_ମୂଲ୍ୟ+3))
                                        ଫେରନ୍ତୁ -ENOMEM;
                                ମୂଲ୍ୟ[n_ମୂଲ୍ୟ++] = '\\';
                                ମୂଲ୍ୟ[n_ମୂଲ୍ୟ++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି COMMENT:
                        ଯଦି (c == '\\')
                                state = COMMENT_ESCAPE;
                        ଅନ୍ୟଥା ଯଦି (strchr(NEWLINE, c)) {
                                state = PRE_ଚାବି;
                                ଧାଡ଼ି++;
                        }
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        ଯଦି (strchr(NEWLINE, c)) {
                                state = PRE_ଚାବି;
                                ଧାଡ଼ି++;
                        } ଅନ୍ୟଥା
                                state = COMMENT;
                        ଭାଙ୍ଗ;
                }
        }

        ଯଦି (IN_SET(state,
                   PRE_ମୂଲ୍ୟ,
                   ମୂଲ୍ୟ_2,
                   ମୂଲ୍ୟ_ESCAPE,
                   SINGLE_QUOTE_ମୂଲ୍ୟ,
                   DOUBLE_QUOTE_ମୂଲ୍ୟ,
                   DOUBLE_QUOTE_ମୂଲ୍ୟ_ESCAPE)) {

                ଚାବି[n_ଚାବି] = 0;

                ଯଦି (ମୂଲ୍ୟ)
                        ମୂଲ୍ୟ[n_ମୂଲ୍ୟ] = 0;

                ଯଦି (state == ମୂଲ୍ୟ_2)
                        ଯଦି (last_ମୂଲ୍ୟ_whitespace != SIZE_MAX)
                                ମୂଲ୍ୟ[last_ମୂଲ୍ୟ_whitespace] = 0;

                /* strip trailing whitespace from key */
                ଯଦି (last_ଚାବି_whitespace != SIZE_MAX)
                        ଚାବି[last_ଚାବି_whitespace] = 0;

                r = ଠେଲ(fname, ଧାଡ଼ି, ଚାବି, ମୂଲ୍ୟ, userdata);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ମୂଲ୍ୟ = ଖାଲି;
        }

        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_utf8ness_and_warn(
                ଅଚଳ ଅକ୍ଷର *filename, unsigned ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅକ୍ଷର *ମୂଲ୍ୟ) {

        ପ୍ରତିଜ୍ଞା(ଚାବି);

        ଯଦି (!utf8_is_valid(ଚାବି)) {
                _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;

                p = utf8_escape_invalid(ଚାବି);
                ଫେରନ୍ତୁ log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), ଧାଡ଼ି, p);
        }

        ଯଦି (ମୂଲ୍ୟ && !utf8_is_valid(ମୂଲ୍ୟ)) {
                _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;

                p = utf8_escape_invalid(ମୂଲ୍ୟ);
                ଫେରନ୍ତୁ log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), ଧାଡ଼ି, ଚାବି, p);
        }

        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_env_file_ଠେଲ(
                ଅଚଳ ଅକ୍ଷର *filename, unsigned ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅକ୍ଷର *ମୂଲ୍ୟ,
                ଶୂନ୍ୟ *userdata) {

        ଅଚଳ ଅକ୍ଷର *k;
        va_list aq, *ap = userdata;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ଚାବି);

        r = check_utf8ness_and_warn(filename, ଧାଡ଼ି, ଚାବି, ମୂଲ୍ୟ);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        va_copy(aq, *ap);

        ଯେତେବେଳେ ((k = va_arg(aq, ଅଚଳ ଅକ୍ଷର *))) {
                ଅକ୍ଷର **v;

                v = va_arg(aq, ଅକ୍ଷର **);

                ଯଦି (streq(ଚାବି, k)) {
                        va_end(aq);
                        free_and_replace(*v, ମୂଲ୍ୟ);

                        ଫେରନ୍ତୁ 1;
                }
        }

        va_end(aq);
        ମୁକ୍ତ(ମୂଲ୍ୟ);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_env_filev(
                FILE *f,
                ଅଚଳ ଅକ୍ଷର *fname,
                va_list ap) {

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;
        va_list aq;

        ପ୍ରତିଜ୍ଞା(f || fname);

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_ଠେଲ, &aq);
        va_end(aq);
        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_env_file_fdv(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *fname, va_list ap) {
        _cleanup_fclose_ FILE *f = ଖାଲି;
        va_list aq;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_ଠେଲ, &aq);
        va_end(aq);
        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_env_file_sentinel(
                FILE *f,
                ଅଚଳ ଅକ୍ଷର *fname,
                ...) {

        va_list ap;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(f || fname);

        va_start(ap, fname);
        r = parse_env_filev(f, fname, ap);
        va_end(ap);

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_env_file_fd_sentinel(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd,
                ଅଚଳ ଅକ୍ଷର *fname, /* only used for logging */
                ...) {

        va_list ap;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        va_start(ap, fname);
        r = parse_env_file_fdv(fd, fname, ap);
        va_end(ap);

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_env_datav(
                ଅଚଳ ଅକ୍ଷର *data,
                size_t size,
                ଅଚଳ ଅକ୍ଷର *fname, /* only used for logging */
                va_list ap) {

        ପ୍ରତିଜ୍ଞା(data);

        ଯଦି (size == SIZE_MAX)
                size = strlen_ptr(data);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((ଶୂନ୍ୟ*) data, size, "r");
        ଯଦି (!f)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ parse_env_filev(f, fname, ap);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_env_data_sentinel(
                ଅଚଳ ଅକ୍ଷର *data,
                size_t size,
                ଅଚଳ ଅକ୍ଷର *fname, /* only used for logging */
                ...) {

        va_list ap;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        va_start(ap, fname);
        r = parse_env_datav(data, size, fname, ap);
        va_end(ap);

        ଫେରନ୍ତୁ r;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା load_env_file_ଠେଲ(
                ଅଚଳ ଅକ୍ଷର *filename, unsigned ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅକ୍ଷର *ମୂଲ୍ୟ,
                ଶୂନ୍ୟ *userdata) {

        ଅକ୍ଷର ***m = userdata;
        ଅକ୍ଷର *p;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ଚାବି);

        r = check_utf8ness_and_warn(filename, ଧାଡ଼ି, ଚାବି, ମୂଲ୍ୟ);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        p = strjoin(ଚାବି, "=", ମୂଲ୍ୟ);
        ଯଦି (!p)
                ଫେରନ୍ତୁ -ENOMEM;

        r = strv_env_replace_consume(m, p);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ମୁକ୍ତ(ମୂଲ୍ୟ);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା load_env_file(FILE *f, ଅଚଳ ଅକ୍ଷର *fname, ଅକ୍ଷର ***ret) {
        _cleanup_strv_free_ ଅକ୍ଷର **m = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(f || fname);
        ପ୍ରତିଜ୍ଞା(ret);

        r = parse_env_file_internal(f, fname, load_env_file_ଠେଲ, &m);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        *ret = TAKE_PTR(m);
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା load_env_file_ଠେଲ_pairs(
                ଅଚଳ ଅକ୍ଷର *filename, unsigned ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅକ୍ଷର *ମୂଲ୍ୟ,
                ଶୂନ୍ୟ *userdata) {

        ଅକ୍ଷର ***m = ASSERT_PTR(userdata);
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ଚାବି);

        r = check_utf8ness_and_warn(filename, ଧାଡ଼ି, ଚାବି, ମୂଲ୍ୟ);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* Check if the key is present */
        ପାଇଁ (ଅକ୍ଷର **t = *m; t && *t; t += 2)
                ଯଦି (streq(t[0], ଚାବି)) {
                        ଯଦି (ମୂଲ୍ୟ)
                                ଫେରନ୍ତୁ free_and_replace(t[1], ମୂଲ୍ୟ);
                        ଅନ୍ୟଥା
                                ଫେରନ୍ତୁ free_and_strdup(t+1, "");
                }

        r = strv_extend(m, ଚାବି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ମୂଲ୍ୟ)
                ଫେରନ୍ତୁ strv_ଠେଲ(m, ମୂଲ୍ୟ);
        ଅନ୍ୟଥା
                ଫେରନ୍ତୁ strv_extend(m, "");
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା load_env_file_pairs(FILE *f, ଅଚଳ ଅକ୍ଷର *fname, ଅକ୍ଷର ***ret) {
        _cleanup_strv_free_ ଅକ୍ଷର **m = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(f || fname);
        ପ୍ରତିଜ୍ଞା(ret);

        r = parse_env_file_internal(f, fname, load_env_file_ଠେଲ_pairs, &m);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        *ret = TAKE_PTR(m);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା load_env_file_pairs_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *fname, ଅକ୍ଷର ***ret) {
        _cleanup_fclose_ FILE *f = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ load_env_file_pairs(f, fname, ret);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା merge_env_file_ଠେଲ(
                ଅଚଳ ଅକ୍ଷର *filename, unsigned ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅକ୍ଷର *ମୂଲ୍ୟ,
                ଶୂନ୍ୟ *userdata) {

        ଅକ୍ଷର ***env = ASSERT_PTR(userdata);
        ଅକ୍ଷର *expanded_ମୂଲ୍ୟ;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ଚାବି);

        ଯଦି (!ମୂଲ୍ୟ) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), ଧାଡ଼ି, ଚାବି);
                ଫେରନ୍ତୁ 0;
        }

        ଯଦି (!env_name_is_valid(ଚାବି)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), ଧାଡ଼ି, ଚାବି);
                ମୁକ୍ତ(ମୂଲ୍ୟ);
                ଫେରନ୍ତୁ 0;
        }

        r = replace_env(ମୂଲ୍ୟ,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_ମୂଲ୍ୟ);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), ଧାଡ଼ି, ମୂଲ୍ୟ);

        free_and_replace(ମୂଲ୍ୟ, expanded_ମୂଲ୍ୟ);

        log_debug("%s:%u: setting %s=%s", filename, ଧାଡ଼ି, ଚାବି, ମୂଲ୍ୟ);

        ଫେରନ୍ତୁ load_env_file_ଠେଲ(filename, ଧାଡ଼ି, ଚାବି, ମୂଲ୍ୟ, env);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା merge_env_file(
                ଅକ୍ଷର ***env,
                FILE *f,
                ଅଚଳ ଅକ୍ଷର *fname) {

        ପ୍ରତିଜ୍ଞା(env);
        ପ୍ରତିଜ୍ଞା(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        ଫେରନ୍ତୁ parse_env_file_internal(f, fname, merge_env_file_ଠେଲ, env);
}

ସ୍ଥିର ଶୂନ୍ୟ env_file_fputs_escaped(FILE *f, ଅଚଳ ଅକ୍ଷର *p) {
        ପ୍ରତିଜ୍ଞା(f);
        ପ୍ରତିଜ୍ଞା(p);

        flockfile(f);

        ଯଦି (string_has_cc(p, ଖାଲି) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                ପାଇଁ (; *p; p++) {
                        ଯଦି (strchr(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } ଅନ୍ୟଥା
                fputs_unlocked(p, f);

        funlockfile(f);
}

ଶୂନ୍ୟ env_file_fputs_assignment(FILE *f, ଅଚଳ ଅକ୍ଷର *k, ଅଚଳ ଅକ୍ଷର *v) {
        ପ୍ରତିଜ୍ଞା(f);
        ପ୍ରତିଜ୍ଞା(k);

        ଯଦି (!v)
                ଫେରନ୍ତୁ;

        fputs(k, f);
        env_file_fputs_escaped(f, v);
        fputc('\n', f);
}

ସ୍ଥିର ଶୂନ୍ୟ write_env_var(FILE *f, ଅଚଳ ଅକ୍ଷର *v) {
        ଅଚଳ ଅକ୍ଷର *p;

        ପ୍ରତିଜ୍ଞା(f);
        ପ୍ରତିଜ୍ଞା(v);

        p = strchr(v, '=');
        ଯଦି (!p) {
                /* Fallback */
                fputs_unlocked(v, f);
                fputc_unlocked('\n', f);
                ଫେରନ୍ତୁ;
        }

        p++;
        fwrite_unlocked(v, 1, p-v, f);

        env_file_fputs_escaped(f, p);

        fputc_unlocked('\n', f);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା write_env_file_label(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *fname, ଅକ୍ଷର **headers, ଅକ୍ଷର **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ପ୍ରତିଜ୍ଞା(fname);

        ଯଦି (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        _cleanup_fclose_ FILE *f = ଖାଲି;
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        ଯଦି (r < 0) {
                ଯଦି (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL))
                        (ଶୂନ୍ୟ) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                ଫେରନ୍ତୁ r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        ଯଦି (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ ଖାଲି, /* created= */ true, label_context);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        r = fchmod_umask(fileno(f), 0644);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        STRV_FOREACH(i, headers) {
                ପ୍ରତିଜ୍ଞା(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା write_vconsole_conf(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *fname, ଅକ୍ଷର **l) {
        ଅକ୍ଷର **headers = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        ଫେରନ୍ତୁ write_env_file_label(dir_fd, fname, headers, l, WRITE_ENV_FILE_LABEL, /* label_context= */ ଖାଲି);
}
