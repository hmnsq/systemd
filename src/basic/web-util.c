/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "alloc-util.h"
#شامل "path-util.h"
#شامل "string-util.h"
#شامل "strv.h"
#شامل "utf8.h"
#شامل "web-util.h"

bool http_url_is_valid(ثابت نویسه *url) {
        ثابت نویسه *p;

        اگر (isempty(url))
                بازگشت false;

        p = STARTSWITH_SET(url, "http://", "https://");
        اگر (!p)
                بازگشت false;

        اگر (isempty(p))
                بازگشت false;

        بازگشت ascii_is_valid(p);
}

bool file_url_is_valid(ثابت نویسه *url) {
        ثابت نویسه *p;

        اگر (isempty(url))
                بازگشت false;

        p = startswith(url, "file:/");
        اگر (isempty(p))
                بازگشت false;

        بازگشت ascii_is_valid(p);
}

صحیح provider_url_parse(ثابت نویسه *url, نویسه **ret_socket, نویسه **ret_resource) {
        ثابت نویسه *p, *e;
        صحیح r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        اگر (isempty(url))
                بازگشت -EINVAL;

        p = startswith(url, "provider:[");
        اگر (!p)
                بازگشت -EINVAL;

        e = strchr(p, ']');
        اگر (!e || e == p)
                بازگشت -EINVAL;
        اگر (e[1] != '/')
                بازگشت -EINVAL;

        _cleanup_free_ نویسه *s = strndup(p, e - p);
        اگر (!s)
                بازگشت -ENOMEM;

        اگر (!path_is_absolute(s) || !path_is_normalized(s))
                بازگشت -EINVAL;

        ثابت نویسه *resource = e + 2;
        اگر (isempty(resource))
                بازگشت -EINVAL;
        اگر (path_is_absolute(resource) || !path_is_normalized(resource))
                بازگشت -EINVAL;
        اگر (string_has_cc(resource, /* ok= */ پوچ) || strpbrk(resource, "?#"))
                بازگشت -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        اگر (r < 0)
                بازگشت r;

        اگر (ret_socket)
                *ret_socket = TAKE_PTR(s);

        بازگشت 0;
}

bool provider_url_is_valid(ثابت نویسه *url) {
        بازگشت provider_url_parse(url, /* ret_socket= */ پوچ, /* ret_resource= */ پوچ) >= 0;
}

bool documentation_url_is_valid(ثابت نویسه *url) {
        ثابت نویسه *p;

        اگر (isempty(url))
                بازگشت false;

        اگر (http_url_is_valid(url) || file_url_is_valid(url))
                بازگشت true;

        p = STARTSWITH_SET(url, "info:", "man:");
        اگر (isempty(p))
                بازگشت false;

        بازگشت ascii_is_valid(p);
}

bool http_header_valid(ثابت نویسه *header) {
        بازگشت header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ پوچ) &&
                strchr(header, ':');
}

bool http_etag_is_valid(ثابت نویسه *etag) {
        اگر (isempty(etag))
                بازگشت false;

        اگر (!endswith(etag, "\""))
                بازگشت false;

        اگر (!STARTSWITH_SET(etag, "\"", "W/\""))
                بازگشت false;

        بازگشت true;
}
