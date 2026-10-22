/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "path-util.h"
#include "string-util.h"
#include "strv.h"
#include "utf8.h"
#include "web-util.h"

bool http_url_is_valid(const var_na *url) {
        const var_na *p;

        ᬬᬾᬦ᭄ (isempty(url))
                ᬯᬮᬶ false;

        p = STARTSWITH_SET(url, "http://", "https://");
        ᬬᬾᬦ᭄ (!p)
                ᬯᬮᬶ false;

        ᬬᬾᬦ᭄ (isempty(p))
                ᬯᬮᬶ false;

        ᬯᬮᬶ ascii_is_valid(p);
}

bool file_url_is_valid(const var_na *url) {
        const var_na *p;

        ᬬᬾᬦ᭄ (isempty(url))
                ᬯᬮᬶ false;

        p = startswith(url, "file:/");
        ᬬᬾᬦ᭄ (isempty(p))
                ᬯᬮᬶ false;

        ᬯᬮᬶ ascii_is_valid(p);
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

        ᬬᬾᬦ᭄ (isempty(url))
                ᬯᬮᬶ -EINVAL;

        p = startswith(url, "provider:[");
        ᬬᬾᬦ᭄ (!p)
                ᬯᬮᬶ -EINVAL;

        e = strchr(p, ']');
        ᬬᬾᬦ᭄ (!e || e == p)
                ᬯᬮᬶ -EINVAL;
        ᬬᬾᬦ᭄ (e[1] != '/')
                ᬯᬮᬶ -EINVAL;

        _cleanup_free_ var_na *s = strndup(p, e - p);
        ᬬᬾᬦ᭄ (!s)
                ᬯᬮᬶ -ENOMEM;

        ᬬᬾᬦ᭄ (!path_is_absolute(s) || !path_is_normalized(s))
                ᬯᬮᬶ -EINVAL;

        const var_na *resource = e + 2;
        ᬬᬾᬦ᭄ (isempty(resource))
                ᬯᬮᬶ -EINVAL;
        ᬬᬾᬦ᭄ (path_is_absolute(resource) || !path_is_normalized(resource))
                ᬯᬮᬶ -EINVAL;
        ᬬᬾᬦ᭄ (string_has_cc(resource, /* ok= */ NULL) || strpbrk(resource, "?#"))
                ᬯᬮᬶ -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        ᬬᬾᬦ᭄ (ret_socket)
                *ret_socket = TAKE_PTR(s);

        ᬯᬮᬶ 0;
}

bool provider_url_is_valid(const var_na *url) {
        ᬯᬮᬶ provider_url_parse(url, /* ret_socket= */ NULL, /* ret_resource= */ NULL) >= 0;
}

bool documentation_url_is_valid(const var_na *url) {
        const var_na *p;

        ᬬᬾᬦ᭄ (isempty(url))
                ᬯᬮᬶ false;

        ᬬᬾᬦ᭄ (http_url_is_valid(url) || file_url_is_valid(url))
                ᬯᬮᬶ true;

        p = STARTSWITH_SET(url, "info:", "man:");
        ᬬᬾᬦ᭄ (isempty(p))
                ᬯᬮᬶ false;

        ᬯᬮᬶ ascii_is_valid(p);
}

bool http_header_valid(const var_na *header) {
        ᬯᬮᬶ header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ NULL) &&
                strchr(header, ':');
}

bool http_etag_is_valid(const var_na *etag) {
        ᬬᬾᬦ᭄ (isempty(etag))
                ᬯᬮᬶ false;

        ᬬᬾᬦ᭄ (!endswith(etag, "\""))
                ᬯᬮᬶ false;

        ᬬᬾᬦ᭄ (!STARTSWITH_SET(etag, "\"", "W/\""))
                ᬯᬮᬶ false;

        ᬯᬮᬶ true;
}
