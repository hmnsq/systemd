/* SPDX-License-Identifier: LGPL-2.1-or-later */

#எனில் defined(__i386__) || defined(__x86_64__)
#அந்தர்பாவ <cpuid.h>
#endif
#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <threads.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "confidential-virt.h"
#அந்தர்பாவ "errno-util.h"                                 /* IWYU pragma: keep */
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fileio.h"                                     /* IWYU pragma: keep */
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "string-table.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "utf8.h"

#எனில் defined(__x86_64__)

நிலையான வெறுமை cpuid(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
        ப்ரதிஜ்ஞா(eax);
        ப்ரதிஜ்ஞா(ebx);
        ப்ரதிஜ்ஞா(ecx);
        ப்ரதிஜ்ஞா(edx);

        log_debug("CPUID func %" PRIx32 " %" PRIx32, *eax, *ecx);
        __cpuid_count(*eax, *ecx, *eax, *ebx, *ecx, *edx);
        log_debug("CPUID result %" PRIx32 " %" PRIx32 " %" PRIx32 " %" PRIx32, *eax, *ebx, *ecx, *edx);
}

நிலையான uint32_t cpuid_leaf(uint32_t eax, எழுத்து ret_sig[நிலையான 13], bool swapped) {
        /* zero-init as some queries explicitly require subleaf == 0 */
        uint32_t sig[3] = {};

        எனில் (swapped)
                cpuid(&eax, &sig[0], &sig[2], &sig[1]);
        இல்லையெனில்
                cpuid(&eax, &sig[0], &sig[1], &sig[2]);
        memcpy(ret_sig, sig, அளவு(sig));
        ret_sig[12] = 0; /* \0-terminate the string to make string comparison possible */

        /* In some CI tests ret_sig doesn't contain valid UTF8 and prints garbage to the console */
        log_debug("CPUID sig '%s'", strna(utf8_is_valid(ret_sig)));

        திரும்பு eax;
}

#define MSR_DEVICE "/dev/cpu/0/msr"

நிலையான முழு msr(uint64_t index, uint64_t *ret) {
        _cleanup_close_ முழு fd = -EBADF;
        uint64_t v;
        ssize_t n;

        ப்ரதிஜ்ஞா(ret);

        fd = open(MSR_DEVICE, O_RDONLY|O_CLOEXEC);
        எனில் (fd < 0)
                திரும்பு log_debug_errno(errno,
                                       "Cannot open MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);

        n = pread(fd, &v, அளவு(v), index);
        எனில் (n < 0)
                திரும்பு log_debug_errno(errno,
                                       "Cannot read MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);
        எனில் (n != அளவு(v))
                திரும்பு log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Short read %zd bytes from MSR device %s (index %" PRIu64 ")",
                                       n, MSR_DEVICE, index);

        log_debug("MSR %" PRIu64 " result %" PRIu64, index, v);
        *ret = v;
        திரும்பு 0;
}

நிலையான bool detect_hyperv_cvm(uint32_t isoltype) {
        uint32_t eax, ebx, ecx, edx, feat;
        எழுத்து sig[13] = {};

        feat = cpuid_leaf(CPUID_HYPERV_VENDOR_AND_MAX_FUNCTIONS, sig, false);

        எனில் (feat < CPUID_HYPERV_MIN || feat > CPUID_HYPERV_MAX)
                திரும்பு false;

        எனில் (memcmp(sig, CPUID_SIG_HYPERV, அளவு(sig)) != 0)
                திரும்பு false;

        log_debug("CPUID is on hyperv");
        eax = CPUID_HYPERV_FEATURES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        எனில் (ebx & CPUID_HYPERV_ISOLATION && !(ebx & CPUID_HYPERV_CPU_MANAGEMENT)) {

                eax = CPUID_HYPERV_ISOLATION_CONFIG;
                ebx = ecx = edx = 0;
                cpuid(&eax, &ebx, &ecx, &edx);

                எனில் ((ebx & CPUID_HYPERV_ISOLATION_TYPE_MASK) == isoltype)
                        திரும்பு true;
        }

        திரும்பு false;
}

நிலையான ConfidentialVirtualization detect_sev(வெறுமை) {
        uint32_t eax, ebx, ecx, edx;
        uint64_t msrval;
        முழு r;

        eax = CPUID_GET_HIGHEST_FUNCTION;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        எனில் (eax < CPUID_AMD_GET_ENCRYPTED_MEMORY_CAPABILITIES)
                திரும்பு CONFIDENTIAL_VIRTUALIZATION_NONE;

        eax = CPUID_AMD_GET_ENCRYPTED_MEMORY_CAPABILITIES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        /* bit 1 == CPU supports SEV feature
         *
         * Note, Azure blocks this CPUID leaf from its SEV-SNP
         * guests, so we must fallback to trying some HyperV
         * specific CPUID checks.
         */
        எனில் (!(eax & EAX_SEV)) {
                log_debug("No sev in CPUID, trying hyperv CPUID");

                எனில் (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_SNP))
                        திரும்பு CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;

                log_debug("No hyperv CPUID");
                திரும்பு CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        r = msr(MSR_AMD64_SEV, &msrval);
        எனில் (r < 0) {
                /* The CPU advertises SEV support and we're running under a hypervisor, but we couldn't read
                 * the SEV MSR to determine the exact mode (e.g. /dev/cpu/0/msr is unavailable because the
                 * msr module isn't loaded). Assume plain SEV. Misreporting a genuine confidential guest as
                 * non-confidential would wrongly make us trust hypervisor-provided data such as firmware credentials. */
                log_debug_errno(r, "Failed to read SEV MSR, assuming SEV: %m");
                திரும்பு CONFIDENTIAL_VIRTUALIZATION_SEV;
        }

        /* Test reverse order, since the SEV-SNP bit implies
         * the SEV-ES bit, which implies the SEV bit */
        எனில் (msrval & MSR_SEV_SNP)
                திரும்பு CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;
        எனில் (msrval & MSR_SEV_ES)
                திரும்பு CONFIDENTIAL_VIRTUALIZATION_SEV_ES;
        எனில் (msrval & MSR_SEV)
                திரும்பு CONFIDENTIAL_VIRTUALIZATION_SEV;

        திரும்பு CONFIDENTIAL_VIRTUALIZATION_NONE;
}

நிலையான ConfidentialVirtualization detect_tdx(வெறுமை) {
        எழுத்து sig[13] = {};

        /* Querying an unsupported CPUID leaf is harmless (it returns the highest basic leaf's data rather
         * than faulting), so reading this leaf and matching the IntelTDX signature is sufficient. */
        cpuid_leaf(CPUID_INTEL_TDX_ENUMERATION, sig, true);

        எனில் (memcmp(sig, CPUID_SIG_INTEL_TDX, அளவு(sig)) == 0)
                திரும்பு CONFIDENTIAL_VIRTUALIZATION_TDX;

        log_debug("No tdx in CPUID, trying hyperv CPUID");

        எனில் (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_TDX))
                திரும்பு CONFIDENTIAL_VIRTUALIZATION_TDX;

        திரும்பு CONFIDENTIAL_VIRTUALIZATION_NONE;
}

நிலையான bool detect_hypervisor(வெறுமை) {
        uint32_t eax, ebx, ecx, edx;
        bool is_hv;

        eax = CPUID_PROCESSOR_INFO_AND_FEATURE_BITS;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        is_hv = ecx & CPUID_FEATURE_HYPERVISOR;

        log_debug("CPUID is hypervisor: %s", yes_no(is_hv));
        திரும்பு is_hv;
}

நிலையான ConfidentialVirtualization detect_confidential_virtualization_impl(வெறுமை) {
        எழுத்து sig[13] = {};

        /* Skip everything on bare metal */
        எனில் (detect_hypervisor()) {
                cpuid_leaf(0, sig, true);

                எனில் (memcmp(sig, CPUID_SIG_AMD, அளவு(sig)) == 0)
                        திரும்பு detect_sev();
                இல்லையெனில் எனில் (memcmp(sig, CPUID_SIG_INTEL, அளவு(sig)) == 0)
                        திரும்பு detect_tdx();
        }

        திரும்பு CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#அல்லதெனில் defined(__s390x__)
நிலையான ConfidentialVirtualization detect_confidential_virtualization_impl(வெறுமை) {
        _cleanup_free_ எழுத்து *s = வெற்று;
        size_t readsize;
        முழு r;

        r = read_full_virtual_file("/sys/firmware/uv/prot_virt_guest", &s, &readsize);
        எனில் (r < 0) {
                log_debug_errno(r, "Unable to read /sys/firmware/uv/prot_virt_guest: %m");
                திரும்பு CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        எனில் (readsize >= 1 && s[0] == '1')
                திரும்பு CONFIDENTIAL_VIRTUALIZATION_PROTVIRT;

        திரும்பு CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#அல்லதெனில் defined(__aarch64__)
நிலையான ConfidentialVirtualization detect_confidential_virtualization_impl(வெறுமை) {
        முழு r;

        r = RET_NERRNO(access("/sys/devices/platform/arm-cca-dev", F_OK));
        எனில் (r < 0) {
                log_debug_errno(r, "Unable to check /sys/devices/platform/arm-cca-dev: %m");
                திரும்பு CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        திரும்பு CONFIDENTIAL_VIRTUALIZATION_CCA;
}
#இல்லையெனில் /* ! x86_64 */
நிலையான ConfidentialVirtualization detect_confidential_virtualization_impl(வெறுமை) {
        log_debug("No confidential virtualization detection on this architecture");
        திரும்பு CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#endif /* ! x86_64 */

ConfidentialVirtualization detect_confidential_virtualization(வெறுமை) {
        நிலையான thread_local ConfidentialVirtualization cached_found = _CONFIDENTIAL_VIRTUALIZATION_INVALID;

        எனில் (cached_found == _CONFIDENTIAL_VIRTUALIZATION_INVALID)
                cached_found = detect_confidential_virtualization_impl();

        திரும்பு cached_found;
}

நிலையான மாறிலி எழுத்து *மாறிலி confidential_virtualization_table[_CONFIDENTIAL_VIRTUALIZATION_MAX] = {
        [CONFIDENTIAL_VIRTUALIZATION_NONE]     = "none",
        [CONFIDENTIAL_VIRTUALIZATION_SEV]      = "sev",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_ES]   = "sev-es",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_SNP]  = "sev-snp",
        [CONFIDENTIAL_VIRTUALIZATION_TDX]      = "tdx",
        [CONFIDENTIAL_VIRTUALIZATION_PROTVIRT] = "protvirt",
        [CONFIDENTIAL_VIRTUALIZATION_CCA]      = "cca",
};

DEFINE_STRING_TABLE_LOOKUP(confidential_virtualization, ConfidentialVirtualization);
