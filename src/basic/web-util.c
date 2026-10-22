/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"
#अन्तर्भाव "utf8.h"
#अन्तर्भाव "web-util.h"

bool http_url_is_valid(अचर वर्ण *url) {
        अचर वर्ण *p;

        यदि (isempty(url))
                फर्काऊ false;

        p = STARTSWITH_SET(url, "http://", "https://");
        यदि (!p)
                फर्काऊ false;

        यदि (isempty(p))
                फर्काऊ false;

        फर्काऊ ascii_is_valid(p);
}

bool file_url_is_valid(अचर वर्ण *url) {
        अचर वर्ण *p;

        यदि (isempty(url))
                फर्काऊ false;

        p = startswith(url, "file:/");
        यदि (isempty(p))
                फर्काऊ false;

        फर्काऊ ascii_is_valid(p);
}

पूर्णाङ्क provider_url_parse(अचर वर्ण *url, वर्ण **ret_socket, वर्ण **ret_resource) {
        अचर वर्ण *p, *e;
        पूर्णाङ्क r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        यदि (isempty(url))
                फर्काऊ -EINVAL;

        p = startswith(url, "provider:[");
        यदि (!p)
                फर्काऊ -EINVAL;

        e = म_अक्षर(p, ']');
        यदि (!e || e == p)
                फर्काऊ -EINVAL;
        यदि (e[1] != '/')
                फर्काऊ -EINVAL;

        _cleanup_free_ वर्ण *s = strndup(p, e - p);
        यदि (!s)
                फर्काऊ -ENOMEM;

        यदि (!path_is_absolute(s) || !path_is_normalized(s))
                फर्काऊ -EINVAL;

        अचर वर्ण *resource = e + 2;
        यदि (isempty(resource))
                फर्काऊ -EINVAL;
        यदि (path_is_absolute(resource) || !path_is_normalized(resource))
                फर्काऊ -EINVAL;
        यदि (string_has_cc(resource, /* ok= */ रिक्त) || strpbrk(resource, "?#"))
                फर्काऊ -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        यदि (r < 0)
                फर्काऊ r;

        यदि (ret_socket)
                *ret_socket = TAKE_PTR(s);

        फर्काऊ 0;
}

bool provider_url_is_valid(अचर वर्ण *url) {
        फर्काऊ provider_url_parse(url, /* ret_socket= */ रिक्त, /* ret_resource= */ रिक्त) >= 0;
}

bool documentation_url_is_valid(अचर वर्ण *url) {
        अचर वर्ण *p;

        यदि (isempty(url))
                फर्काऊ false;

        यदि (http_url_is_valid(url) || file_url_is_valid(url))
                फर्काऊ true;

        p = STARTSWITH_SET(url, "info:", "man:");
        यदि (isempty(p))
                फर्काऊ false;

        फर्काऊ ascii_is_valid(p);
}

bool http_header_valid(अचर वर्ण *header) {
        फर्काऊ header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ रिक्त) &&
                म_अक्षर(header, ':');
}

bool http_etag_is_valid(अचर वर्ण *etag) {
        यदि (isempty(etag))
                फर्काऊ false;

        यदि (!endswith(etag, "\""))
                फर्काऊ false;

        यदि (!STARTSWITH_SET(etag, "\"", "W/\""))
                फर्काऊ false;

        फर्काऊ true;
}
