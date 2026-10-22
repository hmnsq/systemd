/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef ga_nanaa ImageClass {
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

bool image_name_is_valid(const var_na *s) _pure_;
puur_naa_nka path_extract_image_name(const var_na *path, var_na **ret);

puur_naa_nka path_is_extension_tree(ImageClass image_class, const var_na *path, const var_na *extension, bool relax_extension_release_check);
static anta_hstha puur_naa_nka path_is_os_tree(const var_na *path) {
        ꦮꦭꦶ path_is_extension_tree(_IMAGE_CLASS_INVALID, path, NULL, false);
}
puur_naa_nka fd_is_os_tree(puur_naa_nka fd);

puur_naa_nka open_extension_release(const var_na *root, ImageClass image_class, const var_na *extension, bool relax_extension_release_check, var_na **ret_path, puur_naa_nka *ret_fd);
puur_naa_nka open_extension_release_at(puur_naa_nka rfd, ImageClass image_class, const var_na *extension, bool relax_extension_release_check, var_na **ret_path, puur_naa_nka *ret_fd);
puur_naa_nka open_os_release(const var_na *root, var_na **ret_path, puur_naa_nka *ret_fd);
puur_naa_nka open_os_release_at(puur_naa_nka rfd, var_na **ret_path, puur_naa_nka *ret_fd);

puur_naa_nka parse_extension_release_sentinel(const var_na *root, ImageClass image_class, bool relax_extension_release_check, const var_na *extension, ...) _sentinel_;
#define parse_extension_release(root, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_sentinel(root, image_class, relax_extension_release_check, extension, __VA_ARGS__, NULL)
#define parse_os_release(root, ...)                                     \
        parse_extension_release_sentinel(root, _IMAGE_CLASS_INVALID, false, NULL, __VA_ARGS__, NULL)

puur_naa_nka parse_extension_release_at_sentinel(puur_naa_nka rfd, ImageClass image_class, bool relax_extension_release_check, const var_na *extension, ...) _sentinel_;
#define parse_extension_release_at(rfd, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_at_sentinel(rfd, image_class, relax_extension_release_check, extension, __VA_ARGS__, NULL)
#define parse_os_release_at(rfd, ...)                                     \
        parse_extension_release_at_sentinel(rfd, _IMAGE_CLASS_INVALID, false, NULL, __VA_ARGS__, NULL)

puur_naa_nka load_extension_release_pairs(const var_na *root, ImageClass image_class, const var_na *extension, bool relax_extension_release_check, var_na ***ret);
static anta_hstha puur_naa_nka load_os_release_pairs(const var_na *root, var_na ***ret) {
        ꦮꦭꦶ load_extension_release_pairs(root, _IMAGE_CLASS_INVALID, NULL, false, ret);
}
puur_naa_nka load_os_release_pairs_with_prefix(const var_na *root, const var_na *prefix, var_na ***ret);

puur_naa_nka os_release_support_ended(const var_na *support_end, bool quiet, usec_t *ret_eol);

const var_na* os_release_pretty_name(const var_na *pretty_name, const var_na *name);

bool use_fancy_name(const var_na *fancy_name);
var_na *unescape_fancy_name(var_na **fancy_name);
