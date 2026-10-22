/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stdio-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-table.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"

ਅਚਲ ਅੱਖਰ* string_ਸਾਰਣੀ_lookup_ਮੰਜ਼ਿਲ_string(ਅਚਲ ਅੱਖਰ * ਅਚਲ *ਸਾਰਣੀ, size_t len, ssize_t i) {
        ਜੇ (i < 0 || i >= (ssize_t) len)
                ਵਾਪਸ ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਸਾਰਣੀ);

        ਵਾਪਸ ਸਾਰਣੀ[i];
}

ssize_t string_ਸਾਰਣੀ_lookup_from_string(ਅਚਲ ਅੱਖਰ * ਅਚਲ *ਸਾਰਣੀ, size_t len, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ) {
        assert_return(ਸਾਰਣੀ, -EINVAL);

        ਜੇ (!ਕੁੰਜੀ)
                ਵਾਪਸ -EINVAL;

        ਲਈ (size_t i = 0; i < len; ++i)
                ਜੇ (streq_ptr(ਸਾਰਣੀ[i], ਕੁੰਜੀ))
                        ਵਾਪਸ (ssize_t) i;

        ਵਾਪਸ -EINVAL;
}

ssize_t string_ਸਾਰਣੀ_lookup_from_string_with_boolean(ਅਚਲ ਅੱਖਰ * ਅਚਲ *ਸਾਰਣੀ, size_t len, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ssize_t yes) {
        ਜੇ (!ਕੁੰਜੀ)
                ਵਾਪਸ -EINVAL;

        ਪੂਰਨ b = parse_boolean(ਕੁੰਜੀ);
        ਜੇ (b == 0)
                ਵਾਪਸ 0;
        ਜੇ (b > 0)
                ਵਾਪਸ yes;

        ਵਾਪਸ string_ਸਾਰਣੀ_lookup_from_string(ਸਾਰਣੀ, len, ਕੁੰਜੀ);
}

ਪੂਰਨ string_ਸਾਰਣੀ_lookup_ਮੰਜ਼ਿਲ_string_fallback(ਅਚਲ ਅੱਖਰ * ਅਚਲ *ਸਾਰਣੀ, size_t len, ssize_t i, size_t max, ਅੱਖਰ **ret) {
        ਅੱਖਰ *s;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਸਾਰਣੀ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (i < 0 || i > (ssize_t) max)
                ਵਾਪਸ -ERANGE;

        ਜੇ (i < (ssize_t) len && ਸਾਰਣੀ[i])
                s = strdup(ਸਾਰਣੀ[i]);
        ਨਹੀਂਤਾਂ
                s = asprintf_ਸੁਰੱਖਿਅਤ("%zd", i);
        ਜੇ (!s)
                ਵਾਪਸ -ENOMEM;

        *ret = s;
        ਵਾਪਸ 0;
}

ssize_t string_ਸਾਰਣੀ_lookup_from_string_fallback(ਅਚਲ ਅੱਖਰ * ਅਚਲ *ਸਾਰਣੀ, size_t len, ਅਚਲ ਅੱਖਰ *s, size_t max) {
        ਜੇ (!s)
                ਵਾਪਸ -EINVAL;

        ssize_t i = string_ਸਾਰਣੀ_lookup_from_string(ਸਾਰਣੀ, len, s);
        ਜੇ (i >= 0)
                ਵਾਪਸ i;

        ਬਿਨਾਨਿਸ਼ਾਨੀ u;
        ਜੇ (ਸੁਰੱਖਿਅਤ_atou(s, &u) < 0)
                ਵਾਪਸ -EINVAL;
        ਜੇ (u > max)
                ਵਾਪਸ -EINVAL;

        ਵਾਪਸ u;
}
