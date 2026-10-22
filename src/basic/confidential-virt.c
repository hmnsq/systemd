/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਜੇ defined(__i386__) || defined(__x86_64__)
#ਅਨ੍ਤਰ੍ਭਾਵ <cpuid.h>
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <threads.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "confidential-virt.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"                                 /* IWYU pragma: keep */
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"                                     /* IWYU pragma: keep */
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-table.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utf8.h"

#ਜੇ defined(__x86_64__)

ਸਥਿਰ ਖਾਲੀ cpuid(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
        ਪ੍ਰਤਿਜ੍ਞਾ(eax);
        ਪ੍ਰਤਿਜ੍ਞਾ(ebx);
        ਪ੍ਰਤਿਜ੍ਞਾ(ecx);
        ਪ੍ਰਤਿਜ੍ਞਾ(edx);

        log_debug("CPUID func %" PRIx32 " %" PRIx32, *eax, *ecx);
        __cpuid_count(*eax, *ecx, *eax, *ebx, *ecx, *edx);
        log_debug("CPUID result %" PRIx32 " %" PRIx32 " %" PRIx32 " %" PRIx32, *eax, *ebx, *ecx, *edx);
}

ਸਥਿਰ uint32_t cpuid_ਪੱਤੀ(uint32_t eax, ਅੱਖਰ ret_sig[ਸਥਿਰ 13], bool swapped) {
        /* zero-init as some queries explicitly require subleaf == 0 */
        uint32_t sig[3] = {};

        ਜੇ (swapped)
                cpuid(&eax, &sig[0], &sig[2], &sig[1]);
        ਨਹੀਂਤਾਂ
                cpuid(&eax, &sig[0], &sig[1], &sig[2]);
        ਸਮ੍ਰਿਤੀ_ਨਕਲ(ret_sig, sig, ਆਕਾਰ(sig));
        ret_sig[12] = 0; /* \0-terminate the string to make string comparison possible */

        /* In some CI tests ret_sig doesn't contain valid UTF8 and prints garbage to the console */
        log_debug("CPUID sig '%s'", strna(utf8_is_valid(ret_sig)));

        ਵਾਪਸ eax;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MSR_DEVICE "/dev/cpu/0/msr"

ਸਥਿਰ ਪੂਰਨ msr(uint64_t index, uint64_t *ret) {
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        uint64_t v;
        ssize_t n;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        fd = open(MSR_DEVICE, O_RDONLY|O_CLOEXEC);
        ਜੇ (fd < 0)
                ਵਾਪਸ log_debug_errno(errno,
                                       "Cannot open MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);

        n = pread(fd, &v, ਆਕਾਰ(v), index);
        ਜੇ (n < 0)
                ਵਾਪਸ log_debug_errno(errno,
                                       "Cannot read MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);
        ਜੇ (n != ਆਕਾਰ(v))
                ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Short read %zd bytes from MSR device %s (index %" PRIu64 ")",
                                       n, MSR_DEVICE, index);

        log_debug("MSR %" PRIu64 " result %" PRIu64, index, v);
        *ret = v;
        ਵਾਪਸ 0;
}

ਸਥਿਰ bool detect_hyperv_cvm(uint32_t isoltype) {
        uint32_t eax, ebx, ecx, edx, feat;
        ਅੱਖਰ sig[13] = {};

        feat = cpuid_ਪੱਤੀ(CPUID_HYPERV_VENDOR_AND_MAX_FUNCTIONS, sig, false);

        ਜੇ (feat < CPUID_HYPERV_MIN || feat > CPUID_HYPERV_MAX)
                ਵਾਪਸ false;

        ਜੇ (memcmp(sig, CPUID_SIG_HYPERV, ਆਕਾਰ(sig)) != 0)
                ਵਾਪਸ false;

        log_debug("CPUID is on hyperv");
        eax = CPUID_HYPERV_FEATURES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        ਜੇ (ebx & CPUID_HYPERV_ISOLATION && !(ebx & CPUID_HYPERV_CPU_MANAGEMENT)) {

                eax = CPUID_HYPERV_ISOLATION_CONFIG;
                ebx = ecx = edx = 0;
                cpuid(&eax, &ebx, &ecx, &edx);

                ਜੇ ((ebx & CPUID_HYPERV_ISOLATION_TYPE_ਨਕਾਬ) == isoltype)
                        ਵਾਪਸ true;
        }

        ਵਾਪਸ false;
}

ਸਥਿਰ ConfidentialVirtualization detect_sev(ਖਾਲੀ) {
        uint32_t eax, ebx, ecx, edx;
        uint64_t msrval;
        ਪੂਰਨ r;

        eax = CPUID_GET_HIGHEST_FUNCTION;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        ਜੇ (eax < CPUID_AMD_GET_ENCRYPTED_ਸਮ੍ਰਿਤੀ_CAPABILITIES)
                ਵਾਪਸ CONFIDENTIAL_VIRTUALIZATION_NONE;

        eax = CPUID_AMD_GET_ENCRYPTED_ਸਮ੍ਰਿਤੀ_CAPABILITIES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        /* bit 1 == CPU supports SEV feature
         *
         * Note, Azure blocks this CPUID leaf from its SEV-SNP
         * guests, so we must fallback to trying some HyperV
         * specific CPUID checks.
         */
        ਜੇ (!(eax & EAX_SEV)) {
                log_debug("No sev in CPUID, trying hyperv CPUID");

                ਜੇ (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_SNP))
                        ਵਾਪਸ CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;

                log_debug("No hyperv CPUID");
                ਵਾਪਸ CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        r = msr(MSR_AMD64_SEV, &msrval);
        ਜੇ (r < 0) {
                /* The CPU advertises SEV support and we're running under a hypervisor, but we couldn't read
                 * the SEV MSR to determine the exact mode (e.g. /dev/cpu/0/msr is unavailable because the
                 * msr module isn't loaded). Assume plain SEV. Misreporting a genuine confidential guest as
                 * non-confidential would wrongly make us trust hypervisor-provided data such as firmware credentials. */
                log_debug_errno(r, "Failed to read SEV MSR, assuming SEV: %m");
                ਵਾਪਸ CONFIDENTIAL_VIRTUALIZATION_SEV;
        }

        /* Test reverse order, since the SEV-SNP bit implies
         * the SEV-ES bit, which implies the SEV bit */
        ਜੇ (msrval & MSR_SEV_SNP)
                ਵਾਪਸ CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;
        ਜੇ (msrval & MSR_SEV_ES)
                ਵਾਪਸ CONFIDENTIAL_VIRTUALIZATION_SEV_ES;
        ਜੇ (msrval & MSR_SEV)
                ਵਾਪਸ CONFIDENTIAL_VIRTUALIZATION_SEV;

        ਵਾਪਸ CONFIDENTIAL_VIRTUALIZATION_NONE;
}

ਸਥਿਰ ConfidentialVirtualization detect_tdx(ਖਾਲੀ) {
        ਅੱਖਰ sig[13] = {};

        /* Querying an unsupported CPUID leaf is harmless (it returns the highest basic leaf's data rather
         * than faulting), so reading this leaf and matching the IntelTDX signature is sufficient. */
        cpuid_ਪੱਤੀ(CPUID_INTEL_TDX_ENUMERATION, sig, true);

        ਜੇ (memcmp(sig, CPUID_SIG_INTEL_TDX, ਆਕਾਰ(sig)) == 0)
                ਵਾਪਸ CONFIDENTIAL_VIRTUALIZATION_TDX;

        log_debug("No tdx in CPUID, trying hyperv CPUID");

        ਜੇ (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_TDX))
                ਵਾਪਸ CONFIDENTIAL_VIRTUALIZATION_TDX;

        ਵਾਪਸ CONFIDENTIAL_VIRTUALIZATION_NONE;
}

ਸਥਿਰ bool detect_hypervisor(ਖਾਲੀ) {
        uint32_t eax, ebx, ecx, edx;
        bool is_hv;

        eax = CPUID_PROCESSOR_INFO_AND_FEATURE_ਬਿੱਟ;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        is_hv = ecx & CPUID_FEATURE_HYPERVISOR;

        log_debug("CPUID is hypervisor: %s", yes_no(is_hv));
        ਵਾਪਸ is_hv;
}

ਸਥਿਰ ConfidentialVirtualization detect_confidential_virtualization_impl(ਖਾਲੀ) {
        ਅੱਖਰ sig[13] = {};

        /* Skip everything on bare metal */
        ਜੇ (detect_hypervisor()) {
                cpuid_ਪੱਤੀ(0, sig, true);

                ਜੇ (memcmp(sig, CPUID_SIG_AMD, ਆਕਾਰ(sig)) == 0)
                        ਵਾਪਸ detect_sev();
                ਨਹੀਂਤਾਂ ਜੇ (memcmp(sig, CPUID_SIG_INTEL, ਆਕਾਰ(sig)) == 0)
                        ਵਾਪਸ detect_tdx();
        }

        ਵਾਪਸ CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#ਜਾਂਜੇ defined(__s390x__)
ਸਥਿਰ ConfidentialVirtualization detect_confidential_virtualization_impl(ਖਾਲੀ) {
        _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;
        size_t readsize;
        ਪੂਰਨ r;

        r = read_ਭਰਿਆ_virtual_file("/sys/firmware/uv/prot_virt_guest", &s, &readsize);
        ਜੇ (r < 0) {
                log_debug_errno(r, "Unable to read /sys/firmware/uv/prot_virt_guest: %m");
                ਵਾਪਸ CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        ਜੇ (readsize >= 1 && s[0] == '1')
                ਵਾਪਸ CONFIDENTIAL_VIRTUALIZATION_PROTVIRT;

        ਵਾਪਸ CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#ਜਾਂਜੇ defined(__aarch64__)
ਸਥਿਰ ConfidentialVirtualization detect_confidential_virtualization_impl(ਖਾਲੀ) {
        ਪੂਰਨ r;

        r = RET_NERRNO(access("/sys/devices/platform/arm-cca-dev", F_OK));
        ਜੇ (r < 0) {
                log_debug_errno(r, "Unable to check /sys/devices/platform/arm-cca-dev: %m");
                ਵਾਪਸ CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        ਵਾਪਸ CONFIDENTIAL_VIRTUALIZATION_CCA;
}
#ਨਹੀਂਤਾਂ /* ! x86_64 */
ਸਥਿਰ ConfidentialVirtualization detect_confidential_virtualization_impl(ਖਾਲੀ) {
        log_debug("No confidential virtualization detection on this architecture");
        ਵਾਪਸ CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#ਜੇ_ਅੰਤ /* ! x86_64 */

ConfidentialVirtualization detect_confidential_virtualization(ਖਾਲੀ) {
        ਸਥਿਰ thread_local ConfidentialVirtualization cached_ਮਿਲਿਆ = _CONFIDENTIAL_VIRTUALIZATION_INVALID;

        ਜੇ (cached_ਮਿਲਿਆ == _CONFIDENTIAL_VIRTUALIZATION_INVALID)
                cached_ਮਿਲਿਆ = detect_confidential_virtualization_impl();

        ਵਾਪਸ cached_ਮਿਲਿਆ;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ confidential_virtualization_ਸਾਰਣੀ[_CONFIDENTIAL_VIRTUALIZATION_MAX] = {
        [CONFIDENTIAL_VIRTUALIZATION_NONE]     = "none",
        [CONFIDENTIAL_VIRTUALIZATION_SEV]      = "sev",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_ES]   = "sev-es",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_SNP]  = "sev-snp",
        [CONFIDENTIAL_VIRTUALIZATION_TDX]      = "tdx",
        [CONFIDENTIAL_VIRTUALIZATION_PROTVIRT] = "protvirt",
        [CONFIDENTIAL_VIRTUALIZATION_CCA]      = "cca",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(confidential_virtualization, ConfidentialVirtualization);
