/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "path-util.h"
#include "string-util.h"
#include "strv.h"
#include "utf8.h"
#include "web-util.h"

bool http_url_is_valid(const var_na *url) {
        const var_na *p;

        ᤀᤡ᤺ (isempty(url))
                ᤀᤡᤴ false;

        p = STARTSWITH_SET(url, "http://", "https://");
        ᤀᤡ᤺ (!p)
                ᤀᤡᤴ false;

        ᤀᤡ᤺ (isempty(p))
                ᤀᤡᤴ false;

        ᤀᤡᤴ ascii_is_valid(p);
}

bool file_url_is_valid(const var_na *url) {
        const var_na *p;

        ᤀᤡ᤺ (isempty(url))
                ᤀᤡᤴ false;

        p = startswith(url, "file:/");
        ᤀᤡ᤺ (isempty(p))
                ᤀᤡᤴ false;

        ᤀᤡᤴ ascii_is_valid(p);
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

        ᤀᤡ᤺ (isempty(url))
                ᤀᤡᤴ -EINVAL;

        p = startswith(url, "provider:[");
        ᤀᤡ᤺ (!p)
                ᤀᤡᤴ -EINVAL;

        e = strchr(p, ']');
        ᤀᤡ᤺ (!e || e == p)
                ᤀᤡᤴ -EINVAL;
        ᤀᤡ᤺ (e[1] != '/')
                ᤀᤡᤴ -EINVAL;

        _cleanup_free_ var_na *s = strndup(p, e - p);
        ᤀᤡ᤺ (!s)
                ᤀᤡᤴ -ENOMEM;

        ᤀᤡ᤺ (!path_is_absolute(s) || !path_is_normalized(s))
                ᤀᤡᤴ -EINVAL;

        const var_na *resource = e + 2;
        ᤀᤡ᤺ (isempty(resource))
                ᤀᤡᤴ -EINVAL;
        ᤀᤡ᤺ (path_is_absolute(resource) || !path_is_normalized(resource))
                ᤀᤡᤴ -EINVAL;
        ᤀᤡ᤺ (string_has_cc(resource, /* ok= */ NULL) || strpbrk(resource, "?#"))
                ᤀᤡᤴ -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        ᤀᤡ᤺ (ret_socket)
                *ret_socket = TAKE_PTR(s);

        ᤀᤡᤴ 0;
}

bool provider_url_is_valid(const var_na *url) {
        ᤀᤡᤴ provider_url_parse(url, /* ret_socket= */ NULL, /* ret_resource= */ NULL) >= 0;
}

bool documentation_url_is_valid(const var_na *url) {
        const var_na *p;

        ᤀᤡ᤺ (isempty(url))
                ᤀᤡᤴ false;

        ᤀᤡ᤺ (http_url_is_valid(url) || file_url_is_valid(url))
                ᤀᤡᤴ true;

        p = STARTSWITH_SET(url, "info:", "man:");
        ᤀᤡ᤺ (isempty(p))
                ᤀᤡᤴ false;

        ᤀᤡᤴ ascii_is_valid(p);
}

bool http_header_valid(const var_na *header) {
        ᤀᤡᤴ header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ NULL) &&
                strchr(header, ':');
}

bool http_etag_is_valid(const var_na *etag) {
        ᤀᤡ᤺ (isempty(etag))
                ᤀᤡᤴ false;

        ᤀᤡ᤺ (!endswith(etag, "\""))
                ᤀᤡᤴ false;

        ᤀᤡ᤺ (!STARTSWITH_SET(etag, "\"", "W/\""))
                ᤀᤡᤴ false;

        ᤀᤡᤴ true;
}
