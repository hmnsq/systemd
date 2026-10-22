/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

प्रकार क्रमागत ImageClass {
        IMAGE_MACHINE,
        IMAGE_PORTABLE,
        IMAGE_SYSEXT,
        _IMAGE_CLASS_EXTENSION_FIRST = IMAGE_SYSEXT,  /* First "extension" image type, so that we can easily generically iterate through them */
        IMAGE_CONFEXT,
        _IMAGE_CLASS_EXTENSION_LAST = IMAGE_CONFEXT,  /* Last "extension image type */
        _IMAGE_CLASS_MAX,
        _IMAGE_CLASS_INVALID = -EINVAL,
} ImageClass;

DECLARE_STRING_TABLE_LOOKUP(image_class, ImageClass);

/* The *_extension_release flavours will look for /usr/lib/extension-release/extension-release.NAME
 * for sysext images and for /etc/extension-release.d/extension-release.NAME for confext images
 * in accordance with the OS extension specification, rather than for /usr/lib/ or /etc/os-release. */

bool image_name_is_valid(स्थिर अक्षर *s) _pure_;
पूर्णांक path_extract_image_name(स्थिर अक्षर *path, अक्षर **ret);

पूर्णांक path_is_extension_वृक्ष(ImageClass image_class, स्थिर अक्षर *path, स्थिर अक्षर *extension, bool relax_extension_release_check);
अटल अंतरभूत पूर्णांक path_is_os_वृक्ष(स्थिर अक्षर *path) {
        वापस path_is_extension_वृक्ष(_IMAGE_CLASS_INVALID, path, शून्य, false);
}
पूर्णांक fd_is_os_वृक्ष(पूर्णांक fd);

पूर्णांक open_extension_release(स्थिर अक्षर *मूल, ImageClass image_class, स्थिर अक्षर *extension, bool relax_extension_release_check, अक्षर **ret_path, पूर्णांक *ret_fd);
पूर्णांक open_extension_release_at(पूर्णांक rfd, ImageClass image_class, स्थिर अक्षर *extension, bool relax_extension_release_check, अक्षर **ret_path, पूर्णांक *ret_fd);
पूर्णांक open_os_release(स्थिर अक्षर *मूल, अक्षर **ret_path, पूर्णांक *ret_fd);
पूर्णांक open_os_release_at(पूर्णांक rfd, अक्षर **ret_path, पूर्णांक *ret_fd);

पूर्णांक parse_extension_release_sentinel(स्थिर अक्षर *मूल, ImageClass image_class, bool relax_extension_release_check, स्थिर अक्षर *extension, ...) _sentinel_;
#घोषणा parse_extension_release(मूल, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_sentinel(मूल, image_class, relax_extension_release_check, extension, __VA_ARGS__, शून्य)
#घोषणा parse_os_release(मूल, ...)                                     \
        parse_extension_release_sentinel(मूल, _IMAGE_CLASS_INVALID, false, शून्य, __VA_ARGS__, शून्य)

पूर्णांक parse_extension_release_at_sentinel(पूर्णांक rfd, ImageClass image_class, bool relax_extension_release_check, स्थिर अक्षर *extension, ...) _sentinel_;
#घोषणा parse_extension_release_at(rfd, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_at_sentinel(rfd, image_class, relax_extension_release_check, extension, __VA_ARGS__, शून्य)
#घोषणा parse_os_release_at(rfd, ...)                                     \
        parse_extension_release_at_sentinel(rfd, _IMAGE_CLASS_INVALID, false, शून्य, __VA_ARGS__, शून्य)

पूर्णांक load_extension_release_pairs(स्थिर अक्षर *मूल, ImageClass image_class, स्थिर अक्षर *extension, bool relax_extension_release_check, अक्षर ***ret);
अटल अंतरभूत पूर्णांक load_os_release_pairs(स्थिर अक्षर *मूल, अक्षर ***ret) {
        वापस load_extension_release_pairs(मूल, _IMAGE_CLASS_INVALID, शून्य, false, ret);
}
पूर्णांक load_os_release_pairs_with_prefix(स्थिर अक्षर *मूल, स्थिर अक्षर *prefix, अक्षर ***ret);

पूर्णांक os_release_support_ended(स्थिर अक्षर *support_end, bool quiet, usec_t *ret_eol);

स्थिर अक्षर* os_release_pretty_name(स्थिर अक्षर *pretty_name, स्थिर अक्षर *name);

bool use_fancy_name(स्थिर अक्षर *fancy_name);
अक्षर *unescape_fancy_name(अक्षर **fancy_name);
