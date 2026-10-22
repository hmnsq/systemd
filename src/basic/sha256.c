/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <unistd.h>

#समावेश "alloc-util.h"
#समावेश "hexdecoct.h"
#समावेश "sha256.h"
#समावेश "string-util.h"

पूर्णांक sha256_fd(पूर्णांक fd, uint64_t max_आकार, uint8_t ret[अटल SHA256_DIGEST_आकार]) {
        काष्ठा sha256_ctx ctx;
        uint64_t योग_आकार = 0;

        निश्चित(fd >= 0);

        sha256_init_ctx(&ctx);

        क्रम (;;) {
                uint8_t बफर[64 * 1024];
                ssize_t n;

                n = पढ़ो(fd, बफर, माप(बफर));
                अगर (n < 0)
                        वापस -त्रुटि_सं;
                अगर (n == 0)
                        अवरोध;

                अगर (!INC_SAFE(&योग_आकार, n) || योग_आकार > max_आकार)
                        वापस -EFBIG;

                sha256_process_bytes(बफर, n, &ctx);
        }

        sha256_finish_ctx(&ctx, ret);
        वापस 0;
}

पूर्णांक parse_sha256(स्थिर अक्षर *s, uint8_t ret[अटल SHA256_DIGEST_आकार]) {
        _cleanup_free_ uint8_t *तथ्य = शून्य;
        माप_प्रकार आकार = 0;
        पूर्णांक r;

        अगर (!sha256_is_valid(s))
                वापस -EINVAL;

        r = unhexmem_full(s, SHA256_DIGEST_आकार * 2, false, (व्योम**) &तथ्य, &आकार);
        अगर (r < 0)
                वापस r;
        निश्चित(आकार == SHA256_DIGEST_आकार);

        स_नकल(ret, तथ्य, आकार);
        वापस 0;
}

bool sha256_is_valid(स्थिर अक्षर *s) {
        वापस s && in_charset(s, HEXDIGITS) && (म_माप(s) == SHA256_DIGEST_आकार * 2);
}

अक्षर* sha256_direct_hex(स्थिर व्योम *बफर, माप_प्रकार sz) {
        निश्चित(बफर || sz == 0);

        अगर (sz == SIZE_MAX)
                sz = म_माप(बफर);

        वापस hexmem(SHA256_DIRECT(बफर, sz), SHA256_DIGEST_आकार);
}
