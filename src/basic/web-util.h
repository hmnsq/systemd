/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

bool http_url_is_valid(स्थिर अक्षर *url) _pure_;
bool file_url_is_valid(स्थिर अक्षर *url) _pure_;
bool documentation_url_is_valid(स्थिर अक्षर *url) _pure_;

/* Our own "provider:[/path/to/socket]/resource" URL scheme, for acquiring resources from local Varlink services */
पूर्णांक provider_url_parse(स्थिर अक्षर *url, अक्षर **ret_socket, अक्षर **ret_resource);
bool provider_url_is_valid(स्थिर अक्षर *url);
bool http_header_valid(स्थिर अक्षर *header) _pure_;
bool http_etag_is_valid(स्थिर अक्षर *etag) _pure_;
