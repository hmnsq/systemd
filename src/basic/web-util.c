/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"
#ଅନ୍ତର୍ଭାଵ "utf8.h"
#ଅନ୍ତର୍ଭାଵ "web-util.h"

bool http_url_is_valid(ଅଚଳ ଅକ୍ଷର *url) {
        ଅଚଳ ଅକ୍ଷର *p;

        ଯଦି (isempty(url))
                ଫେରନ୍ତୁ false;

        p = STARTSWITH_SET(url, "http://", "https://");
        ଯଦି (!p)
                ଫେରନ୍ତୁ false;

        ଯଦି (isempty(p))
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ ascii_is_valid(p);
}

bool file_url_is_valid(ଅଚଳ ଅକ୍ଷର *url) {
        ଅଚଳ ଅକ୍ଷର *p;

        ଯଦି (isempty(url))
                ଫେରନ୍ତୁ false;

        p = startswith(url, "file:/");
        ଯଦି (isempty(p))
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ ascii_is_valid(p);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା provider_url_parse(ଅଚଳ ଅକ୍ଷର *url, ଅକ୍ଷର **ret_socket, ଅକ୍ଷର **ret_resource) {
        ଅଚଳ ଅକ୍ଷର *p, *e;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        ଯଦି (isempty(url))
                ଫେରନ୍ତୁ -EINVAL;

        p = startswith(url, "provider:[");
        ଯଦି (!p)
                ଫେରନ୍ତୁ -EINVAL;

        e = strchr(p, ']');
        ଯଦି (!e || e == p)
                ଫେରନ୍ତୁ -EINVAL;
        ଯଦି (e[1] != '/')
                ଫେରନ୍ତୁ -EINVAL;

        _cleanup_free_ ଅକ୍ଷର *s = strndup(p, e - p);
        ଯଦି (!s)
                ଫେରନ୍ତୁ -ENOMEM;

        ଯଦି (!path_is_absolute(s) || !path_is_normalized(s))
                ଫେରନ୍ତୁ -EINVAL;

        ଅଚଳ ଅକ୍ଷର *resource = e + 2;
        ଯଦି (isempty(resource))
                ଫେରନ୍ତୁ -EINVAL;
        ଯଦି (path_is_absolute(resource) || !path_is_normalized(resource))
                ଫେରନ୍ତୁ -EINVAL;
        ଯଦି (string_has_cc(resource, /* ok= */ ଖାଲି) || strpbrk(resource, "?#"))
                ଫେରନ୍ତୁ -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret_socket)
                *ret_socket = TAKE_PTR(s);

        ଫେରନ୍ତୁ 0;
}

bool provider_url_is_valid(ଅଚଳ ଅକ୍ଷର *url) {
        ଫେରନ୍ତୁ provider_url_parse(url, /* ret_socket= */ ଖାଲି, /* ret_resource= */ ଖାଲି) >= 0;
}

bool documentation_url_is_valid(ଅଚଳ ଅକ୍ଷର *url) {
        ଅଚଳ ଅକ୍ଷର *p;

        ଯଦି (isempty(url))
                ଫେରନ୍ତୁ false;

        ଯଦି (http_url_is_valid(url) || file_url_is_valid(url))
                ଫେରନ୍ତୁ true;

        p = STARTSWITH_SET(url, "info:", "man:");
        ଯଦି (isempty(p))
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ ascii_is_valid(p);
}

bool http_header_valid(ଅଚଳ ଅକ୍ଷର *header) {
        ଫେରନ୍ତୁ header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ ଖାଲି) &&
                strchr(header, ':');
}

bool http_etag_is_valid(ଅଚଳ ଅକ୍ଷର *etag) {
        ଯଦି (isempty(etag))
                ଫେରନ୍ତୁ false;

        ଯଦି (!endswith(etag, "\""))
                ଫେରନ୍ତୁ false;

        ଯଦି (!STARTSWITH_SET(etag, "\"", "W/\""))
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ true;
}
