/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utf8.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "web-util.h"

bool http_url_is_valid(ਅਚਲ ਅੱਖਰ *url) {
        ਅਚਲ ਅੱਖਰ *p;

        ਜੇ (isempty(url))
                ਵਾਪਸ false;

        p = STARTSWITH_ਸੈੱਟ(url, "http://", "https://");
        ਜੇ (!p)
                ਵਾਪਸ false;

        ਜੇ (isempty(p))
                ਵਾਪਸ false;

        ਵਾਪਸ ascii_is_valid(p);
}

bool file_url_is_valid(ਅਚਲ ਅੱਖਰ *url) {
        ਅਚਲ ਅੱਖਰ *p;

        ਜੇ (isempty(url))
                ਵਾਪਸ false;

        p = startswith(url, "file:/");
        ਜੇ (isempty(p))
                ਵਾਪਸ false;

        ਵਾਪਸ ascii_is_valid(p);
}

ਪੂਰਨ provider_url_parse(ਅਚਲ ਅੱਖਰ *url, ਅੱਖਰ **ret_socket, ਅੱਖਰ **ret_resource) {
        ਅਚਲ ਅੱਖਰ *p, *e;
        ਪੂਰਨ r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        ਜੇ (isempty(url))
                ਵਾਪਸ -EINVAL;

        p = startswith(url, "provider:[");
        ਜੇ (!p)
                ਵਾਪਸ -EINVAL;

        e = strchr(p, ']');
        ਜੇ (!e || e == p)
                ਵਾਪਸ -EINVAL;
        ਜੇ (e[1] != '/')
                ਵਾਪਸ -EINVAL;

        _cleanup_free_ ਅੱਖਰ *s = strndup(p, e - p);
        ਜੇ (!s)
                ਵਾਪਸ -ENOMEM;

        ਜੇ (!ਰਸਤਾ_is_absolute(s) || !ਰਸਤਾ_is_normalized(s))
                ਵਾਪਸ -EINVAL;

        ਅਚਲ ਅੱਖਰ *resource = e + 2;
        ਜੇ (isempty(resource))
                ਵਾਪਸ -EINVAL;
        ਜੇ (ਰਸਤਾ_is_absolute(resource) || !ਰਸਤਾ_is_normalized(resource))
                ਵਾਪਸ -EINVAL;
        ਜੇ (string_has_cc(resource, /* ok= */ ਨਿਰਾ) || strpbrk(resource, "?#"))
                ਵਾਪਸ -EINVAL;

        r = strdup_ਮੰਜ਼ਿਲ_ਭਰਿਆ(ret_resource, resource);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret_socket)
                *ret_socket = TAKE_PTR(s);

        ਵਾਪਸ 0;
}

bool provider_url_is_valid(ਅਚਲ ਅੱਖਰ *url) {
        ਵਾਪਸ provider_url_parse(url, /* ret_socket= */ ਨਿਰਾ, /* ret_resource= */ ਨਿਰਾ) >= 0;
}

bool documentation_url_is_valid(ਅਚਲ ਅੱਖਰ *url) {
        ਅਚਲ ਅੱਖਰ *p;

        ਜੇ (isempty(url))
                ਵਾਪਸ false;

        ਜੇ (http_url_is_valid(url) || file_url_is_valid(url))
                ਵਾਪਸ true;

        p = STARTSWITH_ਸੈੱਟ(url, "info:", "man:");
        ਜੇ (isempty(p))
                ਵਾਪਸ false;

        ਵਾਪਸ ascii_is_valid(p);
}

bool http_ਸਿਰਲੇਖ_valid(ਅਚਲ ਅੱਖਰ *ਸਿਰਲੇਖ) {
        ਵਾਪਸ ਸਿਰਲੇਖ &&
                ascii_is_valid(ਸਿਰਲੇਖ) &&
                !string_has_cc(ਸਿਰਲੇਖ, /* ok= */ ਨਿਰਾ) &&
                strchr(ਸਿਰਲੇਖ, ':');
}

bool http_etag_is_valid(ਅਚਲ ਅੱਖਰ *etag) {
        ਜੇ (isempty(etag))
                ਵਾਪਸ false;

        ਜੇ (!endswith(etag, "\""))
                ਵਾਪਸ false;

        ਜੇ (!STARTSWITH_ਸੈੱਟ(etag, "\"", "W/\""))
                ਵਾਪਸ false;

        ਵਾਪਸ true;
}
