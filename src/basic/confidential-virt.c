/* SPDX-License-Identifier: LGPL-2.1-or-later */

#اگر defined(__i386__) || defined(__x86_64__)
#شامل <cpuid.h>
#اگر_ختم
#شامل <fcntl.h>
#شامل <threads.h>
#شامل <unistd.h>

#شامل "confidential-virt.h"
#شامل "errno-util.h"                                 /* IWYU pragma: keep */
#شامل "fd-util.h"
#شامل "fileio.h"                                     /* IWYU pragma: keep */
#شامل "log.h"
#شامل "string-table.h"
#شامل "string-util.h"
#شامل "utf8.h"

#اگر defined(__x86_64__)

ساکن خالی cpuid(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
        أكد(eax);
        أكد(ebx);
        أكد(ecx);
        أكد(edx);

        log_debug("CPUID func %" PRIx32 " %" PRIx32, *eax, *ecx);
        __cpuid_count(*eax, *ecx, *eax, *ebx, *ecx, *edx);
        log_debug("CPUID result %" PRIx32 " %" PRIx32 " %" PRIx32 " %" PRIx32, *eax, *ebx, *ecx, *edx);
}

ساکن uint32_t cpuid_برگ(uint32_t eax, حرف ret_sig[ساکن 13], bool swapped) {
        /* zero-init as some queries explicitly require subleaf == 0 */
        uint32_t sig[3] = {};

        اگر (swapped)
                cpuid(&eax, &sig[0], &sig[2], &sig[1]);
        ورنہ
                cpuid(&eax, &sig[0], &sig[1], &sig[2]);
        حافظہ_نقل(ret_sig, sig, حجم(sig));
        ret_sig[12] = 0; /* \0-terminate the string to make string comparison possible */

        /* In some CI tests ret_sig doesn't contain valid UTF8 and prints garbage to the console */
        log_debug("CPUID sig '%s'", strna(utf8_is_valid(ret_sig)));

        واپس eax;
}

#تعریف_کرو MSR_DEVICE "/dev/cpu/0/msr"

ساکن صحیح msr(uint64_t index, uint64_t *ret) {
        _cleanup_close_ صحیح fd = -EBADF;
        uint64_t v;
        ssize_t n;

        أكد(ret);

        fd = open(MSR_DEVICE, O_RDONLY|O_CLOEXEC);
        اگر (fd < 0)
                واپس log_debug_errno(errno,
                                       "Cannot open MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);

        n = pread(fd, &v, حجم(v), index);
        اگر (n < 0)
                واپس log_debug_errno(errno,
                                       "Cannot read MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);
        اگر (n != حجم(v))
                واپس log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Short read %zd bytes from MSR device %s (index %" PRIu64 ")",
                                       n, MSR_DEVICE, index);

        log_debug("MSR %" PRIu64 " result %" PRIu64, index, v);
        *ret = v;
        واپس 0;
}

ساکن bool detect_hyperv_cvm(uint32_t isoltype) {
        uint32_t eax, ebx, ecx, edx, feat;
        حرف sig[13] = {};

        feat = cpuid_برگ(CPUID_HYPERV_VENDOR_AND_MAX_FUNCTIONS, sig, false);

        اگر (feat < CPUID_HYPERV_MIN || feat > CPUID_HYPERV_MAX)
                واپس false;

        اگر (memcmp(sig, CPUID_SIG_HYPERV, حجم(sig)) != 0)
                واپس false;

        log_debug("CPUID is on hyperv");
        eax = CPUID_HYPERV_FEATURES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        اگر (ebx & CPUID_HYPERV_ISOLATION && !(ebx & CPUID_HYPERV_CPU_MANAGEMENT)) {

                eax = CPUID_HYPERV_ISOLATION_CONFIG;
                ebx = ecx = edx = 0;
                cpuid(&eax, &ebx, &ecx, &edx);

                اگر ((ebx & CPUID_HYPERV_ISOLATION_TYPE_نقاب) == isoltype)
                        واپس true;
        }

        واپس false;
}

ساکن ConfidentialVirtualization detect_sev(خالی) {
        uint32_t eax, ebx, ecx, edx;
        uint64_t msrval;
        صحیح r;

        eax = CPUID_GET_HIGHEST_FUNCTION;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        اگر (eax < CPUID_AMD_GET_ENCRYPTED_حافظہ_CAPABILITIES)
                واپس CONFIDENTIAL_VIRTUALIZATION_NONE;

        eax = CPUID_AMD_GET_ENCRYPTED_حافظہ_CAPABILITIES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        /* bit 1 == CPU supports SEV feature
         *
         * Note, Azure blocks this CPUID leaf from its SEV-SNP
         * guests, so we must fallback to trying some HyperV
         * specific CPUID checks.
         */
        اگر (!(eax & EAX_SEV)) {
                log_debug("No sev in CPUID, trying hyperv CPUID");

                اگر (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_SNP))
                        واپس CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;

                log_debug("No hyperv CPUID");
                واپس CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        r = msr(MSR_AMD64_SEV, &msrval);
        اگر (r < 0) {
                /* The CPU advertises SEV support and we're running under a hypervisor, but we couldn't read
                 * the SEV MSR to determine the exact mode (e.g. /dev/cpu/0/msr is unavailable because the
                 * msr module isn't loaded). Assume plain SEV. Misreporting a genuine confidential guest as
                 * non-confidential would wrongly make us trust hypervisor-provided data such as firmware credentials. */
                log_debug_errno(r, "Failed to read SEV MSR, assuming SEV: %m");
                واپس CONFIDENTIAL_VIRTUALIZATION_SEV;
        }

        /* Test reverse order, since the SEV-SNP bit implies
         * the SEV-ES bit, which implies the SEV bit */
        اگر (msrval & MSR_SEV_SNP)
                واپس CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;
        اگر (msrval & MSR_SEV_ES)
                واپس CONFIDENTIAL_VIRTUALIZATION_SEV_ES;
        اگر (msrval & MSR_SEV)
                واپس CONFIDENTIAL_VIRTUALIZATION_SEV;

        واپس CONFIDENTIAL_VIRTUALIZATION_NONE;
}

ساکن ConfidentialVirtualization detect_tdx(خالی) {
        حرف sig[13] = {};

        /* Querying an unsupported CPUID leaf is harmless (it returns the highest basic leaf's data rather
         * than faulting), so reading this leaf and matching the IntelTDX signature is sufficient. */
        cpuid_برگ(CPUID_INTEL_TDX_ENUMERATION, sig, true);

        اگر (memcmp(sig, CPUID_SIG_INTEL_TDX, حجم(sig)) == 0)
                واپس CONFIDENTIAL_VIRTUALIZATION_TDX;

        log_debug("No tdx in CPUID, trying hyperv CPUID");

        اگر (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_TDX))
                واپس CONFIDENTIAL_VIRTUALIZATION_TDX;

        واپس CONFIDENTIAL_VIRTUALIZATION_NONE;
}

ساکن bool detect_hypervisor(خالی) {
        uint32_t eax, ebx, ecx, edx;
        bool is_hv;

        eax = CPUID_PROCESSOR_INFO_AND_FEATURE_بٹ;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        is_hv = ecx & CPUID_FEATURE_HYPERVISOR;

        log_debug("CPUID is hypervisor: %s", yes_no(is_hv));
        واپس is_hv;
}

ساکن ConfidentialVirtualization detect_confidential_virtualization_impl(خالی) {
        حرف sig[13] = {};

        /* Skip everything on bare metal */
        اگر (detect_hypervisor()) {
                cpuid_برگ(0, sig, true);

                اگر (memcmp(sig, CPUID_SIG_AMD, حجم(sig)) == 0)
                        واپس detect_sev();
                ورنہ اگر (memcmp(sig, CPUID_SIG_INTEL, حجم(sig)) == 0)
                        واپس detect_tdx();
        }

        واپس CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#وگرنہ_اگر defined(__s390x__)
ساکن ConfidentialVirtualization detect_confidential_virtualization_impl(خالی) {
        _cleanup_free_ حرف *s = تہی;
        size_t readsize;
        صحیح r;

        r = read_بھرا_virtual_file("/sys/firmware/uv/prot_virt_guest", &s, &readsize);
        اگر (r < 0) {
                log_debug_errno(r, "Unable to read /sys/firmware/uv/prot_virt_guest: %m");
                واپس CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        اگر (readsize >= 1 && s[0] == '1')
                واپس CONFIDENTIAL_VIRTUALIZATION_PROTVIRT;

        واپس CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#وگرنہ_اگر defined(__aarch64__)
ساکن ConfidentialVirtualization detect_confidential_virtualization_impl(خالی) {
        صحیح r;

        r = RET_NERRNO(access("/sys/devices/platform/arm-cca-dev", F_OK));
        اگر (r < 0) {
                log_debug_errno(r, "Unable to check /sys/devices/platform/arm-cca-dev: %m");
                واپس CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        واپس CONFIDENTIAL_VIRTUALIZATION_CCA;
}
#ورنہ /* ! x86_64 */
ساکن ConfidentialVirtualization detect_confidential_virtualization_impl(خالی) {
        log_debug("No confidential virtualization detection on this architecture");
        واپس CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#اگر_ختم /* ! x86_64 */

ConfidentialVirtualization detect_confidential_virtualization(خالی) {
        ساکن thread_local ConfidentialVirtualization cached_ملا = _CONFIDENTIAL_VIRTUALIZATION_INVALID;

        اگر (cached_ملا == _CONFIDENTIAL_VIRTUALIZATION_INVALID)
                cached_ملا = detect_confidential_virtualization_impl();

        واپس cached_ملا;
}

ساکن ثابت حرف *ثابت confidential_virtualization_جدول[_CONFIDENTIAL_VIRTUALIZATION_MAX] = {
        [CONFIDENTIAL_VIRTUALIZATION_NONE]     = "none",
        [CONFIDENTIAL_VIRTUALIZATION_SEV]      = "sev",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_ES]   = "sev-es",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_SNP]  = "sev-snp",
        [CONFIDENTIAL_VIRTUALIZATION_TDX]      = "tdx",
        [CONFIDENTIAL_VIRTUALIZATION_PROTVIRT] = "protvirt",
        [CONFIDENTIAL_VIRTUALIZATION_CCA]      = "cca",
};

DEFINE_STRING_جدول_LOOKUP(confidential_virtualization, ConfidentialVirtualization);
