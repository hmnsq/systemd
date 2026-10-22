/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अगर defined(__i386__) || defined(__x86_64__)
#समावेश <cpuid.h>
#पूर्ण_अगर
#समावेश <fcntl.h>
#समावेश <threads.h>
#समावेश <unistd.h>

#समावेश "confidential-virt.h"
#समावेश "errno-util.h"                                 /* IWYU pragma: keep */
#समावेश "fd-util.h"
#समावेश "fileio.h"                                     /* IWYU pragma: keep */
#समावेश "log.h"
#समावेश "string-table.h"
#समावेश "string-util.h"
#समावेश "utf8.h"

#अगर defined(__x86_64__)

अटल व्योम cpuid(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
        निश्चित(eax);
        निश्चित(ebx);
        निश्चित(ecx);
        निश्चित(edx);

        log_debug("CPUID func %" PRIx32 " %" PRIx32, *eax, *ecx);
        __cpuid_count(*eax, *ecx, *eax, *ebx, *ecx, *edx);
        log_debug("CPUID result %" PRIx32 " %" PRIx32 " %" PRIx32 " %" PRIx32, *eax, *ebx, *ecx, *edx);
}

अटल uint32_t cpuid_leaf(uint32_t eax, अक्षर ret_sig[अटल 13], bool swapped) {
        /* zero-init as some queries explicitly require subleaf == 0 */
        uint32_t sig[3] = {};

        अगर (swapped)
                cpuid(&eax, &sig[0], &sig[2], &sig[1]);
        अन्यथा
                cpuid(&eax, &sig[0], &sig[1], &sig[2]);
        स_नकल(ret_sig, sig, माप(sig));
        ret_sig[12] = 0; /* \0-terminate the string to make string comparison possible */

        /* In some CI tests ret_sig doesn't contain valid UTF8 and prints garbage to the console */
        log_debug("CPUID sig '%s'", strna(utf8_is_valid(ret_sig)));

        वापस eax;
}

#घोषणा MSR_DEVICE "/dev/cpu/0/msr"

अटल पूर्णांक msr(uint64_t index, uint64_t *ret) {
        _cleanup_close_ पूर्णांक fd = -EBADF;
        uint64_t v;
        ssize_t n;

        निश्चित(ret);

        fd = खोलो(MSR_DEVICE, O_RDONLY|O_CLOEXEC);
        अगर (fd < 0)
                वापस log_debug_errno(त्रुटि_सं,
                                       "Cannot open MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);

        n = pread(fd, &v, माप(v), index);
        अगर (n < 0)
                वापस log_debug_errno(त्रुटि_सं,
                                       "Cannot read MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);
        अगर (n != माप(v))
                वापस log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Short read %zd bytes from MSR device %s (index %" PRIu64 ")",
                                       n, MSR_DEVICE, index);

        log_debug("MSR %" PRIu64 " result %" PRIu64, index, v);
        *ret = v;
        वापस 0;
}

अटल bool detect_hyperv_cvm(uint32_t isoltype) {
        uint32_t eax, ebx, ecx, edx, feat;
        अक्षर sig[13] = {};

        feat = cpuid_leaf(CPUID_HYPERV_VENDOR_AND_MAX_FUNCTIONS, sig, false);

        अगर (feat < CPUID_HYPERV_MIN || feat > CPUID_HYPERV_MAX)
                वापस false;

        अगर (स_भेद(sig, CPUID_SIG_HYPERV, माप(sig)) != 0)
                वापस false;

        log_debug("CPUID is on hyperv");
        eax = CPUID_HYPERV_FEATURES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        अगर (ebx & CPUID_HYPERV_ISOLATION && !(ebx & CPUID_HYPERV_CPU_MANAGEMENT)) {

                eax = CPUID_HYPERV_ISOLATION_CONFIG;
                ebx = ecx = edx = 0;
                cpuid(&eax, &ebx, &ecx, &edx);

                अगर ((ebx & CPUID_HYPERV_ISOLATION_TYPE_MASK) == isoltype)
                        वापस true;
        }

        वापस false;
}

अटल ConfidentialVirtualization detect_sev(व्योम) {
        uint32_t eax, ebx, ecx, edx;
        uint64_t msrval;
        पूर्णांक r;

        eax = CPUID_GET_HIGHEST_FUNCTION;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        अगर (eax < CPUID_AMD_GET_ENCRYPTED_MEMORY_CAPABILITIES)
                वापस CONFIDENTIAL_VIRTUALIZATION_NONE;

        eax = CPUID_AMD_GET_ENCRYPTED_MEMORY_CAPABILITIES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        /* bit 1 == CPU supports SEV feature
         *
         * Note, Azure blocks this CPUID leaf from its SEV-SNP
         * guests, so we must fallback to trying some HyperV
         * specific CPUID checks.
         */
        अगर (!(eax & EAX_SEV)) {
                log_debug("No sev in CPUID, trying hyperv CPUID");

                अगर (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_SNP))
                        वापस CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;

                log_debug("No hyperv CPUID");
                वापस CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        r = msr(MSR_AMD64_SEV, &msrval);
        अगर (r < 0) {
                /* The CPU advertises SEV support and we're running under a hypervisor, but we couldn't read
                 * the SEV MSR to determine the exact mode (e.g. /dev/cpu/0/msr is unavailable because the
                 * msr module isn't loaded). Assume plain SEV. Misreporting a genuine confidential guest as
                 * non-confidential would wrongly make us trust hypervisor-provided data such as firmware credentials. */
                log_debug_errno(r, "Failed to read SEV MSR, assuming SEV: %m");
                वापस CONFIDENTIAL_VIRTUALIZATION_SEV;
        }

        /* Test reverse order, since the SEV-SNP bit implies
         * the SEV-ES bit, which implies the SEV bit */
        अगर (msrval & MSR_SEV_SNP)
                वापस CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;
        अगर (msrval & MSR_SEV_ES)
                वापस CONFIDENTIAL_VIRTUALIZATION_SEV_ES;
        अगर (msrval & MSR_SEV)
                वापस CONFIDENTIAL_VIRTUALIZATION_SEV;

        वापस CONFIDENTIAL_VIRTUALIZATION_NONE;
}

अटल ConfidentialVirtualization detect_tdx(व्योम) {
        अक्षर sig[13] = {};

        /* Querying an unsupported CPUID leaf is harmless (it returns the highest basic leaf's data rather
         * than faulting), so reading this leaf and matching the IntelTDX signature is sufficient. */
        cpuid_leaf(CPUID_INTEL_TDX_ENUMERATION, sig, true);

        अगर (स_भेद(sig, CPUID_SIG_INTEL_TDX, माप(sig)) == 0)
                वापस CONFIDENTIAL_VIRTUALIZATION_TDX;

        log_debug("No tdx in CPUID, trying hyperv CPUID");

        अगर (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_TDX))
                वापस CONFIDENTIAL_VIRTUALIZATION_TDX;

        वापस CONFIDENTIAL_VIRTUALIZATION_NONE;
}

अटल bool detect_hypervisor(व्योम) {
        uint32_t eax, ebx, ecx, edx;
        bool is_hv;

        eax = CPUID_PROCESSOR_INFO_AND_FEATURE_BITS;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        is_hv = ecx & CPUID_FEATURE_HYPERVISOR;

        log_debug("CPUID is hypervisor: %s", yes_no(is_hv));
        वापस is_hv;
}

अटल ConfidentialVirtualization detect_confidential_virtualization_impl(व्योम) {
        अक्षर sig[13] = {};

        /* Skip everything on bare metal */
        अगर (detect_hypervisor()) {
                cpuid_leaf(0, sig, true);

                अगर (स_भेद(sig, CPUID_SIG_AMD, माप(sig)) == 0)
                        वापस detect_sev();
                अन्यथा अगर (स_भेद(sig, CPUID_SIG_INTEL, माप(sig)) == 0)
                        वापस detect_tdx();
        }

        वापस CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#या_अगर defined(__s390x__)
अटल ConfidentialVirtualization detect_confidential_virtualization_impl(व्योम) {
        _cleanup_free_ अक्षर *s = शून्य;
        माप_प्रकार readsize;
        पूर्णांक r;

        r = read_full_virtual_file("/sys/firmware/uv/prot_virt_guest", &s, &readsize);
        अगर (r < 0) {
                log_debug_errno(r, "Unable to read /sys/firmware/uv/prot_virt_guest: %m");
                वापस CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        अगर (readsize >= 1 && s[0] == '1')
                वापस CONFIDENTIAL_VIRTUALIZATION_PROTVIRT;

        वापस CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#या_अगर defined(__aarch64__)
अटल ConfidentialVirtualization detect_confidential_virtualization_impl(व्योम) {
        पूर्णांक r;

        r = RET_NERRNO(access("/sys/devices/platform/arm-cca-dev", F_OK));
        अगर (r < 0) {
                log_debug_errno(r, "Unable to check /sys/devices/platform/arm-cca-dev: %m");
                वापस CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        वापस CONFIDENTIAL_VIRTUALIZATION_CCA;
}
#अन्यथा /* ! x86_64 */
अटल ConfidentialVirtualization detect_confidential_virtualization_impl(व्योम) {
        log_debug("No confidential virtualization detection on this architecture");
        वापस CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#पूर्ण_अगर /* ! x86_64 */

ConfidentialVirtualization detect_confidential_virtualization(व्योम) {
        अटल thread_local ConfidentialVirtualization cached_found = _CONFIDENTIAL_VIRTUALIZATION_INVALID;

        अगर (cached_found == _CONFIDENTIAL_VIRTUALIZATION_INVALID)
                cached_found = detect_confidential_virtualization_impl();

        वापस cached_found;
}

अटल स्थिर अक्षर *स्थिर confidential_virtualization_table[_CONFIDENTIAL_VIRTUALIZATION_MAX] = {
        [CONFIDENTIAL_VIRTUALIZATION_NONE]     = "none",
        [CONFIDENTIAL_VIRTUALIZATION_SEV]      = "sev",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_ES]   = "sev-es",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_SNP]  = "sev-snp",
        [CONFIDENTIAL_VIRTUALIZATION_TDX]      = "tdx",
        [CONFIDENTIAL_VIRTUALIZATION_PROTVIRT] = "protvirt",
        [CONFIDENTIAL_VIRTUALIZATION_CCA]      = "cca",
};

DEFINE_STRING_TABLE_LOOKUP(confidential_virtualization, ConfidentialVirtualization);
