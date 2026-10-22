/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"
#অন্তর্ভাব "utf8.h"
#অন্তর্ভাব "web-util.h"

bool http_url_is_valid(ধ্রুবক অক্ষর *url) {
        ধ্রুবক অক্ষর *p;

        যদি (isempty(url))
                ফেরত false;

        p = STARTSWITH_SET(url, "http://", "https://");
        যদি (!p)
                ফেরত false;

        যদি (isempty(p))
                ফেরত false;

        ফেরত ascii_is_valid(p);
}

bool file_url_is_valid(ধ্রুবক অক্ষর *url) {
        ধ্রুবক অক্ষর *p;

        যদি (isempty(url))
                ফেরত false;

        p = startswith(url, "file:/");
        যদি (isempty(p))
                ফেরত false;

        ফেরত ascii_is_valid(p);
}

পূর্ণসংখ্যা provider_url_parse(ধ্রুবক অক্ষর *url, অক্ষর **ret_socket, অক্ষর **ret_resource) {
        ধ্রুবক অক্ষর *p, *e;
        পূর্ণসংখ্যা r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        যদি (isempty(url))
                ফেরত -EINVAL;

        p = startswith(url, "provider:[");
        যদি (!p)
                ফেরত -EINVAL;

        e = strchr(p, ']');
        যদি (!e || e == p)
                ফেরত -EINVAL;
        যদি (e[1] != '/')
                ফেরত -EINVAL;

        _cleanup_free_ অক্ষর *s = strndup(p, e - p);
        যদি (!s)
                ফেরত -ENOMEM;

        যদি (!path_is_absolute(s) || !path_is_normalized(s))
                ফেরত -EINVAL;

        ধ্রুবক অক্ষর *resource = e + 2;
        যদি (isempty(resource))
                ফেরত -EINVAL;
        যদি (path_is_absolute(resource) || !path_is_normalized(resource))
                ফেরত -EINVAL;
        যদি (string_has_cc(resource, /* ok= */ ফাঁকা) || strpbrk(resource, "?#"))
                ফেরত -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        যদি (r < 0)
                ফেরত r;

        যদি (ret_socket)
                *ret_socket = TAKE_PTR(s);

        ফেরত 0;
}

bool provider_url_is_valid(ধ্রুবক অক্ষর *url) {
        ফেরত provider_url_parse(url, /* ret_socket= */ ফাঁকা, /* ret_resource= */ ফাঁকা) >= 0;
}

bool documentation_url_is_valid(ধ্রুবক অক্ষর *url) {
        ধ্রুবক অক্ষর *p;

        যদি (isempty(url))
                ফেরত false;

        যদি (http_url_is_valid(url) || file_url_is_valid(url))
                ফেরত true;

        p = STARTSWITH_SET(url, "info:", "man:");
        যদি (isempty(p))
                ফেরত false;

        ফেরত ascii_is_valid(p);
}

bool http_header_valid(ধ্রুবক অক্ষর *header) {
        ফেরত header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ ফাঁকা) &&
                strchr(header, ':');
}

bool http_etag_is_valid(ধ্রুবক অক্ষর *etag) {
        যদি (isempty(etag))
                ফেরত false;

        যদি (!endswith(etag, "\""))
                ফেরত false;

        যদি (!STARTSWITH_SET(etag, "\"", "W/\""))
                ফেরত false;

        ফেরত true;
}
