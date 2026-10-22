/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef enum ImageClass {
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

bool image_name_is_valid(const char *s) _pure_;
ціле path_extract_image_name(const char *path, char **ret);

ціле path_is_extension_tree(ImageClass image_class, const char *path, const char *extension, bool relax_extension_release_check);
static inline ціле path_is_os_tree(const char *path) {
        повернути path_is_extension_tree(_IMAGE_CLASS_INVALID, path, NULL, false);
}
ціле fd_is_os_tree(ціле fd);

ціле open_extension_release(const char *root, ImageClass image_class, const char *extension, bool relax_extension_release_check, char **ret_path, ціле *ret_fd);
ціле open_extension_release_at(ціле rfd, ImageClass image_class, const char *extension, bool relax_extension_release_check, char **ret_path, ціле *ret_fd);
ціле open_os_release(const char *root, char **ret_path, ціле *ret_fd);
ціле open_os_release_at(ціле rfd, char **ret_path, ціле *ret_fd);

ціле parse_extension_release_sentinel(const char *root, ImageClass image_class, bool relax_extension_release_check, const char *extension, ...) _sentinel_;
#define parse_extension_release(root, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_sentinel(root, image_class, relax_extension_release_check, extension, __VA_ARGS__, NULL)
#define parse_os_release(root, ...)                                     \
        parse_extension_release_sentinel(root, _IMAGE_CLASS_INVALID, false, NULL, __VA_ARGS__, NULL)

ціле parse_extension_release_at_sentinel(ціле rfd, ImageClass image_class, bool relax_extension_release_check, const char *extension, ...) _sentinel_;
#define parse_extension_release_at(rfd, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_at_sentinel(rfd, image_class, relax_extension_release_check, extension, __VA_ARGS__, NULL)
#define parse_os_release_at(rfd, ...)                                     \
        parse_extension_release_at_sentinel(rfd, _IMAGE_CLASS_INVALID, false, NULL, __VA_ARGS__, NULL)

ціле load_extension_release_pairs(const char *root, ImageClass image_class, const char *extension, bool relax_extension_release_check, char ***ret);
static inline ціле load_os_release_pairs(const char *root, char ***ret) {
        повернути load_extension_release_pairs(root, _IMAGE_CLASS_INVALID, NULL, false, ret);
}
ціле load_os_release_pairs_with_prefix(const char *root, const char *prefix, char ***ret);

ціле os_release_support_ended(const char *support_end, bool quiet, usec_t *ret_eol);

const char* os_release_pretty_name(const char *pretty_name, const char *name);

bool use_fancy_name(const char *fancy_name);
char *unescape_fancy_name(char **fancy_name);
