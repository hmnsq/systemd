/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"
#ಅನ್ತರ್ಭಾವ "utf8.h"
#ಅನ್ತರ್ಭಾವ "web-util.h"

bool http_url_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;

        ಆದರೆ (isempty(url))
                ಹಿಂತಿರುಗಿ false;

        p = STARTSWITH_ಗಣ(url, "http://", "https://");
        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (isempty(p))
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ ascii_is_valid(p);
}

bool file_url_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;

        ಆದರೆ (isempty(url))
                ಹಿಂತಿರುಗಿ false;

        p = startswith(url, "file:/");
        ಆದರೆ (isempty(p))
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ ascii_is_valid(p);
}

ಪೂರ್ಣಾಂಕ provider_url_parse(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url, ಅಕ್ಷರ **ret_socket, ಅಕ್ಷರ **ret_resource) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, *e;
        ಪೂರ್ಣಾಂಕ r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        ಆದರೆ (isempty(url))
                ಹಿಂತಿರುಗಿ -EINVAL;

        p = startswith(url, "provider:[");
        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ -EINVAL;

        e = strchr(p, ']');
        ಆದರೆ (!e || e == p)
                ಹಿಂತಿರುಗಿ -EINVAL;
        ಆದರೆ (e[1] != '/')
                ಹಿಂತಿರುಗಿ -EINVAL;

        _cleanup_free_ ಅಕ್ಷರ *s = strndup(p, e - p);
        ಆದರೆ (!s)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಆದರೆ (!ಮಾರ್ಗ_is_absolute(s) || !ಮಾರ್ಗ_is_normalized(s))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *resource = e + 2;
        ಆದರೆ (isempty(resource))
                ಹಿಂತಿರುಗಿ -EINVAL;
        ಆದರೆ (ಮಾರ್ಗ_is_absolute(resource) || !ಮಾರ್ಗ_is_normalized(resource))
                ಹಿಂತಿರುಗಿ -EINVAL;
        ಆದರೆ (string_has_cc(resource, /* ok= */ ಖಾಲಿ) || strpbrk(resource, "?#"))
                ಹಿಂತಿರುಗಿ -EINVAL;

        r = strdup_ಗಮ್ಯ_ತುಂಬಿದ(ret_resource, resource);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret_socket)
                *ret_socket = TAKE_PTR(s);

        ಹಿಂತಿರುಗಿ 0;
}

bool provider_url_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url) {
        ಹಿಂತಿರುಗಿ provider_url_parse(url, /* ret_socket= */ ಖಾಲಿ, /* ret_resource= */ ಖಾಲಿ) >= 0;
}

bool documentation_url_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;

        ಆದರೆ (isempty(url))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (http_url_is_valid(url) || file_url_is_valid(url))
                ಹಿಂತಿರುಗಿ true;

        p = STARTSWITH_ಗಣ(url, "info:", "man:");
        ಆದರೆ (isempty(p))
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ ascii_is_valid(p);
}

bool http_ಶೀರ್ಷಿಕೆ_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಶೀರ್ಷಿಕೆ) {
        ಹಿಂತಿರುಗಿ ಶೀರ್ಷಿಕೆ &&
                ascii_is_valid(ಶೀರ್ಷಿಕೆ) &&
                !string_has_cc(ಶೀರ್ಷಿಕೆ, /* ok= */ ಖಾಲಿ) &&
                strchr(ಶೀರ್ಷಿಕೆ, ':');
}

bool http_etag_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *etag) {
        ಆದರೆ (isempty(etag))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (!endswith(etag, "\""))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (!STARTSWITH_ಗಣ(etag, "\"", "W/\""))
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ true;
}
