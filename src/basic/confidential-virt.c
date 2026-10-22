/* SPDX-License-Identifier: LGPL-2.1-or-later */

#اگر defined(__i386__) || defined(__x86_64__)
#شامل <cpuid.h>
#endif
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

ایستا تهی cpuid(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
        أكد(eax);
        أكد(ebx);
        أكد(ecx);
        أكد(edx);

        log_debug("CPUID func %" PRIx32 " %" PRIx32, *eax, *ecx);
        __cpuid_count(*eax, *ecx, *eax, *ebx, *ecx, *edx);
        log_debug("CPUID result %" PRIx32 " %" PRIx32 " %" PRIx32 " %" PRIx32, *eax, *ebx, *ecx, *edx);
}

ایستا uint32_t cpuid_leaf(uint32_t eax, نویسه ret_sig[ایستا 13], bool swapped) {
        /* zero-init as some queries explicitly require subleaf == 0 */
        uint32_t sig[3] = {};

        اگر (swapped)
                cpuid(&eax, &sig[0], &sig[2], &sig[1]);
        وگرنه
                cpuid(&eax, &sig[0], &sig[1], &sig[2]);
        memcpy(ret_sig, sig, اندازه(sig));
        ret_sig[12] = 0; /* \0-terminate the string to make string comparison possible */

        /* In some CI tests ret_sig doesn't contain valid UTF8 and prints garbage to the console */
        log_debug("CPUID sig '%s'", strna(utf8_is_valid(ret_sig)));

        بازگشت eax;
}

#define MSR_DEVICE "/dev/cpu/0/msr"

ایستا صحیح msr(uint64_t index, uint64_t *ret) {
        _cleanup_close_ صحیح fd = -EBADF;
        uint64_t v;
        ssize_t n;

        أكد(ret);

        fd = open(MSR_DEVICE, O_RDONLY|O_CLOEXEC);
        اگر (fd < 0)
                بازگشت log_debug_errno(errno,
                                       "Cannot open MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);

        n = pread(fd, &v, اندازه(v), index);
        اگر (n < 0)
                بازگشت log_debug_errno(errno,
                                       "Cannot read MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);
        اگر (n != اندازه(v))
                بازگشت log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Short read %zd bytes from MSR device %s (index %" PRIu64 ")",
                                       n, MSR_DEVICE, index);

        log_debug("MSR %" PRIu64 " result %" PRIu64, index, v);
        *ret = v;
        بازگشت 0;
}

ایستا bool detect_hyperv_cvm(uint32_t isoltype) {
        uint32_t eax, ebx, ecx, edx, feat;
        نویسه sig[13] = {};

        feat = cpuid_leaf(CPUID_HYPERV_VENDOR_AND_MAX_FUNCTIONS, sig, false);

        اگر (feat < CPUID_HYPERV_MIN || feat > CPUID_HYPERV_MAX)
                بازگشت false;

        اگر (memcmp(sig, CPUID_SIG_HYPERV, اندازه(sig)) != 0)
                بازگشت false;

        log_debug("CPUID is on hyperv");
        eax = CPUID_HYPERV_FEATURES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        اگر (ebx & CPUID_HYPERV_ISOLATION && !(ebx & CPUID_HYPERV_CPU_MANAGEMENT)) {

                eax = CPUID_HYPERV_ISOLATION_CONFIG;
                ebx = ecx = edx = 0;
                cpuid(&eax, &ebx, &ecx, &edx);

                اگر ((ebx & CPUID_HYPERV_ISOLATION_TYPE_MASK) == isoltype)
                        بازگشت true;
        }

        بازگشت false;
}

ایستا ConfidentialVirtualization detect_sev(تهی) {
        uint32_t eax, ebx, ecx, edx;
        uint64_t msrval;
        صحیح r;

        eax = CPUID_GET_HIGHEST_FUNCTION;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        اگر (eax < CPUID_AMD_GET_ENCRYPTED_MEMORY_CAPABILITIES)
                بازگشت CONFIDENTIAL_VIRTUALIZATION_NONE;

        eax = CPUID_AMD_GET_ENCRYPTED_MEMORY_CAPABILITIES;
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
                        بازگشت CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;

                log_debug("No hyperv CPUID");
                بازگشت CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        r = msr(MSR_AMD64_SEV, &msrval);
        اگر (r < 0) {
                /* The CPU advertises SEV support and we're running under a hypervisor, but we couldn't read
                 * the SEV MSR to determine the exact mode (e.g. /dev/cpu/0/msr is unavailable because the
                 * msr module isn't loaded). Assume plain SEV. Misreporting a genuine confidential guest as
                 * non-confidential would wrongly make us trust hypervisor-provided data such as firmware credentials. */
                log_debug_errno(r, "Failed to read SEV MSR, assuming SEV: %m");
                بازگشت CONFIDENTIAL_VIRTUALIZATION_SEV;
        }

        /* Test reverse order, since the SEV-SNP bit implies
         * the SEV-ES bit, which implies the SEV bit */
        اگر (msrval & MSR_SEV_SNP)
                بازگشت CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;
        اگر (msrval & MSR_SEV_ES)
                بازگشت CONFIDENTIAL_VIRTUALIZATION_SEV_ES;
        اگر (msrval & MSR_SEV)
                بازگشت CONFIDENTIAL_VIRTUALIZATION_SEV;

        بازگشت CONFIDENTIAL_VIRTUALIZATION_NONE;
}

ایستا ConfidentialVirtualization detect_tdx(تهی) {
        نویسه sig[13] = {};

        /* Querying an unsupported CPUID leaf is harmless (it returns the highest basic leaf's data rather
         * than faulting), so reading this leaf and matching the IntelTDX signature is sufficient. */
        cpuid_leaf(CPUID_INTEL_TDX_ENUMERATION, sig, true);

        اگر (memcmp(sig, CPUID_SIG_INTEL_TDX, اندازه(sig)) == 0)
                بازگشت CONFIDENTIAL_VIRTUALIZATION_TDX;

        log_debug("No tdx in CPUID, trying hyperv CPUID");

        اگر (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_TDX))
                بازگشت CONFIDENTIAL_VIRTUALIZATION_TDX;

        بازگشت CONFIDENTIAL_VIRTUALIZATION_NONE;
}

ایستا bool detect_hypervisor(تهی) {
        uint32_t eax, ebx, ecx, edx;
        bool is_hv;

        eax = CPUID_PROCESSOR_INFO_AND_FEATURE_BITS;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        is_hv = ecx & CPUID_FEATURE_HYPERVISOR;

        log_debug("CPUID is hypervisor: %s", yes_no(is_hv));
        بازگشت is_hv;
}

ایستا ConfidentialVirtualization detect_confidential_virtualization_impl(تهی) {
        نویسه sig[13] = {};

        /* Skip everything on bare metal */
        اگر (detect_hypervisor()) {
                cpuid_leaf(0, sig, true);

                اگر (memcmp(sig, CPUID_SIG_AMD, اندازه(sig)) == 0)
                        بازگشت detect_sev();
                وگرنه اگر (memcmp(sig, CPUID_SIG_INTEL, اندازه(sig)) == 0)
                        بازگشت detect_tdx();
        }

        بازگشت CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#وگرنهاگر defined(__s390x__)
ایستا ConfidentialVirtualization detect_confidential_virtualization_impl(تهی) {
        _cleanup_free_ نویسه *s = پوچ;
        size_t readsize;
        صحیح r;

        r = read_full_virtual_file("/sys/firmware/uv/prot_virt_guest", &s, &readsize);
        اگر (r < 0) {
                log_debug_errno(r, "Unable to read /sys/firmware/uv/prot_virt_guest: %m");
                بازگشت CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        اگر (readsize >= 1 && s[0] == '1')
                بازگشت CONFIDENTIAL_VIRTUALIZATION_PROTVIRT;

        بازگشت CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#وگرنهاگر defined(__aarch64__)
ایستا ConfidentialVirtualization detect_confidential_virtualization_impl(تهی) {
        صحیح r;

        r = RET_NERRNO(access("/sys/devices/platform/arm-cca-dev", F_OK));
        اگر (r < 0) {
                log_debug_errno(r, "Unable to check /sys/devices/platform/arm-cca-dev: %m");
                بازگشت CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        بازگشت CONFIDENTIAL_VIRTUALIZATION_CCA;
}
#وگرنه /* ! x86_64 */
ایستا ConfidentialVirtualization detect_confidential_virtualization_impl(تهی) {
        log_debug("No confidential virtualization detection on this architecture");
        بازگشت CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#endif /* ! x86_64 */

ConfidentialVirtualization detect_confidential_virtualization(تهی) {
        ایستا thread_local ConfidentialVirtualization cached_found = _CONFIDENTIAL_VIRTUALIZATION_INVALID;

        اگر (cached_found == _CONFIDENTIAL_VIRTUALIZATION_INVALID)
                cached_found = detect_confidential_virtualization_impl();

        بازگشت cached_found;
}

ایستا ثابت نویسه *ثابت confidential_virtualization_table[_CONFIDENTIAL_VIRTUALIZATION_MAX] = {
        [CONFIDENTIAL_VIRTUALIZATION_NONE]     = "none",
        [CONFIDENTIAL_VIRTUALIZATION_SEV]      = "sev",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_ES]   = "sev-es",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_SNP]  = "sev-snp",
        [CONFIDENTIAL_VIRTUALIZATION_TDX]      = "tdx",
        [CONFIDENTIAL_VIRTUALIZATION_PROTVIRT] = "protvirt",
        [CONFIDENTIAL_VIRTUALIZATION_CCA]      = "cca",
};

DEFINE_STRING_TABLE_LOOKUP(confidential_virtualization, ConfidentialVirtualization);
