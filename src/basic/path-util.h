/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

#परिभाषय मार्गः_विभज_BIN(x) x "sbin:" x "bin"
#परिभाषय मार्गः_विभज_BIN_NULSTR(x) x "sbin\0" x "bin\0"

#परिभाषय मार्गः_MERGED_BIN(x) x "bin"
#परिभाषय मार्गः_MERGED_BIN_NULSTR(x) x "bin\0"

#परिभाषय DEFAULT_मार्गः_WITH_पूर्णम्_SBIN मार्गः_विभज_BIN("/usr/local/") ":" मार्गः_विभज_BIN("/usr/") EXTRA_EXEC_अन्वेषणम्_मार्गः_SUFFIX
#परिभाषय DEFAULT_मार्गः_WITH_LOCAL_SBIN मार्गः_विभज_BIN("/usr/local/") ":" मार्गः_MERGED_BIN("/usr/") EXTRA_EXEC_अन्वेषणम्_मार्गः_SUFFIX
#परिभाषय DEFAULT_मार्गः_WITHOUT_SBIN मार्गः_MERGED_BIN("/usr/local/") ":" मार्गः_MERGED_BIN("/usr/") EXTRA_EXEC_अन्वेषणम्_मार्गः_SUFFIX

#परिभाषय DEFAULT_मार्गः_COMPAT मार्गः_विभज_BIN("/usr/local/") ":" मार्गः_विभज_BIN("/usr/") ":" मार्गः_विभज_BIN("/") EXTRA_EXEC_अन्वेषणम्_मार्गः_SUFFIX

ध्रुव वर्ण* default_मार्गः(शून्य);

स्थिर अन्तःस्थ ध्रुव वर्ण* default_user_मार्गः(शून्य) {
#यदि_परिभाषितम् DEFAULT_USER_मार्गः
        प्रत्यागम DEFAULT_USER_मार्गः EXTRA_EXEC_अन्वेषणम्_मार्गः_SUFFIX;
#अन्यथा
        प्रत्यागम default_मार्गः();
#यदि_अन्तः
}

bool is_मार्गः(ध्रुव वर्ण *p) विशुद्धम्;

स्थिर अन्तःस्थ bool मार्गः_is_absolute(ध्रुव वर्ण *p) {
        यदि (!p) /* A NULL pointer is definitely not an absolute path */
                प्रत्यागम false;

        प्रत्यागम p[0] == '/';
}

पूर्णाङ्क मार्गः_विभज_and_make_absolute(ध्रुव वर्ण *p, वर्ण ***ret);
वर्ण* मार्गः_make_absolute(ध्रुव वर्ण *p, ध्रुव वर्ण *उपसर्गः);
पूर्णाङ्क safe_getcwd(वर्ण **ret);
पूर्णाङ्क मार्गः_make_absolute_cwd(ध्रुव वर्ण *p, वर्ण **ret);
पूर्णाङ्क मार्गः_make_relative(ध्रुव वर्ण *from, ध्रुव वर्ण *to, वर्ण **ret);
पूर्णाङ्क मार्गः_make_relative_parent(ध्रुव वर्ण *from_child, ध्रुव वर्ण *to, वर्ण **ret);

प्रकारनिर्देश गणना मार्गः_Start_With_Flags {
        मार्गः_STARTSWITH_REFUSE_DOT_DOT       = 1U << 0,
        मार्गः_STARTSWITH_RETURN_LEADING_SLASH = 1U << 1,
} मार्गः_Start_With_Flags;

वर्ण* मार्गः_startswith_पूर्णम्(ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *उपसर्गः, मार्गः_Start_With_Flags flags) विशुद्धम्;
स्थिर अन्तःस्थ वर्ण* मार्गः_startswith(ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *उपसर्गः) {
        प्रत्यागम मार्गः_startswith_पूर्णम्(मार्गः, उपसर्गः, 0);
}

पूर्णाङ्क मार्गः_compare(ध्रुव वर्ण *a, ध्रुव वर्ण *b) विशुद्धम्;
स्थिर अन्तःस्थ bool मार्गः_equal(ध्रुव वर्ण *a, ध्रुव वर्ण *b) {
        प्रत्यागम मार्गः_compare(a, b) == 0;
}

पूर्णाङ्क मार्गः_compare_filename(ध्रुव वर्ण *a, ध्रुव वर्ण *b);
स्थिर अन्तःस्थ bool मार्गः_equal_filename(ध्रुव वर्ण *a, ध्रुव वर्ण *b) {
        प्रत्यागम मार्गः_compare_filename(a, b) == 0;
}

पूर्णाङ्क मार्गः_equal_or_inode_same_पूर्णम्(ध्रुव वर्ण *a, ध्रुव वर्ण *b, पूर्णाङ्क flags);
स्थिर अन्तःस्थ bool मार्गः_equal_or_inode_same(ध्रुव वर्ण *a, ध्रुव वर्ण *b, पूर्णाङ्क flags) {
        प्रत्यागम मार्गः_equal_or_inode_same_पूर्णम्(a, b, flags) > 0;
}

वर्ण* मार्गः_extend_आन्तरिकम्(वर्ण **x, ...);
#परिभाषय मार्गः_extend(x, ...) मार्गः_extend_आन्तरिकम्(x, __VA_ARGS__, POINTER_MAX)
#परिभाषय मार्गः_join(...) मार्गः_extend_आन्तरिकम्(रिक्त, __VA_ARGS__, POINTER_MAX)

वर्ण* skip_leading_slash(ध्रुव वर्ण *p) विशुद्धम्;

प्रकारनिर्देश गणना मार्गः_Simplify_Flags {
        मार्गः_SIMPLIFY_KEEP_TRAILING_SLASH = 1 << 0,
} मार्गः_Simplify_Flags;

वर्ण* मार्गः_simplify_पूर्णम्(वर्ण *मार्गः, मार्गः_Simplify_Flags flags);
स्थिर अन्तःस्थ वर्ण* मार्गः_simplify(वर्ण *मार्गः) {
        प्रत्यागम मार्गः_simplify_पूर्णम्(मार्गः, 0);
}

पूर्णाङ्क मार्गः_simplify_alloc(ध्रुव वर्ण *मार्गः, वर्ण **ret);

/* Note: the search terminates on the first NULL item. */
#परिभाषय मार्गः_अन्तर्_समुच्चयः(p, ...) मार्गः_strv_अन्तर्भवति(STRV_MAKE(__VA_ARGS__), p)

वर्ण* मार्गः_startswith_strv(ध्रुव वर्ण *p, वर्ण * ध्रुव *strv);
#परिभाषय मार्गः_STARTSWITH_समुच्चयः(p, ...) मार्गः_startswith_strv(p, STRV_MAKE(__VA_ARGS__))

पूर्णाङ्क मार्गः_strv_make_absolute_cwd(वर्ण **l);
वर्ण** मार्गः_strv_resolve(वर्ण **l, ध्रुव वर्ण *मूलम्);
वर्ण** मार्गः_strv_resolve_uniq(वर्ण **l, ध्रुव वर्ण *मूलम्);

पूर्णाङ्क open_and_check_executable(ध्रुव वर्ण *name, ध्रुव वर्ण *मूलम्, वर्ण **ret_मार्गः, पूर्णाङ्क *ret_fd);
पूर्णाङ्क find_executable_पूर्णम्(
                ध्रुव वर्ण *name,
                ध्रुव वर्ण *मूलम्,
                वर्ण * ध्रुव *exec_अन्वेषणम्_मार्गः,
                bool use_मार्गः_envvar,
                वर्ण **ret_filename,
                पूर्णाङ्क *ret_fd);
स्थिर अन्तःस्थ पूर्णाङ्क find_executable(ध्रुव वर्ण *name, वर्ण **ret_filename) {
        प्रत्यागम find_executable_पूर्णम्(name, /* root= */ रिक्त, रिक्त, true, ret_filename, रिक्त);
}

पूर्णाङ्क fsck_exists(शून्य);
पूर्णाङ्क fsck_exists_for_fstype(ध्रुव वर्ण *fstype);

/* Iterates through the path prefixes of the specified path, going up
 * the tree, to root. Also returns "" (and not "/"!) for the root
 * directory. Excludes the specified directory itself */
#परिभाषय मार्गः_FOREACH_उपसर्गः(उपसर्गः, मार्गः)                               \
        प्रति (वर्ण *_slash = ({                                          \
                                मार्गः_simplify(वाक्यं_प्रतिलिख(उपसर्गः, मार्गः));    \
                                streq(उपसर्गः, "/") ? रिक्त : strrchr(उपसर्गः, '/'); \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((उपसर्गः), '/'))

/* Same as PATH_FOREACH_PREFIX but also includes the specified path itself */
#परिभाषय मार्गः_FOREACH_उपसर्गः_अधिकम्(उपसर्गः, मार्गः)                          \
        प्रति (वर्ण *_slash = ({                                          \
                                मार्गः_simplify(वाक्यं_प्रतिलिख(उपसर्गः, मार्गः));    \
                                यदि (streq(उपसर्गः, "/"))                 \
                                        उपसर्गः[0] = 0;                  \
                                strrchr(उपसर्गः, 0);                     \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((उपसर्गः), '/'))

पूर्णाङ्क मार्गः_find_first_component(ध्रुव वर्ण **p, bool accept_dot_dot, ध्रुव वर्ण **ret);
पूर्णाङ्क मार्गः_find_चरमः_component(ध्रुव वर्ण *मार्गः, bool accept_dot_dot, ध्रुव वर्ण **अग्र_2, ध्रुव वर्ण **ret);
ध्रुव वर्ण* चरमः_मार्गः_component(ध्रुव वर्ण *मार्गः);

पूर्णाङ्क मार्गः_विभज_उपसर्गः_filename(ध्रुव वर्ण *मार्गः, वर्ण **ret_dir, वर्ण **ret_filename);
स्थिर अन्तःस्थ पूर्णाङ्क मार्गः_extract_filename(ध्रुव वर्ण *मार्गः, वर्ण **ret) {
        प्रत्यागम मार्गः_विभज_उपसर्गः_filename(मार्गः, रिक्त, ret);
}
स्थिर अन्तःस्थ पूर्णाङ्क मार्गः_extract_directory(ध्रुव वर्ण *मार्गः, वर्ण **ret) {
        पूर्णाङ्क r = मार्गः_विभज_उपसर्गः_filename(मार्गः, ret, रिक्त);
        प्रत्यागम r < 0 ? r : 0; /* suppress O_DIRECTORY */
}

bool filename_part_is_valid(ध्रुव वर्ण *p) विशुद्धम्;
bool filename_is_valid(ध्रुव वर्ण *p) विशुद्धम्;
bool मार्गः_is_valid_पूर्णम्(ध्रुव वर्ण *p, bool accept_dot_dot) विशुद्धम्;
स्थिर अन्तःस्थ bool मार्गः_is_valid(ध्रुव वर्ण *p) {
        प्रत्यागम मार्गः_is_valid_पूर्णम्(p, /* accept_dot_dot= */ true);
}
स्थिर अन्तःस्थ bool मार्गः_is_safe(ध्रुव वर्ण *p) {
        प्रत्यागम मार्गः_is_valid_पूर्णम्(p, /* accept_dot_dot= */ false);
}
bool मार्गः_is_normalized(ध्रुव वर्ण *p) विशुद्धम्;
स्थिर अन्तःस्थ bool filename_or_absolute_मार्गः_is_valid(ध्रुव वर्ण *p) {
        यदि (मार्गः_is_absolute(p))
                प्रत्यागम मार्गः_is_valid(p);

        प्रत्यागम filename_is_valid(p);
}

पूर्णाङ्क file_अन्तर्_same_dir(ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *filename, वर्ण **ret);

bool hidden_or_backup_file(ध्रुव वर्ण *filename) विशुद्धम्;

bool is_device_मार्गः(ध्रुव वर्ण *मार्गः) विशुद्धम्;

bool valid_device_ग्रन्थिः_मार्गः(ध्रुव वर्ण *मार्गः) विशुद्धम्;
bool valid_device_allow_आदर्शः(ध्रुव वर्ण *मार्गः) विशुद्धम्;

bool dot_or_dot_dot(ध्रुव वर्ण *मार्गः) विशुद्धम्;

bool मार्गः_उपपादयति_directory(ध्रुव वर्ण *मार्गः) विशुद्धम्;

स्थिर अन्तःस्थ ध्रुव वर्ण* skip_dev_उपसर्गः(ध्रुव वर्ण *p) {
        ध्रुव वर्ण *e;

        /* Drop any /dev prefix if there is any */

        e = मार्गः_startswith(p, "/dev/");

        प्रत्यागम e ?: p;
}

bool रिक्तानि_or_मूलम्(ध्रुव वर्ण *मार्गः) विशुद्धम्;
ध्रुव वर्ण* रिक्तानि_गन्तव्यम्_मूलम्(ध्रुव वर्ण *मार्गः) विशुद्धम्;
पूर्णाङ्क रिक्तानि_or_मूलम्_harder_गन्तव्यम्_null(ध्रुव वर्ण **मार्गः);

bool मार्गः_strv_अन्तर्भवति(वर्ण * ध्रुव *l, ध्रुव वर्ण *मार्गः);
bool prefixed_मार्गः_strv_अन्तर्भवति(वर्ण * ध्रुव *l, ध्रुव वर्ण *मार्गः);

पूर्णाङ्क मार्गः_glob_can_मेलनम्(ध्रुव वर्ण *आदर्शः, ध्रुव वर्ण *उपसर्गः, वर्ण **ret);
