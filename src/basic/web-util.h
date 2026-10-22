/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

bool http_url_is_valid(స్థిరరాశి అక్షరం *url) శుద్ధ;
bool file_url_is_valid(స్థిరరాశి అక్షరం *url) శుద్ధ;
bool documentation_url_is_valid(స్థిరరాశి అక్షరం *url) శుద్ధ;

/* Our own "provider:[/path/to/socket]/resource" URL scheme, for acquiring resources from local Varlink services */
పూర్ణాంకం provider_url_parse(స్థిరరాశి అక్షరం *url, అక్షరం **ret_socket, అక్షరం **ret_resource);
bool provider_url_is_valid(స్థిరరాశి అక్షరం *url);
bool http_శీర్షిక_valid(స్థిరరాశి అక్షరం *శీర్షిక) శుద్ధ;
bool http_etag_is_valid(స్థిరరాశి అక్షరం *etag) శుద్ధ;
