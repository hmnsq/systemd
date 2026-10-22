/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"
#అన్తర్భావ "utf8.h"
#అన్తర్భావ "web-util.h"

bool http_url_is_valid(స్థిరరాశి అక్షరం *url) {
        స్థిరరాశి అక్షరం *p;

        ఒకవేళ (isempty(url))
                తిరిగి false;

        p = STARTSWITH_సమితి(url, "http://", "https://");
        ఒకవేళ (!p)
                తిరిగి false;

        ఒకవేళ (isempty(p))
                తిరిగి false;

        తిరిగి ascii_is_valid(p);
}

bool file_url_is_valid(స్థిరరాశి అక్షరం *url) {
        స్థిరరాశి అక్షరం *p;

        ఒకవేళ (isempty(url))
                తిరిగి false;

        p = startswith(url, "file:/");
        ఒకవేళ (isempty(p))
                తిరిగి false;

        తిరిగి ascii_is_valid(p);
}

పూర్ణాంకం provider_url_parse(స్థిరరాశి అక్షరం *url, అక్షరం **ret_socket, అక్షరం **ret_resource) {
        స్థిరరాశి అక్షరం *p, *e;
        పూర్ణాంకం r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        ఒకవేళ (isempty(url))
                తిరిగి -EINVAL;

        p = startswith(url, "provider:[");
        ఒకవేళ (!p)
                తిరిగి -EINVAL;

        e = strchr(p, ']');
        ఒకవేళ (!e || e == p)
                తిరిగి -EINVAL;
        ఒకవేళ (e[1] != '/')
                తిరిగి -EINVAL;

        _cleanup_free_ అక్షరం *s = strndup(p, e - p);
        ఒకవేళ (!s)
                తిరిగి -ENOMEM;

        ఒకవేళ (!మార్గం_is_absolute(s) || !మార్గం_is_normalized(s))
                తిరిగి -EINVAL;

        స్థిరరాశి అక్షరం *resource = e + 2;
        ఒకవేళ (isempty(resource))
                తిరిగి -EINVAL;
        ఒకవేళ (మార్గం_is_absolute(resource) || !మార్గం_is_normalized(resource))
                తిరిగి -EINVAL;
        ఒకవేళ (string_has_cc(resource, /* ok= */ ఖాళీ) || strpbrk(resource, "?#"))
                తిరిగి -EINVAL;

        r = strdup_గమ్యం_నిండిన(ret_resource, resource);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret_socket)
                *ret_socket = TAKE_PTR(s);

        తిరిగి 0;
}

bool provider_url_is_valid(స్థిరరాశి అక్షరం *url) {
        తిరిగి provider_url_parse(url, /* ret_socket= */ ఖాళీ, /* ret_resource= */ ఖాళీ) >= 0;
}

bool documentation_url_is_valid(స్థిరరాశి అక్షరం *url) {
        స్థిరరాశి అక్షరం *p;

        ఒకవేళ (isempty(url))
                తిరిగి false;

        ఒకవేళ (http_url_is_valid(url) || file_url_is_valid(url))
                తిరిగి true;

        p = STARTSWITH_సమితి(url, "info:", "man:");
        ఒకవేళ (isempty(p))
                తిరిగి false;

        తిరిగి ascii_is_valid(p);
}

bool http_శీర్షిక_valid(స్థిరరాశి అక్షరం *శీర్షిక) {
        తిరిగి శీర్షిక &&
                ascii_is_valid(శీర్షిక) &&
                !string_has_cc(శీర్షిక, /* ok= */ ఖాళీ) &&
                strchr(శీర్షిక, ':');
}

bool http_etag_is_valid(స్థిరరాశి అక్షరం *etag) {
        ఒకవేళ (isempty(etag))
                తిరిగి false;

        ఒకవేళ (!endswith(etag, "\""))
                తిరిగి false;

        ఒకవేళ (!STARTSWITH_సమితి(etag, "\"", "W/\""))
                తిరిగి false;

        తిరిగి true;
}
