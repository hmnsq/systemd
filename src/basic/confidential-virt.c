/* SPDX-License-Identifier: LGPL-2.1-or-later */

#જો defined(__i386__) || defined(__x86_64__)
#અન્તર્ભાવ <cpuid.h>
#endif
#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <threads.h>
#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "confidential-virt.h"
#અન્તર્ભાવ "errno-util.h"                                 /* IWYU pragma: keep */
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fileio.h"                                     /* IWYU pragma: keep */
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "string-table.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "utf8.h"

#જો defined(__x86_64__)

સ્થિર શૂન્ય cpuid(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
        પ્રતિજ્ઞા(eax);
        પ્રતિજ્ઞા(ebx);
        પ્રતિજ્ઞા(ecx);
        પ્રતિજ્ઞા(edx);

        log_debug("CPUID func %" PRIx32 " %" PRIx32, *eax, *ecx);
        __cpuid_count(*eax, *ecx, *eax, *ebx, *ecx, *edx);
        log_debug("CPUID result %" PRIx32 " %" PRIx32 " %" PRIx32 " %" PRIx32, *eax, *ebx, *ecx, *edx);
}

સ્થિર uint32_t cpuid_leaf(uint32_t eax, અક્ષર ret_sig[સ્થિર 13], bool swapped) {
        /* zero-init as some queries explicitly require subleaf == 0 */
        uint32_t sig[3] = {};

        જો (swapped)
                cpuid(&eax, &sig[0], &sig[2], &sig[1]);
        નહીંતર
                cpuid(&eax, &sig[0], &sig[1], &sig[2]);
        memcpy(ret_sig, sig, કદ(sig));
        ret_sig[12] = 0; /* \0-terminate the string to make string comparison possible */

        /* In some CI tests ret_sig doesn't contain valid UTF8 and prints garbage to the console */
        log_debug("CPUID sig '%s'", strna(utf8_is_valid(ret_sig)));

        પાછા eax;
}

#define MSR_DEVICE "/dev/cpu/0/msr"

સ્થિર પૂર્ણાંક msr(uint64_t index, uint64_t *ret) {
        _cleanup_close_ પૂર્ણાંક fd = -EBADF;
        uint64_t v;
        ssize_t n;

        પ્રતિજ્ઞા(ret);

        fd = open(MSR_DEVICE, O_RDONLY|O_CLOEXEC);
        જો (fd < 0)
                પાછા log_debug_errno(errno,
                                       "Cannot open MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);

        n = pread(fd, &v, કદ(v), index);
        જો (n < 0)
                પાછા log_debug_errno(errno,
                                       "Cannot read MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);
        જો (n != કદ(v))
                પાછા log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Short read %zd bytes from MSR device %s (index %" PRIu64 ")",
                                       n, MSR_DEVICE, index);

        log_debug("MSR %" PRIu64 " result %" PRIu64, index, v);
        *ret = v;
        પાછા 0;
}

સ્થિર bool detect_hyperv_cvm(uint32_t isoltype) {
        uint32_t eax, ebx, ecx, edx, feat;
        અક્ષર sig[13] = {};

        feat = cpuid_leaf(CPUID_HYPERV_VENDOR_AND_MAX_FUNCTIONS, sig, false);

        જો (feat < CPUID_HYPERV_MIN || feat > CPUID_HYPERV_MAX)
                પાછા false;

        જો (memcmp(sig, CPUID_SIG_HYPERV, કદ(sig)) != 0)
                પાછા false;

        log_debug("CPUID is on hyperv");
        eax = CPUID_HYPERV_FEATURES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        જો (ebx & CPUID_HYPERV_ISOLATION && !(ebx & CPUID_HYPERV_CPU_MANAGEMENT)) {

                eax = CPUID_HYPERV_ISOLATION_CONFIG;
                ebx = ecx = edx = 0;
                cpuid(&eax, &ebx, &ecx, &edx);

                જો ((ebx & CPUID_HYPERV_ISOLATION_TYPE_MASK) == isoltype)
                        પાછા true;
        }

        પાછા false;
}

સ્થિર ConfidentialVirtualization detect_sev(શૂન્ય) {
        uint32_t eax, ebx, ecx, edx;
        uint64_t msrval;
        પૂર્ણાંક r;

        eax = CPUID_GET_HIGHEST_FUNCTION;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        જો (eax < CPUID_AMD_GET_ENCRYPTED_MEMORY_CAPABILITIES)
                પાછા CONFIDENTIAL_VIRTUALIZATION_NONE;

        eax = CPUID_AMD_GET_ENCRYPTED_MEMORY_CAPABILITIES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        /* bit 1 == CPU supports SEV feature
         *
         * Note, Azure blocks this CPUID leaf from its SEV-SNP
         * guests, so we must fallback to trying some HyperV
         * specific CPUID checks.
         */
        જો (!(eax & EAX_SEV)) {
                log_debug("No sev in CPUID, trying hyperv CPUID");

                જો (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_SNP))
                        પાછા CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;

                log_debug("No hyperv CPUID");
                પાછા CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        r = msr(MSR_AMD64_SEV, &msrval);
        જો (r < 0) {
                /* The CPU advertises SEV support and we're running under a hypervisor, but we couldn't read
                 * the SEV MSR to determine the exact mode (e.g. /dev/cpu/0/msr is unavailable because the
                 * msr module isn't loaded). Assume plain SEV. Misreporting a genuine confidential guest as
                 * non-confidential would wrongly make us trust hypervisor-provided data such as firmware credentials. */
                log_debug_errno(r, "Failed to read SEV MSR, assuming SEV: %m");
                પાછા CONFIDENTIAL_VIRTUALIZATION_SEV;
        }

        /* Test reverse order, since the SEV-SNP bit implies
         * the SEV-ES bit, which implies the SEV bit */
        જો (msrval & MSR_SEV_SNP)
                પાછા CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;
        જો (msrval & MSR_SEV_ES)
                પાછા CONFIDENTIAL_VIRTUALIZATION_SEV_ES;
        જો (msrval & MSR_SEV)
                પાછા CONFIDENTIAL_VIRTUALIZATION_SEV;

        પાછા CONFIDENTIAL_VIRTUALIZATION_NONE;
}

સ્થિર ConfidentialVirtualization detect_tdx(શૂન્ય) {
        અક્ષર sig[13] = {};

        /* Querying an unsupported CPUID leaf is harmless (it returns the highest basic leaf's data rather
         * than faulting), so reading this leaf and matching the IntelTDX signature is sufficient. */
        cpuid_leaf(CPUID_INTEL_TDX_ENUMERATION, sig, true);

        જો (memcmp(sig, CPUID_SIG_INTEL_TDX, કદ(sig)) == 0)
                પાછા CONFIDENTIAL_VIRTUALIZATION_TDX;

        log_debug("No tdx in CPUID, trying hyperv CPUID");

        જો (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_TDX))
                પાછા CONFIDENTIAL_VIRTUALIZATION_TDX;

        પાછા CONFIDENTIAL_VIRTUALIZATION_NONE;
}

સ્થિર bool detect_hypervisor(શૂન્ય) {
        uint32_t eax, ebx, ecx, edx;
        bool is_hv;

        eax = CPUID_PROCESSOR_INFO_AND_FEATURE_BITS;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        is_hv = ecx & CPUID_FEATURE_HYPERVISOR;

        log_debug("CPUID is hypervisor: %s", yes_no(is_hv));
        પાછા is_hv;
}

સ્થિર ConfidentialVirtualization detect_confidential_virtualization_impl(શૂન્ય) {
        અક્ષર sig[13] = {};

        /* Skip everything on bare metal */
        જો (detect_hypervisor()) {
                cpuid_leaf(0, sig, true);

                જો (memcmp(sig, CPUID_SIG_AMD, કદ(sig)) == 0)
                        પાછા detect_sev();
                નહીંતર જો (memcmp(sig, CPUID_SIG_INTEL, કદ(sig)) == 0)
                        પાછા detect_tdx();
        }

        પાછા CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#અથવાજો defined(__s390x__)
સ્થિર ConfidentialVirtualization detect_confidential_virtualization_impl(શૂન્ય) {
        _cleanup_free_ અક્ષર *s = ખાલી;
        size_t readsize;
        પૂર્ણાંક r;

        r = read_full_virtual_file("/sys/firmware/uv/prot_virt_guest", &s, &readsize);
        જો (r < 0) {
                log_debug_errno(r, "Unable to read /sys/firmware/uv/prot_virt_guest: %m");
                પાછા CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        જો (readsize >= 1 && s[0] == '1')
                પાછા CONFIDENTIAL_VIRTUALIZATION_PROTVIRT;

        પાછા CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#અથવાજો defined(__aarch64__)
સ્થિર ConfidentialVirtualization detect_confidential_virtualization_impl(શૂન્ય) {
        પૂર્ણાંક r;

        r = RET_NERRNO(access("/sys/devices/platform/arm-cca-dev", F_OK));
        જો (r < 0) {
                log_debug_errno(r, "Unable to check /sys/devices/platform/arm-cca-dev: %m");
                પાછા CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        પાછા CONFIDENTIAL_VIRTUALIZATION_CCA;
}
#નહીંતર /* ! x86_64 */
સ્થિર ConfidentialVirtualization detect_confidential_virtualization_impl(શૂન્ય) {
        log_debug("No confidential virtualization detection on this architecture");
        પાછા CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#endif /* ! x86_64 */

ConfidentialVirtualization detect_confidential_virtualization(શૂન્ય) {
        સ્થિર thread_local ConfidentialVirtualization cached_found = _CONFIDENTIAL_VIRTUALIZATION_INVALID;

        જો (cached_found == _CONFIDENTIAL_VIRTUALIZATION_INVALID)
                cached_found = detect_confidential_virtualization_impl();

        પાછા cached_found;
}

સ્થિર અચલ અક્ષર *અચલ confidential_virtualization_table[_CONFIDENTIAL_VIRTUALIZATION_MAX] = {
        [CONFIDENTIAL_VIRTUALIZATION_NONE]     = "none",
        [CONFIDENTIAL_VIRTUALIZATION_SEV]      = "sev",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_ES]   = "sev-es",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_SNP]  = "sev-snp",
        [CONFIDENTIAL_VIRTUALIZATION_TDX]      = "tdx",
        [CONFIDENTIAL_VIRTUALIZATION_PROTVIRT] = "protvirt",
        [CONFIDENTIAL_VIRTUALIZATION_CCA]      = "cca",
};

DEFINE_STRING_TABLE_LOOKUP(confidential_virtualization, ConfidentialVirtualization);
