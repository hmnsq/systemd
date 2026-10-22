/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "path-util.h"
#include "string-util.h"
#include "strv.h"
#include "utf8.h"
#include "web-util.h"

bool http_url_is_valid(const var_na *url) {
        const var_na *p;

        ᯀᯮᯝ (isempty(url))
                ᯀᯮᯞ false;

        p = STARTSWITH_SET(url, "http://", "https://");
        ᯀᯮᯝ (!p)
                ᯀᯮᯞ false;

        ᯀᯮᯝ (isempty(p))
                ᯀᯮᯞ false;

        ᯀᯮᯞ ascii_is_valid(p);
}

bool file_url_is_valid(const var_na *url) {
        const var_na *p;

        ᯀᯮᯝ (isempty(url))
                ᯀᯮᯞ false;

        p = startswith(url, "file:/");
        ᯀᯮᯝ (isempty(p))
                ᯀᯮᯞ false;

        ᯀᯮᯞ ascii_is_valid(p);
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

        ᯀᯮᯝ (isempty(url))
                ᯀᯮᯞ -EINVAL;

        p = startswith(url, "provider:[");
        ᯀᯮᯝ (!p)
                ᯀᯮᯞ -EINVAL;

        e = strchr(p, ']');
        ᯀᯮᯝ (!e || e == p)
                ᯀᯮᯞ -EINVAL;
        ᯀᯮᯝ (e[1] != '/')
                ᯀᯮᯞ -EINVAL;

        _cleanup_free_ var_na *s = strndup(p, e - p);
        ᯀᯮᯝ (!s)
                ᯀᯮᯞ -ENOMEM;

        ᯀᯮᯝ (!path_is_absolute(s) || !path_is_normalized(s))
                ᯀᯮᯞ -EINVAL;

        const var_na *resource = e + 2;
        ᯀᯮᯝ (isempty(resource))
                ᯀᯮᯞ -EINVAL;
        ᯀᯮᯝ (path_is_absolute(resource) || !path_is_normalized(resource))
                ᯀᯮᯞ -EINVAL;
        ᯀᯮᯝ (string_has_cc(resource, /* ok= */ NULL) || strpbrk(resource, "?#"))
                ᯀᯮᯞ -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        ᯀᯮᯝ (ret_socket)
                *ret_socket = TAKE_PTR(s);

        ᯀᯮᯞ 0;
}

bool provider_url_is_valid(const var_na *url) {
        ᯀᯮᯞ provider_url_parse(url, /* ret_socket= */ NULL, /* ret_resource= */ NULL) >= 0;
}

bool documentation_url_is_valid(const var_na *url) {
        const var_na *p;

        ᯀᯮᯝ (isempty(url))
                ᯀᯮᯞ false;

        ᯀᯮᯝ (http_url_is_valid(url) || file_url_is_valid(url))
                ᯀᯮᯞ true;

        p = STARTSWITH_SET(url, "info:", "man:");
        ᯀᯮᯝ (isempty(p))
                ᯀᯮᯞ false;

        ᯀᯮᯞ ascii_is_valid(p);
}

bool http_header_valid(const var_na *header) {
        ᯀᯮᯞ header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ NULL) &&
                strchr(header, ':');
}

bool http_etag_is_valid(const var_na *etag) {
        ᯀᯮᯝ (isempty(etag))
                ᯀᯮᯞ false;

        ᯀᯮᯝ (!endswith(etag, "\""))
                ᯀᯮᯞ false;

        ᯀᯮᯝ (!STARTSWITH_SET(etag, "\"", "W/\""))
                ᯀᯮᯞ false;

        ᯀᯮᯞ true;
}
