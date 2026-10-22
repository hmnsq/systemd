/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"
#અન્તર્ભાવ "utf8.h"
#અન્તર્ભાવ "web-util.h"

bool http_url_is_valid(અચલ અક્ષર *url) {
        અચલ અક્ષર *p;

        જો (isempty(url))
                પાછા false;

        p = STARTSWITH_SET(url, "http://", "https://");
        જો (!p)
                પાછા false;

        જો (isempty(p))
                પાછા false;

        પાછા ascii_is_valid(p);
}

bool file_url_is_valid(અચલ અક્ષર *url) {
        અચલ અક્ષર *p;

        જો (isempty(url))
                પાછા false;

        p = startswith(url, "file:/");
        જો (isempty(p))
                પાછા false;

        પાછા ascii_is_valid(p);
}

પૂર્ણાંક provider_url_parse(અચલ અક્ષર *url, અક્ષર **ret_socket, અક્ષર **ret_resource) {
        અચલ અક્ષર *p, *e;
        પૂર્ણાંક r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        જો (isempty(url))
                પાછા -EINVAL;

        p = startswith(url, "provider:[");
        જો (!p)
                પાછા -EINVAL;

        e = strchr(p, ']');
        જો (!e || e == p)
                પાછા -EINVAL;
        જો (e[1] != '/')
                પાછા -EINVAL;

        _cleanup_free_ અક્ષર *s = strndup(p, e - p);
        જો (!s)
                પાછા -ENOMEM;

        જો (!path_is_absolute(s) || !path_is_normalized(s))
                પાછા -EINVAL;

        અચલ અક્ષર *resource = e + 2;
        જો (isempty(resource))
                પાછા -EINVAL;
        જો (path_is_absolute(resource) || !path_is_normalized(resource))
                પાછા -EINVAL;
        જો (string_has_cc(resource, /* ok= */ ખાલી) || strpbrk(resource, "?#"))
                પાછા -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        જો (r < 0)
                પાછા r;

        જો (ret_socket)
                *ret_socket = TAKE_PTR(s);

        પાછા 0;
}

bool provider_url_is_valid(અચલ અક્ષર *url) {
        પાછા provider_url_parse(url, /* ret_socket= */ ખાલી, /* ret_resource= */ ખાલી) >= 0;
}

bool documentation_url_is_valid(અચલ અક્ષર *url) {
        અચલ અક્ષર *p;

        જો (isempty(url))
                પાછા false;

        જો (http_url_is_valid(url) || file_url_is_valid(url))
                પાછા true;

        p = STARTSWITH_SET(url, "info:", "man:");
        જો (isempty(p))
                પાછા false;

        પાછા ascii_is_valid(p);
}

bool http_header_valid(અચલ અક્ષર *header) {
        પાછા header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ ખાલી) &&
                strchr(header, ':');
}

bool http_etag_is_valid(અચલ અક્ષર *etag) {
        જો (isempty(etag))
                પાછા false;

        જો (!endswith(etag, "\""))
                પાછા false;

        જો (!STARTSWITH_SET(etag, "\"", "W/\""))
                પાછા false;

        પાછા true;
}
