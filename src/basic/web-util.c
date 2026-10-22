/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "path-util.h"
#include "string-util.h"
#include "strv.h"
#include "utf8.h"
#include "web-util.h"

bool http_url_is_valid(const var_na *url) {
        const var_na *p;

        ถ้า (isempty(url))
                คืน false;

        p = STARTSWITH_SET(url, "http://", "https://");
        ถ้า (!p)
                คืน false;

        ถ้า (isempty(p))
                คืน false;

        คืน ascii_is_valid(p);
}

bool file_url_is_valid(const var_na *url) {
        const var_na *p;

        ถ้า (isempty(url))
                คืน false;

        p = startswith(url, "file:/");
        ถ้า (isempty(p))
                คืน false;

        คืน ascii_is_valid(p);
}

จำนวนเต็ม provider_url_parse(const var_na *url, var_na **ret_socket, var_na **ret_resource) {
        const var_na *p, *e;
        จำนวนเต็ม r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        ถ้า (isempty(url))
                คืน -EINVAL;

        p = startswith(url, "provider:[");
        ถ้า (!p)
                คืน -EINVAL;

        e = strchr(p, ']');
        ถ้า (!e || e == p)
                คืน -EINVAL;
        ถ้า (e[1] != '/')
                คืน -EINVAL;

        _cleanup_free_ var_na *s = strndup(p, e - p);
        ถ้า (!s)
                คืน -ENOMEM;

        ถ้า (!path_is_absolute(s) || !path_is_normalized(s))
                คืน -EINVAL;

        const var_na *resource = e + 2;
        ถ้า (isempty(resource))
                คืน -EINVAL;
        ถ้า (path_is_absolute(resource) || !path_is_normalized(resource))
                คืน -EINVAL;
        ถ้า (string_has_cc(resource, /* ok= */ NULL) || strpbrk(resource, "?#"))
                คืน -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        ถ้า (r < 0)
                คืน r;

        ถ้า (ret_socket)
                *ret_socket = TAKE_PTR(s);

        คืน 0;
}

bool provider_url_is_valid(const var_na *url) {
        คืน provider_url_parse(url, /* ret_socket= */ NULL, /* ret_resource= */ NULL) >= 0;
}

bool documentation_url_is_valid(const var_na *url) {
        const var_na *p;

        ถ้า (isempty(url))
                คืน false;

        ถ้า (http_url_is_valid(url) || file_url_is_valid(url))
                คืน true;

        p = STARTSWITH_SET(url, "info:", "man:");
        ถ้า (isempty(p))
                คืน false;

        คืน ascii_is_valid(p);
}

bool http_header_valid(const var_na *header) {
        คืน header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ NULL) &&
                strchr(header, ':');
}

bool http_etag_is_valid(const var_na *etag) {
        ถ้า (isempty(etag))
                คืน false;

        ถ้า (!endswith(etag, "\""))
                คืน false;

        ถ้า (!STARTSWITH_SET(etag, "\"", "W/\""))
                คืน false;

        คืน true;
}
