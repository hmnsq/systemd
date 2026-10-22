/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ImageClass {
        IMAGE_MACHINE,
        IMAGE_PORTABLE,
        IMAGE_SYSEXT,
        IMAGE_CLASS_EXTENSION_ਪਹਿਲਾ = IMAGE_SYSEXT,  /* First "extension" image type, so that we can easily generically iterate through them */
        IMAGE_CONFEXT,
        IMAGE_CLASS_EXTENSION_ਆਖਰੀ = IMAGE_CONFEXT,  /* Last "extension image type */
        _IMAGE_CLASS_MAX,
        _IMAGE_CLASS_INVALID = -EINVAL,
} ImageClass;

DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(image_class, ImageClass);

/* The *_extension_release flavours will look for /usr/lib/extension-release/extension-release.NAME
 * for sysext images and for /etc/extension-release.d/extension-release.NAME for confext images
 * in accordance with the OS extension specification, rather than for /usr/lib/ or /etc/os-release. */

bool image_name_is_valid(ਅਚਲ ਅੱਖਰ *s) ਸ਼ੁੱਧ;
ਪੂਰਨ ਰਸਤਾ_ਕੱਢੋ_image_name(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret);

ਪੂਰਨ ਰਸਤਾ_is_extension_ਰੁੱਖ(ImageClass image_class, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *extension, bool relax_extension_release_check);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਰਸਤਾ_is_os_ਰੁੱਖ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        ਵਾਪਸ ਰਸਤਾ_is_extension_ਰੁੱਖ(_IMAGE_CLASS_INVALID, ਰਸਤਾ, ਨਿਰਾ, false);
}
ਪੂਰਨ fd_is_os_ਰੁੱਖ(ਪੂਰਨ fd);

ਪੂਰਨ open_extension_release(ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ImageClass image_class, ਅਚਲ ਅੱਖਰ *extension, bool relax_extension_release_check, ਅੱਖਰ **ret_ਰਸਤਾ, ਪੂਰਨ *ret_fd);
ਪੂਰਨ open_extension_release_at(ਪੂਰਨ rfd, ImageClass image_class, ਅਚਲ ਅੱਖਰ *extension, bool relax_extension_release_check, ਅੱਖਰ **ret_ਰਸਤਾ, ਪੂਰਨ *ret_fd);
ਪੂਰਨ open_os_release(ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ਅੱਖਰ **ret_ਰਸਤਾ, ਪੂਰਨ *ret_fd);
ਪੂਰਨ open_os_release_at(ਪੂਰਨ rfd, ਅੱਖਰ **ret_ਰਸਤਾ, ਪੂਰਨ *ret_fd);

ਪੂਰਨ parse_extension_release_sentinel(ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ImageClass image_class, bool relax_extension_release_check, ਅਚਲ ਅੱਖਰ *extension, ...) _sentinel_;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ parse_extension_release(ਜੜ੍ਹ, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_sentinel(ਜੜ੍ਹ, image_class, relax_extension_release_check, extension, __VA_ARGS__, ਨਿਰਾ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ parse_os_release(ਜੜ੍ਹ, ...)                                     \
        parse_extension_release_sentinel(ਜੜ੍ਹ, _IMAGE_CLASS_INVALID, false, ਨਿਰਾ, __VA_ARGS__, ਨਿਰਾ)

ਪੂਰਨ parse_extension_release_at_sentinel(ਪੂਰਨ rfd, ImageClass image_class, bool relax_extension_release_check, ਅਚਲ ਅੱਖਰ *extension, ...) _sentinel_;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ parse_extension_release_at(rfd, image_class, extension, relax_extension_release_check, ...) \
        parse_extension_release_at_sentinel(rfd, image_class, relax_extension_release_check, extension, __VA_ARGS__, ਨਿਰਾ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ parse_os_release_at(rfd, ...)                                     \
        parse_extension_release_at_sentinel(rfd, _IMAGE_CLASS_INVALID, false, ਨਿਰਾ, __VA_ARGS__, ਨਿਰਾ)

ਪੂਰਨ ਲੱਦੋ_extension_release_ਜੋੜੇ(ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ImageClass image_class, ਅਚਲ ਅੱਖਰ *extension, bool relax_extension_release_check, ਅੱਖਰ ***ret);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਲੱਦੋ_os_release_ਜੋੜੇ(ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ਅੱਖਰ ***ret) {
        ਵਾਪਸ ਲੱਦੋ_extension_release_ਜੋੜੇ(ਜੜ੍ਹ, _IMAGE_CLASS_INVALID, ਨਿਰਾ, false, ret);
}
ਪੂਰਨ ਲੱਦੋ_os_release_ਜੋੜੇ_with_ਅਗੇਤਰ(ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, ਅੱਖਰ ***ret);

ਪੂਰਨ os_release_support_ended(ਅਚਲ ਅੱਖਰ *support_ਸਿਰਾ, bool quiet, usec_t *ret_eol);

ਅਚਲ ਅੱਖਰ* os_release_pretty_name(ਅਚਲ ਅੱਖਰ *pretty_name, ਅਚਲ ਅੱਖਰ *name);

bool use_fancy_name(ਅਚਲ ਅੱਖਰ *fancy_name);
ਅੱਖਰ *unescape_fancy_name(ਅੱਖਰ **fancy_name);
