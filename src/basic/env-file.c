/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <sys/stat.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "env-file.h"
#अन्तर्भाव "env-util.h"
#अन्तर्भाव "escape.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "fs-util.h"
#अन्तर्भाव "label-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"
#अन्तर्भाव "tmpfile-util.h"
#अन्तर्भाव "utf8.h"

प्रकारनिर्देश पूर्णाङ्क (*थप_env_func_t)(
                अचर वर्ण *filename,
                अचिन्हित पङ्क्ति,
                अचर वर्ण *साँचो,
                वर्ण *मान,
                शून्य *userdata);

स्थिर पूर्णाङ्क parse_env_file_internal(
                खाता *f,
                अचर वर्ण *fname,
                थप_env_func_t थप,
                शून्य *userdata) {

        माप_प्रकार n_साँचो = 0, n_मान = 0, last_मान_whitespace = SIZE_MAX, last_साँचो_whitespace = SIZE_MAX;
        _cleanup_free_ वर्ण *contents = रिक्त, *साँचो = रिक्त, *मान = रिक्त;
        अचिन्हित पङ्क्ति = 1;
        पूर्णाङ्क r;

        गणना {
                PRE_साँचो,
                साँचो_2,
                PRE_मान,
                मान_2,
                मान_ESCAPE,
                SINGLE_QUOTE_मान,
                DOUBLE_QUOTE_मान,
                DOUBLE_QUOTE_मान_ESCAPE,
                COMMENT,
                COMMENT_ESCAPE
        } state = PRE_साँचो;

        प्रतिज्ञा(f || fname);
        प्रतिज्ञा(थप);

        यदि (f)
                r = read_full_stream(f, &contents, रिक्त);
        अन्यथा
                r = read_full_file(fname, &contents, रिक्त);
        यदि (r < 0)
                फर्काऊ r;

        लागि (वर्ण *p = contents; *p; p++) {
                वर्ण c = *p;

                विकल्प (state) {

                पक्ष PRE_साँचो:
                        यदि (म_अक्षर(COMMENTS, c))
                                state = COMMENT;
                        अन्यथा यदि (!म_अक्षर(WHITESPACE, c)) {
                                state = साँचो_2;
                                last_साँचो_whitespace = SIZE_MAX;

                                यदि (!GREEDY_REALLOC(साँचो, n_साँचो+2))
                                        फर्काऊ -ENOMEM;

                                साँचो[n_साँचो++] = c;
                        }
                        रोक;

                पक्ष साँचो_2:
                        यदि (म_अक्षर(NEWLINE, c)) {
                                state = PRE_साँचो;
                                पङ्क्ति++;
                                n_साँचो = 0;
                        } अन्यथा यदि (c == '=') {
                                state = PRE_मान;
                                last_मान_whitespace = SIZE_MAX;
                        } अन्यथा {
                                यदि (!म_अक्षर(WHITESPACE, c))
                                        last_साँचो_whitespace = SIZE_MAX;
                                अन्यथा यदि (last_साँचो_whitespace == SIZE_MAX)
                                         last_साँचो_whitespace = n_साँचो;

                                यदि (!GREEDY_REALLOC(साँचो, n_साँचो+2))
                                        फर्काऊ -ENOMEM;

                                साँचो[n_साँचो++] = c;
                        }

                        रोक;

                पक्ष PRE_मान:
                        यदि (म_अक्षर(NEWLINE, c)) {
                                state = PRE_साँचो;
                                पङ्क्ति++;
                                साँचो[n_साँचो] = 0;

                                यदि (मान)
                                        मान[n_मान] = 0;

                                /* strip trailing whitespace from key */
                                यदि (last_साँचो_whitespace != SIZE_MAX)
                                        साँचो[last_साँचो_whitespace] = 0;

                                r = थप(fname, पङ्क्ति, साँचो, मान, userdata);
                                यदि (r < 0)
                                        फर्काऊ r;

                                n_साँचो = 0;
                                मान = रिक्त;
                                n_मान = 0;

                        } अन्यथा यदि (c == '\'')
                                state = SINGLE_QUOTE_मान;
                        अन्यथा यदि (c == '"')
                                state = DOUBLE_QUOTE_मान;
                        अन्यथा यदि (c == '\\')
                                state = मान_ESCAPE;
                        अन्यथा यदि (!म_अक्षर(WHITESPACE, c)) {
                                state = मान_2;

                                यदि (!GREEDY_REALLOC(मान, n_मान+2))
                                        फर्काऊ -ENOMEM;

                                मान[n_मान++] = c;
                        }

                        रोक;

                पक्ष मान_2:
                        यदि (म_अक्षर(NEWLINE, c)) {
                                state = PRE_साँचो;
                                पङ्क्ति++;

                                साँचो[n_साँचो] = 0;

                                यदि (मान)
                                        मान[n_मान] = 0;

                                /* Chomp off trailing whitespace from value */
                                यदि (last_मान_whitespace != SIZE_MAX)
                                        मान[last_मान_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                यदि (last_साँचो_whitespace != SIZE_MAX)
                                        साँचो[last_साँचो_whitespace] = 0;

                                r = थप(fname, पङ्क्ति, साँचो, मान, userdata);
                                यदि (r < 0)
                                        फर्काऊ r;

                                n_साँचो = 0;
                                मान = रिक्त;
                                n_मान = 0;

                        } अन्यथा यदि (c == '\\') {
                                state = मान_ESCAPE;
                                last_मान_whitespace = SIZE_MAX;
                        } अन्यथा {
                                यदि (!म_अक्षर(WHITESPACE, c))
                                        last_मान_whitespace = SIZE_MAX;
                                अन्यथा यदि (last_मान_whitespace == SIZE_MAX)
                                        last_मान_whitespace = n_मान;

                                यदि (!GREEDY_REALLOC(मान, n_मान+2))
                                        फर्काऊ -ENOMEM;

                                मान[n_मान++] = c;
                        }

                        रोक;

                पक्ष मान_ESCAPE:
                        state = मान_2;

                        यदि (!म_अक्षर(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                यदि (!GREEDY_REALLOC(मान, n_मान+2))
                                        फर्काऊ -ENOMEM;

                                मान[n_मान++] = c;
                        }
                        रोक;

                पक्ष SINGLE_QUOTE_मान:
                        यदि (c == '\'')
                                state = PRE_मान;
                        अन्यथा {
                                यदि (!GREEDY_REALLOC(मान, n_मान+2))
                                        फर्काऊ -ENOMEM;

                                मान[n_मान++] = c;
                        }

                        रोक;

                पक्ष DOUBLE_QUOTE_मान:
                        यदि (c == '"')
                                state = PRE_मान;
                        अन्यथा यदि (c == '\\')
                                state = DOUBLE_QUOTE_मान_ESCAPE;
                        अन्यथा {
                                यदि (!GREEDY_REALLOC(मान, n_मान+2))
                                        फर्काऊ -ENOMEM;

                                मान[n_मान++] = c;
                        }

                        रोक;

                पक्ष DOUBLE_QUOTE_मान_ESCAPE:
                        state = DOUBLE_QUOTE_मान;

                        यदि (म_अक्षर(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                यदि (!GREEDY_REALLOC(मान, n_मान+2))
                                        फर्काऊ -ENOMEM;
                                मान[n_मान++] = c;
                        } अन्यथा यदि (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                यदि (!GREEDY_REALLOC(मान, n_मान+3))
                                        फर्काऊ -ENOMEM;
                                मान[n_मान++] = '\\';
                                मान[n_मान++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        रोक;

                पक्ष COMMENT:
                        यदि (c == '\\')
                                state = COMMENT_ESCAPE;
                        अन्यथा यदि (म_अक्षर(NEWLINE, c)) {
                                state = PRE_साँचो;
                                पङ्क्ति++;
                        }
                        रोक;

                पक्ष COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        यदि (म_अक्षर(NEWLINE, c)) {
                                state = PRE_साँचो;
                                पङ्क्ति++;
                        } अन्यथा
                                state = COMMENT;
                        रोक;
                }
        }

        यदि (IN_SET(state,
                   PRE_मान,
                   मान_2,
                   मान_ESCAPE,
                   SINGLE_QUOTE_मान,
                   DOUBLE_QUOTE_मान,
                   DOUBLE_QUOTE_मान_ESCAPE)) {

                साँचो[n_साँचो] = 0;

                यदि (मान)
                        मान[n_मान] = 0;

                यदि (state == मान_2)
                        यदि (last_मान_whitespace != SIZE_MAX)
                                मान[last_मान_whitespace] = 0;

                /* strip trailing whitespace from key */
                यदि (last_साँचो_whitespace != SIZE_MAX)
                        साँचो[last_साँचो_whitespace] = 0;

                r = थप(fname, पङ्क्ति, साँचो, मान, userdata);
                यदि (r < 0)
                        फर्काऊ r;

                मान = रिक्त;
        }

        फर्काऊ 0;
}

स्थिर पूर्णाङ्क check_utf8ness_and_warn(
                अचर वर्ण *filename, अचिन्हित पङ्क्ति,
                अचर वर्ण *साँचो, वर्ण *मान) {

        प्रतिज्ञा(साँचो);

        यदि (!utf8_is_valid(साँचो)) {
                _cleanup_free_ वर्ण *p = रिक्त;

                p = utf8_escape_invalid(साँचो);
                फर्काऊ log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), पङ्क्ति, p);
        }

        यदि (मान && !utf8_is_valid(मान)) {
                _cleanup_free_ वर्ण *p = रिक्त;

                p = utf8_escape_invalid(मान);
                फर्काऊ log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), पङ्क्ति, साँचो, p);
        }

        फर्काऊ 0;
}

स्थिर पूर्णाङ्क parse_env_file_थप(
                अचर वर्ण *filename, अचिन्हित पङ्क्ति,
                अचर वर्ण *साँचो, वर्ण *मान,
                शून्य *userdata) {

        अचर वर्ण *k;
        बहु_सूची aq, *ap = userdata;
        पूर्णाङ्क r;

        प्रतिज्ञा(साँचो);

        r = check_utf8ness_and_warn(filename, पङ्क्ति, साँचो, मान);
        यदि (r < 0)
                फर्काऊ r;

        va_copy(aq, *ap);

        जबसम्म ((k = बहु_तर्क(aq, अचर वर्ण *))) {
                वर्ण **v;

                v = बहु_तर्क(aq, वर्ण **);

                यदि (streq(साँचो, k)) {
                        बहु_पूर्ण(aq);
                        free_and_replace(*v, मान);

                        फर्काऊ 1;
                }
        }

        बहु_पूर्ण(aq);
        मुक्त(मान);

        फर्काऊ 0;
}

पूर्णाङ्क parse_env_filev(
                खाता *f,
                अचर वर्ण *fname,
                बहु_सूची ap) {

        पूर्णाङ्क r;
        बहु_सूची aq;

        प्रतिज्ञा(f || fname);

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_थप, &aq);
        बहु_पूर्ण(aq);
        फर्काऊ r;
}

पूर्णाङ्क parse_env_file_fdv(पूर्णाङ्क fd, अचर वर्ण *fname, बहु_सूची ap) {
        _cleanup_fclose_ खाता *f = रिक्त;
        बहु_सूची aq;
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        यदि (r < 0)
                फर्काऊ r;

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_थप, &aq);
        बहु_पूर्ण(aq);
        फर्काऊ r;
}

पूर्णाङ्क parse_env_file_sentinel(
                खाता *f,
                अचर वर्ण *fname,
                ...) {

        बहु_सूची ap;
        पूर्णाङ्क r;

        प्रतिज्ञा(f || fname);

        बहु_शुरू(ap, fname);
        r = parse_env_filev(f, fname, ap);
        बहु_पूर्ण(ap);

        फर्काऊ r;
}

पूर्णाङ्क parse_env_file_fd_sentinel(
                पूर्णाङ्क fd,
                अचर वर्ण *fname, /* only used for logging */
                ...) {

        बहु_सूची ap;
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        बहु_शुरू(ap, fname);
        r = parse_env_file_fdv(fd, fname, ap);
        बहु_पूर्ण(ap);

        फर्काऊ r;
}

पूर्णाङ्क parse_env_datav(
                अचर वर्ण *data,
                माप_प्रकार size,
                अचर वर्ण *fname, /* only used for logging */
                बहु_सूची ap) {

        प्रतिज्ञा(data);

        यदि (size == SIZE_MAX)
                size = strlen_ptr(data);

        _cleanup_fclose_ खाता *f = fmemopen_unlocked((शून्य*) data, size, "r");
        यदि (!f)
                फर्काऊ -ENOMEM;

        फर्काऊ parse_env_filev(f, fname, ap);
}

पूर्णाङ्क parse_env_data_sentinel(
                अचर वर्ण *data,
                माप_प्रकार size,
                अचर वर्ण *fname, /* only used for logging */
                ...) {

        बहु_सूची ap;
        पूर्णाङ्क r;

        बहु_शुरू(ap, fname);
        r = parse_env_datav(data, size, fname, ap);
        बहु_पूर्ण(ap);

        फर्काऊ r;
}

स्थिर पूर्णाङ्क load_env_file_थप(
                अचर वर्ण *filename, अचिन्हित पङ्क्ति,
                अचर वर्ण *साँचो, वर्ण *मान,
                शून्य *userdata) {

        वर्ण ***m = userdata;
        वर्ण *p;
        पूर्णाङ्क r;

        प्रतिज्ञा(साँचो);

        r = check_utf8ness_and_warn(filename, पङ्क्ति, साँचो, मान);
        यदि (r < 0)
                फर्काऊ r;

        p = strjoin(साँचो, "=", मान);
        यदि (!p)
                फर्काऊ -ENOMEM;

        r = strv_env_replace_consume(m, p);
        यदि (r < 0)
                फर्काऊ r;

        मुक्त(मान);
        फर्काऊ 0;
}

पूर्णाङ्क load_env_file(खाता *f, अचर वर्ण *fname, वर्ण ***ret) {
        _cleanup_strv_free_ वर्ण **m = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(f || fname);
        प्रतिज्ञा(ret);

        r = parse_env_file_internal(f, fname, load_env_file_थप, &m);
        यदि (r < 0)
                फर्काऊ r;

        *ret = TAKE_PTR(m);
        फर्काऊ 0;
}

स्थिर पूर्णाङ्क load_env_file_थप_pairs(
                अचर वर्ण *filename, अचिन्हित पङ्क्ति,
                अचर वर्ण *साँचो, वर्ण *मान,
                शून्य *userdata) {

        वर्ण ***m = ASSERT_PTR(userdata);
        पूर्णाङ्क r;

        प्रतिज्ञा(साँचो);

        r = check_utf8ness_and_warn(filename, पङ्क्ति, साँचो, मान);
        यदि (r < 0)
                फर्काऊ r;

        /* Check if the key is present */
        लागि (वर्ण **t = *m; t && *t; t += 2)
                यदि (streq(t[0], साँचो)) {
                        यदि (मान)
                                फर्काऊ free_and_replace(t[1], मान);
                        अन्यथा
                                फर्काऊ free_and_strdup(t+1, "");
                }

        r = strv_extend(m, साँचो);
        यदि (r < 0)
                फर्काऊ r;

        यदि (मान)
                फर्काऊ strv_थप(m, मान);
        अन्यथा
                फर्काऊ strv_extend(m, "");
}

पूर्णाङ्क load_env_file_pairs(खाता *f, अचर वर्ण *fname, वर्ण ***ret) {
        _cleanup_strv_free_ वर्ण **m = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(f || fname);
        प्रतिज्ञा(ret);

        r = parse_env_file_internal(f, fname, load_env_file_थप_pairs, &m);
        यदि (r < 0)
                फर्काऊ r;

        *ret = TAKE_PTR(m);
        फर्काऊ 0;
}

पूर्णाङ्क load_env_file_pairs_fd(पूर्णाङ्क fd, अचर वर्ण *fname, वर्ण ***ret) {
        _cleanup_fclose_ खाता *f = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ load_env_file_pairs(f, fname, ret);
}

स्थिर पूर्णाङ्क merge_env_file_थप(
                अचर वर्ण *filename, अचिन्हित पङ्क्ति,
                अचर वर्ण *साँचो, वर्ण *मान,
                शून्य *userdata) {

        वर्ण ***env = ASSERT_PTR(userdata);
        वर्ण *expanded_मान;
        पूर्णाङ्क r;

        प्रतिज्ञा(साँचो);

        यदि (!मान) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), पङ्क्ति, साँचो);
                फर्काऊ 0;
        }

        यदि (!env_name_is_valid(साँचो)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), पङ्क्ति, साँचो);
                मुक्त(मान);
                फर्काऊ 0;
        }

        r = replace_env(मान,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_मान);
        यदि (r < 0)
                फर्काऊ log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), पङ्क्ति, मान);

        free_and_replace(मान, expanded_मान);

        log_debug("%s:%u: setting %s=%s", filename, पङ्क्ति, साँचो, मान);

        फर्काऊ load_env_file_थप(filename, पङ्क्ति, साँचो, मान, env);
}

पूर्णाङ्क merge_env_file(
                वर्ण ***env,
                खाता *f,
                अचर वर्ण *fname) {

        प्रतिज्ञा(env);
        प्रतिज्ञा(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        फर्काऊ parse_env_file_internal(f, fname, merge_env_file_थप, env);
}

स्थिर शून्य env_file_fputs_escaped(खाता *f, अचर वर्ण *p) {
        प्रतिज्ञा(f);
        प्रतिज्ञा(p);

        flockfile(f);

        यदि (string_has_cc(p, रिक्त) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                लागि (; *p; p++) {
                        यदि (म_अक्षर(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } अन्यथा
                fputs_unlocked(p, f);

        funlockfile(f);
}

शून्य env_file_fputs_assignment(खाता *f, अचर वर्ण *k, अचर वर्ण *v) {
        प्रतिज्ञा(f);
        प्रतिज्ञा(k);

        यदि (!v)
                फर्काऊ;

        ख_माला_दो(k, f);
        env_file_fputs_escaped(f, v);
        ख_अक्षर_दो('\n', f);
}

स्थिर शून्य write_env_var(खाता *f, अचर वर्ण *v) {
        अचर वर्ण *p;

        प्रतिज्ञा(f);
        प्रतिज्ञा(v);

        p = म_अक्षर(v, '=');
        यदि (!p) {
                /* Fallback */
                fputs_unlocked(v, f);
                fputc_unlocked('\n', f);
                फर्काऊ;
        }

        p++;
        fwrite_unlocked(v, 1, p-v, f);

        env_file_fputs_escaped(f, p);

        fputc_unlocked('\n', f);
}

पूर्णाङ्क write_env_file_label(पूर्णाङ्क dir_fd, अचर वर्ण *fname, वर्ण **headers, वर्ण **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        पूर्णाङ्क r;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);
        प्रतिज्ञा(fname);

        यदि (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                यदि (r < 0)
                        फर्काऊ r;
        }

        _cleanup_fclose_ खाता *f = रिक्त;
        _cleanup_free_ वर्ण *p = रिक्त;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        यदि (r < 0) {
                यदि (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL))
                        (शून्य) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                फर्काऊ r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        यदि (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ रिक्त, /* created= */ true, label_context);
                यदि (r < 0)
                        फर्काऊ r;
        }

        r = fchmod_umask(fileno(f), 0644);
        यदि (r < 0)
                फर्काऊ r;

        STRV_FOREACH(i, headers) {
                प्रतिज्ञा(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        यदि (r < 0)
                फर्काऊ r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        फर्काऊ 0;
}

पूर्णाङ्क write_vconsole_conf(पूर्णाङ्क dir_fd, अचर वर्ण *fname, वर्ण **l) {
        वर्ण **headers = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        फर्काऊ write_env_file_label(dir_fd, fname, headers, l, WRITE_ENV_FILE_LABEL, /* label_context= */ रिक्त);
}
