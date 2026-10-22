/* SPDX-License-Identifier: LGPL-2.1-or-later */

#എങ്കിൽ defined(__i386__) || defined(__x86_64__)
#അന്തര്ഭാവ <cpuid.h>
#endif
#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <threads.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "confidential-virt.h"
#അന്തര്ഭാവ "errno-util.h"                                 /* IWYU pragma: keep */
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fileio.h"                                     /* IWYU pragma: keep */
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "string-table.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "utf8.h"

#എങ്കിൽ defined(__x86_64__)

സ്ഥിതം ശൂന്യം cpuid(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
        പ്രതിജ്ഞാ(eax);
        പ്രതിജ്ഞാ(ebx);
        പ്രതിജ്ഞാ(ecx);
        പ്രതിജ്ഞാ(edx);

        log_debug("CPUID func %" PRIx32 " %" PRIx32, *eax, *ecx);
        __cpuid_count(*eax, *ecx, *eax, *ebx, *ecx, *edx);
        log_debug("CPUID result %" PRIx32 " %" PRIx32 " %" PRIx32 " %" PRIx32, *eax, *ebx, *ecx, *edx);
}

സ്ഥിതം uint32_t cpuid_leaf(uint32_t eax, അക്ഷരം ret_sig[സ്ഥിതം 13], bool swapped) {
        /* zero-init as some queries explicitly require subleaf == 0 */
        uint32_t sig[3] = {};

        എങ്കിൽ (swapped)
                cpuid(&eax, &sig[0], &sig[2], &sig[1]);
        അല്ലെങ്കിൽ
                cpuid(&eax, &sig[0], &sig[1], &sig[2]);
        memcpy(ret_sig, sig, വലുപ്പം(sig));
        ret_sig[12] = 0; /* \0-terminate the string to make string comparison possible */

        /* In some CI tests ret_sig doesn't contain valid UTF8 and prints garbage to the console */
        log_debug("CPUID sig '%s'", strna(utf8_is_valid(ret_sig)));

        തിരികെ eax;
}

#define MSR_DEVICE "/dev/cpu/0/msr"

സ്ഥിതം പൂർണ്ണസംഖ്യ msr(uint64_t index, uint64_t *ret) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
        uint64_t v;
        ssize_t n;

        പ്രതിജ്ഞാ(ret);

        fd = open(MSR_DEVICE, O_RDONLY|O_CLOEXEC);
        എങ്കിൽ (fd < 0)
                തിരികെ log_debug_errno(errno,
                                       "Cannot open MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);

        n = pread(fd, &v, വലുപ്പം(v), index);
        എങ്കിൽ (n < 0)
                തിരികെ log_debug_errno(errno,
                                       "Cannot read MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);
        എങ്കിൽ (n != വലുപ്പം(v))
                തിരികെ log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Short read %zd bytes from MSR device %s (index %" PRIu64 ")",
                                       n, MSR_DEVICE, index);

        log_debug("MSR %" PRIu64 " result %" PRIu64, index, v);
        *ret = v;
        തിരികെ 0;
}

സ്ഥിതം bool detect_hyperv_cvm(uint32_t isoltype) {
        uint32_t eax, ebx, ecx, edx, feat;
        അക്ഷരം sig[13] = {};

        feat = cpuid_leaf(CPUID_HYPERV_VENDOR_AND_MAX_FUNCTIONS, sig, false);

        എങ്കിൽ (feat < CPUID_HYPERV_MIN || feat > CPUID_HYPERV_MAX)
                തിരികെ false;

        എങ്കിൽ (memcmp(sig, CPUID_SIG_HYPERV, വലുപ്പം(sig)) != 0)
                തിരികെ false;

        log_debug("CPUID is on hyperv");
        eax = CPUID_HYPERV_FEATURES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        എങ്കിൽ (ebx & CPUID_HYPERV_ISOLATION && !(ebx & CPUID_HYPERV_CPU_MANAGEMENT)) {

                eax = CPUID_HYPERV_ISOLATION_CONFIG;
                ebx = ecx = edx = 0;
                cpuid(&eax, &ebx, &ecx, &edx);

                എങ്കിൽ ((ebx & CPUID_HYPERV_ISOLATION_TYPE_MASK) == isoltype)
                        തിരികെ true;
        }

        തിരികെ false;
}

സ്ഥിതം ConfidentialVirtualization detect_sev(ശൂന്യം) {
        uint32_t eax, ebx, ecx, edx;
        uint64_t msrval;
        പൂർണ്ണസംഖ്യ r;

        eax = CPUID_GET_HIGHEST_FUNCTION;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        എങ്കിൽ (eax < CPUID_AMD_GET_ENCRYPTED_MEMORY_CAPABILITIES)
                തിരികെ CONFIDENTIAL_VIRTUALIZATION_NONE;

        eax = CPUID_AMD_GET_ENCRYPTED_MEMORY_CAPABILITIES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        /* bit 1 == CPU supports SEV feature
         *
         * Note, Azure blocks this CPUID leaf from its SEV-SNP
         * guests, so we must fallback to trying some HyperV
         * specific CPUID checks.
         */
        എങ്കിൽ (!(eax & EAX_SEV)) {
                log_debug("No sev in CPUID, trying hyperv CPUID");

                എങ്കിൽ (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_SNP))
                        തിരികെ CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;

                log_debug("No hyperv CPUID");
                തിരികെ CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        r = msr(MSR_AMD64_SEV, &msrval);
        എങ്കിൽ (r < 0) {
                /* The CPU advertises SEV support and we're running under a hypervisor, but we couldn't read
                 * the SEV MSR to determine the exact mode (e.g. /dev/cpu/0/msr is unavailable because the
                 * msr module isn't loaded). Assume plain SEV. Misreporting a genuine confidential guest as
                 * non-confidential would wrongly make us trust hypervisor-provided data such as firmware credentials. */
                log_debug_errno(r, "Failed to read SEV MSR, assuming SEV: %m");
                തിരികെ CONFIDENTIAL_VIRTUALIZATION_SEV;
        }

        /* Test reverse order, since the SEV-SNP bit implies
         * the SEV-ES bit, which implies the SEV bit */
        എങ്കിൽ (msrval & MSR_SEV_SNP)
                തിരികെ CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;
        എങ്കിൽ (msrval & MSR_SEV_ES)
                തിരികെ CONFIDENTIAL_VIRTUALIZATION_SEV_ES;
        എങ്കിൽ (msrval & MSR_SEV)
                തിരികെ CONFIDENTIAL_VIRTUALIZATION_SEV;

        തിരികെ CONFIDENTIAL_VIRTUALIZATION_NONE;
}

സ്ഥിതം ConfidentialVirtualization detect_tdx(ശൂന്യം) {
        അക്ഷരം sig[13] = {};

        /* Querying an unsupported CPUID leaf is harmless (it returns the highest basic leaf's data rather
         * than faulting), so reading this leaf and matching the IntelTDX signature is sufficient. */
        cpuid_leaf(CPUID_INTEL_TDX_ENUMERATION, sig, true);

        എങ്കിൽ (memcmp(sig, CPUID_SIG_INTEL_TDX, വലുപ്പം(sig)) == 0)
                തിരികെ CONFIDENTIAL_VIRTUALIZATION_TDX;

        log_debug("No tdx in CPUID, trying hyperv CPUID");

        എങ്കിൽ (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_TDX))
                തിരികെ CONFIDENTIAL_VIRTUALIZATION_TDX;

        തിരികെ CONFIDENTIAL_VIRTUALIZATION_NONE;
}

സ്ഥിതം bool detect_hypervisor(ശൂന്യം) {
        uint32_t eax, ebx, ecx, edx;
        bool is_hv;

        eax = CPUID_PROCESSOR_INFO_AND_FEATURE_BITS;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        is_hv = ecx & CPUID_FEATURE_HYPERVISOR;

        log_debug("CPUID is hypervisor: %s", yes_no(is_hv));
        തിരികെ is_hv;
}

സ്ഥിതം ConfidentialVirtualization detect_confidential_virtualization_impl(ശൂന്യം) {
        അക്ഷരം sig[13] = {};

        /* Skip everything on bare metal */
        എങ്കിൽ (detect_hypervisor()) {
                cpuid_leaf(0, sig, true);

                എങ്കിൽ (memcmp(sig, CPUID_SIG_AMD, വലുപ്പം(sig)) == 0)
                        തിരികെ detect_sev();
                അല്ലെങ്കിൽ എങ്കിൽ (memcmp(sig, CPUID_SIG_INTEL, വലുപ്പം(sig)) == 0)
                        തിരികെ detect_tdx();
        }

        തിരികെ CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#അല്ലെങ്കിലെങ്കിൽ defined(__s390x__)
സ്ഥിതം ConfidentialVirtualization detect_confidential_virtualization_impl(ശൂന്യം) {
        _cleanup_free_ അക്ഷരം *s = ശൂന്യ;
        size_t readsize;
        പൂർണ്ണസംഖ്യ r;

        r = read_full_virtual_file("/sys/firmware/uv/prot_virt_guest", &s, &readsize);
        എങ്കിൽ (r < 0) {
                log_debug_errno(r, "Unable to read /sys/firmware/uv/prot_virt_guest: %m");
                തിരികെ CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        എങ്കിൽ (readsize >= 1 && s[0] == '1')
                തിരികെ CONFIDENTIAL_VIRTUALIZATION_PROTVIRT;

        തിരികെ CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#അല്ലെങ്കിലെങ്കിൽ defined(__aarch64__)
സ്ഥിതം ConfidentialVirtualization detect_confidential_virtualization_impl(ശൂന്യം) {
        പൂർണ്ണസംഖ്യ r;

        r = RET_NERRNO(access("/sys/devices/platform/arm-cca-dev", F_OK));
        എങ്കിൽ (r < 0) {
                log_debug_errno(r, "Unable to check /sys/devices/platform/arm-cca-dev: %m");
                തിരികെ CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        തിരികെ CONFIDENTIAL_VIRTUALIZATION_CCA;
}
#അല്ലെങ്കിൽ /* ! x86_64 */
സ്ഥിതം ConfidentialVirtualization detect_confidential_virtualization_impl(ശൂന്യം) {
        log_debug("No confidential virtualization detection on this architecture");
        തിരികെ CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#endif /* ! x86_64 */

ConfidentialVirtualization detect_confidential_virtualization(ശൂന്യം) {
        സ്ഥിതം thread_local ConfidentialVirtualization cached_found = _CONFIDENTIAL_VIRTUALIZATION_INVALID;

        എങ്കിൽ (cached_found == _CONFIDENTIAL_VIRTUALIZATION_INVALID)
                cached_found = detect_confidential_virtualization_impl();

        തിരികെ cached_found;
}

സ്ഥിതം സ്ഥിരം അക്ഷരം *സ്ഥിരം confidential_virtualization_table[_CONFIDENTIAL_VIRTUALIZATION_MAX] = {
        [CONFIDENTIAL_VIRTUALIZATION_NONE]     = "none",
        [CONFIDENTIAL_VIRTUALIZATION_SEV]      = "sev",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_ES]   = "sev-es",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_SNP]  = "sev-snp",
        [CONFIDENTIAL_VIRTUALIZATION_TDX]      = "tdx",
        [CONFIDENTIAL_VIRTUALIZATION_PROTVIRT] = "protvirt",
        [CONFIDENTIAL_VIRTUALIZATION_CCA]      = "cca",
};

DEFINE_STRING_TABLE_LOOKUP(confidential_virtualization, ConfidentialVirtualization);
