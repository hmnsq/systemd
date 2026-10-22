/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

bool http_url_is_valid(ثابت نویسه *url) _pure_;
bool file_url_is_valid(ثابت نویسه *url) _pure_;
bool documentation_url_is_valid(ثابت نویسه *url) _pure_;

/* Our own "provider:[/path/to/socket]/resource" URL scheme, for acquiring resources from local Varlink services */
صحیح provider_url_parse(ثابت نویسه *url, نویسه **ret_socket, نویسه **ret_resource);
bool provider_url_is_valid(ثابت نویسه *url);
bool http_header_valid(ثابت نویسه *header) _pure_;
bool http_etag_is_valid(ثابت نویسه *etag) _pure_;
