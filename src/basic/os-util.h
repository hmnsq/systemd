/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା ImageClass {
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

bool image_name_is_valid(ଅଚଳ ଅକ୍ଷର *s) _pure_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_extract_image_name(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_is_extension_ବୃକ୍ଷ(ImageClass image_class, ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *extension, bool relax_extension_release_check);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_is_os_ବୃକ୍ଷ(ଅଚଳ ଅକ୍ଷର *path) {
        ଫେରନ୍ତୁ path_is_extension_ବୃକ୍ଷ(_IMAGE_CLASS_INVALID, path, ଖାଲି, false);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_is_os_ବୃକ୍ଷ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_extension_release(ଅଚଳ ଅକ୍ଷର *ମୂଳ, ImageClass image_class, ଅଚଳ ଅକ୍ଷର *extension, bool relax_extension_release_check, ଅକ୍ଷର **ret_path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_fd);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_extension_release_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd, ImageClass image_class, ଅଚଳ ଅକ୍ଷର *extension, bool relax_extension_release_check, ଅକ୍ଷର **ret_path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_fd);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_os_release(ଅଚଳ ଅକ୍ଷର *ମୂଳ, ଅକ୍ଷର **ret_path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_fd);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_os_release_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd, ଅକ୍ଷର **ret_path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_fd);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_extension_release_sentinel(ଅଚଳ ଅକ୍ଷର *ମୂଳ, ImageClass image_class, bool relax_extension_release_check, ଅଚଳ ଅକ୍ଷର *extension, ...) _sentinel_;
#define parse_extension_release(ମୂଳ, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_sentinel(ମୂଳ, image_class, relax_extension_release_check, extension, __VA_ARGS__, ଖାଲି)
#define parse_os_release(ମୂଳ, ...)                                     \
        parse_extension_release_sentinel(ମୂଳ, _IMAGE_CLASS_INVALID, false, ଖାଲି, __VA_ARGS__, ଖାଲି)

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_extension_release_at_sentinel(ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd, ImageClass image_class, bool relax_extension_release_check, ଅଚଳ ଅକ୍ଷର *extension, ...) _sentinel_;
#define parse_extension_release_at(rfd, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_at_sentinel(rfd, image_class, relax_extension_release_check, extension, __VA_ARGS__, ଖାଲି)
#define parse_os_release_at(rfd, ...)                                     \
        parse_extension_release_at_sentinel(rfd, _IMAGE_CLASS_INVALID, false, ଖାଲି, __VA_ARGS__, ଖାଲି)

ପୂର୍ଣ୍ଣସଂଖ୍ୟା load_extension_release_pairs(ଅଚଳ ଅକ୍ଷର *ମୂଳ, ImageClass image_class, ଅଚଳ ଅକ୍ଷର *extension, bool relax_extension_release_check, ଅକ୍ଷର ***ret);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା load_os_release_pairs(ଅଚଳ ଅକ୍ଷର *ମୂଳ, ଅକ୍ଷର ***ret) {
        ଫେରନ୍ତୁ load_extension_release_pairs(ମୂଳ, _IMAGE_CLASS_INVALID, ଖାଲି, false, ret);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା load_os_release_pairs_with_prefix(ଅଚଳ ଅକ୍ଷର *ମୂଳ, ଅଚଳ ଅକ୍ଷର *prefix, ଅକ୍ଷର ***ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା os_release_support_ended(ଅଚଳ ଅକ୍ଷର *support_end, bool quiet, usec_t *ret_eol);

ଅଚଳ ଅକ୍ଷର* os_release_pretty_name(ଅଚଳ ଅକ୍ଷର *pretty_name, ଅଚଳ ଅକ୍ଷର *name);

bool use_fancy_name(ଅଚଳ ଅକ୍ଷର *fancy_name);
ଅକ୍ଷର *unescape_fancy_name(ଅକ୍ଷର **fancy_name);
