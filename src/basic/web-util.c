/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "path-util.h"
#include "string-util.h"
#include "strv.h"
#include "utf8.h"
#include "web-util.h"

bool http_url_is_valid(const char *url) {
        const char *p;

        якщо (isempty(url))
                повернути false;

        p = STARTSWITH_SET(url, "http://", "https://");
        якщо (!p)
                повернути false;

        якщо (isempty(p))
                повернути false;

        повернути ascii_is_valid(p);
}

bool file_url_is_valid(const char *url) {
        const char *p;

        якщо (isempty(url))
                повернути false;

        p = startswith(url, "file:/");
        якщо (isempty(p))
                повернути false;

        повернути ascii_is_valid(p);
}

ціле provider_url_parse(const char *url, char **ret_socket, char **ret_resource) {
        const char *p, *e;
        ціле r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        якщо (isempty(url))
                повернути -EINVAL;

        p = startswith(url, "provider:[");
        якщо (!p)
                повернути -EINVAL;

        e = strchr(p, ']');
        якщо (!e || e == p)
                повернути -EINVAL;
        якщо (e[1] != '/')
                повернути -EINVAL;

        _cleanup_free_ char *s = strndup(p, e - p);
        якщо (!s)
                повернути -ENOMEM;

        якщо (!path_is_absolute(s) || !path_is_normalized(s))
                повернути -EINVAL;

        const char *resource = e + 2;
        якщо (isempty(resource))
                повернути -EINVAL;
        якщо (path_is_absolute(resource) || !path_is_normalized(resource))
                повернути -EINVAL;
        якщо (string_has_cc(resource, /* ok= */ NULL) || strpbrk(resource, "?#"))
                повернути -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        якщо (r < 0)
                повернути r;

        якщо (ret_socket)
                *ret_socket = TAKE_PTR(s);

        повернути 0;
}

bool provider_url_is_valid(const char *url) {
        повернути provider_url_parse(url, /* ret_socket= */ NULL, /* ret_resource= */ NULL) >= 0;
}

bool documentation_url_is_valid(const char *url) {
        const char *p;

        якщо (isempty(url))
                повернути false;

        якщо (http_url_is_valid(url) || file_url_is_valid(url))
                повернути true;

        p = STARTSWITH_SET(url, "info:", "man:");
        якщо (isempty(p))
                повернути false;

        повернути ascii_is_valid(p);
}

bool http_header_valid(const char *header) {
        повернути header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ NULL) &&
                strchr(header, ':');
}

bool http_etag_is_valid(const char *etag) {
        якщо (isempty(etag))
                повернути false;

        якщо (!endswith(etag, "\""))
                повернути false;

        якщо (!STARTSWITH_SET(etag, "\"", "W/\""))
                повернути false;

        повернути true;
}
