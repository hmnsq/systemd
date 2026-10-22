/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"
#ಅನ್ತರ್ಭಾವ "utf8.h"
#ಅನ್ತರ್ಭಾವ "web-util.h"

bool http_url_is_valid(ಧ್ರುವ ವರ್ಣ *url) {
        ಧ್ರುವ ವರ್ಣ *p;

        ಯದಿ (isempty(url))
                ಪ್ರತ್ಯಾಗಮ false;

        p = STARTSWITH_SET(url, "http://", "https://");
        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (isempty(p))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ ascii_is_valid(p);
}

bool file_url_is_valid(ಧ್ರುವ ವರ್ಣ *url) {
        ಧ್ರುವ ವರ್ಣ *p;

        ಯದಿ (isempty(url))
                ಪ್ರತ್ಯಾಗಮ false;

        p = startswith(url, "file:/");
        ಯದಿ (isempty(p))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ ascii_is_valid(p);
}

ಪೂರ್ಣಾಙ್ಕ provider_url_parse(ಧ್ರುವ ವರ್ಣ *url, ವರ್ಣ **ret_socket, ವರ್ಣ **ret_resource) {
        ಧ್ರುವ ವರ್ಣ *p, *e;
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        ಯದಿ (isempty(url))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        p = startswith(url, "provider:[");
        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        e = strchr(p, ']');
        ಯದಿ (!e || e == p)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        ಯದಿ (e[1] != '/')
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        _cleanup_free_ ವರ್ಣ *s = strndup(p, e - p);
        ಯದಿ (!s)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಯದಿ (!path_is_absolute(s) || !path_is_normalized(s))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಧ್ರುವ ವರ್ಣ *resource = e + 2;
        ಯದಿ (isempty(resource))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        ಯದಿ (path_is_absolute(resource) || !path_is_normalized(resource))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        ಯದಿ (string_has_cc(resource, /* ok= */ ರಿಕ್ತ) || strpbrk(resource, "?#"))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret_socket)
                *ret_socket = TAKE_PTR(s);

        ಪ್ರತ್ಯಾಗಮ 0;
}

bool provider_url_is_valid(ಧ್ರುವ ವರ್ಣ *url) {
        ಪ್ರತ್ಯಾಗಮ provider_url_parse(url, /* ret_socket= */ ರಿಕ್ತ, /* ret_resource= */ ರಿಕ್ತ) >= 0;
}

bool documentation_url_is_valid(ಧ್ರುವ ವರ್ಣ *url) {
        ಧ್ರುವ ವರ್ಣ *p;

        ಯದಿ (isempty(url))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (http_url_is_valid(url) || file_url_is_valid(url))
                ಪ್ರತ್ಯಾಗಮ true;

        p = STARTSWITH_SET(url, "info:", "man:");
        ಯದಿ (isempty(p))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ ascii_is_valid(p);
}

bool http_header_valid(ಧ್ರುವ ವರ್ಣ *header) {
        ಪ್ರತ್ಯಾಗಮ header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ ರಿಕ್ತ) &&
                strchr(header, ':');
}

bool http_etag_is_valid(ಧ್ರುವ ವರ್ಣ *etag) {
        ಯದಿ (isempty(etag))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (!endswith(etag, "\""))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (!STARTSWITH_SET(etag, "\"", "W/\""))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ true;
}
