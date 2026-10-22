/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "path-util.h"
#include "string-util.h"
#include "strv.h"
#include "utf8.h"
#include "web-util.h"

bool http_url_is_valid(const char *url) {
        const char *p;

        如果 (isempty(url))
                返回 false;

        p = STARTSWITH_SET(url, "http://", "https://");
        如果 (!p)
                返回 false;

        如果 (isempty(p))
                返回 false;

        返回 ascii_is_valid(p);
}

bool file_url_is_valid(const char *url) {
        const char *p;

        如果 (isempty(url))
                返回 false;

        p = startswith(url, "file:/");
        如果 (isempty(p))
                返回 false;

        返回 ascii_is_valid(p);
}

整数 provider_url_parse(const char *url, char **ret_socket, char **ret_resource) {
        const char *p, *e;
        整数 r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        如果 (isempty(url))
                返回 -EINVAL;

        p = startswith(url, "provider:[");
        如果 (!p)
                返回 -EINVAL;

        e = strchr(p, ']');
        如果 (!e || e == p)
                返回 -EINVAL;
        如果 (e[1] != '/')
                返回 -EINVAL;

        _cleanup_free_ char *s = strndup(p, e - p);
        如果 (!s)
                返回 -ENOMEM;

        如果 (!path_is_absolute(s) || !path_is_normalized(s))
                返回 -EINVAL;

        const char *resource = e + 2;
        如果 (isempty(resource))
                返回 -EINVAL;
        如果 (path_is_absolute(resource) || !path_is_normalized(resource))
                返回 -EINVAL;
        如果 (string_has_cc(resource, /* ok= */ NULL) || strpbrk(resource, "?#"))
                返回 -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        如果 (r < 0)
                返回 r;

        如果 (ret_socket)
                *ret_socket = TAKE_PTR(s);

        返回 0;
}

bool provider_url_is_valid(const char *url) {
        返回 provider_url_parse(url, /* ret_socket= */ NULL, /* ret_resource= */ NULL) >= 0;
}

bool documentation_url_is_valid(const char *url) {
        const char *p;

        如果 (isempty(url))
                返回 false;

        如果 (http_url_is_valid(url) || file_url_is_valid(url))
                返回 true;

        p = STARTSWITH_SET(url, "info:", "man:");
        如果 (isempty(p))
                返回 false;

        返回 ascii_is_valid(p);
}

bool http_header_valid(const char *header) {
        返回 header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ NULL) &&
                strchr(header, ':');
}

bool http_etag_is_valid(const char *etag) {
        如果 (isempty(etag))
                返回 false;

        如果 (!endswith(etag, "\""))
                返回 false;

        如果 (!STARTSWITH_SET(etag, "\"", "W/\""))
                返回 false;

        返回 true;
}
