/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

bool http_url_is_valid(ध्रुव वर्ण *url) _pure_;
bool file_url_is_valid(ध्रुव वर्ण *url) _pure_;
bool documentation_url_is_valid(ध्रुव वर्ण *url) _pure_;

/* Our own "provider:[/path/to/socket]/resource" URL scheme, for acquiring resources from local Varlink services */
पूर्णाङ्क provider_url_parse(ध्रुव वर्ण *url, वर्ण **ret_socket, वर्ण **ret_resource);
bool provider_url_is_valid(ध्रुव वर्ण *url);
bool http_header_valid(ध्रुव वर्ण *header) _pure_;
bool http_etag_is_valid(ध्रुव वर्ण *etag) _pure_;
