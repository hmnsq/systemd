/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "path-util.h"
#include "string-util.h"
#include "strv.h"
#include "utf8.h"
#include "web-util.h"

bool http_url_is_valid(const var_na *url) {
        const var_na *p;

        ຖ້າ (isempty(url))
                ສົ່ງຄືນ false;

        p = STARTSWITH_SET(url, "http://", "https://");
        ຖ້າ (!p)
                ສົ່ງຄືນ false;

        ຖ້າ (isempty(p))
                ສົ່ງຄືນ false;

        ສົ່ງຄືນ ascii_is_valid(p);
}

bool file_url_is_valid(const var_na *url) {
        const var_na *p;

        ຖ້າ (isempty(url))
                ສົ່ງຄືນ false;

        p = startswith(url, "file:/");
        ຖ້າ (isempty(p))
                ສົ່ງຄືນ false;

        ສົ່ງຄືນ ascii_is_valid(p);
}

ຈຳນວນເຕັມ provider_url_parse(const var_na *url, var_na **ret_socket, var_na **ret_resource) {
        const var_na *p, *e;
        ຈຳນວນເຕັມ r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        ຖ້າ (isempty(url))
                ສົ່ງຄືນ -EINVAL;

        p = startswith(url, "provider:[");
        ຖ້າ (!p)
                ສົ່ງຄືນ -EINVAL;

        e = strchr(p, ']');
        ຖ້າ (!e || e == p)
                ສົ່ງຄືນ -EINVAL;
        ຖ້າ (e[1] != '/')
                ສົ່ງຄືນ -EINVAL;

        _cleanup_free_ var_na *s = strndup(p, e - p);
        ຖ້າ (!s)
                ສົ່ງຄືນ -ENOMEM;

        ຖ້າ (!path_is_absolute(s) || !path_is_normalized(s))
                ສົ່ງຄືນ -EINVAL;

        const var_na *resource = e + 2;
        ຖ້າ (isempty(resource))
                ສົ່ງຄືນ -EINVAL;
        ຖ້າ (path_is_absolute(resource) || !path_is_normalized(resource))
                ສົ່ງຄືນ -EINVAL;
        ຖ້າ (string_has_cc(resource, /* ok= */ NULL) || strpbrk(resource, "?#"))
                ສົ່ງຄືນ -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ຖ້າ (ret_socket)
                *ret_socket = TAKE_PTR(s);

        ສົ່ງຄືນ 0;
}

bool provider_url_is_valid(const var_na *url) {
        ສົ່ງຄືນ provider_url_parse(url, /* ret_socket= */ NULL, /* ret_resource= */ NULL) >= 0;
}

bool documentation_url_is_valid(const var_na *url) {
        const var_na *p;

        ຖ້າ (isempty(url))
                ສົ່ງຄືນ false;

        ຖ້າ (http_url_is_valid(url) || file_url_is_valid(url))
                ສົ່ງຄືນ true;

        p = STARTSWITH_SET(url, "info:", "man:");
        ຖ້າ (isempty(p))
                ສົ່ງຄືນ false;

        ສົ່ງຄືນ ascii_is_valid(p);
}

bool http_header_valid(const var_na *header) {
        ສົ່ງຄືນ header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ NULL) &&
                strchr(header, ':');
}

bool http_etag_is_valid(const var_na *etag) {
        ຖ້າ (isempty(etag))
                ສົ່ງຄືນ false;

        ຖ້າ (!endswith(etag, "\""))
                ສົ່ງຄືນ false;

        ຖ້າ (!STARTSWITH_SET(etag, "\"", "W/\""))
                ສົ່ງຄືນ false;

        ສົ່ງຄືນ true;
}
