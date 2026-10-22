/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ఒకవేళ defined(__i386__) || defined(__x86_64__)
#అన్తర్భావ <cpuid.h>
#అయితే_ముగింపు
#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <threads.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "confidential-virt.h"
#అన్తర్భావ "errno-util.h"                                 /* IWYU pragma: keep */
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fileio.h"                                     /* IWYU pragma: keep */
#అన్తర్భావ "log.h"
#అన్తర్భావ "string-table.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "utf8.h"

#ఒకవేళ defined(__x86_64__)

స్థిర శూన్య cpuid(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
        ప్రతిజ్ఞా(eax);
        ప్రతిజ్ఞా(ebx);
        ప్రతిజ్ఞా(ecx);
        ప్రతిజ్ఞా(edx);

        log_debug("CPUID func %" PRIx32 " %" PRIx32, *eax, *ecx);
        __cpuid_count(*eax, *ecx, *eax, *ebx, *ecx, *edx);
        log_debug("CPUID result %" PRIx32 " %" PRIx32 " %" PRIx32 " %" PRIx32, *eax, *ebx, *ecx, *edx);
}

స్థిర uint32_t cpuid_ఆకు(uint32_t eax, అక్షరం ret_sig[స్థిర 13], bool swapped) {
        /* zero-init as some queries explicitly require subleaf == 0 */
        uint32_t sig[3] = {};

        ఒకవేళ (swapped)
                cpuid(&eax, &sig[0], &sig[2], &sig[1]);
        లేకపోతే
                cpuid(&eax, &sig[0], &sig[1], &sig[2]);
        జ్ఞాపకం_నకలు(ret_sig, sig, పరిమాణం(sig));
        ret_sig[12] = 0; /* \0-terminate the string to make string comparison possible */

        /* In some CI tests ret_sig doesn't contain valid UTF8 and prints garbage to the console */
        log_debug("CPUID sig '%s'", strna(utf8_is_valid(ret_sig)));

        తిరిగి eax;
}

#నిర్వచించు MSR_DEVICE "/dev/cpu/0/msr"

స్థిర పూర్ణాంకం msr(uint64_t index, uint64_t *ret) {
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        uint64_t v;
        ssize_t n;

        ప్రతిజ్ఞా(ret);

        fd = open(MSR_DEVICE, O_RDONLY|O_CLOEXEC);
        ఒకవేళ (fd < 0)
                తిరిగి log_debug_errno(errno,
                                       "Cannot open MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);

        n = pread(fd, &v, పరిమాణం(v), index);
        ఒకవేళ (n < 0)
                తిరిగి log_debug_errno(errno,
                                       "Cannot read MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);
        ఒకవేళ (n != పరిమాణం(v))
                తిరిగి log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Short read %zd bytes from MSR device %s (index %" PRIu64 ")",
                                       n, MSR_DEVICE, index);

        log_debug("MSR %" PRIu64 " result %" PRIu64, index, v);
        *ret = v;
        తిరిగి 0;
}

స్థిర bool detect_hyperv_cvm(uint32_t isoltype) {
        uint32_t eax, ebx, ecx, edx, feat;
        అక్షరం sig[13] = {};

        feat = cpuid_ఆకు(CPUID_HYPERV_VENDOR_AND_MAX_FUNCTIONS, sig, false);

        ఒకవేళ (feat < CPUID_HYPERV_MIN || feat > CPUID_HYPERV_MAX)
                తిరిగి false;

        ఒకవేళ (memcmp(sig, CPUID_SIG_HYPERV, పరిమాణం(sig)) != 0)
                తిరిగి false;

        log_debug("CPUID is on hyperv");
        eax = CPUID_HYPERV_FEATURES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        ఒకవేళ (ebx & CPUID_HYPERV_ISOLATION && !(ebx & CPUID_HYPERV_CPU_MANAGEMENT)) {

                eax = CPUID_HYPERV_ISOLATION_CONFIG;
                ebx = ecx = edx = 0;
                cpuid(&eax, &ebx, &ecx, &edx);

                ఒకవేళ ((ebx & CPUID_HYPERV_ISOLATION_TYPE_ముసుగు) == isoltype)
                        తిరిగి true;
        }

        తిరిగి false;
}

స్థిర ConfidentialVirtualization detect_sev(శూన్య) {
        uint32_t eax, ebx, ecx, edx;
        uint64_t msrval;
        పూర్ణాంకం r;

        eax = CPUID_GET_HIGHEST_FUNCTION;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        ఒకవేళ (eax < CPUID_AMD_GET_ENCRYPTED_జ్ఞాపకం_CAPABILITIES)
                తిరిగి CONFIDENTIAL_VIRTUALIZATION_NONE;

        eax = CPUID_AMD_GET_ENCRYPTED_జ్ఞాపకం_CAPABILITIES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        /* bit 1 == CPU supports SEV feature
         *
         * Note, Azure blocks this CPUID leaf from its SEV-SNP
         * guests, so we must fallback to trying some HyperV
         * specific CPUID checks.
         */
        ఒకవేళ (!(eax & EAX_SEV)) {
                log_debug("No sev in CPUID, trying hyperv CPUID");

                ఒకవేళ (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_SNP))
                        తిరిగి CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;

                log_debug("No hyperv CPUID");
                తిరిగి CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        r = msr(MSR_AMD64_SEV, &msrval);
        ఒకవేళ (r < 0) {
                /* The CPU advertises SEV support and we're running under a hypervisor, but we couldn't read
                 * the SEV MSR to determine the exact mode (e.g. /dev/cpu/0/msr is unavailable because the
                 * msr module isn't loaded). Assume plain SEV. Misreporting a genuine confidential guest as
                 * non-confidential would wrongly make us trust hypervisor-provided data such as firmware credentials. */
                log_debug_errno(r, "Failed to read SEV MSR, assuming SEV: %m");
                తిరిగి CONFIDENTIAL_VIRTUALIZATION_SEV;
        }

        /* Test reverse order, since the SEV-SNP bit implies
         * the SEV-ES bit, which implies the SEV bit */
        ఒకవేళ (msrval & MSR_SEV_SNP)
                తిరిగి CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;
        ఒకవేళ (msrval & MSR_SEV_ES)
                తిరిగి CONFIDENTIAL_VIRTUALIZATION_SEV_ES;
        ఒకవేళ (msrval & MSR_SEV)
                తిరిగి CONFIDENTIAL_VIRTUALIZATION_SEV;

        తిరిగి CONFIDENTIAL_VIRTUALIZATION_NONE;
}

స్థిర ConfidentialVirtualization detect_tdx(శూన్య) {
        అక్షరం sig[13] = {};

        /* Querying an unsupported CPUID leaf is harmless (it returns the highest basic leaf's data rather
         * than faulting), so reading this leaf and matching the IntelTDX signature is sufficient. */
        cpuid_ఆకు(CPUID_INTEL_TDX_ENUMERATION, sig, true);

        ఒకవేళ (memcmp(sig, CPUID_SIG_INTEL_TDX, పరిమాణం(sig)) == 0)
                తిరిగి CONFIDENTIAL_VIRTUALIZATION_TDX;

        log_debug("No tdx in CPUID, trying hyperv CPUID");

        ఒకవేళ (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_TDX))
                తిరిగి CONFIDENTIAL_VIRTUALIZATION_TDX;

        తిరిగి CONFIDENTIAL_VIRTUALIZATION_NONE;
}

స్థిర bool detect_hypervisor(శూన్య) {
        uint32_t eax, ebx, ecx, edx;
        bool is_hv;

        eax = CPUID_PROCESSOR_INFO_AND_FEATURE_బిట్లు;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        is_hv = ecx & CPUID_FEATURE_HYPERVISOR;

        log_debug("CPUID is hypervisor: %s", yes_no(is_hv));
        తిరిగి is_hv;
}

స్థిర ConfidentialVirtualization detect_confidential_virtualization_impl(శూన్య) {
        అక్షరం sig[13] = {};

        /* Skip everything on bare metal */
        ఒకవేళ (detect_hypervisor()) {
                cpuid_ఆకు(0, sig, true);

                ఒకవేళ (memcmp(sig, CPUID_SIG_AMD, పరిమాణం(sig)) == 0)
                        తిరిగి detect_sev();
                లేకపోతే ఒకవేళ (memcmp(sig, CPUID_SIG_INTEL, పరిమాణం(sig)) == 0)
                        తిరిగి detect_tdx();
        }

        తిరిగి CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#లేదావేళ defined(__s390x__)
స్థిర ConfidentialVirtualization detect_confidential_virtualization_impl(శూన్య) {
        _cleanup_free_ అక్షరం *s = ఖాళీ;
        size_t readsize;
        పూర్ణాంకం r;

        r = read_నిండిన_virtual_file("/sys/firmware/uv/prot_virt_guest", &s, &readsize);
        ఒకవేళ (r < 0) {
                log_debug_errno(r, "Unable to read /sys/firmware/uv/prot_virt_guest: %m");
                తిరిగి CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        ఒకవేళ (readsize >= 1 && s[0] == '1')
                తిరిగి CONFIDENTIAL_VIRTUALIZATION_PROTVIRT;

        తిరిగి CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#లేదావేళ defined(__aarch64__)
స్థిర ConfidentialVirtualization detect_confidential_virtualization_impl(శూన్య) {
        పూర్ణాంకం r;

        r = RET_NERRNO(access("/sys/devices/platform/arm-cca-dev", F_OK));
        ఒకవేళ (r < 0) {
                log_debug_errno(r, "Unable to check /sys/devices/platform/arm-cca-dev: %m");
                తిరిగి CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        తిరిగి CONFIDENTIAL_VIRTUALIZATION_CCA;
}
#లేకపోతే /* ! x86_64 */
స్థిర ConfidentialVirtualization detect_confidential_virtualization_impl(శూన్య) {
        log_debug("No confidential virtualization detection on this architecture");
        తిరిగి CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#అయితే_ముగింపు /* ! x86_64 */

ConfidentialVirtualization detect_confidential_virtualization(శూన్య) {
        స్థిర thread_local ConfidentialVirtualization cached_దొరికింది = _CONFIDENTIAL_VIRTUALIZATION_INVALID;

        ఒకవేళ (cached_దొరికింది == _CONFIDENTIAL_VIRTUALIZATION_INVALID)
                cached_దొరికింది = detect_confidential_virtualization_impl();

        తిరిగి cached_దొరికింది;
}

స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి confidential_virtualization_పట్టిక[_CONFIDENTIAL_VIRTUALIZATION_MAX] = {
        [CONFIDENTIAL_VIRTUALIZATION_NONE]     = "none",
        [CONFIDENTIAL_VIRTUALIZATION_SEV]      = "sev",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_ES]   = "sev-es",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_SNP]  = "sev-snp",
        [CONFIDENTIAL_VIRTUALIZATION_TDX]      = "tdx",
        [CONFIDENTIAL_VIRTUALIZATION_PROTVIRT] = "protvirt",
        [CONFIDENTIAL_VIRTUALIZATION_CCA]      = "cca",
};

DEFINE_STRING_పట్టిక_LOOKUP(confidential_virtualization, ConfidentialVirtualization);
