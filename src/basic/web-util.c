/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"
#অন্তর্ভাব "utf8.h"
#অন্তর্ভাব "web-util.h"

bool http_url_is_valid(ধ্রুব বর্ণ *url) {
        ধ্রুব বর্ণ *p;

        যদি (isempty(url))
                প্রত্যাগম false;

        p = STARTSWITH_SET(url, "http://", "https://");
        যদি (!p)
                প্রত্যাগম false;

        যদি (isempty(p))
                প্রত্যাগম false;

        প্রত্যাগম ascii_is_valid(p);
}

bool file_url_is_valid(ধ্রুব বর্ণ *url) {
        ধ্রুব বর্ণ *p;

        যদি (isempty(url))
                প্রত্যাগম false;

        p = startswith(url, "file:/");
        যদি (isempty(p))
                প্রত্যাগম false;

        প্রত্যাগম ascii_is_valid(p);
}

পূর্ণাঙ্ক provider_url_parse(ধ্রুব বর্ণ *url, বর্ণ **ret_socket, বর্ণ **ret_resource) {
        ধ্রুব বর্ণ *p, *e;
        পূর্ণাঙ্ক r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        যদি (isempty(url))
                প্রত্যাগম -EINVAL;

        p = startswith(url, "provider:[");
        যদি (!p)
                প্রত্যাগম -EINVAL;

        e = strchr(p, ']');
        যদি (!e || e == p)
                প্রত্যাগম -EINVAL;
        যদি (e[1] != '/')
                প্রত্যাগম -EINVAL;

        _cleanup_free_ বর্ণ *s = strndup(p, e - p);
        যদি (!s)
                প্রত্যাগম -ENOMEM;

        যদি (!path_is_absolute(s) || !path_is_normalized(s))
                প্রত্যাগম -EINVAL;

        ধ্রুব বর্ণ *resource = e + 2;
        যদি (isempty(resource))
                প্রত্যাগম -EINVAL;
        যদি (path_is_absolute(resource) || !path_is_normalized(resource))
                প্রত্যাগম -EINVAL;
        যদি (string_has_cc(resource, /* ok= */ রিক্ত) || strpbrk(resource, "?#"))
                প্রত্যাগম -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (ret_socket)
                *ret_socket = TAKE_PTR(s);

        প্রত্যাগম 0;
}

bool provider_url_is_valid(ধ্রুব বর্ণ *url) {
        প্রত্যাগম provider_url_parse(url, /* ret_socket= */ রিক্ত, /* ret_resource= */ রিক্ত) >= 0;
}

bool documentation_url_is_valid(ধ্রুব বর্ণ *url) {
        ধ্রুব বর্ণ *p;

        যদি (isempty(url))
                প্রত্যাগম false;

        যদি (http_url_is_valid(url) || file_url_is_valid(url))
                প্রত্যাগম true;

        p = STARTSWITH_SET(url, "info:", "man:");
        যদি (isempty(p))
                প্রত্যাগম false;

        প্রত্যাগম ascii_is_valid(p);
}

bool http_header_valid(ধ্রুব বর্ণ *header) {
        প্রত্যাগম header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ রিক্ত) &&
                strchr(header, ':');
}

bool http_etag_is_valid(ধ্রুব বর্ণ *etag) {
        যদি (isempty(etag))
                প্রত্যাগম false;

        যদি (!endswith(etag, "\""))
                প্রত্যাগম false;

        যদি (!STARTSWITH_SET(etag, "\"", "W/\""))
                প্রত্যাগম false;

        প্রত্যাগম true;
}
