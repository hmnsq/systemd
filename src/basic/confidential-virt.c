/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಆದರೆ defined(__i386__) || defined(__x86_64__)
#ಅನ್ತರ್ಭಾವ <cpuid.h>
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <threads.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "confidential-virt.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"                                 /* IWYU pragma: keep */
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"                                     /* IWYU pragma: keep */
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "string-table.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "utf8.h"

#ಆದರೆ defined(__x86_64__)

ಸ್ಥಿರ ಶೂನ್ಯ cpuid(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
        ಪ್ರತಿಜ್ಞಾ(eax);
        ಪ್ರತಿಜ್ಞಾ(ebx);
        ಪ್ರತಿಜ್ಞಾ(ecx);
        ಪ್ರತಿಜ್ಞಾ(edx);

        log_debug("CPUID func %" PRIx32 " %" PRIx32, *eax, *ecx);
        __cpuid_count(*eax, *ecx, *eax, *ebx, *ecx, *edx);
        log_debug("CPUID result %" PRIx32 " %" PRIx32 " %" PRIx32 " %" PRIx32, *eax, *ebx, *ecx, *edx);
}

ಸ್ಥಿರ uint32_t cpuid_ಎಲೆ(uint32_t eax, ಅಕ್ಷರ ret_sig[ಸ್ಥಿರ 13], bool swapped) {
        /* zero-init as some queries explicitly require subleaf == 0 */
        uint32_t sig[3] = {};

        ಆದರೆ (swapped)
                cpuid(&eax, &sig[0], &sig[2], &sig[1]);
        ಇಲ್ಲದಿದ್ದರೆ
                cpuid(&eax, &sig[0], &sig[1], &sig[2]);
        ಸ್ಮರಣೆ_ನಕಲು(ret_sig, sig, ಗಾತ್ರ(sig));
        ret_sig[12] = 0; /* \0-terminate the string to make string comparison possible */

        /* In some CI tests ret_sig doesn't contain valid UTF8 and prints garbage to the console */
        log_debug("CPUID sig '%s'", strna(utf8_is_valid(ret_sig)));

        ಹಿಂತಿರುಗಿ eax;
}

#ವ್ಯಾಖ್ಯಾನಿಸು MSR_DEVICE "/dev/cpu/0/msr"

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ msr(uint64_t index, uint64_t *ret) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        uint64_t v;
        ssize_t n;

        ಪ್ರತಿಜ್ಞಾ(ret);

        fd = open(MSR_DEVICE, O_RDONLY|O_CLOEXEC);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(errno,
                                       "Cannot open MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);

        n = pread(fd, &v, ಗಾತ್ರ(v), index);
        ಆದರೆ (n < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(errno,
                                       "Cannot read MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);
        ಆದರೆ (n != ಗಾತ್ರ(v))
                ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Short read %zd bytes from MSR device %s (index %" PRIu64 ")",
                                       n, MSR_DEVICE, index);

        log_debug("MSR %" PRIu64 " result %" PRIu64, index, v);
        *ret = v;
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ bool detect_hyperv_cvm(uint32_t isoltype) {
        uint32_t eax, ebx, ecx, edx, feat;
        ಅಕ್ಷರ sig[13] = {};

        feat = cpuid_ಎಲೆ(CPUID_HYPERV_VENDOR_AND_MAX_FUNCTIONS, sig, false);

        ಆದರೆ (feat < CPUID_HYPERV_MIN || feat > CPUID_HYPERV_MAX)
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (memcmp(sig, CPUID_SIG_HYPERV, ಗಾತ್ರ(sig)) != 0)
                ಹಿಂತಿರುಗಿ false;

        log_debug("CPUID is on hyperv");
        eax = CPUID_HYPERV_FEATURES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        ಆದರೆ (ebx & CPUID_HYPERV_ISOLATION && !(ebx & CPUID_HYPERV_CPU_MANAGEMENT)) {

                eax = CPUID_HYPERV_ISOLATION_CONFIG;
                ebx = ecx = edx = 0;
                cpuid(&eax, &ebx, &ecx, &edx);

                ಆದರೆ ((ebx & CPUID_HYPERV_ISOLATION_TYPE_ಮುಖವಾಡ) == isoltype)
                        ಹಿಂತಿರುಗಿ true;
        }

        ಹಿಂತಿರುಗಿ false;
}

ಸ್ಥಿರ ConfidentialVirtualization detect_sev(ಶೂನ್ಯ) {
        uint32_t eax, ebx, ecx, edx;
        uint64_t msrval;
        ಪೂರ್ಣಾಂಕ r;

        eax = CPUID_GET_HIGHEST_FUNCTION;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        ಆದರೆ (eax < CPUID_AMD_GET_ENCRYPTED_ಸ್ಮರಣೆ_CAPABILITIES)
                ಹಿಂತಿರುಗಿ CONFIDENTIAL_VIRTUALIZATION_NONE;

        eax = CPUID_AMD_GET_ENCRYPTED_ಸ್ಮರಣೆ_CAPABILITIES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        /* bit 1 == CPU supports SEV feature
         *
         * Note, Azure blocks this CPUID leaf from its SEV-SNP
         * guests, so we must fallback to trying some HyperV
         * specific CPUID checks.
         */
        ಆದರೆ (!(eax & EAX_SEV)) {
                log_debug("No sev in CPUID, trying hyperv CPUID");

                ಆದರೆ (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_SNP))
                        ಹಿಂತಿರುಗಿ CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;

                log_debug("No hyperv CPUID");
                ಹಿಂತಿರುಗಿ CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        r = msr(MSR_AMD64_SEV, &msrval);
        ಆದರೆ (r < 0) {
                /* The CPU advertises SEV support and we're running under a hypervisor, but we couldn't read
                 * the SEV MSR to determine the exact mode (e.g. /dev/cpu/0/msr is unavailable because the
                 * msr module isn't loaded). Assume plain SEV. Misreporting a genuine confidential guest as
                 * non-confidential would wrongly make us trust hypervisor-provided data such as firmware credentials. */
                log_debug_errno(r, "Failed to read SEV MSR, assuming SEV: %m");
                ಹಿಂತಿರುಗಿ CONFIDENTIAL_VIRTUALIZATION_SEV;
        }

        /* Test reverse order, since the SEV-SNP bit implies
         * the SEV-ES bit, which implies the SEV bit */
        ಆದರೆ (msrval & MSR_SEV_SNP)
                ಹಿಂತಿರುಗಿ CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;
        ಆದರೆ (msrval & MSR_SEV_ES)
                ಹಿಂತಿರುಗಿ CONFIDENTIAL_VIRTUALIZATION_SEV_ES;
        ಆದರೆ (msrval & MSR_SEV)
                ಹಿಂತಿರುಗಿ CONFIDENTIAL_VIRTUALIZATION_SEV;

        ಹಿಂತಿರುಗಿ CONFIDENTIAL_VIRTUALIZATION_NONE;
}

ಸ್ಥಿರ ConfidentialVirtualization detect_tdx(ಶೂನ್ಯ) {
        ಅಕ್ಷರ sig[13] = {};

        /* Querying an unsupported CPUID leaf is harmless (it returns the highest basic leaf's data rather
         * than faulting), so reading this leaf and matching the IntelTDX signature is sufficient. */
        cpuid_ಎಲೆ(CPUID_INTEL_TDX_ENUMERATION, sig, true);

        ಆದರೆ (memcmp(sig, CPUID_SIG_INTEL_TDX, ಗಾತ್ರ(sig)) == 0)
                ಹಿಂತಿರುಗಿ CONFIDENTIAL_VIRTUALIZATION_TDX;

        log_debug("No tdx in CPUID, trying hyperv CPUID");

        ಆದರೆ (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_TDX))
                ಹಿಂತಿರುಗಿ CONFIDENTIAL_VIRTUALIZATION_TDX;

        ಹಿಂತಿರುಗಿ CONFIDENTIAL_VIRTUALIZATION_NONE;
}

ಸ್ಥಿರ bool detect_hypervisor(ಶೂನ್ಯ) {
        uint32_t eax, ebx, ecx, edx;
        bool is_hv;

        eax = CPUID_PROCESSOR_INFO_AND_FEATURE_ಬಿಟ್ಟುಗಳು;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        is_hv = ecx & CPUID_FEATURE_HYPERVISOR;

        log_debug("CPUID is hypervisor: %s", yes_no(is_hv));
        ಹಿಂತಿರುಗಿ is_hv;
}

ಸ್ಥಿರ ConfidentialVirtualization detect_confidential_virtualization_impl(ಶೂನ್ಯ) {
        ಅಕ್ಷರ sig[13] = {};

        /* Skip everything on bare metal */
        ಆದರೆ (detect_hypervisor()) {
                cpuid_ಎಲೆ(0, sig, true);

                ಆದರೆ (memcmp(sig, CPUID_SIG_AMD, ಗಾತ್ರ(sig)) == 0)
                        ಹಿಂತಿರುಗಿ detect_sev();
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (memcmp(sig, CPUID_SIG_INTEL, ಗಾತ್ರ(sig)) == 0)
                        ಹಿಂತಿರುಗಿ detect_tdx();
        }

        ಹಿಂತಿರುಗಿ CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#ಅಥವಾಆದರೆ defined(__s390x__)
ಸ್ಥಿರ ConfidentialVirtualization detect_confidential_virtualization_impl(ಶೂನ್ಯ) {
        _cleanup_free_ ಅಕ್ಷರ *s = ಖಾಲಿ;
        size_t readsize;
        ಪೂರ್ಣಾಂಕ r;

        r = read_ತುಂಬಿದ_virtual_file("/sys/firmware/uv/prot_virt_guest", &s, &readsize);
        ಆದರೆ (r < 0) {
                log_debug_errno(r, "Unable to read /sys/firmware/uv/prot_virt_guest: %m");
                ಹಿಂತಿರುಗಿ CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        ಆದರೆ (readsize >= 1 && s[0] == '1')
                ಹಿಂತಿರುಗಿ CONFIDENTIAL_VIRTUALIZATION_PROTVIRT;

        ಹಿಂತಿರುಗಿ CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#ಅಥವಾಆದರೆ defined(__aarch64__)
ಸ್ಥಿರ ConfidentialVirtualization detect_confidential_virtualization_impl(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಂಕ r;

        r = RET_NERRNO(access("/sys/devices/platform/arm-cca-dev", F_OK));
        ಆದರೆ (r < 0) {
                log_debug_errno(r, "Unable to check /sys/devices/platform/arm-cca-dev: %m");
                ಹಿಂತಿರುಗಿ CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        ಹಿಂತಿರುಗಿ CONFIDENTIAL_VIRTUALIZATION_CCA;
}
#ಇಲ್ಲದಿದ್ದರೆ /* ! x86_64 */
ಸ್ಥಿರ ConfidentialVirtualization detect_confidential_virtualization_impl(ಶೂನ್ಯ) {
        log_debug("No confidential virtualization detection on this architecture");
        ಹಿಂತಿರುಗಿ CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#ಆದರೆ_ಅಂತ್ಯ /* ! x86_64 */

ConfidentialVirtualization detect_confidential_virtualization(ಶೂನ್ಯ) {
        ಸ್ಥಿರ thread_local ConfidentialVirtualization cached_ಸಿಕ್ಕಿತು = _CONFIDENTIAL_VIRTUALIZATION_INVALID;

        ಆದರೆ (cached_ಸಿಕ್ಕಿತು == _CONFIDENTIAL_VIRTUALIZATION_INVALID)
                cached_ಸಿಕ್ಕಿತು = detect_confidential_virtualization_impl();

        ಹಿಂತಿರುಗಿ cached_ಸಿಕ್ಕಿತು;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ confidential_virtualization_ಕೋಷ್ಟಕ[_CONFIDENTIAL_VIRTUALIZATION_MAX] = {
        [CONFIDENTIAL_VIRTUALIZATION_NONE]     = "none",
        [CONFIDENTIAL_VIRTUALIZATION_SEV]      = "sev",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_ES]   = "sev-es",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_SNP]  = "sev-snp",
        [CONFIDENTIAL_VIRTUALIZATION_TDX]      = "tdx",
        [CONFIDENTIAL_VIRTUALIZATION_PROTVIRT] = "protvirt",
        [CONFIDENTIAL_VIRTUALIZATION_CCA]      = "cca",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(confidential_virtualization, ConfidentialVirtualization);
