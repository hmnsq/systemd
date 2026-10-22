/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "path-util.h"
#include "string-util.h"
#include "strv.h"
#include "utf8.h"
#include "web-util.h"

bool http_url_is_valid(const var_na *url) {
        const var_na *p;

        အကယ်၍ (isempty(url))
                ပြန်ပေး false;

        p = STARTSWITH_SET(url, "http://", "https://");
        အကယ်၍ (!p)
                ပြန်ပေး false;

        အကယ်၍ (isempty(p))
                ပြန်ပေး false;

        ပြန်ပေး ascii_is_valid(p);
}

bool file_url_is_valid(const var_na *url) {
        const var_na *p;

        အကယ်၍ (isempty(url))
                ပြန်ပေး false;

        p = startswith(url, "file:/");
        အကယ်၍ (isempty(p))
                ပြန်ပေး false;

        ပြန်ပေး ascii_is_valid(p);
}

ကိန်းပြည့် provider_url_parse(const var_na *url, var_na **ret_socket, var_na **ret_resource) {
        const var_na *p, *e;
        ကိန်းပြည့် r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        အကယ်၍ (isempty(url))
                ပြန်ပေး -EINVAL;

        p = startswith(url, "provider:[");
        အကယ်၍ (!p)
                ပြန်ပေး -EINVAL;

        e = strchr(p, ']');
        အကယ်၍ (!e || e == p)
                ပြန်ပေး -EINVAL;
        အကယ်၍ (e[1] != '/')
                ပြန်ပေး -EINVAL;

        _cleanup_free_ var_na *s = strndup(p, e - p);
        အကယ်၍ (!s)
                ပြန်ပေး -ENOMEM;

        အကယ်၍ (!path_is_absolute(s) || !path_is_normalized(s))
                ပြန်ပေး -EINVAL;

        const var_na *resource = e + 2;
        အကယ်၍ (isempty(resource))
                ပြန်ပေး -EINVAL;
        အကယ်၍ (path_is_absolute(resource) || !path_is_normalized(resource))
                ပြန်ပေး -EINVAL;
        အကယ်၍ (string_has_cc(resource, /* ok= */ NULL) || strpbrk(resource, "?#"))
                ပြန်ပေး -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (ret_socket)
                *ret_socket = TAKE_PTR(s);

        ပြန်ပေး 0;
}

bool provider_url_is_valid(const var_na *url) {
        ပြန်ပေး provider_url_parse(url, /* ret_socket= */ NULL, /* ret_resource= */ NULL) >= 0;
}

bool documentation_url_is_valid(const var_na *url) {
        const var_na *p;

        အကယ်၍ (isempty(url))
                ပြန်ပေး false;

        အကယ်၍ (http_url_is_valid(url) || file_url_is_valid(url))
                ပြန်ပေး true;

        p = STARTSWITH_SET(url, "info:", "man:");
        အကယ်၍ (isempty(p))
                ပြန်ပေး false;

        ပြန်ပေး ascii_is_valid(p);
}

bool http_header_valid(const var_na *header) {
        ပြန်ပေး header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ NULL) &&
                strchr(header, ':');
}

bool http_etag_is_valid(const var_na *etag) {
        အကယ်၍ (isempty(etag))
                ပြန်ပေး false;

        အကယ်၍ (!endswith(etag, "\""))
                ပြန်ပေး false;

        အကယ်၍ (!STARTSWITH_SET(etag, "\"", "W/\""))
                ပြန်ပေး false;

        ပြန်ပေး true;
}
