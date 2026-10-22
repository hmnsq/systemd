/* SPDX-License-Identifier: CC0-1.0 */

#సూచన once

#అన్తర్భావ "forward.h"

నిర్మాణం siphash {
        uint64_t v0;
        uint64_t v1;
        uint64_t v2;
        uint64_t v3;
        uint64_t padding;
        size_t inlen;
};

శూన్య siphash24_init(నిర్మాణం siphash *state, స్థిరరాశి uint8_t k[స్థిర 16]);
శూన్య siphash24_compress(స్థిరరాశి శూన్య *లోపల, size_t inlen, నిర్మాణం siphash *state);
#నిర్వచించు siphash24_compress_byte(byte, state) siphash24_compress((స్థిరరాశి uint8_t[]) { (byte) }, 1, (state))
#నిర్వచించు siphash24_compress_typesafe(లోపల, state)                  \
        siphash24_compress(&(లోపల), పరిమాణం(typeof(లోపల)), (state))

స్థిర అంతఃవరుస శూన్య siphash24_compress_boolean(bool లోపల, నిర్మాణం siphash *state) {
        siphash24_compress_byte(లోపల, state);
}

స్థిర అంతఃవరుస శూన్య siphash24_compress_usec_t(usec_t లోపల, నిర్మాణం siphash *state) {
        uint64_t u = htole64(లోపల);
        siphash24_compress_typesafe(u, state);
}

స్థిర అంతఃవరుస శూన్య siphash24_compress_సురక్షిత(స్థిరరాశి శూన్య *లోపల, size_t inlen, నిర్మాణం siphash *state) {
        ఒకవేళ (inlen == 0)
                తిరిగి;

        siphash24_compress(లోపల, inlen, state);
}

శూన్య siphash24_compress_string(స్థిరరాశి అక్షరం *లోపల, నిర్మాణం siphash *state);
శూన్య siphash24_compress_iovec(స్థిరరాశి నిర్మాణం iovec *iov, నిర్మాణం siphash *state);

uint64_t siphash24_finalize(నిర్మాణం siphash *state);

uint64_t siphash24(స్థిరరాశి శూన్య *లోపల, size_t inlen, స్థిరరాశి uint8_t k[స్థిర 16]);

uint64_t siphash24_uint64(uint64_t లోపల, స్థిరరాశి uint8_t k[స్థిర 16]);

uint64_t siphash24_string(స్థిరరాశి అక్షరం *s, స్థిరరాశి uint8_t k[స్థిర 16]);
