/* SPDX-License-Identifier: LGPL-2.1-or-later */

#যদি defined(__i386__) || defined(__x86_64__)
#অন্তর্ভাব <cpuid.h>
#endif
#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <threads.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "confidential-virt.h"
#অন্তর্ভাব "errno-util.h"                                 /* IWYU pragma: keep */
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "fileio.h"                                     /* IWYU pragma: keep */
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "string-table.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "utf8.h"

#যদি defined(__x86_64__)

স্থির শূন্য cpuid(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
        প্রতিজ্ঞা(eax);
        প্রতিজ্ঞা(ebx);
        প্রতিজ্ঞা(ecx);
        প্রতিজ্ঞা(edx);

        log_debug("CPUID func %" PRIx32 " %" PRIx32, *eax, *ecx);
        __cpuid_count(*eax, *ecx, *eax, *ebx, *ecx, *edx);
        log_debug("CPUID result %" PRIx32 " %" PRIx32 " %" PRIx32 " %" PRIx32, *eax, *ebx, *ecx, *edx);
}

স্থির uint32_t cpuid_leaf(uint32_t eax, অক্ষর ret_sig[স্থির 13], bool swapped) {
        /* zero-init as some queries explicitly require subleaf == 0 */
        uint32_t sig[3] = {};

        যদি (swapped)
                cpuid(&eax, &sig[0], &sig[2], &sig[1]);
        অন্যথা
                cpuid(&eax, &sig[0], &sig[1], &sig[2]);
        memcpy(ret_sig, sig, আকার(sig));
        ret_sig[12] = 0; /* \0-terminate the string to make string comparison possible */

        /* In some CI tests ret_sig doesn't contain valid UTF8 and prints garbage to the console */
        log_debug("CPUID sig '%s'", strna(utf8_is_valid(ret_sig)));

        ফেরত eax;
}

#define MSR_DEVICE "/dev/cpu/0/msr"

স্থির পূর্ণসংখ্যা msr(uint64_t index, uint64_t *ret) {
        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;
        uint64_t v;
        ssize_t n;

        প্রতিজ্ঞা(ret);

        fd = open(MSR_DEVICE, O_RDONLY|O_CLOEXEC);
        যদি (fd < 0)
                ফেরত log_debug_errno(errno,
                                       "Cannot open MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);

        n = pread(fd, &v, আকার(v), index);
        যদি (n < 0)
                ফেরত log_debug_errno(errno,
                                       "Cannot read MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);
        যদি (n != আকার(v))
                ফেরত log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Short read %zd bytes from MSR device %s (index %" PRIu64 ")",
                                       n, MSR_DEVICE, index);

        log_debug("MSR %" PRIu64 " result %" PRIu64, index, v);
        *ret = v;
        ফেরত 0;
}

স্থির bool detect_hyperv_cvm(uint32_t isoltype) {
        uint32_t eax, ebx, ecx, edx, feat;
        অক্ষর sig[13] = {};

        feat = cpuid_leaf(CPUID_HYPERV_VENDOR_AND_MAX_FUNCTIONS, sig, false);

        যদি (feat < CPUID_HYPERV_MIN || feat > CPUID_HYPERV_MAX)
                ফেরত false;

        যদি (memcmp(sig, CPUID_SIG_HYPERV, আকার(sig)) != 0)
                ফেরত false;

        log_debug("CPUID is on hyperv");
        eax = CPUID_HYPERV_FEATURES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        যদি (ebx & CPUID_HYPERV_ISOLATION && !(ebx & CPUID_HYPERV_CPU_MANAGEMENT)) {

                eax = CPUID_HYPERV_ISOLATION_CONFIG;
                ebx = ecx = edx = 0;
                cpuid(&eax, &ebx, &ecx, &edx);

                যদি ((ebx & CPUID_HYPERV_ISOLATION_TYPE_MASK) == isoltype)
                        ফেরত true;
        }

        ফেরত false;
}

স্থির ConfidentialVirtualization detect_sev(শূন্য) {
        uint32_t eax, ebx, ecx, edx;
        uint64_t msrval;
        পূর্ণসংখ্যা r;

        eax = CPUID_GET_HIGHEST_FUNCTION;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        যদি (eax < CPUID_AMD_GET_ENCRYPTED_MEMORY_CAPABILITIES)
                ফেরত CONFIDENTIAL_VIRTUALIZATION_NONE;

        eax = CPUID_AMD_GET_ENCRYPTED_MEMORY_CAPABILITIES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        /* bit 1 == CPU supports SEV feature
         *
         * Note, Azure blocks this CPUID leaf from its SEV-SNP
         * guests, so we must fallback to trying some HyperV
         * specific CPUID checks.
         */
        যদি (!(eax & EAX_SEV)) {
                log_debug("No sev in CPUID, trying hyperv CPUID");

                যদি (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_SNP))
                        ফেরত CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;

                log_debug("No hyperv CPUID");
                ফেরত CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        r = msr(MSR_AMD64_SEV, &msrval);
        যদি (r < 0) {
                /* The CPU advertises SEV support and we're running under a hypervisor, but we couldn't read
                 * the SEV MSR to determine the exact mode (e.g. /dev/cpu/0/msr is unavailable because the
                 * msr module isn't loaded). Assume plain SEV. Misreporting a genuine confidential guest as
                 * non-confidential would wrongly make us trust hypervisor-provided data such as firmware credentials. */
                log_debug_errno(r, "Failed to read SEV MSR, assuming SEV: %m");
                ফেরত CONFIDENTIAL_VIRTUALIZATION_SEV;
        }

        /* Test reverse order, since the SEV-SNP bit implies
         * the SEV-ES bit, which implies the SEV bit */
        যদি (msrval & MSR_SEV_SNP)
                ফেরত CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;
        যদি (msrval & MSR_SEV_ES)
                ফেরত CONFIDENTIAL_VIRTUALIZATION_SEV_ES;
        যদি (msrval & MSR_SEV)
                ফেরত CONFIDENTIAL_VIRTUALIZATION_SEV;

        ফেরত CONFIDENTIAL_VIRTUALIZATION_NONE;
}

স্থির ConfidentialVirtualization detect_tdx(শূন্য) {
        অক্ষর sig[13] = {};

        /* Querying an unsupported CPUID leaf is harmless (it returns the highest basic leaf's data rather
         * than faulting), so reading this leaf and matching the IntelTDX signature is sufficient. */
        cpuid_leaf(CPUID_INTEL_TDX_ENUMERATION, sig, true);

        যদি (memcmp(sig, CPUID_SIG_INTEL_TDX, আকার(sig)) == 0)
                ফেরত CONFIDENTIAL_VIRTUALIZATION_TDX;

        log_debug("No tdx in CPUID, trying hyperv CPUID");

        যদি (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_TDX))
                ফেরত CONFIDENTIAL_VIRTUALIZATION_TDX;

        ফেরত CONFIDENTIAL_VIRTUALIZATION_NONE;
}

স্থির bool detect_hypervisor(শূন্য) {
        uint32_t eax, ebx, ecx, edx;
        bool is_hv;

        eax = CPUID_PROCESSOR_INFO_AND_FEATURE_BITS;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        is_hv = ecx & CPUID_FEATURE_HYPERVISOR;

        log_debug("CPUID is hypervisor: %s", yes_no(is_hv));
        ফেরত is_hv;
}

স্থির ConfidentialVirtualization detect_confidential_virtualization_impl(শূন্য) {
        অক্ষর sig[13] = {};

        /* Skip everything on bare metal */
        যদি (detect_hypervisor()) {
                cpuid_leaf(0, sig, true);

                যদি (memcmp(sig, CPUID_SIG_AMD, আকার(sig)) == 0)
                        ফেরত detect_sev();
                অন্যথা যদি (memcmp(sig, CPUID_SIG_INTEL, আকার(sig)) == 0)
                        ফেরত detect_tdx();
        }

        ফেরত CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#নতুবা_যদি defined(__s390x__)
স্থির ConfidentialVirtualization detect_confidential_virtualization_impl(শূন্য) {
        _cleanup_free_ অক্ষর *s = ফাঁকা;
        size_t readsize;
        পূর্ণসংখ্যা r;

        r = read_full_virtual_file("/sys/firmware/uv/prot_virt_guest", &s, &readsize);
        যদি (r < 0) {
                log_debug_errno(r, "Unable to read /sys/firmware/uv/prot_virt_guest: %m");
                ফেরত CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        যদি (readsize >= 1 && s[0] == '1')
                ফেরত CONFIDENTIAL_VIRTUALIZATION_PROTVIRT;

        ফেরত CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#নতুবা_যদি defined(__aarch64__)
স্থির ConfidentialVirtualization detect_confidential_virtualization_impl(শূন্য) {
        পূর্ণসংখ্যা r;

        r = RET_NERRNO(access("/sys/devices/platform/arm-cca-dev", F_OK));
        যদি (r < 0) {
                log_debug_errno(r, "Unable to check /sys/devices/platform/arm-cca-dev: %m");
                ফেরত CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        ফেরত CONFIDENTIAL_VIRTUALIZATION_CCA;
}
#অন্যথা /* ! x86_64 */
স্থির ConfidentialVirtualization detect_confidential_virtualization_impl(শূন্য) {
        log_debug("No confidential virtualization detection on this architecture");
        ফেরত CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#endif /* ! x86_64 */

ConfidentialVirtualization detect_confidential_virtualization(শূন্য) {
        স্থির thread_local ConfidentialVirtualization cached_found = _CONFIDENTIAL_VIRTUALIZATION_INVALID;

        যদি (cached_found == _CONFIDENTIAL_VIRTUALIZATION_INVALID)
                cached_found = detect_confidential_virtualization_impl();

        ফেরত cached_found;
}

স্থির ধ্রুবক অক্ষর *ধ্রুবক confidential_virtualization_table[_CONFIDENTIAL_VIRTUALIZATION_MAX] = {
        [CONFIDENTIAL_VIRTUALIZATION_NONE]     = "none",
        [CONFIDENTIAL_VIRTUALIZATION_SEV]      = "sev",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_ES]   = "sev-es",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_SNP]  = "sev-snp",
        [CONFIDENTIAL_VIRTUALIZATION_TDX]      = "tdx",
        [CONFIDENTIAL_VIRTUALIZATION_PROTVIRT] = "protvirt",
        [CONFIDENTIAL_VIRTUALIZATION_CCA]      = "cca",
};

DEFINE_STRING_TABLE_LOOKUP(confidential_virtualization, ConfidentialVirtualization);
