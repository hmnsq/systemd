/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

bool http_url_is_valid(ধ্রুব বর্ণ *url) _pure_;
bool file_url_is_valid(ধ্রুব বর্ণ *url) _pure_;
bool documentation_url_is_valid(ধ্রুব বর্ণ *url) _pure_;

/* Our own "provider:[/path/to/socket]/resource" URL scheme, for acquiring resources from local Varlink services */
পূর্ণাঙ্ক provider_url_parse(ধ্রুব বর্ণ *url, বর্ণ **ret_socket, বর্ণ **ret_resource);
bool provider_url_is_valid(ধ্রুব বর্ণ *url);
bool http_header_valid(ধ্রুব বর্ণ *header) _pure_;
bool http_etag_is_valid(ধ্রুব বর্ণ *etag) _pure_;
