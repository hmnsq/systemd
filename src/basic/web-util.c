/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"
#அந்தர்பாவ "utf8.h"
#அந்தர்பாவ "web-util.h"

bool http_url_is_valid(மாறிலி எழுத்து *url) {
        மாறிலி எழுத்து *p;

        எனில் (isempty(url))
                திரும்பு false;

        p = STARTSWITH_SET(url, "http://", "https://");
        எனில் (!p)
                திரும்பு false;

        எனில் (isempty(p))
                திரும்பு false;

        திரும்பு ascii_is_valid(p);
}

bool file_url_is_valid(மாறிலி எழுத்து *url) {
        மாறிலி எழுத்து *p;

        எனில் (isempty(url))
                திரும்பு false;

        p = startswith(url, "file:/");
        எனில் (isempty(p))
                திரும்பு false;

        திரும்பு ascii_is_valid(p);
}

முழு provider_url_parse(மாறிலி எழுத்து *url, எழுத்து **ret_socket, எழுத்து **ret_resource) {
        மாறிலி எழுத்து *p, *e;
        முழு r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        எனில் (isempty(url))
                திரும்பு -EINVAL;

        p = startswith(url, "provider:[");
        எனில் (!p)
                திரும்பு -EINVAL;

        e = strchr(p, ']');
        எனில் (!e || e == p)
                திரும்பு -EINVAL;
        எனில் (e[1] != '/')
                திரும்பு -EINVAL;

        _cleanup_free_ எழுத்து *s = strndup(p, e - p);
        எனில் (!s)
                திரும்பு -ENOMEM;

        எனில் (!path_is_absolute(s) || !path_is_normalized(s))
                திரும்பு -EINVAL;

        மாறிலி எழுத்து *resource = e + 2;
        எனில் (isempty(resource))
                திரும்பு -EINVAL;
        எனில் (path_is_absolute(resource) || !path_is_normalized(resource))
                திரும்பு -EINVAL;
        எனில் (string_has_cc(resource, /* ok= */ வெற்று) || strpbrk(resource, "?#"))
                திரும்பு -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret_socket)
                *ret_socket = TAKE_PTR(s);

        திரும்பு 0;
}

bool provider_url_is_valid(மாறிலி எழுத்து *url) {
        திரும்பு provider_url_parse(url, /* ret_socket= */ வெற்று, /* ret_resource= */ வெற்று) >= 0;
}

bool documentation_url_is_valid(மாறிலி எழுத்து *url) {
        மாறிலி எழுத்து *p;

        எனில் (isempty(url))
                திரும்பு false;

        எனில் (http_url_is_valid(url) || file_url_is_valid(url))
                திரும்பு true;

        p = STARTSWITH_SET(url, "info:", "man:");
        எனில் (isempty(p))
                திரும்பு false;

        திரும்பு ascii_is_valid(p);
}

bool http_header_valid(மாறிலி எழுத்து *header) {
        திரும்பு header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ வெற்று) &&
                strchr(header, ':');
}

bool http_etag_is_valid(மாறிலி எழுத்து *etag) {
        எனில் (isempty(etag))
                திரும்பு false;

        எனில் (!endswith(etag, "\""))
                திரும்பு false;

        எனில் (!STARTSWITH_SET(etag, "\"", "W/\""))
                திரும்பு false;

        திரும்பு true;
}
