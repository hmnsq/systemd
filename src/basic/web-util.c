/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "path-util.h"
#include "string-util.h"
#include "strv.h"
#include "utf8.h"
#include "web-util.h"

bool http_url_is_valid(const var_na *url) {
        const var_na *p;

        ꦪꦼꦤ꧀ (isempty(url))
                ꦮꦭꦶ false;

        p = STARTSWITH_SET(url, "http://", "https://");
        ꦪꦼꦤ꧀ (!p)
                ꦮꦭꦶ false;

        ꦪꦼꦤ꧀ (isempty(p))
                ꦮꦭꦶ false;

        ꦮꦭꦶ ascii_is_valid(p);
}

bool file_url_is_valid(const var_na *url) {
        const var_na *p;

        ꦪꦼꦤ꧀ (isempty(url))
                ꦮꦭꦶ false;

        p = startswith(url, "file:/");
        ꦪꦼꦤ꧀ (isempty(p))
                ꦮꦭꦶ false;

        ꦮꦭꦶ ascii_is_valid(p);
}

puur_naa_nka provider_url_parse(const var_na *url, var_na **ret_socket, var_na **ret_resource) {
        const var_na *p, *e;
        puur_naa_nka r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        ꦪꦼꦤ꧀ (isempty(url))
                ꦮꦭꦶ -EINVAL;

        p = startswith(url, "provider:[");
        ꦪꦼꦤ꧀ (!p)
                ꦮꦭꦶ -EINVAL;

        e = strchr(p, ']');
        ꦪꦼꦤ꧀ (!e || e == p)
                ꦮꦭꦶ -EINVAL;
        ꦪꦼꦤ꧀ (e[1] != '/')
                ꦮꦭꦶ -EINVAL;

        _cleanup_free_ var_na *s = strndup(p, e - p);
        ꦪꦼꦤ꧀ (!s)
                ꦮꦭꦶ -ENOMEM;

        ꦪꦼꦤ꧀ (!path_is_absolute(s) || !path_is_normalized(s))
                ꦮꦭꦶ -EINVAL;

        const var_na *resource = e + 2;
        ꦪꦼꦤ꧀ (isempty(resource))
                ꦮꦭꦶ -EINVAL;
        ꦪꦼꦤ꧀ (path_is_absolute(resource) || !path_is_normalized(resource))
                ꦮꦭꦶ -EINVAL;
        ꦪꦼꦤ꧀ (string_has_cc(resource, /* ok= */ NULL) || strpbrk(resource, "?#"))
                ꦮꦭꦶ -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (ret_socket)
                *ret_socket = TAKE_PTR(s);

        ꦮꦭꦶ 0;
}

bool provider_url_is_valid(const var_na *url) {
        ꦮꦭꦶ provider_url_parse(url, /* ret_socket= */ NULL, /* ret_resource= */ NULL) >= 0;
}

bool documentation_url_is_valid(const var_na *url) {
        const var_na *p;

        ꦪꦼꦤ꧀ (isempty(url))
                ꦮꦭꦶ false;

        ꦪꦼꦤ꧀ (http_url_is_valid(url) || file_url_is_valid(url))
                ꦮꦭꦶ true;

        p = STARTSWITH_SET(url, "info:", "man:");
        ꦪꦼꦤ꧀ (isempty(p))
                ꦮꦭꦶ false;

        ꦮꦭꦶ ascii_is_valid(p);
}

bool http_header_valid(const var_na *header) {
        ꦮꦭꦶ header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ NULL) &&
                strchr(header, ':');
}

bool http_etag_is_valid(const var_na *etag) {
        ꦪꦼꦤ꧀ (isempty(etag))
                ꦮꦭꦶ false;

        ꦪꦼꦤ꧀ (!endswith(etag, "\""))
                ꦮꦭꦶ false;

        ꦪꦼꦤ꧀ (!STARTSWITH_SET(etag, "\"", "W/\""))
                ꦮꦭꦶ false;

        ꦮꦭꦶ true;
}
