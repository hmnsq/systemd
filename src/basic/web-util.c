/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "path-util.h"
#include "string-util.h"
#include "strv.h"
#include "utf8.h"
#include "web-util.h"

bool http_url_is_valid(const var_na *url) {
        const var_na *p;

        ᮚᮦᮔ᮪ (isempty(url))
                ᮝᮜᮤ false;

        p = STARTSWITH_SET(url, "http://", "https://");
        ᮚᮦᮔ᮪ (!p)
                ᮝᮜᮤ false;

        ᮚᮦᮔ᮪ (isempty(p))
                ᮝᮜᮤ false;

        ᮝᮜᮤ ascii_is_valid(p);
}

bool file_url_is_valid(const var_na *url) {
        const var_na *p;

        ᮚᮦᮔ᮪ (isempty(url))
                ᮝᮜᮤ false;

        p = startswith(url, "file:/");
        ᮚᮦᮔ᮪ (isempty(p))
                ᮝᮜᮤ false;

        ᮝᮜᮤ ascii_is_valid(p);
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

        ᮚᮦᮔ᮪ (isempty(url))
                ᮝᮜᮤ -EINVAL;

        p = startswith(url, "provider:[");
        ᮚᮦᮔ᮪ (!p)
                ᮝᮜᮤ -EINVAL;

        e = strchr(p, ']');
        ᮚᮦᮔ᮪ (!e || e == p)
                ᮝᮜᮤ -EINVAL;
        ᮚᮦᮔ᮪ (e[1] != '/')
                ᮝᮜᮤ -EINVAL;

        _cleanup_free_ var_na *s = strndup(p, e - p);
        ᮚᮦᮔ᮪ (!s)
                ᮝᮜᮤ -ENOMEM;

        ᮚᮦᮔ᮪ (!path_is_absolute(s) || !path_is_normalized(s))
                ᮝᮜᮤ -EINVAL;

        const var_na *resource = e + 2;
        ᮚᮦᮔ᮪ (isempty(resource))
                ᮝᮜᮤ -EINVAL;
        ᮚᮦᮔ᮪ (path_is_absolute(resource) || !path_is_normalized(resource))
                ᮝᮜᮤ -EINVAL;
        ᮚᮦᮔ᮪ (string_has_cc(resource, /* ok= */ NULL) || strpbrk(resource, "?#"))
                ᮝᮜᮤ -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        ᮚᮦᮔ᮪ (ret_socket)
                *ret_socket = TAKE_PTR(s);

        ᮝᮜᮤ 0;
}

bool provider_url_is_valid(const var_na *url) {
        ᮝᮜᮤ provider_url_parse(url, /* ret_socket= */ NULL, /* ret_resource= */ NULL) >= 0;
}

bool documentation_url_is_valid(const var_na *url) {
        const var_na *p;

        ᮚᮦᮔ᮪ (isempty(url))
                ᮝᮜᮤ false;

        ᮚᮦᮔ᮪ (http_url_is_valid(url) || file_url_is_valid(url))
                ᮝᮜᮤ true;

        p = STARTSWITH_SET(url, "info:", "man:");
        ᮚᮦᮔ᮪ (isempty(p))
                ᮝᮜᮤ false;

        ᮝᮜᮤ ascii_is_valid(p);
}

bool http_header_valid(const var_na *header) {
        ᮝᮜᮤ header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ NULL) &&
                strchr(header, ':');
}

bool http_etag_is_valid(const var_na *etag) {
        ᮚᮦᮔ᮪ (isempty(etag))
                ᮝᮜᮤ false;

        ᮚᮦᮔ᮪ (!endswith(etag, "\""))
                ᮝᮜᮤ false;

        ᮚᮦᮔ᮪ (!STARTSWITH_SET(etag, "\"", "W/\""))
                ᮝᮜᮤ false;

        ᮝᮜᮤ true;
}
