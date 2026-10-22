/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

bool http_url_is_valid(ಧ್ರುವ ವರ್ಣ *url) _pure_;
bool file_url_is_valid(ಧ್ರುವ ವರ್ಣ *url) _pure_;
bool documentation_url_is_valid(ಧ್ರುವ ವರ್ಣ *url) _pure_;

/* Our own "provider:[/path/to/socket]/resource" URL scheme, for acquiring resources from local Varlink services */
ಪೂರ್ಣಾಙ್ಕ provider_url_parse(ಧ್ರುವ ವರ್ಣ *url, ವರ್ಣ **ret_socket, ವರ್ಣ **ret_resource);
bool provider_url_is_valid(ಧ್ರುವ ವರ್ಣ *url);
bool http_header_valid(ಧ್ರುವ ವರ್ಣ *header) _pure_;
bool http_etag_is_valid(ಧ್ರುವ ವರ್ಣ *etag) _pure_;
