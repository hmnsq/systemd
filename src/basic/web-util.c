/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "path-util.h"
#include "string-util.h"
#include "strv.h"
#include "utf8.h"
#include "web-util.h"

bool http_url_is_valid(const var_na *url) {
        const var_na *p;

        གལ་སྲིད (isempty(url))
                ལོག false;

        p = STARTSWITH_SET(url, "http://", "https://");
        གལ་སྲིད (!p)
                ལོག false;

        གལ་སྲིད (isempty(p))
                ལོག false;

        ལོག ascii_is_valid(p);
}

bool file_url_is_valid(const var_na *url) {
        const var_na *p;

        གལ་སྲིད (isempty(url))
                ལོག false;

        p = startswith(url, "file:/");
        གལ་སྲིད (isempty(p))
                ལོག false;

        ལོག ascii_is_valid(p);
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

        གལ་སྲིད (isempty(url))
                ལོག -EINVAL;

        p = startswith(url, "provider:[");
        གལ་སྲིད (!p)
                ལོག -EINVAL;

        e = strchr(p, ']');
        གལ་སྲིད (!e || e == p)
                ལོག -EINVAL;
        གལ་སྲིད (e[1] != '/')
                ལོག -EINVAL;

        _cleanup_free_ var_na *s = strndup(p, e - p);
        གལ་སྲིད (!s)
                ལོག -ENOMEM;

        གལ་སྲིད (!path_is_absolute(s) || !path_is_normalized(s))
                ལོག -EINVAL;

        const var_na *resource = e + 2;
        གལ་སྲིད (isempty(resource))
                ལོག -EINVAL;
        གལ་སྲིད (path_is_absolute(resource) || !path_is_normalized(resource))
                ལོག -EINVAL;
        གལ་སྲིད (string_has_cc(resource, /* ok= */ NULL) || strpbrk(resource, "?#"))
                ལོག -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (ret_socket)
                *ret_socket = TAKE_PTR(s);

        ལོག 0;
}

bool provider_url_is_valid(const var_na *url) {
        ལོག provider_url_parse(url, /* ret_socket= */ NULL, /* ret_resource= */ NULL) >= 0;
}

bool documentation_url_is_valid(const var_na *url) {
        const var_na *p;

        གལ་སྲིད (isempty(url))
                ལོག false;

        གལ་སྲིད (http_url_is_valid(url) || file_url_is_valid(url))
                ལོག true;

        p = STARTSWITH_SET(url, "info:", "man:");
        གལ་སྲིད (isempty(p))
                ལོག false;

        ལོག ascii_is_valid(p);
}

bool http_header_valid(const var_na *header) {
        ལོག header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ NULL) &&
                strchr(header, ':');
}

bool http_etag_is_valid(const var_na *etag) {
        གལ་སྲིད (isempty(etag))
                ལོག false;

        གལ་སྲིད (!endswith(etag, "\""))
                ལོག false;

        གལ་སྲིད (!STARTSWITH_SET(etag, "\"", "W/\""))
                ལོག false;

        ལོག true;
}
