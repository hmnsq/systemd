/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

రకనిర్వచనం గణన ImageClass {
        IMAGE_MACHINE,
        IMAGE_PORTABLE,
        IMAGE_SYSEXT,
        IMAGE_CLASS_EXTENSION_మొదటి = IMAGE_SYSEXT,  /* First "extension" image type, so that we can easily generically iterate through them */
        IMAGE_CONFEXT,
        IMAGE_CLASS_EXTENSION_చివరి = IMAGE_CONFEXT,  /* Last "extension image type */
        _IMAGE_CLASS_MAX,
        _IMAGE_CLASS_INVALID = -EINVAL,
} ImageClass;

DECLARE_STRING_పట్టిక_LOOKUP(image_class, ImageClass);

/* The *_extension_release flavours will look for /usr/lib/extension-release/extension-release.NAME
 * for sysext images and for /etc/extension-release.d/extension-release.NAME for confext images
 * in accordance with the OS extension specification, rather than for /usr/lib/ or /etc/os-release. */

bool image_name_is_valid(స్థిరరాశి అక్షరం *s) శుద్ధ;
పూర్ణాంకం మార్గం_వెలికితీయి_image_name(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret);

పూర్ణాంకం మార్గం_is_extension_వృక్షం(ImageClass image_class, స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *extension, bool relax_extension_release_check);
స్థిర అంతఃవరుస పూర్ణాంకం మార్గం_is_os_వృక్షం(స్థిరరాశి అక్షరం *మార్గం) {
        తిరిగి మార్గం_is_extension_వృక్షం(_IMAGE_CLASS_INVALID, మార్గం, ఖాళీ, false);
}
పూర్ణాంకం fd_is_os_వృక్షం(పూర్ణాంకం fd);

పూర్ణాంకం open_extension_release(స్థిరరాశి అక్షరం *మూలం, ImageClass image_class, స్థిరరాశి అక్షరం *extension, bool relax_extension_release_check, అక్షరం **ret_మార్గం, పూర్ణాంకం *ret_fd);
పూర్ణాంకం open_extension_release_at(పూర్ణాంకం rfd, ImageClass image_class, స్థిరరాశి అక్షరం *extension, bool relax_extension_release_check, అక్షరం **ret_మార్గం, పూర్ణాంకం *ret_fd);
పూర్ణాంకం open_os_release(స్థిరరాశి అక్షరం *మూలం, అక్షరం **ret_మార్గం, పూర్ణాంకం *ret_fd);
పూర్ణాంకం open_os_release_at(పూర్ణాంకం rfd, అక్షరం **ret_మార్గం, పూర్ణాంకం *ret_fd);

పూర్ణాంకం parse_extension_release_sentinel(స్థిరరాశి అక్షరం *మూలం, ImageClass image_class, bool relax_extension_release_check, స్థిరరాశి అక్షరం *extension, ...) _sentinel_;
#నిర్వచించు parse_extension_release(మూలం, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_sentinel(మూలం, image_class, relax_extension_release_check, extension, __VA_ARGS__, ఖాళీ)
#నిర్వచించు parse_os_release(మూలం, ...)                                     \
        parse_extension_release_sentinel(మూలం, _IMAGE_CLASS_INVALID, false, ఖాళీ, __VA_ARGS__, ఖాళీ)

పూర్ణాంకం parse_extension_release_at_sentinel(పూర్ణాంకం rfd, ImageClass image_class, bool relax_extension_release_check, స్థిరరాశి అక్షరం *extension, ...) _sentinel_;
#నిర్వచించు parse_extension_release_at(rfd, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_at_sentinel(rfd, image_class, relax_extension_release_check, extension, __VA_ARGS__, ఖాళీ)
#నిర్వచించు parse_os_release_at(rfd, ...)                                     \
        parse_extension_release_at_sentinel(rfd, _IMAGE_CLASS_INVALID, false, ఖాళీ, __VA_ARGS__, ఖాళీ)

పూర్ణాంకం ఎక్కించు_extension_release_జతలు(స్థిరరాశి అక్షరం *మూలం, ImageClass image_class, స్థిరరాశి అక్షరం *extension, bool relax_extension_release_check, అక్షరం ***ret);
స్థిర అంతఃవరుస పూర్ణాంకం ఎక్కించు_os_release_జతలు(స్థిరరాశి అక్షరం *మూలం, అక్షరం ***ret) {
        తిరిగి ఎక్కించు_extension_release_జతలు(మూలం, _IMAGE_CLASS_INVALID, ఖాళీ, false, ret);
}
పూర్ణాంకం ఎక్కించు_os_release_జతలు_with_ఉపసర్గ(స్థిరరాశి అక్షరం *మూలం, స్థిరరాశి అక్షరం *ఉపసర్గ, అక్షరం ***ret);

పూర్ణాంకం os_release_support_ended(స్థిరరాశి అక్షరం *support_చివర, bool quiet, usec_t *ret_eol);

స్థిరరాశి అక్షరం* os_release_pretty_name(స్థిరరాశి అక్షరం *pretty_name, స్థిరరాశి అక్షరం *name);

bool use_fancy_name(స్థిరరాశి అక్షరం *fancy_name);
అక్షరం *unescape_fancy_name(అక్షరం **fancy_name);
