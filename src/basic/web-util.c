/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"
#अन्तर्भाव "utf8.h"
#अन्तर्भाव "web-util.h"

bool http_url_is_valid(ध्रुव वर्ण *url) {
        ध्रुव वर्ण *p;

        सचे (isempty(url))
                पटिनिवत्त false;

        p = STARTSWITH_SET(url, "http://", "https://");
        सचे (!p)
                पटिनिवत्त false;

        सचे (isempty(p))
                पटिनिवत्त false;

        पटिनिवत्त ascii_is_valid(p);
}

bool file_url_is_valid(ध्रुव वर्ण *url) {
        ध्रुव वर्ण *p;

        सचे (isempty(url))
                पटिनिवत्त false;

        p = startswith(url, "file:/");
        सचे (isempty(p))
                पटिनिवत्त false;

        पटिनिवत्त ascii_is_valid(p);
}

पूर्णाङ्क provider_url_parse(ध्रुव वर्ण *url, वर्ण **ret_socket, वर्ण **ret_resource) {
        ध्रुव वर्ण *p, *e;
        पूर्णाङ्क r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        सचे (isempty(url))
                पटिनिवत्त -EINVAL;

        p = startswith(url, "provider:[");
        सचे (!p)
                पटिनिवत्त -EINVAL;

        e = strchr(p, ']');
        सचे (!e || e == p)
                पटिनिवत्त -EINVAL;
        सचे (e[1] != '/')
                पटिनिवत्त -EINVAL;

        _cleanup_free_ वर्ण *s = strndup(p, e - p);
        सचे (!s)
                पटिनिवत्त -ENOMEM;

        सचे (!path_is_absolute(s) || !path_is_normalized(s))
                पटिनिवत्त -EINVAL;

        ध्रुव वर्ण *resource = e + 2;
        सचे (isempty(resource))
                पटिनिवत्त -EINVAL;
        सचे (path_is_absolute(resource) || !path_is_normalized(resource))
                पटिनिवत्त -EINVAL;
        सचे (string_has_cc(resource, /* ok= */ रिक्त) || strpbrk(resource, "?#"))
                पटिनिवत्त -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (ret_socket)
                *ret_socket = TAKE_PTR(s);

        पटिनिवत्त 0;
}

bool provider_url_is_valid(ध्रुव वर्ण *url) {
        पटिनिवत्त provider_url_parse(url, /* ret_socket= */ रिक्त, /* ret_resource= */ रिक्त) >= 0;
}

bool documentation_url_is_valid(ध्रुव वर्ण *url) {
        ध्रुव वर्ण *p;

        सचे (isempty(url))
                पटिनिवत्त false;

        सचे (http_url_is_valid(url) || file_url_is_valid(url))
                पटिनिवत्त true;

        p = STARTSWITH_SET(url, "info:", "man:");
        सचे (isempty(p))
                पटिनिवत्त false;

        पटिनिवत्त ascii_is_valid(p);
}

bool http_header_valid(ध्रुव वर्ण *header) {
        पटिनिवत्त header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ रिक्त) &&
                strchr(header, ':');
}

bool http_etag_is_valid(ध्रुव वर्ण *etag) {
        सचे (isempty(etag))
                पटिनिवत्त false;

        सचे (!endswith(etag, "\""))
                पटिनिवत्त false;

        सचे (!STARTSWITH_SET(etag, "\"", "W/\""))
                पटिनिवत्त false;

        पटिनिवत्त true;
}
