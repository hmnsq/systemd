/* SPDX-License-Identifier: LGPL-2.1-or-later */

#यदि defined(__i386__) || defined(__x86_64__)
#अन्तर्भाव <cpuid.h>
#endif
#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <threads.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "confidential-virt.h"
#अन्तर्भाव "errno-util.h"                                 /* IWYU pragma: keep */
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fileio.h"                                     /* IWYU pragma: keep */
#अन्तर्भाव "log.h"
#अन्तर्भाव "string-table.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "utf8.h"

#यदि defined(__x86_64__)

स्थिर शून्य cpuid(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
        प्रतिज्ञा(eax);
        प्रतिज्ञा(ebx);
        प्रतिज्ञा(ecx);
        प्रतिज्ञा(edx);

        log_debug("CPUID func %" PRIx32 " %" PRIx32, *eax, *ecx);
        __cpuid_count(*eax, *ecx, *eax, *ebx, *ecx, *edx);
        log_debug("CPUID result %" PRIx32 " %" PRIx32 " %" PRIx32 " %" PRIx32, *eax, *ebx, *ecx, *edx);
}

स्थिर uint32_t cpuid_leaf(uint32_t eax, वर्ण ret_sig[स्थिर 13], bool swapped) {
        /* zero-init as some queries explicitly require subleaf == 0 */
        uint32_t sig[3] = {};

        यदि (swapped)
                cpuid(&eax, &sig[0], &sig[2], &sig[1]);
        अन्यथा
                cpuid(&eax, &sig[0], &sig[1], &sig[2]);
        स_नकल(ret_sig, sig, परिमाण(sig));
        ret_sig[12] = 0; /* \0-terminate the string to make string comparison possible */

        /* In some CI tests ret_sig doesn't contain valid UTF8 and prints garbage to the console */
        log_debug("CPUID sig '%s'", strna(utf8_is_valid(ret_sig)));

        फर्काऊ eax;
}

#define MSR_DEVICE "/dev/cpu/0/msr"

स्थिर पूर्णाङ्क msr(uint64_t index, uint64_t *ret) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        uint64_t v;
        ssize_t n;

        प्रतिज्ञा(ret);

        fd = खोलो(MSR_DEVICE, O_RDONLY|O_CLOEXEC);
        यदि (fd < 0)
                फर्काऊ log_debug_errno(त्रुटि_सं,
                                       "Cannot open MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);

        n = pread(fd, &v, परिमाण(v), index);
        यदि (n < 0)
                फर्काऊ log_debug_errno(त्रुटि_सं,
                                       "Cannot read MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);
        यदि (n != परिमाण(v))
                फर्काऊ log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Short read %zd bytes from MSR device %s (index %" PRIu64 ")",
                                       n, MSR_DEVICE, index);

        log_debug("MSR %" PRIu64 " result %" PRIu64, index, v);
        *ret = v;
        फर्काऊ 0;
}

स्थिर bool detect_hyperv_cvm(uint32_t isoltype) {
        uint32_t eax, ebx, ecx, edx, feat;
        वर्ण sig[13] = {};

        feat = cpuid_leaf(CPUID_HYPERV_VENDOR_AND_MAX_FUNCTIONS, sig, false);

        यदि (feat < CPUID_HYPERV_MIN || feat > CPUID_HYPERV_MAX)
                फर्काऊ false;

        यदि (स_भेद(sig, CPUID_SIG_HYPERV, परिमाण(sig)) != 0)
                फर्काऊ false;

        log_debug("CPUID is on hyperv");
        eax = CPUID_HYPERV_FEATURES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        यदि (ebx & CPUID_HYPERV_ISOLATION && !(ebx & CPUID_HYPERV_CPU_MANAGEMENT)) {

                eax = CPUID_HYPERV_ISOLATION_CONFIG;
                ebx = ecx = edx = 0;
                cpuid(&eax, &ebx, &ecx, &edx);

                यदि ((ebx & CPUID_HYPERV_ISOLATION_TYPE_MASK) == isoltype)
                        फर्काऊ true;
        }

        फर्काऊ false;
}

स्थिर ConfidentialVirtualization detect_sev(शून्य) {
        uint32_t eax, ebx, ecx, edx;
        uint64_t msrval;
        पूर्णाङ्क r;

        eax = CPUID_GET_HIGHEST_FUNCTION;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        यदि (eax < CPUID_AMD_GET_ENCRYPTED_MEMORY_CAPABILITIES)
                फर्काऊ CONFIDENTIAL_VIRTUALIZATION_NONE;

        eax = CPUID_AMD_GET_ENCRYPTED_MEMORY_CAPABILITIES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        /* bit 1 == CPU supports SEV feature
         *
         * Note, Azure blocks this CPUID leaf from its SEV-SNP
         * guests, so we must fallback to trying some HyperV
         * specific CPUID checks.
         */
        यदि (!(eax & EAX_SEV)) {
                log_debug("No sev in CPUID, trying hyperv CPUID");

                यदि (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_SNP))
                        फर्काऊ CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;

                log_debug("No hyperv CPUID");
                फर्काऊ CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        r = msr(MSR_AMD64_SEV, &msrval);
        यदि (r < 0) {
                /* The CPU advertises SEV support and we're running under a hypervisor, but we couldn't read
                 * the SEV MSR to determine the exact mode (e.g. /dev/cpu/0/msr is unavailable because the
                 * msr module isn't loaded). Assume plain SEV. Misreporting a genuine confidential guest as
                 * non-confidential would wrongly make us trust hypervisor-provided data such as firmware credentials. */
                log_debug_errno(r, "Failed to read SEV MSR, assuming SEV: %m");
                फर्काऊ CONFIDENTIAL_VIRTUALIZATION_SEV;
        }

        /* Test reverse order, since the SEV-SNP bit implies
         * the SEV-ES bit, which implies the SEV bit */
        यदि (msrval & MSR_SEV_SNP)
                फर्काऊ CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;
        यदि (msrval & MSR_SEV_ES)
                फर्काऊ CONFIDENTIAL_VIRTUALIZATION_SEV_ES;
        यदि (msrval & MSR_SEV)
                फर्काऊ CONFIDENTIAL_VIRTUALIZATION_SEV;

        फर्काऊ CONFIDENTIAL_VIRTUALIZATION_NONE;
}

स्थिर ConfidentialVirtualization detect_tdx(शून्य) {
        वर्ण sig[13] = {};

        /* Querying an unsupported CPUID leaf is harmless (it returns the highest basic leaf's data rather
         * than faulting), so reading this leaf and matching the IntelTDX signature is sufficient. */
        cpuid_leaf(CPUID_INTEL_TDX_ENUMERATION, sig, true);

        यदि (स_भेद(sig, CPUID_SIG_INTEL_TDX, परिमाण(sig)) == 0)
                फर्काऊ CONFIDENTIAL_VIRTUALIZATION_TDX;

        log_debug("No tdx in CPUID, trying hyperv CPUID");

        यदि (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_TDX))
                फर्काऊ CONFIDENTIAL_VIRTUALIZATION_TDX;

        फर्काऊ CONFIDENTIAL_VIRTUALIZATION_NONE;
}

स्थिर bool detect_hypervisor(शून्य) {
        uint32_t eax, ebx, ecx, edx;
        bool is_hv;

        eax = CPUID_PROCESSOR_INFO_AND_FEATURE_BITS;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        is_hv = ecx & CPUID_FEATURE_HYPERVISOR;

        log_debug("CPUID is hypervisor: %s", yes_no(is_hv));
        फर्काऊ is_hv;
}

स्थिर ConfidentialVirtualization detect_confidential_virtualization_impl(शून्य) {
        वर्ण sig[13] = {};

        /* Skip everything on bare metal */
        यदि (detect_hypervisor()) {
                cpuid_leaf(0, sig, true);

                यदि (स_भेद(sig, CPUID_SIG_AMD, परिमाण(sig)) == 0)
                        फर्काऊ detect_sev();
                अन्यथा यदि (स_भेद(sig, CPUID_SIG_INTEL, परिमाण(sig)) == 0)
                        फर्काऊ detect_tdx();
        }

        फर्काऊ CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#अथवायदि defined(__s390x__)
स्थिर ConfidentialVirtualization detect_confidential_virtualization_impl(शून्य) {
        _cleanup_free_ वर्ण *s = रिक्त;
        माप_प्रकार readsize;
        पूर्णाङ्क r;

        r = read_full_virtual_file("/sys/firmware/uv/prot_virt_guest", &s, &readsize);
        यदि (r < 0) {
                log_debug_errno(r, "Unable to read /sys/firmware/uv/prot_virt_guest: %m");
                फर्काऊ CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        यदि (readsize >= 1 && s[0] == '1')
                फर्काऊ CONFIDENTIAL_VIRTUALIZATION_PROTVIRT;

        फर्काऊ CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#अथवायदि defined(__aarch64__)
स्थिर ConfidentialVirtualization detect_confidential_virtualization_impl(शून्य) {
        पूर्णाङ्क r;

        r = RET_NERRNO(access("/sys/devices/platform/arm-cca-dev", F_OK));
        यदि (r < 0) {
                log_debug_errno(r, "Unable to check /sys/devices/platform/arm-cca-dev: %m");
                फर्काऊ CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        फर्काऊ CONFIDENTIAL_VIRTUALIZATION_CCA;
}
#अन्यथा /* ! x86_64 */
स्थिर ConfidentialVirtualization detect_confidential_virtualization_impl(शून्य) {
        log_debug("No confidential virtualization detection on this architecture");
        फर्काऊ CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#endif /* ! x86_64 */

ConfidentialVirtualization detect_confidential_virtualization(शून्य) {
        स्थिर thread_local ConfidentialVirtualization cached_found = _CONFIDENTIAL_VIRTUALIZATION_INVALID;

        यदि (cached_found == _CONFIDENTIAL_VIRTUALIZATION_INVALID)
                cached_found = detect_confidential_virtualization_impl();

        फर्काऊ cached_found;
}

स्थिर अचर वर्ण *अचर confidential_virtualization_table[_CONFIDENTIAL_VIRTUALIZATION_MAX] = {
        [CONFIDENTIAL_VIRTUALIZATION_NONE]     = "none",
        [CONFIDENTIAL_VIRTUALIZATION_SEV]      = "sev",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_ES]   = "sev-es",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_SNP]  = "sev-snp",
        [CONFIDENTIAL_VIRTUALIZATION_TDX]      = "tdx",
        [CONFIDENTIAL_VIRTUALIZATION_PROTVIRT] = "protvirt",
        [CONFIDENTIAL_VIRTUALIZATION_CCA]      = "cca",
};

DEFINE_STRING_TABLE_LOOKUP(confidential_virtualization, ConfidentialVirtualization);
