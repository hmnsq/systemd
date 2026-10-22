/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

തരനിർവചനം എണ്ണൽ ImageClass {
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

bool image_name_is_valid(സ്ഥിരം അക്ഷരം *s) _pure_;
പൂർണ്ണസംഖ്യ path_extract_image_name(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret);

പൂർണ്ണസംഖ്യ path_is_extension_വൃക്ഷം(ImageClass image_class, സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *extension, bool relax_extension_release_check);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ path_is_os_വൃക്ഷം(സ്ഥിരം അക്ഷരം *path) {
        തിരികെ path_is_extension_വൃക്ഷം(_IMAGE_CLASS_INVALID, path, ശൂന്യ, false);
}
പൂർണ്ണസംഖ്യ fd_is_os_വൃക്ഷം(പൂർണ്ണസംഖ്യ fd);

പൂർണ്ണസംഖ്യ open_extension_release(സ്ഥിരം അക്ഷരം *വേര്, ImageClass image_class, സ്ഥിരം അക്ഷരം *extension, bool relax_extension_release_check, അക്ഷരം **ret_path, പൂർണ്ണസംഖ്യ *ret_fd);
പൂർണ്ണസംഖ്യ open_extension_release_at(പൂർണ്ണസംഖ്യ rfd, ImageClass image_class, സ്ഥിരം അക്ഷരം *extension, bool relax_extension_release_check, അക്ഷരം **ret_path, പൂർണ്ണസംഖ്യ *ret_fd);
പൂർണ്ണസംഖ്യ open_os_release(സ്ഥിരം അക്ഷരം *വേര്, അക്ഷരം **ret_path, പൂർണ്ണസംഖ്യ *ret_fd);
പൂർണ്ണസംഖ്യ open_os_release_at(പൂർണ്ണസംഖ്യ rfd, അക്ഷരം **ret_path, പൂർണ്ണസംഖ്യ *ret_fd);

പൂർണ്ണസംഖ്യ parse_extension_release_sentinel(സ്ഥിരം അക്ഷരം *വേര്, ImageClass image_class, bool relax_extension_release_check, സ്ഥിരം അക്ഷരം *extension, ...) _sentinel_;
#define parse_extension_release(വേര്, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_sentinel(വേര്, image_class, relax_extension_release_check, extension, __VA_ARGS__, ശൂന്യ)
#define parse_os_release(വേര്, ...)                                     \
        parse_extension_release_sentinel(വേര്, _IMAGE_CLASS_INVALID, false, ശൂന്യ, __VA_ARGS__, ശൂന്യ)

പൂർണ്ണസംഖ്യ parse_extension_release_at_sentinel(പൂർണ്ണസംഖ്യ rfd, ImageClass image_class, bool relax_extension_release_check, സ്ഥിരം അക്ഷരം *extension, ...) _sentinel_;
#define parse_extension_release_at(rfd, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_at_sentinel(rfd, image_class, relax_extension_release_check, extension, __VA_ARGS__, ശൂന്യ)
#define parse_os_release_at(rfd, ...)                                     \
        parse_extension_release_at_sentinel(rfd, _IMAGE_CLASS_INVALID, false, ശൂന്യ, __VA_ARGS__, ശൂന്യ)

പൂർണ്ണസംഖ്യ load_extension_release_pairs(സ്ഥിരം അക്ഷരം *വേര്, ImageClass image_class, സ്ഥിരം അക്ഷരം *extension, bool relax_extension_release_check, അക്ഷരം ***ret);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ load_os_release_pairs(സ്ഥിരം അക്ഷരം *വേര്, അക്ഷരം ***ret) {
        തിരികെ load_extension_release_pairs(വേര്, _IMAGE_CLASS_INVALID, ശൂന്യ, false, ret);
}
പൂർണ്ണസംഖ്യ load_os_release_pairs_with_prefix(സ്ഥിരം അക്ഷരം *വേര്, സ്ഥിരം അക്ഷരം *prefix, അക്ഷരം ***ret);

പൂർണ്ണസംഖ്യ os_release_support_ended(സ്ഥിരം അക്ഷരം *support_end, bool quiet, usec_t *ret_eol);

സ്ഥിരം അക്ഷരം* os_release_pretty_name(സ്ഥിരം അക്ഷരം *pretty_name, സ്ഥിരം അക്ഷരം *name);

bool use_fancy_name(സ്ഥിരം അക്ഷരം *fancy_name);
അക്ഷരം *unescape_fancy_name(അക്ഷരം **fancy_name);
