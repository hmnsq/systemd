/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

પ્રકારવ્યાખ્યા ગણના ImageClass {
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

bool image_name_is_valid(અચલ અક્ષર *s) _pure_;
પૂર્ણાંક path_extract_image_name(અચલ અક્ષર *path, અક્ષર **ret);

પૂર્ણાંક path_is_extension_વૃક્ષ(ImageClass image_class, અચલ અક્ષર *path, અચલ અક્ષર *extension, bool relax_extension_release_check);
સ્થિર અંતઃરેખ પૂર્ણાંક path_is_os_વૃક્ષ(અચલ અક્ષર *path) {
        પાછા path_is_extension_વૃક્ષ(_IMAGE_CLASS_INVALID, path, ખાલી, false);
}
પૂર્ણાંક fd_is_os_વૃક્ષ(પૂર્ણાંક fd);

પૂર્ણાંક open_extension_release(અચલ અક્ષર *મૂળ, ImageClass image_class, અચલ અક્ષર *extension, bool relax_extension_release_check, અક્ષર **ret_path, પૂર્ણાંક *ret_fd);
પૂર્ણાંક open_extension_release_at(પૂર્ણાંક rfd, ImageClass image_class, અચલ અક્ષર *extension, bool relax_extension_release_check, અક્ષર **ret_path, પૂર્ણાંક *ret_fd);
પૂર્ણાંક open_os_release(અચલ અક્ષર *મૂળ, અક્ષર **ret_path, પૂર્ણાંક *ret_fd);
પૂર્ણાંક open_os_release_at(પૂર્ણાંક rfd, અક્ષર **ret_path, પૂર્ણાંક *ret_fd);

પૂર્ણાંક parse_extension_release_sentinel(અચલ અક્ષર *મૂળ, ImageClass image_class, bool relax_extension_release_check, અચલ અક્ષર *extension, ...) _sentinel_;
#define parse_extension_release(મૂળ, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_sentinel(મૂળ, image_class, relax_extension_release_check, extension, __VA_ARGS__, ખાલી)
#define parse_os_release(મૂળ, ...)                                     \
        parse_extension_release_sentinel(મૂળ, _IMAGE_CLASS_INVALID, false, ખાલી, __VA_ARGS__, ખાલી)

પૂર્ણાંક parse_extension_release_at_sentinel(પૂર્ણાંક rfd, ImageClass image_class, bool relax_extension_release_check, અચલ અક્ષર *extension, ...) _sentinel_;
#define parse_extension_release_at(rfd, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_at_sentinel(rfd, image_class, relax_extension_release_check, extension, __VA_ARGS__, ખાલી)
#define parse_os_release_at(rfd, ...)                                     \
        parse_extension_release_at_sentinel(rfd, _IMAGE_CLASS_INVALID, false, ખાલી, __VA_ARGS__, ખાલી)

પૂર્ણાંક load_extension_release_pairs(અચલ અક્ષર *મૂળ, ImageClass image_class, અચલ અક્ષર *extension, bool relax_extension_release_check, અક્ષર ***ret);
સ્થિર અંતઃરેખ પૂર્ણાંક load_os_release_pairs(અચલ અક્ષર *મૂળ, અક્ષર ***ret) {
        પાછા load_extension_release_pairs(મૂળ, _IMAGE_CLASS_INVALID, ખાલી, false, ret);
}
પૂર્ણાંક load_os_release_pairs_with_prefix(અચલ અક્ષર *મૂળ, અચલ અક્ષર *prefix, અક્ષર ***ret);

પૂર્ણાંક os_release_support_ended(અચલ અક્ષર *support_end, bool quiet, usec_t *ret_eol);

અચલ અક્ષર* os_release_pretty_name(અચલ અક્ષર *pretty_name, અચલ અક્ષર *name);

bool use_fancy_name(અચલ અક્ષર *fancy_name);
અક્ષર *unescape_fancy_name(અક્ષર **fancy_name);
