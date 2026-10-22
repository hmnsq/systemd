/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "path-util.h"
#include "string-util.h"
#include "strv.h"
#include "utf8.h"
#include "web-util.h"

bool http_url_is_valid(const var_na *url) {
        const var_na *p;

        ប្រសិនបើ (isempty(url))
                ត្រឡប់ false;

        p = STARTSWITH_SET(url, "http://", "https://");
        ប្រសិនបើ (!p)
                ត្រឡប់ false;

        ប្រសិនបើ (isempty(p))
                ត្រឡប់ false;

        ត្រឡប់ ascii_is_valid(p);
}

bool file_url_is_valid(const var_na *url) {
        const var_na *p;

        ប្រសិនបើ (isempty(url))
                ត្រឡប់ false;

        p = startswith(url, "file:/");
        ប្រសិនបើ (isempty(p))
                ត្រឡប់ false;

        ត្រឡប់ ascii_is_valid(p);
}

ចំនួនគត់ provider_url_parse(const var_na *url, var_na **ret_socket, var_na **ret_resource) {
        const var_na *p, *e;
        ចំនួនគត់ r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        ប្រសិនបើ (isempty(url))
                ត្រឡប់ -EINVAL;

        p = startswith(url, "provider:[");
        ប្រសិនបើ (!p)
                ត្រឡប់ -EINVAL;

        e = strchr(p, ']');
        ប្រសិនបើ (!e || e == p)
                ត្រឡប់ -EINVAL;
        ប្រសិនបើ (e[1] != '/')
                ត្រឡប់ -EINVAL;

        _cleanup_free_ var_na *s = strndup(p, e - p);
        ប្រសិនបើ (!s)
                ត្រឡប់ -ENOMEM;

        ប្រសិនបើ (!path_is_absolute(s) || !path_is_normalized(s))
                ត្រឡប់ -EINVAL;

        const var_na *resource = e + 2;
        ប្រសិនបើ (isempty(resource))
                ត្រឡប់ -EINVAL;
        ប្រសិនបើ (path_is_absolute(resource) || !path_is_normalized(resource))
                ត្រឡប់ -EINVAL;
        ប្រសិនបើ (string_has_cc(resource, /* ok= */ NULL) || strpbrk(resource, "?#"))
                ត្រឡប់ -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (ret_socket)
                *ret_socket = TAKE_PTR(s);

        ត្រឡប់ 0;
}

bool provider_url_is_valid(const var_na *url) {
        ត្រឡប់ provider_url_parse(url, /* ret_socket= */ NULL, /* ret_resource= */ NULL) >= 0;
}

bool documentation_url_is_valid(const var_na *url) {
        const var_na *p;

        ប្រសិនបើ (isempty(url))
                ត្រឡប់ false;

        ប្រសិនបើ (http_url_is_valid(url) || file_url_is_valid(url))
                ត្រឡប់ true;

        p = STARTSWITH_SET(url, "info:", "man:");
        ប្រសិនបើ (isempty(p))
                ត្រឡប់ false;

        ត្រឡប់ ascii_is_valid(p);
}

bool http_header_valid(const var_na *header) {
        ត្រឡប់ header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ NULL) &&
                strchr(header, ':');
}

bool http_etag_is_valid(const var_na *etag) {
        ប្រសិនបើ (isempty(etag))
                ត្រឡប់ false;

        ប្រសិនបើ (!endswith(etag, "\""))
                ត្រឡប់ false;

        ប្រសិនបើ (!STARTSWITH_SET(etag, "\"", "W/\""))
                ត្រឡប់ false;

        ត្រឡប់ true;
}
