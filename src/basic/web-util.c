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
                врати false;

        p = STARTSWITH_SET(url, "http://", "https://");
        ако (!p)
                врати false;

        ако (isempty(p))
                врати false;

        врати ascii_is_valid(p);
}

bool file_url_is_valid(const char *url) {
        const char *p;

        ако (isempty(url))
                врати false;

        p = startswith(url, "file:/");
        ако (isempty(p))
                врати false;

        врати ascii_is_valid(p);
}

цео provider_url_parse(const char *url, char **ret_socket, char **ret_resource) {
        const char *p, *e;
        цео r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        ако (isempty(url))
                врати -EINVAL;

        p = startswith(url, "provider:[");
        ако (!p)
                врати -EINVAL;

        e = strchr(p, ']');
        ако (!e || e == p)
                врати -EINVAL;
        ако (e[1] != '/')
                врати -EINVAL;

        _cleanup_free_ char *s = strndup(p, e - p);
        ако (!s)
                врати -ENOMEM;

        ако (!path_is_absolute(s) || !path_is_normalized(s))
                врати -EINVAL;

        const char *resource = e + 2;
        ако (isempty(resource))
                врати -EINVAL;
        ако (path_is_absolute(resource) || !path_is_normalized(resource))
                врати -EINVAL;
        ако (string_has_cc(resource, /* ok= */ NULL) || strpbrk(resource, "?#"))
                врати -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        ако (r < 0)
                врати r;

        ако (ret_socket)
                *ret_socket = TAKE_PTR(s);

        врати 0;
}

bool provider_url_is_valid(const char *url) {
        врати provider_url_parse(url, /* ret_socket= */ NULL, /* ret_resource= */ NULL) >= 0;
}

bool documentation_url_is_valid(const char *url) {
        const char *p;

        ако (isempty(url))
                врати false;

        ако (http_url_is_valid(url) || file_url_is_valid(url))
                врати true;

        p = STARTSWITH_SET(url, "info:", "man:");
        ако (isempty(p))
                врати false;

        врати ascii_is_valid(p);
}

bool http_header_valid(const char *header) {
        врати header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ NULL) &&
                strchr(header, ':');
}

bool http_etag_is_valid(const char *etag) {
        ако (isempty(etag))
                врати false;

        ако (!endswith(etag, "\""))
                врати false;

        ако (!STARTSWITH_SET(etag, "\"", "W/\""))
                врати false;

        врати true;
}
