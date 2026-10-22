/* SPDX-License-Identifier: LGPL-2.1-or-later */

#إذا defined(__i386__) || defined(__x86_64__)
#ضمن <cpuid.h>
#endif
#ضمن <fcntl.h>
#ضمن <threads.h>
#ضمن <unistd.h>

#ضمن "confidential-virt.h"
#ضمن "errno-util.h"                                 /* IWYU pragma: keep */
#ضمن "fd-util.h"
#ضمن "fileio.h"                                     /* IWYU pragma: keep */
#ضمن "log.h"
#ضمن "string-table.h"
#ضمن "string-util.h"
#ضمن "utf8.h"

#إذا defined(__x86_64__)

ساكن فراغ cpuid(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
        أكد(eax);
        أكد(ebx);
        أكد(ecx);
        أكد(edx);

        log_debug("CPUID func %" PRIx32 " %" PRIx32, *eax, *ecx);
        __cpuid_count(*eax, *ecx, *eax, *ebx, *ecx, *edx);
        log_debug("CPUID result %" PRIx32 " %" PRIx32 " %" PRIx32 " %" PRIx32, *eax, *ebx, *ecx, *edx);
}

ساكن uint32_t cpuid_leaf(uint32_t eax, حرف ret_sig[ساكن 13], bool swapped) {
        /* zero-init as some queries explicitly require subleaf == 0 */
        uint32_t sig[3] = {};

        إذا (swapped)
                cpuid(&eax, &sig[0], &sig[2], &sig[1]);
        وإلا
                cpuid(&eax, &sig[0], &sig[1], &sig[2]);
        memcpy(ret_sig, sig, حجم(sig));
        ret_sig[12] = 0; /* \0-terminate the string to make string comparison possible */

        /* In some CI tests ret_sig doesn't contain valid UTF8 and prints garbage to the console */
        log_debug("CPUID sig '%s'", strna(utf8_is_valid(ret_sig)));

        أرجع eax;
}

#define MSR_DEVICE "/dev/cpu/0/msr"

ساكن صحيح msr(uint64_t index, uint64_t *ret) {
        _cleanup_close_ صحيح fd = -EBADF;
        uint64_t v;
        ssize_t n;

        أكد(ret);

        fd = open(MSR_DEVICE, O_RDONLY|O_CLOEXEC);
        إذا (fd < 0)
                أرجع log_debug_errno(errno,
                                       "Cannot open MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);

        n = pread(fd, &v, حجم(v), index);
        إذا (n < 0)
                أرجع log_debug_errno(errno,
                                       "Cannot read MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);
        إذا (n != حجم(v))
                أرجع log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Short read %zd bytes from MSR device %s (index %" PRIu64 ")",
                                       n, MSR_DEVICE, index);

        log_debug("MSR %" PRIu64 " result %" PRIu64, index, v);
        *ret = v;
        أرجع 0;
}

ساكن bool detect_hyperv_cvm(uint32_t isoltype) {
        uint32_t eax, ebx, ecx, edx, feat;
        حرف sig[13] = {};

        feat = cpuid_leaf(CPUID_HYPERV_VENDOR_AND_MAX_FUNCTIONS, sig, false);

        إذا (feat < CPUID_HYPERV_MIN || feat > CPUID_HYPERV_MAX)
                أرجع false;

        إذا (memcmp(sig, CPUID_SIG_HYPERV, حجم(sig)) != 0)
                أرجع false;

        log_debug("CPUID is on hyperv");
        eax = CPUID_HYPERV_FEATURES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        إذا (ebx & CPUID_HYPERV_ISOLATION && !(ebx & CPUID_HYPERV_CPU_MANAGEMENT)) {

                eax = CPUID_HYPERV_ISOLATION_CONFIG;
                ebx = ecx = edx = 0;
                cpuid(&eax, &ebx, &ecx, &edx);

                إذا ((ebx & CPUID_HYPERV_ISOLATION_TYPE_MASK) == isoltype)
                        أرجع true;
        }

        أرجع false;
}

ساكن ConfidentialVirtualization detect_sev(فراغ) {
        uint32_t eax, ebx, ecx, edx;
        uint64_t msrval;
        صحيح r;

        eax = CPUID_GET_HIGHEST_FUNCTION;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        إذا (eax < CPUID_AMD_GET_ENCRYPTED_MEMORY_CAPABILITIES)
                أرجع CONFIDENTIAL_VIRTUALIZATION_NONE;

        eax = CPUID_AMD_GET_ENCRYPTED_MEMORY_CAPABILITIES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        /* bit 1 == CPU supports SEV feature
         *
         * Note, Azure blocks this CPUID leaf from its SEV-SNP
         * guests, so we must fallback to trying some HyperV
         * specific CPUID checks.
         */
        إذا (!(eax & EAX_SEV)) {
                log_debug("No sev in CPUID, trying hyperv CPUID");

                إذا (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_SNP))
                        أرجع CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;

                log_debug("No hyperv CPUID");
                أرجع CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        r = msr(MSR_AMD64_SEV, &msrval);
        إذا (r < 0) {
                /* The CPU advertises SEV support and we're running under a hypervisor, but we couldn't read
                 * the SEV MSR to determine the exact mode (e.g. /dev/cpu/0/msr is unavailable because the
                 * msr module isn't loaded). Assume plain SEV. Misreporting a genuine confidential guest as
                 * non-confidential would wrongly make us trust hypervisor-provided data such as firmware credentials. */
                log_debug_errno(r, "Failed to read SEV MSR, assuming SEV: %m");
                أرجع CONFIDENTIAL_VIRTUALIZATION_SEV;
        }

        /* Test reverse order, since the SEV-SNP bit implies
         * the SEV-ES bit, which implies the SEV bit */
        إذا (msrval & MSR_SEV_SNP)
                أرجع CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;
        إذا (msrval & MSR_SEV_ES)
                أرجع CONFIDENTIAL_VIRTUALIZATION_SEV_ES;
        إذا (msrval & MSR_SEV)
                أرجع CONFIDENTIAL_VIRTUALIZATION_SEV;

        أرجع CONFIDENTIAL_VIRTUALIZATION_NONE;
}

ساكن ConfidentialVirtualization detect_tdx(فراغ) {
        حرف sig[13] = {};

        /* Querying an unsupported CPUID leaf is harmless (it returns the highest basic leaf's data rather
         * than faulting), so reading this leaf and matching the IntelTDX signature is sufficient. */
        cpuid_leaf(CPUID_INTEL_TDX_ENUMERATION, sig, true);

        إذا (memcmp(sig, CPUID_SIG_INTEL_TDX, حجم(sig)) == 0)
                أرجع CONFIDENTIAL_VIRTUALIZATION_TDX;

        log_debug("No tdx in CPUID, trying hyperv CPUID");

        إذا (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_TDX))
                أرجع CONFIDENTIAL_VIRTUALIZATION_TDX;

        أرجع CONFIDENTIAL_VIRTUALIZATION_NONE;
}

ساكن bool detect_hypervisor(فراغ) {
        uint32_t eax, ebx, ecx, edx;
        bool is_hv;

        eax = CPUID_PROCESSOR_INFO_AND_FEATURE_BITS;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        is_hv = ecx & CPUID_FEATURE_HYPERVISOR;

        log_debug("CPUID is hypervisor: %s", yes_no(is_hv));
        أرجع is_hv;
}

ساكن ConfidentialVirtualization detect_confidential_virtualization_impl(فراغ) {
        حرف sig[13] = {};

        /* Skip everything on bare metal */
        إذا (detect_hypervisor()) {
                cpuid_leaf(0, sig, true);

                إذا (memcmp(sig, CPUID_SIG_AMD, حجم(sig)) == 0)
                        أرجع detect_sev();
                وإلا إذا (memcmp(sig, CPUID_SIG_INTEL, حجم(sig)) == 0)
                        أرجع detect_tdx();
        }

        أرجع CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#وإلاإذا defined(__s390x__)
ساكن ConfidentialVirtualization detect_confidential_virtualization_impl(فراغ) {
        _cleanup_free_ حرف *s = عدم;
        size_t readsize;
        صحيح r;

        r = read_full_virtual_file("/sys/firmware/uv/prot_virt_guest", &s, &readsize);
        إذا (r < 0) {
                log_debug_errno(r, "Unable to read /sys/firmware/uv/prot_virt_guest: %m");
                أرجع CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        إذا (readsize >= 1 && s[0] == '1')
                أرجع CONFIDENTIAL_VIRTUALIZATION_PROTVIRT;

        أرجع CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#وإلاإذا defined(__aarch64__)
ساكن ConfidentialVirtualization detect_confidential_virtualization_impl(فراغ) {
        صحيح r;

        r = RET_NERRNO(access("/sys/devices/platform/arm-cca-dev", F_OK));
        إذا (r < 0) {
                log_debug_errno(r, "Unable to check /sys/devices/platform/arm-cca-dev: %m");
                أرجع CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        أرجع CONFIDENTIAL_VIRTUALIZATION_CCA;
}
#وإلا /* ! x86_64 */
ساكن ConfidentialVirtualization detect_confidential_virtualization_impl(فراغ) {
        log_debug("No confidential virtualization detection on this architecture");
        أرجع CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#endif /* ! x86_64 */

ConfidentialVirtualization detect_confidential_virtualization(فراغ) {
        ساكن thread_local ConfidentialVirtualization cached_found = _CONFIDENTIAL_VIRTUALIZATION_INVALID;

        إذا (cached_found == _CONFIDENTIAL_VIRTUALIZATION_INVALID)
                cached_found = detect_confidential_virtualization_impl();

        أرجع cached_found;
}

ساكن ثابت حرف *ثابت confidential_virtualization_table[_CONFIDENTIAL_VIRTUALIZATION_MAX] = {
        [CONFIDENTIAL_VIRTUALIZATION_NONE]     = "none",
        [CONFIDENTIAL_VIRTUALIZATION_SEV]      = "sev",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_ES]   = "sev-es",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_SNP]  = "sev-snp",
        [CONFIDENTIAL_VIRTUALIZATION_TDX]      = "tdx",
        [CONFIDENTIAL_VIRTUALIZATION_PROTVIRT] = "protvirt",
        [CONFIDENTIAL_VIRTUALIZATION_CCA]      = "cca",
};

DEFINE_STRING_TABLE_LOOKUP(confidential_virtualization, ConfidentialVirtualization);
