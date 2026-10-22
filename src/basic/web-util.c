/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "path-util.h"
#অন্তর্ভাৱ "string-util.h"
#অন্তর্ভাৱ "strv.h"
#অন্তর্ভাৱ "utf8.h"
#অন্তর্ভাৱ "web-util.h"

bool http_url_is_valid(ধ্রুৱ ৱর্ণ *url) {
        ধ্রুৱ ৱর্ণ *p;

        যদি (isempty(url))
                ঘূৰাই false;

        p = STARTSWITH_SET(url, "http://", "https://");
        যদি (!p)
                ঘূৰাই false;

        যদি (isempty(p))
                ঘূৰাই false;

        ঘূৰাই ascii_is_valid(p);
}

bool file_url_is_valid(ধ্রুৱ ৱর্ণ *url) {
        ধ্রুৱ ৱর্ণ *p;

        যদি (isempty(url))
                ঘূৰাই false;

        p = startswith(url, "file:/");
        যদি (isempty(p))
                ঘূৰাই false;

        ঘূৰাই ascii_is_valid(p);
}

পূর্ণাঙ্ক provider_url_parse(ধ্রুৱ ৱর্ণ *url, ৱর্ণ **ret_socket, ৱর্ণ **ret_resource) {
        ধ্রুৱ ৱর্ণ *p, *e;
        পূর্ণাঙ্ক r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        যদি (isempty(url))
                ঘূৰাই -EINVAL;

        p = startswith(url, "provider:[");
        যদি (!p)
                ঘূৰাই -EINVAL;

        e = strchr(p, ']');
        যদি (!e || e == p)
                ঘূৰাই -EINVAL;
        যদি (e[1] != '/')
                ঘূৰাই -EINVAL;

        _cleanup_free_ ৱর্ণ *s = strndup(p, e - p);
        যদি (!s)
                ঘূৰাই -ENOMEM;

        যদি (!path_is_absolute(s) || !path_is_normalized(s))
                ঘূৰাই -EINVAL;

        ধ্রুৱ ৱর্ণ *resource = e + 2;
        যদি (isempty(resource))
                ঘূৰাই -EINVAL;
        যদি (path_is_absolute(resource) || !path_is_normalized(resource))
                ঘূৰাই -EINVAL;
        যদি (string_has_cc(resource, /* ok= */ রিক্ত) || strpbrk(resource, "?#"))
                ঘূৰাই -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (ret_socket)
                *ret_socket = TAKE_PTR(s);

        ঘূৰাই 0;
}

bool provider_url_is_valid(ধ্রুৱ ৱর্ণ *url) {
        ঘূৰাই provider_url_parse(url, /* ret_socket= */ রিক্ত, /* ret_resource= */ রিক্ত) >= 0;
}

bool documentation_url_is_valid(ধ্রুৱ ৱর্ণ *url) {
        ধ্রুৱ ৱর্ণ *p;

        যদি (isempty(url))
                ঘূৰাই false;

        যদি (http_url_is_valid(url) || file_url_is_valid(url))
                ঘূৰাই true;

        p = STARTSWITH_SET(url, "info:", "man:");
        যদি (isempty(p))
                ঘূৰাই false;

        ঘূৰাই ascii_is_valid(p);
}

bool http_header_valid(ধ্রুৱ ৱর্ণ *header) {
        ঘূৰাই header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ রিক্ত) &&
                strchr(header, ':');
}

bool http_etag_is_valid(ধ্রুৱ ৱর্ণ *etag) {
        যদি (isempty(etag))
                ঘূৰাই false;

        যদি (!endswith(etag, "\""))
                ঘূৰাই false;

        যদি (!STARTSWITH_SET(etag, "\"", "W/\""))
                ঘূৰাই false;

        ঘূৰাই true;
}
