/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ImageClass {
        IMAGE_MACHINE,
        IMAGE_PORTABLE,
        IMAGE_SYSEXT,
        IMAGE_CLASS_EXTENSION_ಮೊದಲ = IMAGE_SYSEXT,  /* First "extension" image type, so that we can easily generically iterate through them */
        IMAGE_CONFEXT,
        IMAGE_CLASS_EXTENSION_ಕೊನೆಯ = IMAGE_CONFEXT,  /* Last "extension image type */
        _IMAGE_CLASS_MAX,
        _IMAGE_CLASS_INVALID = -EINVAL,
} ImageClass;

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(image_class, ImageClass);

/* The *_extension_release flavours will look for /usr/lib/extension-release/extension-release.NAME
 * for sysext images and for /etc/extension-release.d/extension-release.NAME for confext images
 * in accordance with the OS extension specification, rather than for /usr/lib/ or /etc/os-release. */

bool image_name_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) ಶುದ್ಧ;
ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_ಹೊರತೆಗೆ_image_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret);

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_is_extension_ವೃಕ್ಷ(ImageClass image_class, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extension, bool relax_extension_release_check);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_is_os_ವೃಕ್ಷ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_is_extension_ವೃಕ್ಷ(_IMAGE_CLASS_INVALID, ಮಾರ್ಗ, ಖಾಲಿ, false);
}
ಪೂರ್ಣಾಂಕ fd_is_os_ವೃಕ್ಷ(ಪೂರ್ಣಾಂಕ fd);

ಪೂರ್ಣಾಂಕ open_extension_release(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ImageClass image_class, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extension, bool relax_extension_release_check, ಅಕ್ಷರ **ret_ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ *ret_fd);
ಪೂರ್ಣಾಂಕ open_extension_release_at(ಪೂರ್ಣಾಂಕ rfd, ImageClass image_class, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extension, bool relax_extension_release_check, ಅಕ್ಷರ **ret_ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ *ret_fd);
ಪೂರ್ಣಾಂಕ open_os_release(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ಅಕ್ಷರ **ret_ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ *ret_fd);
ಪೂರ್ಣಾಂಕ open_os_release_at(ಪೂರ್ಣಾಂಕ rfd, ಅಕ್ಷರ **ret_ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ *ret_fd);

ಪೂರ್ಣಾಂಕ parse_extension_release_sentinel(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ImageClass image_class, bool relax_extension_release_check, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extension, ...) _sentinel_;
#ವ್ಯಾಖ್ಯಾನಿಸು parse_extension_release(ಬೇರು, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_sentinel(ಬೇರು, image_class, relax_extension_release_check, extension, __VA_ARGS__, ಖಾಲಿ)
#ವ್ಯಾಖ್ಯಾನಿಸು parse_os_release(ಬೇರು, ...)                                     \
        parse_extension_release_sentinel(ಬೇರು, _IMAGE_CLASS_INVALID, false, ಖಾಲಿ, __VA_ARGS__, ಖಾಲಿ)

ಪೂರ್ಣಾಂಕ parse_extension_release_at_sentinel(ಪೂರ್ಣಾಂಕ rfd, ImageClass image_class, bool relax_extension_release_check, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extension, ...) _sentinel_;
#ವ್ಯಾಖ್ಯಾನಿಸು parse_extension_release_at(rfd, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_at_sentinel(rfd, image_class, relax_extension_release_check, extension, __VA_ARGS__, ಖಾಲಿ)
#ವ್ಯಾಖ್ಯಾನಿಸು parse_os_release_at(rfd, ...)                                     \
        parse_extension_release_at_sentinel(rfd, _IMAGE_CLASS_INVALID, false, ಖಾಲಿ, __VA_ARGS__, ಖಾಲಿ)

ಪೂರ್ಣಾಂಕ ತುಂಬು_extension_release_ಜೋಡಿಗಳು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ImageClass image_class, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extension, bool relax_extension_release_check, ಅಕ್ಷರ ***ret);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ತುಂಬು_os_release_ಜೋಡಿಗಳು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ಅಕ್ಷರ ***ret) {
        ಹಿಂತಿರುಗಿ ತುಂಬು_extension_release_ಜೋಡಿಗಳು(ಬೇರು, _IMAGE_CLASS_INVALID, ಖಾಲಿ, false, ret);
}
ಪೂರ್ಣಾಂಕ ತುಂಬು_os_release_ಜೋಡಿಗಳು_with_ಪೂರ್ವಪ್ರತ್ಯಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, ಅಕ್ಷರ ***ret);

ಪೂರ್ಣಾಂಕ os_release_support_ended(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *support_ಕೊನೆ, bool quiet, usec_t *ret_eol);

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* os_release_pretty_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pretty_name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);

bool use_fancy_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fancy_name);
ಅಕ್ಷರ *unescape_fancy_name(ಅಕ್ಷರ **fancy_name);
