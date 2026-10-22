/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

నిర్మాణం iovec_wrapper {
        నిర్మాణం iovec *iovec;
        size_t లెక్క;
};

శూన్య iovw_పూర్తి_free(నిర్మాణం iovec_wrapper *iovw);
శూన్య iovw_పూర్తి(నిర్మాణం iovec_wrapper *iovw);

నిర్మాణం iovec_wrapper* iovw_free(నిర్మాణం iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(నిర్మాణం iovec_wrapper*, iovw_free);

నిర్మాణం iovec_wrapper* iovw_free_free(నిర్మాణం iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(నిర్మాణం iovec_wrapper*, iovw_free_free);

పూర్ణాంకం iovw_compare(స్థిరరాశి నిర్మాణం iovec_wrapper *a, స్థిరరాశి నిర్మాణం iovec_wrapper *b) శుద్ధ;
స్థిర అంతఃవరుస bool iovw_equal(స్థిరరాశి నిర్మాణం iovec_wrapper *a, స్థిరరాశి నిర్మాణం iovec_wrapper *b) {
        తిరిగి iovw_compare(a, b) == 0;
}

పూర్ణాంకం iovw_put_నిండిన(నిర్మాణం iovec_wrapper *iovw, bool accept_zero, శూన్య *దత్తాంశం, size_t len);
స్థిర అంతఃవరుస పూర్ణాంకం iovw_put(నిర్మాణం iovec_wrapper *iovw, శూన్య *దత్తాంశం, size_t len) {
        తిరిగి iovw_put_నిండిన(iovw, false, దత్తాంశం, len);
}
పూర్ణాంకం iovw_put_iov_నిండిన(నిర్మాణం iovec_wrapper *iovw, bool accept_zero, స్థిరరాశి నిర్మాణం iovec *iov);
స్థిర అంతఃవరుస పూర్ణాంకం iovw_put_iov(నిర్మాణం iovec_wrapper *iovw, స్థిరరాశి నిర్మాణం iovec *iov) {
        తిరిగి iovw_put_iov_నిండిన(iovw, false, iov);
}
పూర్ణాంకం iovw_put_iovw_నిండిన(నిర్మాణం iovec_wrapper *iovw, bool accept_zero, స్థిరరాశి నిర్మాణం iovec_wrapper *ఆధారం);
స్థిర అంతఃవరుస పూర్ణాంకం iovw_put_iovw(నిర్మాణం iovec_wrapper *iovw, స్థిరరాశి నిర్మాణం iovec_wrapper *ఆధారం) {
        తిరిగి iovw_put_iovw_నిండిన(iovw, false, ఆధారం);
}
పూర్ణాంకం iovw_consume_నిండిన(నిర్మాణం iovec_wrapper *iovw, bool accept_zero, శూన్య *దత్తాంశం, size_t len);
స్థిర అంతఃవరుస పూర్ణాంకం iovw_consume(నిర్మాణం iovec_wrapper *iovw, శూన్య *దత్తాంశం, size_t len) {
        తిరిగి iovw_consume_నిండిన(iovw, false, దత్తాంశం, len);
}
పూర్ణాంకం iovw_consume_iov_నిండిన(నిర్మాణం iovec_wrapper *iovw, bool accept_zero, నిర్మాణం iovec *iov);
స్థిర అంతఃవరుస పూర్ణాంకం iovw_consume_iov(నిర్మాణం iovec_wrapper *iovw, నిర్మాణం iovec *iov) {
        తిరిగి iovw_consume_iov_నిండిన(iovw, false, iov);
}
పూర్ణాంకం iovw_extend_నిండిన(నిర్మాణం iovec_wrapper *iovw, bool accept_zero, స్థిరరాశి శూన్య *దత్తాంశం, size_t len);
స్థిర అంతఃవరుస పూర్ణాంకం iovw_extend(నిర్మాణం iovec_wrapper *iovw, స్థిరరాశి శూన్య *దత్తాంశం, size_t len) {
        తిరిగి iovw_extend_నిండిన(iovw, false, దత్తాంశం, len);
}
పూర్ణాంకం iovw_extend_iov_నిండిన(నిర్మాణం iovec_wrapper *iovw, bool accept_zero, స్థిరరాశి నిర్మాణం iovec *iov);
స్థిర అంతఃవరుస పూర్ణాంకం iovw_extend_iov(నిర్మాణం iovec_wrapper *iovw, స్థిరరాశి నిర్మాణం iovec *iov) {
        తిరిగి iovw_extend_iov_నిండిన(iovw, false, iov);
}
పూర్ణాంకం iovw_extend_iovw_నిండిన(నిర్మాణం iovec_wrapper *iovw, bool accept_zero, స్థిరరాశి నిర్మాణం iovec_wrapper *ఆధారం);
స్థిర అంతఃవరుస పూర్ణాంకం iovw_extend_iovw(నిర్మాణం iovec_wrapper *iovw, స్థిరరాశి నిర్మాణం iovec_wrapper *ఆధారం) {
        తిరిగి iovw_extend_iovw_నిండిన(iovw, false, ఆధారం);
}

స్థిర అంతఃవరుస bool iovw_isempty(స్థిరరాశి నిర్మాణం iovec_wrapper *iovw) {
        తిరిగి !iovw || iovw->లెక్క == 0;
}

పూర్ణాంకం iovw_put_string_క్షేత్రం_నిండిన(నిర్మాణం iovec_wrapper *iovw, bool replace, స్థిరరాశి అక్షరం *క్షేత్రం, స్థిరరాశి అక్షరం *విలువ);
స్థిర అంతఃవరుస పూర్ణాంకం iovw_put_string_క్షేత్రం(నిర్మాణం iovec_wrapper *iovw, స్థిరరాశి అక్షరం *క్షేత్రం, స్థిరరాశి అక్షరం *విలువ) {
        తిరిగి iovw_put_string_క్షేత్రం_నిండిన(iovw, false, క్షేత్రం, విలువ);
}
స్థిర అంతఃవరుస పూర్ణాంకం iovw_replace_string_క్షేత్రం(నిర్మాణం iovec_wrapper *iovw, స్థిరరాశి అక్షరం *క్షేత్రం, స్థిరరాశి అక్షరం *విలువ) {
        తిరిగి iovw_put_string_క్షేత్రం_నిండిన(iovw, true, క్షేత్రం, విలువ);
}
పూర్ణాంకం iovw_put_string_fieldf_నిండిన(నిర్మాణం iovec_wrapper *iovw, bool replace, స్థిరరాశి అక్షరం *క్షేత్రం, స్థిరరాశి అక్షరం *format, ...) _printf_(4, 5);
#నిర్వచించు iovw_put_string_fieldf(iovw, ...)     iovw_put_string_fieldf_నిండిన(iovw, false, __VA_ARGS__)
#నిర్వచించు iovw_replace_string_fieldf(iovw, ...) iovw_put_string_fieldf_నిండిన(iovw, true, __VA_ARGS__)
పూర్ణాంకం iovw_put_string_క్షేత్రం_free(నిర్మాణం iovec_wrapper *iovw, స్థిరరాశి అక్షరం *క్షేత్రం, అక్షరం *విలువ);
శూన్య iovw_rebase(నిర్మాణం iovec_wrapper *iovw, శూన్య *old, శూన్య *new);
size_t iovw_కొలత(స్థిరరాశి నిర్మాణం iovec_wrapper *iovw);
పూర్ణాంకం iovw_concat(స్థిరరాశి నిర్మాణం iovec_wrapper *iovw, నిర్మాణం iovec *ret);
అక్షరం* iovw_గమ్యం_cstring(స్థిరరాశి నిర్మాణం iovec_wrapper *iovw);

పూర్ణాంకం iovec_విభజించు(స్థిరరాశి నిర్మాణం iovec *iov, size_t నిడివి_కొలత, నిర్మాణం iovec_wrapper *ret);
పూర్ణాంకం iovw_విలీనం(స్థిరరాశి నిర్మాణం iovec_wrapper *iovw, size_t నిడివి_కొలత, నిర్మాణం iovec *ret);
