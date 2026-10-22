/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଯଦି defined(__i386__) || defined(__x86_64__)
#ଅନ୍ତର୍ଭାଵ <cpuid.h>
#endif
#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <threads.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "confidential-virt.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"                                 /* IWYU pragma: keep */
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fileio.h"                                     /* IWYU pragma: keep */
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "string-table.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "utf8.h"

#ଯଦି defined(__x86_64__)

ସ୍ଥିର ଶୂନ୍ୟ cpuid(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
        ପ୍ରତିଜ୍ଞା(eax);
        ପ୍ରତିଜ୍ଞା(ebx);
        ପ୍ରତିଜ୍ଞା(ecx);
        ପ୍ରତିଜ୍ଞା(edx);

        log_debug("CPUID func %" PRIx32 " %" PRIx32, *eax, *ecx);
        __cpuid_count(*eax, *ecx, *eax, *ebx, *ecx, *edx);
        log_debug("CPUID result %" PRIx32 " %" PRIx32 " %" PRIx32 " %" PRIx32, *eax, *ebx, *ecx, *edx);
}

ସ୍ଥିର uint32_t cpuid_leaf(uint32_t eax, ଅକ୍ଷର ret_sig[ସ୍ଥିର 13], bool swapped) {
        /* zero-init as some queries explicitly require subleaf == 0 */
        uint32_t sig[3] = {};

        ଯଦି (swapped)
                cpuid(&eax, &sig[0], &sig[2], &sig[1]);
        ଅନ୍ୟଥା
                cpuid(&eax, &sig[0], &sig[1], &sig[2]);
        memcpy(ret_sig, sig, ଆକାର(sig));
        ret_sig[12] = 0; /* \0-terminate the string to make string comparison possible */

        /* In some CI tests ret_sig doesn't contain valid UTF8 and prints garbage to the console */
        log_debug("CPUID sig '%s'", strna(utf8_is_valid(ret_sig)));

        ଫେରନ୍ତୁ eax;
}

#define MSR_DEVICE "/dev/cpu/0/msr"

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା msr(uint64_t index, uint64_t *ret) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        uint64_t v;
        ssize_t n;

        ପ୍ରତିଜ୍ଞା(ret);

        fd = open(MSR_DEVICE, O_RDONLY|O_CLOEXEC);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ log_debug_errno(errno,
                                       "Cannot open MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);

        n = pread(fd, &v, ଆକାର(v), index);
        ଯଦି (n < 0)
                ଫେରନ୍ତୁ log_debug_errno(errno,
                                       "Cannot read MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);
        ଯଦି (n != ଆକାର(v))
                ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Short read %zd bytes from MSR device %s (index %" PRIu64 ")",
                                       n, MSR_DEVICE, index);

        log_debug("MSR %" PRIu64 " result %" PRIu64, index, v);
        *ret = v;
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର bool detect_hyperv_cvm(uint32_t isoltype) {
        uint32_t eax, ebx, ecx, edx, feat;
        ଅକ୍ଷର sig[13] = {};

        feat = cpuid_leaf(CPUID_HYPERV_VENDOR_AND_MAX_FUNCTIONS, sig, false);

        ଯଦି (feat < CPUID_HYPERV_MIN || feat > CPUID_HYPERV_MAX)
                ଫେରନ୍ତୁ false;

        ଯଦି (memcmp(sig, CPUID_SIG_HYPERV, ଆକାର(sig)) != 0)
                ଫେରନ୍ତୁ false;

        log_debug("CPUID is on hyperv");
        eax = CPUID_HYPERV_FEATURES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        ଯଦି (ebx & CPUID_HYPERV_ISOLATION && !(ebx & CPUID_HYPERV_CPU_MANAGEMENT)) {

                eax = CPUID_HYPERV_ISOLATION_CONFIG;
                ebx = ecx = edx = 0;
                cpuid(&eax, &ebx, &ecx, &edx);

                ଯଦି ((ebx & CPUID_HYPERV_ISOLATION_TYPE_MASK) == isoltype)
                        ଫେରନ୍ତୁ true;
        }

        ଫେରନ୍ତୁ false;
}

ସ୍ଥିର ConfidentialVirtualization detect_sev(ଶୂନ୍ୟ) {
        uint32_t eax, ebx, ecx, edx;
        uint64_t msrval;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        eax = CPUID_GET_HIGHEST_FUNCTION;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        ଯଦି (eax < CPUID_AMD_GET_ENCRYPTED_MEMORY_CAPABILITIES)
                ଫେରନ୍ତୁ CONFIDENTIAL_VIRTUALIZATION_NONE;

        eax = CPUID_AMD_GET_ENCRYPTED_MEMORY_CAPABILITIES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        /* bit 1 == CPU supports SEV feature
         *
         * Note, Azure blocks this CPUID leaf from its SEV-SNP
         * guests, so we must fallback to trying some HyperV
         * specific CPUID checks.
         */
        ଯଦି (!(eax & EAX_SEV)) {
                log_debug("No sev in CPUID, trying hyperv CPUID");

                ଯଦି (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_SNP))
                        ଫେରନ୍ତୁ CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;

                log_debug("No hyperv CPUID");
                ଫେରନ୍ତୁ CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        r = msr(MSR_AMD64_SEV, &msrval);
        ଯଦି (r < 0) {
                /* The CPU advertises SEV support and we're running under a hypervisor, but we couldn't read
                 * the SEV MSR to determine the exact mode (e.g. /dev/cpu/0/msr is unavailable because the
                 * msr module isn't loaded). Assume plain SEV. Misreporting a genuine confidential guest as
                 * non-confidential would wrongly make us trust hypervisor-provided data such as firmware credentials. */
                log_debug_errno(r, "Failed to read SEV MSR, assuming SEV: %m");
                ଫେରନ୍ତୁ CONFIDENTIAL_VIRTUALIZATION_SEV;
        }

        /* Test reverse order, since the SEV-SNP bit implies
         * the SEV-ES bit, which implies the SEV bit */
        ଯଦି (msrval & MSR_SEV_SNP)
                ଫେରନ୍ତୁ CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;
        ଯଦି (msrval & MSR_SEV_ES)
                ଫେରନ୍ତୁ CONFIDENTIAL_VIRTUALIZATION_SEV_ES;
        ଯଦି (msrval & MSR_SEV)
                ଫେରନ୍ତୁ CONFIDENTIAL_VIRTUALIZATION_SEV;

        ଫେରନ୍ତୁ CONFIDENTIAL_VIRTUALIZATION_NONE;
}

ସ୍ଥିର ConfidentialVirtualization detect_tdx(ଶୂନ୍ୟ) {
        ଅକ୍ଷର sig[13] = {};

        /* Querying an unsupported CPUID leaf is harmless (it returns the highest basic leaf's data rather
         * than faulting), so reading this leaf and matching the IntelTDX signature is sufficient. */
        cpuid_leaf(CPUID_INTEL_TDX_ENUMERATION, sig, true);

        ଯଦି (memcmp(sig, CPUID_SIG_INTEL_TDX, ଆକାର(sig)) == 0)
                ଫେରନ୍ତୁ CONFIDENTIAL_VIRTUALIZATION_TDX;

        log_debug("No tdx in CPUID, trying hyperv CPUID");

        ଯଦି (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_TDX))
                ଫେରନ୍ତୁ CONFIDENTIAL_VIRTUALIZATION_TDX;

        ଫେରନ୍ତୁ CONFIDENTIAL_VIRTUALIZATION_NONE;
}

ସ୍ଥିର bool detect_hypervisor(ଶୂନ୍ୟ) {
        uint32_t eax, ebx, ecx, edx;
        bool is_hv;

        eax = CPUID_PROCESSOR_INFO_AND_FEATURE_BITS;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        is_hv = ecx & CPUID_FEATURE_HYPERVISOR;

        log_debug("CPUID is hypervisor: %s", yes_no(is_hv));
        ଫେରନ୍ତୁ is_hv;
}

ସ୍ଥିର ConfidentialVirtualization detect_confidential_virtualization_impl(ଶୂନ୍ୟ) {
        ଅକ୍ଷର sig[13] = {};

        /* Skip everything on bare metal */
        ଯଦି (detect_hypervisor()) {
                cpuid_leaf(0, sig, true);

                ଯଦି (memcmp(sig, CPUID_SIG_AMD, ଆକାର(sig)) == 0)
                        ଫେରନ୍ତୁ detect_sev();
                ଅନ୍ୟଥା ଯଦି (memcmp(sig, CPUID_SIG_INTEL, ଆକାର(sig)) == 0)
                        ଫେରନ୍ତୁ detect_tdx();
        }

        ଫେରନ୍ତୁ CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#ନଚେତଯଦି defined(__s390x__)
ସ୍ଥିର ConfidentialVirtualization detect_confidential_virtualization_impl(ଶୂନ୍ୟ) {
        _cleanup_free_ ଅକ୍ଷର *s = ଖାଲି;
        size_t readsize;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = read_full_virtual_file("/sys/firmware/uv/prot_virt_guest", &s, &readsize);
        ଯଦି (r < 0) {
                log_debug_errno(r, "Unable to read /sys/firmware/uv/prot_virt_guest: %m");
                ଫେରନ୍ତୁ CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        ଯଦି (readsize >= 1 && s[0] == '1')
                ଫେରନ୍ତୁ CONFIDENTIAL_VIRTUALIZATION_PROTVIRT;

        ଫେରନ୍ତୁ CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#ନଚେତଯଦି defined(__aarch64__)
ସ୍ଥିର ConfidentialVirtualization detect_confidential_virtualization_impl(ଶୂନ୍ୟ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = RET_NERRNO(access("/sys/devices/platform/arm-cca-dev", F_OK));
        ଯଦି (r < 0) {
                log_debug_errno(r, "Unable to check /sys/devices/platform/arm-cca-dev: %m");
                ଫେରନ୍ତୁ CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        ଫେରନ୍ତୁ CONFIDENTIAL_VIRTUALIZATION_CCA;
}
#ଅନ୍ୟଥା /* ! x86_64 */
ସ୍ଥିର ConfidentialVirtualization detect_confidential_virtualization_impl(ଶୂନ୍ୟ) {
        log_debug("No confidential virtualization detection on this architecture");
        ଫେରନ୍ତୁ CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#endif /* ! x86_64 */

ConfidentialVirtualization detect_confidential_virtualization(ଶୂନ୍ୟ) {
        ସ୍ଥିର thread_local ConfidentialVirtualization cached_found = _CONFIDENTIAL_VIRTUALIZATION_INVALID;

        ଯଦି (cached_found == _CONFIDENTIAL_VIRTUALIZATION_INVALID)
                cached_found = detect_confidential_virtualization_impl();

        ଫେରନ୍ତୁ cached_found;
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *ଅଚଳ confidential_virtualization_table[_CONFIDENTIAL_VIRTUALIZATION_MAX] = {
        [CONFIDENTIAL_VIRTUALIZATION_NONE]     = "none",
        [CONFIDENTIAL_VIRTUALIZATION_SEV]      = "sev",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_ES]   = "sev-es",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_SNP]  = "sev-snp",
        [CONFIDENTIAL_VIRTUALIZATION_TDX]      = "tdx",
        [CONFIDENTIAL_VIRTUALIZATION_PROTVIRT] = "protvirt",
        [CONFIDENTIAL_VIRTUALIZATION_CCA]      = "cca",
};

DEFINE_STRING_TABLE_LOOKUP(confidential_virtualization, ConfidentialVirtualization);
