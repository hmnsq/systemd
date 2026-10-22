/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"
#अन्तर्भाव "utf8.h"
#अन्तर्भाव "web-util.h"

bool http_url_is_valid(स्थिरांक अक्षर *url) {
        स्थिरांक अक्षर *p;

        जर (isempty(url))
                परत false;

        p = STARTSWITH_SET(url, "http://", "https://");
        जर (!p)
                परत false;

        जर (isempty(p))
                परत false;

        परत ascii_is_valid(p);
}

bool file_url_is_valid(स्थिरांक अक्षर *url) {
        स्थिरांक अक्षर *p;

        जर (isempty(url))
                परत false;

        p = startswith(url, "file:/");
        जर (isempty(p))
                परत false;

        परत ascii_is_valid(p);
}

पूर्णांक provider_url_parse(स्थिरांक अक्षर *url, अक्षर **ret_socket, अक्षर **ret_resource) {
        स्थिरांक अक्षर *p, *e;
        पूर्णांक r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        जर (isempty(url))
                परत -EINVAL;

        p = startswith(url, "provider:[");
        जर (!p)
                परत -EINVAL;

        e = म_अक्षर(p, ']');
        जर (!e || e == p)
                परत -EINVAL;
        जर (e[1] != '/')
                परत -EINVAL;

        _cleanup_free_ अक्षर *s = strndup(p, e - p);
        जर (!s)
                परत -ENOMEM;

        जर (!path_is_absolute(s) || !path_is_normalized(s))
                परत -EINVAL;

        स्थिरांक अक्षर *resource = e + 2;
        जर (isempty(resource))
                परत -EINVAL;
        जर (path_is_absolute(resource) || !path_is_normalized(resource))
                परत -EINVAL;
        जर (string_has_cc(resource, /* ok= */ रिक्त) || strpbrk(resource, "?#"))
                परत -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        जर (r < 0)
                परत r;

        जर (ret_socket)
                *ret_socket = TAKE_PTR(s);

        परत 0;
}

bool provider_url_is_valid(स्थिरांक अक्षर *url) {
        परत provider_url_parse(url, /* ret_socket= */ रिक्त, /* ret_resource= */ रिक्त) >= 0;
}

bool documentation_url_is_valid(स्थिरांक अक्षर *url) {
        स्थिरांक अक्षर *p;

        जर (isempty(url))
                परत false;

        जर (http_url_is_valid(url) || file_url_is_valid(url))
                परत true;

        p = STARTSWITH_SET(url, "info:", "man:");
        जर (isempty(p))
                परत false;

        परत ascii_is_valid(p);
}

bool http_header_valid(स्थिरांक अक्षर *header) {
        परत header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ रिक्त) &&
                म_अक्षर(header, ':');
}

bool http_etag_is_valid(स्थिरांक अक्षर *etag) {
        जर (isempty(etag))
                परत false;

        जर (!endswith(etag, "\""))
                परत false;

        जर (!STARTSWITH_SET(etag, "\"", "W/\""))
                परत false;

        परत true;
}
