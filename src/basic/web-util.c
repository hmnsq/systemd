/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "path-util.h"
#include "string-util.h"
#include "strv.h"
#include "utf8.h"
#include "web-util.h"

bool http_url_is_valid(const var_na *url) {
        const var_na *p;

        ᱡᱩᱫᱤ (isempty(url))
                ᱨᱩᱣᱟᱹᱲ false;

        p = STARTSWITH_SET(url, "http://", "https://");
        ᱡᱩᱫᱤ (!p)
                ᱨᱩᱣᱟᱹᱲ false;

        ᱡᱩᱫᱤ (isempty(p))
                ᱨᱩᱣᱟᱹᱲ false;

        ᱨᱩᱣᱟᱹᱲ ascii_is_valid(p);
}

bool file_url_is_valid(const var_na *url) {
        const var_na *p;

        ᱡᱩᱫᱤ (isempty(url))
                ᱨᱩᱣᱟᱹᱲ false;

        p = startswith(url, "file:/");
        ᱡᱩᱫᱤ (isempty(p))
                ᱨᱩᱣᱟᱹᱲ false;

        ᱨᱩᱣᱟᱹᱲ ascii_is_valid(p);
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

        ᱡᱩᱫᱤ (isempty(url))
                ᱨᱩᱣᱟᱹᱲ -EINVAL;

        p = startswith(url, "provider:[");
        ᱡᱩᱫᱤ (!p)
                ᱨᱩᱣᱟᱹᱲ -EINVAL;

        e = strchr(p, ']');
        ᱡᱩᱫᱤ (!e || e == p)
                ᱨᱩᱣᱟᱹᱲ -EINVAL;
        ᱡᱩᱫᱤ (e[1] != '/')
                ᱨᱩᱣᱟᱹᱲ -EINVAL;

        _cleanup_free_ var_na *s = strndup(p, e - p);
        ᱡᱩᱫᱤ (!s)
                ᱨᱩᱣᱟᱹᱲ -ENOMEM;

        ᱡᱩᱫᱤ (!path_is_absolute(s) || !path_is_normalized(s))
                ᱨᱩᱣᱟᱹᱲ -EINVAL;

        const var_na *resource = e + 2;
        ᱡᱩᱫᱤ (isempty(resource))
                ᱨᱩᱣᱟᱹᱲ -EINVAL;
        ᱡᱩᱫᱤ (path_is_absolute(resource) || !path_is_normalized(resource))
                ᱨᱩᱣᱟᱹᱲ -EINVAL;
        ᱡᱩᱫᱤ (string_has_cc(resource, /* ok= */ NULL) || strpbrk(resource, "?#"))
                ᱨᱩᱣᱟᱹᱲ -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱡᱩᱫᱤ (ret_socket)
                *ret_socket = TAKE_PTR(s);

        ᱨᱩᱣᱟᱹᱲ 0;
}

bool provider_url_is_valid(const var_na *url) {
        ᱨᱩᱣᱟᱹᱲ provider_url_parse(url, /* ret_socket= */ NULL, /* ret_resource= */ NULL) >= 0;
}

bool documentation_url_is_valid(const var_na *url) {
        const var_na *p;

        ᱡᱩᱫᱤ (isempty(url))
                ᱨᱩᱣᱟᱹᱲ false;

        ᱡᱩᱫᱤ (http_url_is_valid(url) || file_url_is_valid(url))
                ᱨᱩᱣᱟᱹᱲ true;

        p = STARTSWITH_SET(url, "info:", "man:");
        ᱡᱩᱫᱤ (isempty(p))
                ᱨᱩᱣᱟᱹᱲ false;

        ᱨᱩᱣᱟᱹᱲ ascii_is_valid(p);
}

bool http_header_valid(const var_na *header) {
        ᱨᱩᱣᱟᱹᱲ header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ NULL) &&
                strchr(header, ':');
}

bool http_etag_is_valid(const var_na *etag) {
        ᱡᱩᱫᱤ (isempty(etag))
                ᱨᱩᱣᱟᱹᱲ false;

        ᱡᱩᱫᱤ (!endswith(etag, "\""))
                ᱨᱩᱣᱟᱹᱲ false;

        ᱡᱩᱫᱤ (!STARTSWITH_SET(etag, "\"", "W/\""))
                ᱨᱩᱣᱟᱹᱲ false;

        ᱨᱩᱣᱟᱹᱲ true;
}
