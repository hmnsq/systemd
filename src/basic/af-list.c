/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <string.h>
#అన్తర్భావ <sys/socket.h>

#అన్తర్భావ "af-list.h"
#అన్తర్భావ "string-util.h"

స్థిర స్థిరరాశి నిర్మాణం af_name* lookup_af(నమోదు స్థిరరాశి అక్షరం *str, నమోదు GPERF_LEN_TYPE len);

#అన్తర్భావ "af-from-name.inc"
#అన్తర్భావ "af-to-name.inc"

స్థిరరాశి అక్షరం* af_గమ్యం_name(పూర్ణాంకం id) {

        ఒకవేళ (id <= 0)
                తిరిగి ఖాళీ;

        ఒకవేళ ((size_t) id >= ELEMENTSOF(af_పేర్లు))
                తిరిగి ఖాళీ;

        తిరిగి af_పేర్లు[id];
}

స్థిరరాశి అక్షరం* af_గమ్యం_name_short(పూర్ణాంకం id) {
        స్థిరరాశి అక్షరం *f;

        ఒకవేళ (id == AF_UNSPEC)
                తిరిగి "*";

        f = af_గమ్యం_name(id);
        ఒకవేళ (!f)
                తిరిగి "unknown";

        ప్రతిజ్ఞా(startswith(f, "AF_"));
        తిరిగి f + 3;
}

పూర్ణాంకం af_from_name(స్థిరరాశి అక్షరం *name) {
        size_t len = అక్షరశ్రేణి_పొడవు(ASSERT_PTR(name));

        స్థిరరాశి నిర్మాణం af_name *sc = lookup_af(name, len);
        ఒకవేళ (!sc && len + అక్షరశ్రేణి_పొడవు("AF_") <= MAX_మాట_నిడివి) {
                స్థిరరాశి అక్షరం *t = strjoina("AF_", name);
                sc = lookup_af(t, అక్షరశ్రేణి_పొడవు("AF_") + len);
        }
        ఒకవేళ (!sc)
                తిరిగి -EINVAL;

        తిరిగి sc->id;
}

పూర్ణాంకం af_max(శూన్య) {
        తిరిగి ELEMENTSOF(af_పేర్లు);
}

స్థిరరాశి అక్షరం* af_గమ్యం_ipv4_ipv6(పూర్ణాంకం id) {
        /* Pretty often we want to map the address family to the typically used protocol name for IPv4 +
         * IPv6. Let's add special helpers for that. */
        తిరిగి id == AF_INET ? "ipv4" :
                id == AF_INET6 ? "ipv6" : ఖాళీ;
}

పూర్ణాంకం af_from_ipv4_ipv6(స్థిరరాశి అక్షరం *af) {
        తిరిగి streq_ptr(af, "ipv4") ? AF_INET :
                streq_ptr(af, "ipv6") ? AF_INET6 : AF_UNSPEC;
}
