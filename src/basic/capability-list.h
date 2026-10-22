/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

/* Space for capability_to_string() in case we write out a numeric capability because we don't know the name
 * for it. "0x3e" is the largest string we might output, in both sensese of the word "largest": two chars for
 * "0x", two bytes for the hex value, and one trailing NUL byte. */
#నిర్వచించు CAPABILITY_గమ్యం_STRING_MAX (2 + 2 + 1)

స్థిరరాశి అక్షరం* capability_గమ్యం_name(పూర్ణాంకం id);
స్థిరరాశి అక్షరం* capability_గమ్యం_string(పూర్ణాంకం id, అక్షరం buf[స్థిర CAPABILITY_గమ్యం_STRING_MAX]);
#నిర్వచించు CAPABILITY_గమ్యం_STRING(id) capability_గమ్యం_string(id, (అక్షరం[CAPABILITY_గమ్యం_STRING_MAX]) {})

పూర్ణాంకం capability_from_name(స్థిరరాశి అక్షరం *name);
గుర్తులేని capability_list_నిడివి(శూన్య);

పూర్ణాంకం capability_సమితి_గమ్యం_string(uint64_t సమితి, అక్షరం **ret);
పూర్ణాంకం capability_సమితి_గమ్యం_string_negative(uint64_t సమితి, అక్షరం **ret);
పూర్ణాంకం capability_సమితి_గమ్యం_strv(uint64_t సమితి, అక్షరం ***ret);
పూర్ణాంకం capability_సమితి_from_string(స్థిరరాశి అక్షరం *s, uint64_t *ret);
