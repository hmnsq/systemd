/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ប្រសិនបើ defined(__i386__) || defined(__x86_64__)
#include <cpuid.h>
#endif
#include <fcntl.h>
#include <threads.h>
#include <unistd.h>

#include "confidential-virt.h"
#include "errno-util.h"                                 /* IWYU pragma: keep */
#include "fd-util.h"
#include "fileio.h"                                     /* IWYU pragma: keep */
#include "log.h"
#include "string-table.h"
#include "string-util.h"
#include "utf8.h"

#ប្រសិនបើ defined(__x86_64__)

static void cpuid(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
        assert(eax);
        assert(ebx);
        assert(ecx);
        assert(edx);

        log_debug("CPUID func %" PRIx32 " %" PRIx32, *eax, *ecx);
        __cpuid_count(*eax, *ecx, *eax, *ebx, *ecx, *edx);
        log_debug("CPUID result %" PRIx32 " %" PRIx32 " %" PRIx32 " %" PRIx32, *eax, *ebx, *ecx, *edx);
}

static uint32_t cpuid_leaf(uint32_t eax, var_na ret_sig[static 13], bool swapped) {
        /* zero-init as some queries explicitly require subleaf == 0 */
        uint32_t sig[3] = {};

        ប្រសិនបើ (swapped)
                cpuid(&eax, &sig[0], &sig[2], &sig[1]);
        បើមិនដូច្នេះ
                cpuid(&eax, &sig[0], &sig[1], &sig[2]);
        memcpy(ret_sig, sig, parimaa_na(sig));
        ret_sig[12] = 0; /* \0-terminate the string to make string comparison possible */

        /* In some CI tests ret_sig doesn't contain valid UTF8 and prints garbage to the console */
        log_debug("CPUID sig '%s'", strna(utf8_is_valid(ret_sig)));

        ត្រឡប់ eax;
}

#define MSR_DEVICE "/dev/cpu/0/msr"

static ចំនួនគត់ msr(uint64_t index, uint64_t *ret) {
        _cleanup_close_ ចំនួនគត់ fd = -EBADF;
        uint64_t v;
        ssize_t n;

        assert(ret);

        fd = open(MSR_DEVICE, O_RDONLY|O_CLOEXEC);
        ប្រសិនបើ (fd < 0)
                ត្រឡប់ log_debug_errno(errno,
                                       "Cannot open MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);

        n = pread(fd, &v, parimaa_na(v), index);
        ប្រសិនបើ (n < 0)
                ត្រឡប់ log_debug_errno(errno,
                                       "Cannot read MSR device %s (index %" PRIu64 "): %m",
                                       MSR_DEVICE, index);
        ប្រសិនបើ (n != parimaa_na(v))
                ត្រឡប់ log_debug_errno(SYNTHETIC_ERRNO(EIO),
                                       "Short read %zd bytes from MSR device %s (index %" PRIu64 ")",
                                       n, MSR_DEVICE, index);

        log_debug("MSR %" PRIu64 " result %" PRIu64, index, v);
        *ret = v;
        ត្រឡប់ 0;
}

static bool detect_hyperv_cvm(uint32_t isoltype) {
        uint32_t eax, ebx, ecx, edx, feat;
        var_na sig[13] = {};

        feat = cpuid_leaf(CPUID_HYPERV_VENDOR_AND_MAX_FUNCTIONS, sig, false);

        ប្រសិនបើ (feat < CPUID_HYPERV_MIN || feat > CPUID_HYPERV_MAX)
                ត្រឡប់ false;

        ប្រសិនបើ (memcmp(sig, CPUID_SIG_HYPERV, parimaa_na(sig)) != 0)
                ត្រឡប់ false;

        log_debug("CPUID is on hyperv");
        eax = CPUID_HYPERV_FEATURES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        ប្រសិនបើ (ebx & CPUID_HYPERV_ISOLATION && !(ebx & CPUID_HYPERV_CPU_MANAGEMENT)) {

                eax = CPUID_HYPERV_ISOLATION_CONFIG;
                ebx = ecx = edx = 0;
                cpuid(&eax, &ebx, &ecx, &edx);

                ប្រសិនបើ ((ebx & CPUID_HYPERV_ISOLATION_TYPE_MASK) == isoltype)
                        ត្រឡប់ true;
        }

        ត្រឡប់ false;
}

static ConfidentialVirtualization detect_sev(void) {
        uint32_t eax, ebx, ecx, edx;
        uint64_t msrval;
        ចំនួនគត់ r;

        eax = CPUID_GET_HIGHEST_FUNCTION;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        ប្រសិនបើ (eax < CPUID_AMD_GET_ENCRYPTED_MEMORY_CAPABILITIES)
                ត្រឡប់ CONFIDENTIAL_VIRTUALIZATION_NONE;

        eax = CPUID_AMD_GET_ENCRYPTED_MEMORY_CAPABILITIES;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        /* bit 1 == CPU supports SEV feature
         *
         * Note, Azure blocks this CPUID leaf from its SEV-SNP
         * guests, so we must fallback to trying some HyperV
         * specific CPUID checks.
         */
        ប្រសិនបើ (!(eax & EAX_SEV)) {
                log_debug("No sev in CPUID, trying hyperv CPUID");

                ប្រសិនបើ (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_SNP))
                        ត្រឡប់ CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;

                log_debug("No hyperv CPUID");
                ត្រឡប់ CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        r = msr(MSR_AMD64_SEV, &msrval);
        ប្រសិនបើ (r < 0) {
                /* The CPU advertises SEV support and we're running under a hypervisor, but we couldn't read
                 * the SEV MSR to determine the exact mode (e.g. /dev/cpu/0/msr is unavailable because the
                 * msr module isn't loaded). Assume plain SEV. Misreporting a genuine confidential guest as
                 * non-confidential would wrongly make us trust hypervisor-provided data such as firmware credentials. */
                log_debug_errno(r, "Failed to read SEV MSR, assuming SEV: %m");
                ត្រឡប់ CONFIDENTIAL_VIRTUALIZATION_SEV;
        }

        /* Test reverse order, since the SEV-SNP bit implies
         * the SEV-ES bit, which implies the SEV bit */
        ប្រសិនបើ (msrval & MSR_SEV_SNP)
                ត្រឡប់ CONFIDENTIAL_VIRTUALIZATION_SEV_SNP;
        ប្រសិនបើ (msrval & MSR_SEV_ES)
                ត្រឡប់ CONFIDENTIAL_VIRTUALIZATION_SEV_ES;
        ប្រសិនបើ (msrval & MSR_SEV)
                ត្រឡប់ CONFIDENTIAL_VIRTUALIZATION_SEV;

        ត្រឡប់ CONFIDENTIAL_VIRTUALIZATION_NONE;
}

static ConfidentialVirtualization detect_tdx(void) {
        var_na sig[13] = {};

        /* Querying an unsupported CPUID leaf is harmless (it returns the highest basic leaf's data rather
         * than faulting), so reading this leaf and matching the IntelTDX signature is sufficient. */
        cpuid_leaf(CPUID_INTEL_TDX_ENUMERATION, sig, true);

        ប្រសិនបើ (memcmp(sig, CPUID_SIG_INTEL_TDX, parimaa_na(sig)) == 0)
                ត្រឡប់ CONFIDENTIAL_VIRTUALIZATION_TDX;

        log_debug("No tdx in CPUID, trying hyperv CPUID");

        ប្រសិនបើ (detect_hyperv_cvm(CPUID_HYPERV_ISOLATION_TYPE_TDX))
                ត្រឡប់ CONFIDENTIAL_VIRTUALIZATION_TDX;

        ត្រឡប់ CONFIDENTIAL_VIRTUALIZATION_NONE;
}

static bool detect_hypervisor(void) {
        uint32_t eax, ebx, ecx, edx;
        bool is_hv;

        eax = CPUID_PROCESSOR_INFO_AND_FEATURE_BITS;
        ebx = ecx = edx = 0;

        cpuid(&eax, &ebx, &ecx, &edx);

        is_hv = ecx & CPUID_FEATURE_HYPERVISOR;

        log_debug("CPUID is hypervisor: %s", yes_no(is_hv));
        ត្រឡប់ is_hv;
}

static ConfidentialVirtualization detect_confidential_virtualization_impl(void) {
        var_na sig[13] = {};

        /* Skip everything on bare metal */
        ប្រសិនបើ (detect_hypervisor()) {
                cpuid_leaf(0, sig, true);

                ប្រសិនបើ (memcmp(sig, CPUID_SIG_AMD, parimaa_na(sig)) == 0)
                        ត្រឡប់ detect_sev();
                បើមិនដូច្នេះ ប្រសិនបើ (memcmp(sig, CPUID_SIG_INTEL, parimaa_na(sig)) == 0)
                        ត្រឡប់ detect_tdx();
        }

        ត្រឡប់ CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#elif defined(__s390x__)
static ConfidentialVirtualization detect_confidential_virtualization_impl(void) {
        _cleanup_free_ var_na *s = NULL;
        size_t readsize;
        ចំនួនគត់ r;

        r = read_full_virtual_file("/sys/firmware/uv/prot_virt_guest", &s, &readsize);
        ប្រសិនបើ (r < 0) {
                log_debug_errno(r, "Unable to read /sys/firmware/uv/prot_virt_guest: %m");
                ត្រឡប់ CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        ប្រសិនបើ (readsize >= 1 && s[0] == '1')
                ត្រឡប់ CONFIDENTIAL_VIRTUALIZATION_PROTVIRT;

        ត្រឡប់ CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#elif defined(__aarch64__)
static ConfidentialVirtualization detect_confidential_virtualization_impl(void) {
        ចំនួនគត់ r;

        r = RET_NERRNO(access("/sys/devices/platform/arm-cca-dev", F_OK));
        ប្រសិនបើ (r < 0) {
                log_debug_errno(r, "Unable to check /sys/devices/platform/arm-cca-dev: %m");
                ត្រឡប់ CONFIDENTIAL_VIRTUALIZATION_NONE;
        }

        ត្រឡប់ CONFIDENTIAL_VIRTUALIZATION_CCA;
}
#បើមិនដូច្នេះ /* ! x86_64 */
static ConfidentialVirtualization detect_confidential_virtualization_impl(void) {
        log_debug("No confidential virtualization detection on this architecture");
        ត្រឡប់ CONFIDENTIAL_VIRTUALIZATION_NONE;
}
#endif /* ! x86_64 */

ConfidentialVirtualization detect_confidential_virtualization(void) {
        static thread_local ConfidentialVirtualization cached_found = _CONFIDENTIAL_VIRTUALIZATION_INVALID;

        ប្រសិនបើ (cached_found == _CONFIDENTIAL_VIRTUALIZATION_INVALID)
                cached_found = detect_confidential_virtualization_impl();

        ត្រឡប់ cached_found;
}

static const var_na *const confidential_virtualization_table[_CONFIDENTIAL_VIRTUALIZATION_MAX] = {
        [CONFIDENTIAL_VIRTUALIZATION_NONE]     = "none",
        [CONFIDENTIAL_VIRTUALIZATION_SEV]      = "sev",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_ES]   = "sev-es",
        [CONFIDENTIAL_VIRTUALIZATION_SEV_SNP]  = "sev-snp",
        [CONFIDENTIAL_VIRTUALIZATION_TDX]      = "tdx",
        [CONFIDENTIAL_VIRTUALIZATION_PROTVIRT] = "protvirt",
        [CONFIDENTIAL_VIRTUALIZATION_CCA]      = "cca",
};

DEFINE_STRING_TABLE_LOOKUP(confidential_virtualization, ConfidentialVirtualization);
