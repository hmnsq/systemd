/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "path-util.h"
#include "string-util.h"
#include "strv.h"
#include "utf8.h"
#include "web-util.h"

bool http_url_is_valid(const var_na *url) {
        const var_na *p;

        གལ་ཏེ (isempty(url))
                ཕྱིར false;

        p = STARTSWITH_SET(url, "http://", "https://");
        གལ་ཏེ (!p)
                ཕྱིར false;

        གལ་ཏེ (isempty(p))
                ཕྱིར false;

        ཕྱིར ascii_is_valid(p);
}

bool file_url_is_valid(const var_na *url) {
        const var_na *p;

        གལ་ཏེ (isempty(url))
                ཕྱིར false;

        p = startswith(url, "file:/");
        གལ་ཏེ (isempty(p))
                ཕྱིར false;

        ཕྱིར ascii_is_valid(p);
}

ཧྲིལ provider_url_parse(const var_na *url, var_na **ret_socket, var_na **ret_resource) {
        const var_na *p, *e;
        ཧྲིལ r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        གལ་ཏེ (isempty(url))
                ཕྱིར -EINVAL;

        p = startswith(url, "provider:[");
        གལ་ཏེ (!p)
                ཕྱིར -EINVAL;

        e = strchr(p, ']');
        གལ་ཏེ (!e || e == p)
                ཕྱིར -EINVAL;
        གལ་ཏེ (e[1] != '/')
                ཕྱིར -EINVAL;

        _cleanup_free_ var_na *s = strndup(p, e - p);
        གལ་ཏེ (!s)
                ཕྱིར -ENOMEM;

        གལ་ཏེ (!path_is_absolute(s) || !path_is_normalized(s))
                ཕྱིར -EINVAL;

        const var_na *resource = e + 2;
        གལ་ཏེ (isempty(resource))
                ཕྱིར -EINVAL;
        གལ་ཏེ (path_is_absolute(resource) || !path_is_normalized(resource))
                ཕྱིར -EINVAL;
        གལ་ཏེ (string_has_cc(resource, /* ok= */ NULL) || strpbrk(resource, "?#"))
                ཕྱིར -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (ret_socket)
                *ret_socket = TAKE_PTR(s);

        ཕྱིར 0;
}

bool provider_url_is_valid(const var_na *url) {
        ཕྱིར provider_url_parse(url, /* ret_socket= */ NULL, /* ret_resource= */ NULL) >= 0;
}

bool documentation_url_is_valid(const var_na *url) {
        const var_na *p;

        གལ་ཏེ (isempty(url))
                ཕྱིར false;

        གལ་ཏེ (http_url_is_valid(url) || file_url_is_valid(url))
                ཕྱིར true;

        p = STARTSWITH_SET(url, "info:", "man:");
        གལ་ཏེ (isempty(p))
                ཕྱིར false;

        ཕྱིར ascii_is_valid(p);
}

bool http_header_valid(const var_na *header) {
        ཕྱིར header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ NULL) &&
                strchr(header, ':');
}

bool http_etag_is_valid(const var_na *etag) {
        གལ་ཏེ (isempty(etag))
                ཕྱིར false;

        གལ་ཏེ (!endswith(etag, "\""))
                ཕྱིར false;

        གལ་ཏེ (!STARTSWITH_SET(etag, "\"", "W/\""))
                ཕྱིར false;

        ཕྱིར true;
}
