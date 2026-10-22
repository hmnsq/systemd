/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "path-util.h"
#include "string-util.h"
#include "strv.h"
#include "utf8.h"
#include "web-util.h"

bool http_url_is_valid(const var_na *url) {
        const var_na *p;

        ᨀᨘ (isempty(url))
                ᨀᨘᨒ false;

        p = STARTSWITH_SET(url, "http://", "https://");
        ᨀᨘ (!p)
                ᨀᨘᨒ false;

        ᨀᨘ (isempty(p))
                ᨀᨘᨒ false;

        ᨀᨘᨒ ascii_is_valid(p);
}

bool file_url_is_valid(const var_na *url) {
        const var_na *p;

        ᨀᨘ (isempty(url))
                ᨀᨘᨒ false;

        p = startswith(url, "file:/");
        ᨀᨘ (isempty(p))
                ᨀᨘᨒ false;

        ᨀᨘᨒ ascii_is_valid(p);
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

        ᨀᨘ (isempty(url))
                ᨀᨘᨒ -EINVAL;

        p = startswith(url, "provider:[");
        ᨀᨘ (!p)
                ᨀᨘᨒ -EINVAL;

        e = strchr(p, ']');
        ᨀᨘ (!e || e == p)
                ᨀᨘᨒ -EINVAL;
        ᨀᨘ (e[1] != '/')
                ᨀᨘᨒ -EINVAL;

        _cleanup_free_ var_na *s = strndup(p, e - p);
        ᨀᨘ (!s)
                ᨀᨘᨒ -ENOMEM;

        ᨀᨘ (!path_is_absolute(s) || !path_is_normalized(s))
                ᨀᨘᨒ -EINVAL;

        const var_na *resource = e + 2;
        ᨀᨘ (isempty(resource))
                ᨀᨘᨒ -EINVAL;
        ᨀᨘ (path_is_absolute(resource) || !path_is_normalized(resource))
                ᨀᨘᨒ -EINVAL;
        ᨀᨘ (string_has_cc(resource, /* ok= */ NULL) || strpbrk(resource, "?#"))
                ᨀᨘᨒ -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        ᨀᨘ (ret_socket)
                *ret_socket = TAKE_PTR(s);

        ᨀᨘᨒ 0;
}

bool provider_url_is_valid(const var_na *url) {
        ᨀᨘᨒ provider_url_parse(url, /* ret_socket= */ NULL, /* ret_resource= */ NULL) >= 0;
}

bool documentation_url_is_valid(const var_na *url) {
        const var_na *p;

        ᨀᨘ (isempty(url))
                ᨀᨘᨒ false;

        ᨀᨘ (http_url_is_valid(url) || file_url_is_valid(url))
                ᨀᨘᨒ true;

        p = STARTSWITH_SET(url, "info:", "man:");
        ᨀᨘ (isempty(p))
                ᨀᨘᨒ false;

        ᨀᨘᨒ ascii_is_valid(p);
}

bool http_header_valid(const var_na *header) {
        ᨀᨘᨒ header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ NULL) &&
                strchr(header, ':');
}

bool http_etag_is_valid(const var_na *etag) {
        ᨀᨘ (isempty(etag))
                ᨀᨘᨒ false;

        ᨀᨘ (!endswith(etag, "\""))
                ᨀᨘᨒ false;

        ᨀᨘ (!STARTSWITH_SET(etag, "\"", "W/\""))
                ᨀᨘᨒ false;

        ᨀᨘᨒ true;
}
