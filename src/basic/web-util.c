/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "path-util.h"
#include "string-util.h"
#include "strv.h"
#include "utf8.h"
#include "web-util.h"

bool http_url_is_valid(const char *url) {
        const char *p;

        もし (isempty(url))
                戻る false;

        p = STARTSWITH_SET(url, "http://", "https://");
        もし (!p)
                戻る false;

        もし (isempty(p))
                戻る false;

        戻る ascii_is_valid(p);
}

bool file_url_is_valid(const char *url) {
        const char *p;

        もし (isempty(url))
                戻る false;

        p = startswith(url, "file:/");
        もし (isempty(p))
                戻る false;

        戻る ascii_is_valid(p);
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

        もし (isempty(url))
                戻る -EINVAL;

        p = startswith(url, "provider:[");
        もし (!p)
                戻る -EINVAL;

        e = strchr(p, ']');
        もし (!e || e == p)
                戻る -EINVAL;
        もし (e[1] != '/')
                戻る -EINVAL;

        _cleanup_free_ char *s = strndup(p, e - p);
        もし (!s)
                戻る -ENOMEM;

        もし (!path_is_absolute(s) || !path_is_normalized(s))
                戻る -EINVAL;

        const char *resource = e + 2;
        もし (isempty(resource))
                戻る -EINVAL;
        もし (path_is_absolute(resource) || !path_is_normalized(resource))
                戻る -EINVAL;
        もし (string_has_cc(resource, /* ok= */ NULL) || strpbrk(resource, "?#"))
                戻る -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        もし (r < 0)
                戻る r;

        もし (ret_socket)
                *ret_socket = TAKE_PTR(s);

        戻る 0;
}

bool provider_url_is_valid(const char *url) {
        戻る provider_url_parse(url, /* ret_socket= */ NULL, /* ret_resource= */ NULL) >= 0;
}

bool documentation_url_is_valid(const char *url) {
        const char *p;

        もし (isempty(url))
                戻る false;

        もし (http_url_is_valid(url) || file_url_is_valid(url))
                戻る true;

        p = STARTSWITH_SET(url, "info:", "man:");
        もし (isempty(p))
                戻る false;

        戻る ascii_is_valid(p);
}

bool http_header_valid(const char *header) {
        戻る header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ NULL) &&
                strchr(header, ':');
}

bool http_etag_is_valid(const char *etag) {
        もし (isempty(etag))
                戻る false;

        もし (!endswith(etag, "\""))
                戻る false;

        もし (!STARTSWITH_SET(etag, "\"", "W/\""))
                戻る false;

        戻る true;
}
