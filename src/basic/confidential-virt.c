/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಯದಿ defined(__i386__) || defined(__x86_64__)
#ಅನ್ತರ್ಭಾವ <cpuid.h>
#endif
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

#ಯದಿ defined(__x86_64__)

ಸ್ಥಿರ ಶೂನ್ಯ cpuid(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
        ಪ್ರತಿಜ್ಞಾ(eax);
        ಪ್ರತಿಜ್ಞಾ(ebx);
        ಪ್ರತಿಜ್ಞಾ(ecx);
        ಪ್ರತಿಜ್ಞಾ(edx);

        log_debug("CPUID func %" PRIx32 " %" PRIx32, *eax, *ecx);
        __cpuid_count(*eax, *ecx, *eax, *ebx, *ecx, *edx);
        log_debug("CPUID result %" PRIx32 " %" PRIx32 " %" PRIx32 " %" PRIx32, *eax, *ebx, *ecx, *edx);
}

ಸ್ಥಿರ uint32_t cpuid_leaf(uint32_t eax, ವರ್ಣ ret_sig[ಸ್ಥಿರ 13], bool swapped) {
        /* zero-init as some queries explicitly require subleaf == 0 */
        uint32_t sig[3] = {};

        ಯದಿ (swapped)
                cpuid(&eax, &sig[0], &sig[2], &sig[1]);
        ಅನ್ಯಥಾ
                cpuid(&eax, &sig[0], &sig[1], &sig[2]);
        memcpy(ret_sig, sig, ಪರಿಮಾಣ(sig));
        ret_sig[12] = 0; /* \0-terminate the string to make string comparison possible */

        /* In some CI tests ret_sig doesn't contain valid UTF8 and prints garbage to the console */
        log_debug("CPUID sig '%s'", strna(utf8_is_valid(ret_sig)));

        ಪ್ರತ್ಯಾಗಮ eax;
}

#define MSR_DEVICE "/dev/cpu/0/msr"

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ msr(uint64_t index, uint64_t *ret) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
        uint64_t v;
        ssize_t n;

        ಪ್ರತಿಜ್ಞಾ(ret);

        fd = open(MSR_DEVICE, O_RDONLY|O_CLOEXEC);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(errno,
                                       "Cannot open MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);

        n = pread(fd, &v, ಪರಿಮಾಣ(v), index);
        ಯದಿ (n < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(errno,
                                       "Cannot read MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);
        ಯದಿ (n != ಪರಿಮಾಣ(v))
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Short read %zd bytes from MSR device %s (index %" PRIu64 ")",
                                       n, MSR_DEVICE, index);

        log_debug("MSR %" PRIu64 " result %" PRIu64, index, v);
        *ret = v;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ bool detect_hyperv_cvm(uint32_t isoltype) {
        uint32_t eax, ebx, ecx, edx, feat;
        ವರ್ಣ sig[13] = {};

        feat = cpuid_leaf(CPUID_HYPERV_VENDOR_AND_MAX_FUNCTIONS, sig, false);

        ಯದಿ (feat < CPUID_HYPERV_MIN || feat > CPUID_HYPERV_MAX)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (memcmp(sig, CPUID_SIG_HYPERV, ಪರಿಮಾಣ(sig)) != 0)
                ಪ್ರತ್ಯಾಗಮ false;

        log_debug("CPUID is on hyperv");
        eax = CPUID_HYPERV_FEATURES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        ಯದಿ (ebx & CPUID_HYPERV_ISOLATION && !(ebx & CPUID_HYPERV_CPU_MANAGEMENT)) {

                eax = CPUID_HYPERV_ISOLATION_CONFIG;
                ebx = ecx = edx = 0;
                cpuid(&eax, &ebx, &ecx, &edx);

                ಯದಿ ((ebx & CPUID_HYPERV_ISOLATION_TYPE_MASK) == isoltype)
                        ಪ್ರತ್ಯಾಗಮ true;
        }

        ಪ್ರತ್ಯಾಗಮ false;
}

ಸ್ಥಿರ ConfidentialVirtualization detect_sev(ಶೂನ್ಯ) {
        uint32_t eax, ebx, ecx, edx;
        uint64_t msrval;
        ಪೂರ್ಣಾಙ್ಕ r;

        eax = CPUID_GET_HIGHEST_FUNCTION;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        ಯದಿ (eax < CPUID_AMD_GET_ENCRYPTED_MEMORY_CAPABILITIES)
                ಪ್ರತ್ಯಾಗಮ CONFIDENTIAL_VIRTUALIZATION_NONE;

        eax = CPUID_AMD_GET_ENCRYPTED_MEMORY_CAPABILITIES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        /* bit 1 == CPU supports SEV feature
         *
         * Note, Azure blocks this CPUID leaf from its SEV-SNP
         * guests, so we must fallback to trying some HyperV
         * specific CPUID checks.
         */
        ಯದಿ (!(eax & EAX_SEV)) {
                log_debug("No sev in CPUID, trying hyperv CPUID");

                ಯದಿ (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_SNP))
                        ಪ್ರತ್ಯಾಗಮ CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;

                log_debug("No hyperv CPUID");
                ಪ್ರತ್ಯಾಗಮ CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        r = msr(MSR_AMD64_SEV, &msrval);
        ಯದಿ (r < 0) {
                /* The CPU advertises SEV support and we're running under a hypervisor, but we couldn't read
                 * the SEV MSR to determine the exact mode (e.g. /dev/cpu/0/msr is unavailable because the
                 * msr module isn't loaded). Assume plain SEV. Misreporting a genuine confidential guest as
                 * non-confidential would wrongly make us trust hypervisor-provided data such as firmware credentials. */
                log_debug_errno(r, "Failed to read SEV MSR, assuming SEV: %m");
                ಪ್ರತ್ಯಾಗಮ CONFIDENTIAL_VIRTUALIZATION_SEV;
        }

        /* Test reverse order, since the SEV-SNP bit implies
         * the SEV-ES bit, which implies the SEV bit */
        ಯದಿ (msrval & MSR_SEV_SNP)
                ಪ್ರತ್ಯಾಗಮ CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;
        ಯದಿ (msrval & MSR_SEV_ES)
                ಪ್ರತ್ಯಾಗಮ CONFIDENTIAL_VIRTUALIZATION_SEV_ES;
        ಯದಿ (msrval & MSR_SEV)
                ಪ್ರತ್ಯಾಗಮ CONFIDENTIAL_VIRTUALIZATION_SEV;

        ಪ್ರತ್ಯಾಗಮ CONFIDENTIAL_VIRTUALIZATION_NONE;
}

ಸ್ಥಿರ ConfidentialVirtualization detect_tdx(ಶೂನ್ಯ) {
        ವರ್ಣ sig[13] = {};

        /* Querying an unsupported CPUID leaf is harmless (it returns the highest basic leaf's data rather
         * than faulting), so reading this leaf and matching the IntelTDX signature is sufficient. */
        cpuid_leaf(CPUID_INTEL_TDX_ENUMERATION, sig, true);

        ಯದಿ (memcmp(sig, CPUID_SIG_INTEL_TDX, ಪರಿಮಾಣ(sig)) == 0)
                ಪ್ರತ್ಯಾಗಮ CONFIDENTIAL_VIRTUALIZATION_TDX;

        log_debug("No tdx in CPUID, trying hyperv CPUID");

        ಯದಿ (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_TDX))
                ಪ್ರತ್ಯಾಗಮ CONFIDENTIAL_VIRTUALIZATION_TDX;

        ಪ್ರತ್ಯಾಗಮ CONFIDENTIAL_VIRTUALIZATION_NONE;
}

ಸ್ಥಿರ bool detect_hypervisor(ಶೂನ್ಯ) {
        uint32_t eax, ebx, ecx, edx;
        bool is_hv;

        eax = CPUID_PROCESSOR_INFO_AND_FEATURE_BITS;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        is_hv = ecx & CPUID_FEATURE_HYPERVISOR;

        log_debug("CPUID is hypervisor: %s", yes_no(is_hv));
        ಪ್ರತ್ಯಾಗಮ is_hv;
}

ಸ್ಥಿರ ConfidentialVirtualization detect_confidential_virtualization_impl(ಶೂನ್ಯ) {
        ವರ್ಣ sig[13] = {};

        /* Skip everything on bare metal */
        ಯದಿ (detect_hypervisor()) {
                cpuid_leaf(0, sig, true);

                ಯದಿ (memcmp(sig, CPUID_SIG_AMD, ಪರಿಮಾಣ(sig)) == 0)
                        ಪ್ರತ್ಯಾಗಮ detect_sev();
                ಅನ್ಯಥಾ ಯದಿ (memcmp(sig, CPUID_SIG_INTEL, ಪರಿಮಾಣ(sig)) == 0)
                        ಪ್ರತ್ಯಾಗಮ detect_tdx();
        }

        ಪ್ರತ್ಯಾಗಮ CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#ನೋಚೇದ್ಯದಿ defined(__s390x__)
ಸ್ಥಿರ ConfidentialVirtualization detect_confidential_virtualization_impl(ಶೂನ್ಯ) {
        _cleanup_free_ ವರ್ಣ *s = ರಿಕ್ತ;
        size_t readsize;
        ಪೂರ್ಣಾಙ್ಕ r;

        r = read_full_virtual_file("/sys/firmware/uv/prot_virt_guest", &s, &readsize);
        ಯದಿ (r < 0) {
                log_debug_errno(r, "Unable to read /sys/firmware/uv/prot_virt_guest: %m");
                ಪ್ರತ್ಯಾಗಮ CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        ಯದಿ (readsize >= 1 && s[0] == '1')
                ಪ್ರತ್ಯಾಗಮ CONFIDENTIAL_VIRTUALIZATION_PROTVIRT;

        ಪ್ರತ್ಯಾಗಮ CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#ನೋಚೇದ್ಯದಿ defined(__aarch64__)
ಸ್ಥಿರ ConfidentialVirtualization detect_confidential_virtualization_impl(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಙ್ಕ r;

        r = RET_NERRNO(access("/sys/devices/platform/arm-cca-dev", F_OK));
        ಯದಿ (r < 0) {
                log_debug_errno(r, "Unable to check /sys/devices/platform/arm-cca-dev: %m");
                ಪ್ರತ್ಯಾಗಮ CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        ಪ್ರತ್ಯಾಗಮ CONFIDENTIAL_VIRTUALIZATION_CCA;
}
#ಅನ್ಯಥಾ /* ! x86_64 */
ಸ್ಥಿರ ConfidentialVirtualization detect_confidential_virtualization_impl(ಶೂನ್ಯ) {
        log_debug("No confidential virtualization detection on this architecture");
        ಪ್ರತ್ಯಾಗಮ CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#endif /* ! x86_64 */

ConfidentialVirtualization detect_confidential_virtualization(ಶೂನ್ಯ) {
        ಸ್ಥಿರ thread_local ConfidentialVirtualization cached_found = _CONFIDENTIAL_VIRTUALIZATION_INVALID;

        ಯದಿ (cached_found == _CONFIDENTIAL_VIRTUALIZATION_INVALID)
                cached_found = detect_confidential_virtualization_impl();

        ಪ್ರತ್ಯಾಗಮ cached_found;
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *ಧ್ರುವ confidential_virtualization_table[_CONFIDENTIAL_VIRTUALIZATION_MAX] = {
        [CONFIDENTIAL_VIRTUALIZATION_NONE]     = "none",
        [CONFIDENTIAL_VIRTUALIZATION_SEV]      = "sev",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_ES]   = "sev-es",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_SNP]  = "sev-snp",
        [CONFIDENTIAL_VIRTUALIZATION_TDX]      = "tdx",
        [CONFIDENTIAL_VIRTUALIZATION_PROTVIRT] = "protvirt",
        [CONFIDENTIAL_VIRTUALIZATION_CCA]      = "cca",
};

DEFINE_STRING_TABLE_LOOKUP(confidential_virtualization, ConfidentialVirtualization);
