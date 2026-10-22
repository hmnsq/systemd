/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

/* Space for capability_to_string() in case we write out a numeric capability because we don't know the name
 * for it. "0x3e" is the largest string we might output, in both sensese of the word "largest": two chars for
 * "0x", two bytes for the hex value, and one trailing NUL byte. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAPABILITY_ਮੰਜ਼ਿਲ_STRING_MAX (2 + 2 + 1)

ਅਚਲ ਅੱਖਰ* capability_ਮੰਜ਼ਿਲ_name(ਪੂਰਨ id);
ਅਚਲ ਅੱਖਰ* capability_ਮੰਜ਼ਿਲ_string(ਪੂਰਨ id, ਅੱਖਰ buf[ਸਥਿਰ CAPABILITY_ਮੰਜ਼ਿਲ_STRING_MAX]);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAPABILITY_ਮੰਜ਼ਿਲ_STRING(id) capability_ਮੰਜ਼ਿਲ_string(id, (ਅੱਖਰ[CAPABILITY_ਮੰਜ਼ਿਲ_STRING_MAX]) {})

ਪੂਰਨ capability_from_name(ਅਚਲ ਅੱਖਰ *name);
ਬਿਨਾਨਿਸ਼ਾਨੀ capability_list_ਲੰਮਾਈ(ਖਾਲੀ);

ਪੂਰਨ capability_ਸੈੱਟ_ਮੰਜ਼ਿਲ_string(uint64_t ਸੈੱਟ, ਅੱਖਰ **ret);
ਪੂਰਨ capability_ਸੈੱਟ_ਮੰਜ਼ਿਲ_string_negative(uint64_t ਸੈੱਟ, ਅੱਖਰ **ret);
ਪੂਰਨ capability_ਸੈੱਟ_ਮੰਜ਼ਿਲ_strv(uint64_t ਸੈੱਟ, ਅੱਖਰ ***ret);
ਪੂਰਨ capability_ਸੈੱਟ_from_string(ਅਚਲ ਅੱਖਰ *s, uint64_t *ret);
