/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <sys/stat.h>
#समावेश <unistd.h>

#समावेश "alloc-util.h"
#समावेश "env-file.h"
#समावेश "env-util.h"
#समावेश "escape.h"
#समावेश "fd-util.h"
#समावेश "fileio.h"
#समावेश "fs-util.h"
#समावेश "label-util.h"
#समावेश "log.h"
#समावेश "string-util.h"
#समावेश "strv.h"
#समावेश "tmpfile-util.h"
#समावेश "utf8.h"

प्रकार पूर्णांक (*धकेलो_env_func_t)(
                स्थिर अक्षर *filename,
                अचिन्हित पंक्ति,
                स्थिर अक्षर *कुंजी,
                अक्षर *मूल्य,
                व्योम *userdata);

अटल पूर्णांक parse_env_file_internal(
                खाता *f,
                स्थिर अक्षर *fname,
                धकेलो_env_func_t धकेलो,
                व्योम *userdata) {

        माप_प्रकार n_कुंजी = 0, n_मूल्य = 0, last_मूल्य_whitespace = SIZE_MAX, last_कुंजी_whitespace = SIZE_MAX;
        _cleanup_free_ अक्षर *contents = शून्य, *कुंजी = शून्य, *मूल्य = शून्य;
        अचिन्हित पंक्ति = 1;
        पूर्णांक r;

        क्रमागत {
                PRE_कुंजी,
                कुंजी_2,
                PRE_मूल्य,
                मूल्य_2,
                मूल्य_ESCAPE,
                SINGLE_QUOTE_मूल्य,
                DOUBLE_QUOTE_मूल्य,
                DOUBLE_QUOTE_मूल्य_ESCAPE,
                COMMENT,
                COMMENT_ESCAPE
        } state = PRE_कुंजी;

        निश्चित(f || fname);
        निश्चित(धकेलो);

        अगर (f)
                r = read_full_stream(f, &contents, शून्य);
        अन्यथा
                r = read_full_file(fname, &contents, शून्य);
        अगर (r < 0)
                वापस r;

        क्रम (अक्षर *p = contents; *p; p++) {
                अक्षर c = *p;

                चयन (state) {

                हाल PRE_कुंजी:
                        अगर (म_अक्षर(COMMENTS, c))
                                state = COMMENT;
                        अन्यथा अगर (!म_अक्षर(WHITESPACE, c)) {
                                state = कुंजी_2;
                                last_कुंजी_whitespace = SIZE_MAX;

                                अगर (!GREEDY_REALLOC(कुंजी, n_कुंजी+2))
                                        वापस -ENOMEM;

                                कुंजी[n_कुंजी++] = c;
                        }
                        अवरोध;

                हाल कुंजी_2:
                        अगर (म_अक्षर(NEWLINE, c)) {
                                state = PRE_कुंजी;
                                पंक्ति++;
                                n_कुंजी = 0;
                        } अन्यथा अगर (c == '=') {
                                state = PRE_मूल्य;
                                last_मूल्य_whitespace = SIZE_MAX;
                        } अन्यथा {
                                अगर (!म_अक्षर(WHITESPACE, c))
                                        last_कुंजी_whitespace = SIZE_MAX;
                                अन्यथा अगर (last_कुंजी_whitespace == SIZE_MAX)
                                         last_कुंजी_whitespace = n_कुंजी;

                                अगर (!GREEDY_REALLOC(कुंजी, n_कुंजी+2))
                                        वापस -ENOMEM;

                                कुंजी[n_कुंजी++] = c;
                        }

                        अवरोध;

                हाल PRE_मूल्य:
                        अगर (म_अक्षर(NEWLINE, c)) {
                                state = PRE_कुंजी;
                                पंक्ति++;
                                कुंजी[n_कुंजी] = 0;

                                अगर (मूल्य)
                                        मूल्य[n_मूल्य] = 0;

                                /* strip trailing whitespace from key */
                                अगर (last_कुंजी_whitespace != SIZE_MAX)
                                        कुंजी[last_कुंजी_whitespace] = 0;

                                r = धकेलो(fname, पंक्ति, कुंजी, मूल्य, userdata);
                                अगर (r < 0)
                                        वापस r;

                                n_कुंजी = 0;
                                मूल्य = शून्य;
                                n_मूल्य = 0;

                        } अन्यथा अगर (c == '\'')
                                state = SINGLE_QUOTE_मूल्य;
                        अन्यथा अगर (c == '"')
                                state = DOUBLE_QUOTE_मूल्य;
                        अन्यथा अगर (c == '\\')
                                state = मूल्य_ESCAPE;
                        अन्यथा अगर (!म_अक्षर(WHITESPACE, c)) {
                                state = मूल्य_2;

                                अगर (!GREEDY_REALLOC(मूल्य, n_मूल्य+2))
                                        वापस -ENOMEM;

                                मूल्य[n_मूल्य++] = c;
                        }

                        अवरोध;

                हाल मूल्य_2:
                        अगर (म_अक्षर(NEWLINE, c)) {
                                state = PRE_कुंजी;
                                पंक्ति++;

                                कुंजी[n_कुंजी] = 0;

                                अगर (मूल्य)
                                        मूल्य[n_मूल्य] = 0;

                                /* Chomp off trailing whitespace from value */
                                अगर (last_मूल्य_whitespace != SIZE_MAX)
                                        मूल्य[last_मूल्य_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                अगर (last_कुंजी_whitespace != SIZE_MAX)
                                        कुंजी[last_कुंजी_whitespace] = 0;

                                r = धकेलो(fname, पंक्ति, कुंजी, मूल्य, userdata);
                                अगर (r < 0)
                                        वापस r;

                                n_कुंजी = 0;
                                मूल्य = शून्य;
                                n_मूल्य = 0;

                        } अन्यथा अगर (c == '\\') {
                                state = मूल्य_ESCAPE;
                                last_मूल्य_whitespace = SIZE_MAX;
                        } अन्यथा {
                                अगर (!म_अक्षर(WHITESPACE, c))
                                        last_मूल्य_whitespace = SIZE_MAX;
                                अन्यथा अगर (last_मूल्य_whitespace == SIZE_MAX)
                                        last_मूल्य_whitespace = n_मूल्य;

                                अगर (!GREEDY_REALLOC(मूल्य, n_मूल्य+2))
                                        वापस -ENOMEM;

                                मूल्य[n_मूल्य++] = c;
                        }

                        अवरोध;

                हाल मूल्य_ESCAPE:
                        state = मूल्य_2;

                        अगर (!म_अक्षर(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                अगर (!GREEDY_REALLOC(मूल्य, n_मूल्य+2))
                                        वापस -ENOMEM;

                                मूल्य[n_मूल्य++] = c;
                        }
                        अवरोध;

                हाल SINGLE_QUOTE_मूल्य:
                        अगर (c == '\'')
                                state = PRE_मूल्य;
                        अन्यथा {
                                अगर (!GREEDY_REALLOC(मूल्य, n_मूल्य+2))
                                        वापस -ENOMEM;

                                मूल्य[n_मूल्य++] = c;
                        }

                        अवरोध;

                हाल DOUBLE_QUOTE_मूल्य:
                        अगर (c == '"')
                                state = PRE_मूल्य;
                        अन्यथा अगर (c == '\\')
                                state = DOUBLE_QUOTE_मूल्य_ESCAPE;
                        अन्यथा {
                                अगर (!GREEDY_REALLOC(मूल्य, n_मूल्य+2))
                                        वापस -ENOMEM;

                                मूल्य[n_मूल्य++] = c;
                        }

                        अवरोध;

                हाल DOUBLE_QUOTE_मूल्य_ESCAPE:
                        state = DOUBLE_QUOTE_मूल्य;

                        अगर (म_अक्षर(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                अगर (!GREEDY_REALLOC(मूल्य, n_मूल्य+2))
                                        वापस -ENOMEM;
                                मूल्य[n_मूल्य++] = c;
                        } अन्यथा अगर (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                अगर (!GREEDY_REALLOC(मूल्य, n_मूल्य+3))
                                        वापस -ENOMEM;
                                मूल्य[n_मूल्य++] = '\\';
                                मूल्य[n_मूल्य++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        अवरोध;

                हाल COMMENT:
                        अगर (c == '\\')
                                state = COMMENT_ESCAPE;
                        अन्यथा अगर (म_अक्षर(NEWLINE, c)) {
                                state = PRE_कुंजी;
                                पंक्ति++;
                        }
                        अवरोध;

                हाल COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        अगर (म_अक्षर(NEWLINE, c)) {
                                state = PRE_कुंजी;
                                पंक्ति++;
                        } अन्यथा
                                state = COMMENT;
                        अवरोध;
                }
        }

        अगर (IN_SET(state,
                   PRE_मूल्य,
                   मूल्य_2,
                   मूल्य_ESCAPE,
                   SINGLE_QUOTE_मूल्य,
                   DOUBLE_QUOTE_मूल्य,
                   DOUBLE_QUOTE_मूल्य_ESCAPE)) {

                कुंजी[n_कुंजी] = 0;

                अगर (मूल्य)
                        मूल्य[n_मूल्य] = 0;

                अगर (state == मूल्य_2)
                        अगर (last_मूल्य_whitespace != SIZE_MAX)
                                मूल्य[last_मूल्य_whitespace] = 0;

                /* strip trailing whitespace from key */
                अगर (last_कुंजी_whitespace != SIZE_MAX)
                        कुंजी[last_कुंजी_whitespace] = 0;

                r = धकेलो(fname, पंक्ति, कुंजी, मूल्य, userdata);
                अगर (r < 0)
                        वापस r;

                मूल्य = शून्य;
        }

        वापस 0;
}

अटल पूर्णांक check_utf8ness_and_warn(
                स्थिर अक्षर *filename, अचिन्हित पंक्ति,
                स्थिर अक्षर *कुंजी, अक्षर *मूल्य) {

        निश्चित(कुंजी);

        अगर (!utf8_is_valid(कुंजी)) {
                _cleanup_free_ अक्षर *p = शून्य;

                p = utf8_escape_invalid(कुंजी);
                वापस log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), पंक्ति, p);
        }

        अगर (मूल्य && !utf8_is_valid(मूल्य)) {
                _cleanup_free_ अक्षर *p = शून्य;

                p = utf8_escape_invalid(मूल्य);
                वापस log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), पंक्ति, कुंजी, p);
        }

        वापस 0;
}

अटल पूर्णांक parse_env_file_धकेलो(
                स्थिर अक्षर *filename, अचिन्हित पंक्ति,
                स्थिर अक्षर *कुंजी, अक्षर *मूल्य,
                व्योम *userdata) {

        स्थिर अक्षर *k;
        बहु_सूची aq, *ap = userdata;
        पूर्णांक r;

        निश्चित(कुंजी);

        r = check_utf8ness_and_warn(filename, पंक्ति, कुंजी, मूल्य);
        अगर (r < 0)
                वापस r;

        va_copy(aq, *ap);

        जबतक ((k = बहु_तर्क(aq, स्थिर अक्षर *))) {
                अक्षर **v;

                v = बहु_तर्क(aq, अक्षर **);

                अगर (streq(कुंजी, k)) {
                        बहु_पूर्ण(aq);
                        free_and_replace(*v, मूल्य);

                        वापस 1;
                }
        }

        बहु_पूर्ण(aq);
        मुक्त(मूल्य);

        वापस 0;
}

पूर्णांक parse_env_filev(
                खाता *f,
                स्थिर अक्षर *fname,
                बहु_सूची ap) {

        पूर्णांक r;
        बहु_सूची aq;

        निश्चित(f || fname);

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_धकेलो, &aq);
        बहु_पूर्ण(aq);
        वापस r;
}

पूर्णांक parse_env_file_fdv(पूर्णांक fd, स्थिर अक्षर *fname, बहु_सूची ap) {
        _cleanup_fclose_ खाता *f = शून्य;
        बहु_सूची aq;
        पूर्णांक r;

        निश्चित(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        अगर (r < 0)
                वापस r;

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_धकेलो, &aq);
        बहु_पूर्ण(aq);
        वापस r;
}

पूर्णांक parse_env_file_sentinel(
                खाता *f,
                स्थिर अक्षर *fname,
                ...) {

        बहु_सूची ap;
        पूर्णांक r;

        निश्चित(f || fname);

        बहु_शुरू(ap, fname);
        r = parse_env_filev(f, fname, ap);
        बहु_पूर्ण(ap);

        वापस r;
}

पूर्णांक parse_env_file_fd_sentinel(
                पूर्णांक fd,
                स्थिर अक्षर *fname, /* only used for logging */
                ...) {

        बहु_सूची ap;
        पूर्णांक r;

        निश्चित(fd >= 0);

        बहु_शुरू(ap, fname);
        r = parse_env_file_fdv(fd, fname, ap);
        बहु_पूर्ण(ap);

        वापस r;
}

पूर्णांक parse_env_datav(
                स्थिर अक्षर *तथ्य,
                माप_प्रकार आकार,
                स्थिर अक्षर *fname, /* only used for logging */
                बहु_सूची ap) {

        निश्चित(तथ्य);

        अगर (आकार == SIZE_MAX)
                आकार = strlen_ptr(तथ्य);

        _cleanup_fclose_ खाता *f = fmemopen_unlocked((व्योम*) तथ्य, आकार, "r");
        अगर (!f)
                वापस -ENOMEM;

        वापस parse_env_filev(f, fname, ap);
}

पूर्णांक parse_env_तथ्य_sentinel(
                स्थिर अक्षर *तथ्य,
                माप_प्रकार आकार,
                स्थिर अक्षर *fname, /* only used for logging */
                ...) {

        बहु_सूची ap;
        पूर्णांक r;

        बहु_शुरू(ap, fname);
        r = parse_env_datav(तथ्य, आकार, fname, ap);
        बहु_पूर्ण(ap);

        वापस r;
}

अटल पूर्णांक load_env_file_धकेलो(
                स्थिर अक्षर *filename, अचिन्हित पंक्ति,
                स्थिर अक्षर *कुंजी, अक्षर *मूल्य,
                व्योम *userdata) {

        अक्षर ***m = userdata;
        अक्षर *p;
        पूर्णांक r;

        निश्चित(कुंजी);

        r = check_utf8ness_and_warn(filename, पंक्ति, कुंजी, मूल्य);
        अगर (r < 0)
                वापस r;

        p = strjoin(कुंजी, "=", मूल्य);
        अगर (!p)
                वापस -ENOMEM;

        r = strv_env_replace_consume(m, p);
        अगर (r < 0)
                वापस r;

        मुक्त(मूल्य);
        वापस 0;
}

पूर्णांक load_env_file(खाता *f, स्थिर अक्षर *fname, अक्षर ***ret) {
        _cleanup_strv_free_ अक्षर **m = शून्य;
        पूर्णांक r;

        निश्चित(f || fname);
        निश्चित(ret);

        r = parse_env_file_internal(f, fname, load_env_file_धकेलो, &m);
        अगर (r < 0)
                वापस r;

        *ret = TAKE_PTR(m);
        वापस 0;
}

अटल पूर्णांक load_env_file_धकेलो_pairs(
                स्थिर अक्षर *filename, अचिन्हित पंक्ति,
                स्थिर अक्षर *कुंजी, अक्षर *मूल्य,
                व्योम *userdata) {

        अक्षर ***m = ASSERT_PTR(userdata);
        पूर्णांक r;

        निश्चित(कुंजी);

        r = check_utf8ness_and_warn(filename, पंक्ति, कुंजी, मूल्य);
        अगर (r < 0)
                वापस r;

        /* Check if the key is present */
        क्रम (अक्षर **t = *m; t && *t; t += 2)
                अगर (streq(t[0], कुंजी)) {
                        अगर (मूल्य)
                                वापस free_and_replace(t[1], मूल्य);
                        अन्यथा
                                वापस free_and_strdup(t+1, "");
                }

        r = strv_extend(m, कुंजी);
        अगर (r < 0)
                वापस r;

        अगर (मूल्य)
                वापस strv_धकेलो(m, मूल्य);
        अन्यथा
                वापस strv_extend(m, "");
}

पूर्णांक load_env_file_pairs(खाता *f, स्थिर अक्षर *fname, अक्षर ***ret) {
        _cleanup_strv_free_ अक्षर **m = शून्य;
        पूर्णांक r;

        निश्चित(f || fname);
        निश्चित(ret);

        r = parse_env_file_internal(f, fname, load_env_file_धकेलो_pairs, &m);
        अगर (r < 0)
                वापस r;

        *ret = TAKE_PTR(m);
        वापस 0;
}

पूर्णांक load_env_file_pairs_fd(पूर्णांक fd, स्थिर अक्षर *fname, अक्षर ***ret) {
        _cleanup_fclose_ खाता *f = शून्य;
        पूर्णांक r;

        निश्चित(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        अगर (r < 0)
                वापस r;

        वापस load_env_file_pairs(f, fname, ret);
}

अटल पूर्णांक merge_env_file_धकेलो(
                स्थिर अक्षर *filename, अचिन्हित पंक्ति,
                स्थिर अक्षर *कुंजी, अक्षर *मूल्य,
                व्योम *userdata) {

        अक्षर ***env = ASSERT_PTR(userdata);
        अक्षर *expanded_मूल्य;
        पूर्णांक r;

        निश्चित(कुंजी);

        अगर (!मूल्य) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), पंक्ति, कुंजी);
                वापस 0;
        }

        अगर (!env_name_is_valid(कुंजी)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), पंक्ति, कुंजी);
                मुक्त(मूल्य);
                वापस 0;
        }

        r = replace_env(मूल्य,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_मूल्य);
        अगर (r < 0)
                वापस log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), पंक्ति, मूल्य);

        free_and_replace(मूल्य, expanded_मूल्य);

        log_debug("%s:%u: setting %s=%s", filename, पंक्ति, कुंजी, मूल्य);

        वापस load_env_file_धकेलो(filename, पंक्ति, कुंजी, मूल्य, env);
}

पूर्णांक merge_env_file(
                अक्षर ***env,
                खाता *f,
                स्थिर अक्षर *fname) {

        निश्चित(env);
        निश्चित(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        वापस parse_env_file_internal(f, fname, merge_env_file_धकेलो, env);
}

अटल व्योम env_file_fputs_escaped(खाता *f, स्थिर अक्षर *p) {
        निश्चित(f);
        निश्चित(p);

        flockfile(f);

        अगर (string_has_cc(p, शून्य) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                क्रम (; *p; p++) {
                        अगर (म_अक्षर(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } अन्यथा
                fputs_unlocked(p, f);

        funlockfile(f);
}

व्योम env_file_fputs_assignment(खाता *f, स्थिर अक्षर *k, स्थिर अक्षर *v) {
        निश्चित(f);
        निश्चित(k);

        अगर (!v)
                वापस;

        ख_माला_दो(k, f);
        env_file_fputs_escaped(f, v);
        ख_अक्षर_दो('\n', f);
}

अटल व्योम write_env_var(खाता *f, स्थिर अक्षर *v) {
        स्थिर अक्षर *p;

        निश्चित(f);
        निश्चित(v);

        p = म_अक्षर(v, '=');
        अगर (!p) {
                /* Fallback */
                fputs_unlocked(v, f);
                fputc_unlocked('\n', f);
                वापस;
        }

        p++;
        fwrite_unlocked(v, 1, p-v, f);

        env_file_fputs_escaped(f, p);

        fputc_unlocked('\n', f);
}

पूर्णांक write_env_file_label(पूर्णांक dir_fd, स्थिर अक्षर *fname, अक्षर **headers, अक्षर **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        पूर्णांक r;

        निश्चित(dir_fd >= 0 || dir_fd == AT_FDCWD);
        निश्चित(fname);

        अगर (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                अगर (r < 0)
                        वापस r;
        }

        _cleanup_fclose_ खाता *f = शून्य;
        _cleanup_free_ अक्षर *p = शून्य;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        अगर (r < 0) {
                अगर (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL))
                        (व्योम) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                वापस r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        अगर (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ शून्य, /* created= */ true, label_context);
                अगर (r < 0)
                        वापस r;
        }

        r = fchmod_umask(fileno(f), 0644);
        अगर (r < 0)
                वापस r;

        STRV_FOREACH(i, headers) {
                निश्चित(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        अगर (r < 0)
                वापस r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        वापस 0;
}

पूर्णांक write_vconsole_conf(पूर्णांक dir_fd, स्थिर अक्षर *fname, अक्षर **l) {
        अक्षर **headers = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        वापस write_env_file_label(dir_fd, fname, headers, l, WRITE_ENV_FILE_LABEL, /* label_context= */ शून्य);
}
