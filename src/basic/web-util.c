/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "path-util.h"
#include "string-util.h"
#include "strv.h"
#include "utf8.h"
#include "web-util.h"

bool http_url_is_valid(const var_na *url) {
        const var_na *p;

        නම් (isempty(url))
                ආපසු false;

        p = STARTSWITH_SET(url, "http://", "https://");
        නම් (!p)
                ආපසු false;

        නම් (isempty(p))
                ආපසු false;

        ආපසු ascii_is_valid(p);
}

bool file_url_is_valid(const var_na *url) {
        const var_na *p;

        නම් (isempty(url))
                ආපසු false;

        p = startswith(url, "file:/");
        නම් (isempty(p))
                ආපසු false;

        ආපසු ascii_is_valid(p);
}

පූර්ණ_සංඛ්‍යා provider_url_parse(const var_na *url, var_na **ret_socket, var_na **ret_resource) {
        const var_na *p, *e;
        පූර්ණ_සංඛ්‍යා r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        නම් (isempty(url))
                ආපසු -EINVAL;

        p = startswith(url, "provider:[");
        නම් (!p)
                ආපසු -EINVAL;

        e = strchr(p, ']');
        නම් (!e || e == p)
                ආපසු -EINVAL;
        නම් (e[1] != '/')
                ආපසු -EINVAL;

        _cleanup_free_ var_na *s = strndup(p, e - p);
        නම් (!s)
                ආපසු -ENOMEM;

        නම් (!path_is_absolute(s) || !path_is_normalized(s))
                ආපසු -EINVAL;

        const var_na *resource = e + 2;
        නම් (isempty(resource))
                ආපසු -EINVAL;
        නම් (path_is_absolute(resource) || !path_is_normalized(resource))
                ආපසු -EINVAL;
        නම් (string_has_cc(resource, /* ok= */ NULL) || strpbrk(resource, "?#"))
                ආපසු -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        නම් (r < 0)
                ආපසු r;

        නම් (ret_socket)
                *ret_socket = TAKE_PTR(s);

        ආපසු 0;
}

bool provider_url_is_valid(const var_na *url) {
        ආපසු provider_url_parse(url, /* ret_socket= */ NULL, /* ret_resource= */ NULL) >= 0;
}

bool documentation_url_is_valid(const var_na *url) {
        const var_na *p;

        නම් (isempty(url))
                ආපසු false;

        නම් (http_url_is_valid(url) || file_url_is_valid(url))
                ආපසු true;

        p = STARTSWITH_SET(url, "info:", "man:");
        නම් (isempty(p))
                ආපසු false;

        ආපසු ascii_is_valid(p);
}

bool http_header_valid(const var_na *header) {
        ආපසු header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ NULL) &&
                strchr(header, ':');
}

bool http_etag_is_valid(const var_na *etag) {
        නම් (isempty(etag))
                ආපසු false;

        නම් (!endswith(etag, "\""))
                ආපසු false;

        නම් (!STARTSWITH_SET(etag, "\"", "W/\""))
                ආපසු false;

        ආපසු true;
}
