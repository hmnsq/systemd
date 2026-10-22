/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

bool http_url_is_valid(ধ্রুবক অক্ষর *url) _pure_;
bool file_url_is_valid(ধ্রুবক অক্ষর *url) _pure_;
bool documentation_url_is_valid(ধ্রুবক অক্ষর *url) _pure_;

/* Our own "provider:[/path/to/socket]/resource" URL scheme, for acquiring resources from local Varlink services */
পূর্ণসংখ্যা provider_url_parse(ধ্রুবক অক্ষর *url, অক্ষর **ret_socket, অক্ষর **ret_resource);
bool provider_url_is_valid(ধ্রুবক অক্ষর *url);
bool http_header_valid(ধ্রুবক অক্ষর *header) _pure_;
bool http_etag_is_valid(ধ্রুবক অক্ষর *etag) _pure_;
