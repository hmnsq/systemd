/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "path-util.h"
#include "string-util.h"
#include "strv.h"
#include "utf8.h"
#include "web-util.h"

bool http_url_is_valid(const char *url) {
        const char *p;

        ако (isempty(url))
                върни false;

        p = STARTSWITH_SET(url, "http://", "https://");
        ако (!p)
                върни false;

        ако (isempty(p))
                върни false;

        върни ascii_is_valid(p);
}

bool file_url_is_valid(const char *url) {
        const char *p;

        ако (isempty(url))
                върни false;

        p = startswith(url, "file:/");
        ако (isempty(p))
                върни false;

        върни ascii_is_valid(p);
}

цяло provider_url_parse(const char *url, char **ret_socket, char **ret_resource) {
        const char *p, *e;
        цяло r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        ако (isempty(url))
                върни -EINVAL;

        p = startswith(url, "provider:[");
        ако (!p)
                върни -EINVAL;

        e = strchr(p, ']');
        ако (!e || e == p)
                върни -EINVAL;
        ако (e[1] != '/')
                върни -EINVAL;

        _cleanup_free_ char *s = strndup(p, e - p);
        ако (!s)
                върни -ENOMEM;

        ако (!path_is_absolute(s) || !path_is_normalized(s))
                върни -EINVAL;

        const char *resource = e + 2;
        ако (isempty(resource))
                върни -EINVAL;
        ако (path_is_absolute(resource) || !path_is_normalized(resource))
                върни -EINVAL;
        ако (string_has_cc(resource, /* ok= */ NULL) || strpbrk(resource, "?#"))
                върни -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        ако (r < 0)
                върни r;

        ако (ret_socket)
                *ret_socket = TAKE_PTR(s);

        върни 0;
}

bool provider_url_is_valid(const char *url) {
        върни provider_url_parse(url, /* ret_socket= */ NULL, /* ret_resource= */ NULL) >= 0;
}

bool documentation_url_is_valid(const char *url) {
        const char *p;

        ако (isempty(url))
                върни false;

        ако (http_url_is_valid(url) || file_url_is_valid(url))
                върни true;

        p = STARTSWITH_SET(url, "info:", "man:");
        ако (isempty(p))
                върни false;

        върни ascii_is_valid(p);
}

bool http_header_valid(const char *header) {
        върни header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ NULL) &&
                strchr(header, ':');
}

bool http_etag_is_valid(const char *etag) {
        ако (isempty(etag))
                върни false;

        ако (!endswith(etag, "\""))
                върни false;

        ако (!STARTSWITH_SET(etag, "\"", "W/\""))
                върни false;

        върни true;
}
