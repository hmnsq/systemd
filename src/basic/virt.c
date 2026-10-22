/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਜੇ defined(__i386__) || defined(__x86_64__)
#ਅਨ੍ਤਰ੍ਭਾਵ <cpuid.h>
#ਜੇ_ਅੰਤ
#ਜੇ defined(__aarch64__)
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/auxv.h>
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <threads.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "dirent-util.h"        /* IWYU pragma: keep */
#ਅਨ੍ਤਰ੍ਭਾਵ "env-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "extract-word.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "namespace-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "process-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-table.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "virt.h"

ਗਿਣਤੀ {
      SMBIOS_VM_ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੈੱਟ,
      SMBIOS_VM_ਦੋਆਧਾਰੀ_ਅੰਕ_UNSET,
      SMBIOS_VM_ਦੋਆਧਾਰੀ_ਅੰਕ_UNKNOWN,
};

ਸਥਿਰ Virtualization detect_vm_cpuid(ਖਾਲੀ) {

        /* CPUID is an x86 specific interface. */
#ਜੇ defined(__i386__) || defined(__x86_64__)

        ਸਥਿਰ ਅਚਲ ਬਣਤਰ {
                ਅਚਲ ਅੱਖਰ sig[13];
                Virtualization id;
        } vm_ਸਾਰਣੀ[] = {
                { "XenVMMXenVMM", VIRTUALIZATION_XEN       },
                { "KVMKVMKVM",    VIRTUALIZATION_KVM       }, /* qemu with KVM */
                { "Linux KVM Hv", VIRTUALIZATION_KVM       }, /* qemu with KVM + HyperV Enlightenments */
                { "TCGTCGTCGTCG", VIRTUALIZATION_QEMU      }, /* qemu without KVM */
                /* http://kb.vmware.com/selfservice/microsites/search.do?language=en_US&cmd=displayKC&externalId=1009458 */
                { "VMwareVMware", VIRTUALIZATION_VMWARE    },
                /* https://docs.microsoft.com/en-us/virtualization/hyper-v-on-windows/reference/tlfs */
                { "Microsoft Hv", VIRTUALIZATION_MICROSOFT },
                /* https://wiki.freebsd.org/bhyve */
                { "bhyve bhyve ", VIRTUALIZATION_BHYVE     },
                { "QNXQVMBSQG",   VIRTUALIZATION_QNX       },
                /* https://projectacrn.org */
                { "ACRNACRNACRN", VIRTUALIZATION_ACRN      },
                /* https://www.lockheedmartin.com/en-us/products/Hardened-Security-for-Intel-Processors.html */
                { "SRESRESRESRE", VIRTUALIZATION_SRE       },
                { "Apple VZ",     VIRTUALIZATION_APPLE     },
        };

        uint32_t eax, ebx, ecx, edx;
        bool hypervisor;

        /* http://lwn.net/Articles/301888/ */

        /* First detect whether there is a hypervisor */
        ਜੇ (__get_cpuid(1, &eax, &ebx, &ecx, &edx) == 0)
                ਵਾਪਸ VIRTUALIZATION_NONE;

        hypervisor = ecx & 0x80000000U;

        ਜੇ (hypervisor) {
                ਸੰਘ {
                        uint32_t sig32[3];
                        ਅੱਖਰ ਪਾਠ[13];
                } sig = {};

                /* There is a hypervisor, see what it is */
                __cpuid(0x40000000U, eax, ebx, ecx, edx);

                sig.sig32[0] = ebx;
                sig.sig32[1] = ecx;
                sig.sig32[2] = edx;

                log_debug("Virtualization found, CPUID=%s", sig.ਪਾਠ);

                FOREACH_ELEMENT(vm, vm_ਸਾਰਣੀ)
                        ਜੇ (memcmp_nn(sig.ਪਾਠ, ਆਕਾਰ(sig.ਪਾਠ),
                                      vm->sig, ਆਕਾਰ(vm->sig)) == 0)
                                ਵਾਪਸ vm->id;

                log_debug("Unknown virtualization with CPUID=%s. Add to vm_table[]?", sig.ਪਾਠ);
                ਵਾਪਸ VIRTUALIZATION_VM_OTHER;
        }
#ਜੇ_ਅੰਤ
        log_debug("No virtualization found in CPUID");

        ਵਾਪਸ VIRTUALIZATION_NONE;
}

ਸਥਿਰ Virtualization detect_vm_device_ਰੁੱਖ(ਖਾਲੀ) {
#ਜੇ defined(__arm__) || defined(__aarch64__) || defined(__powerpc__) || defined(__powerpc64__) || defined(__riscv)
        _cleanup_free_ ਅੱਖਰ *hvtype = ਨਿਰਾ;
        ਪੂਰਨ r;

        r = read_one_ਸਤਰ_file("/proc/device-tree/hypervisor/compatible", &hvtype);
        ਜੇ (r == -ENOENT) {
                ਜੇ (access("/proc/device-tree/ibm,partition-name", F_OK) == 0 &&
                    access("/proc/device-tree/hmc-managed?", F_OK) == 0 &&
                    access("/proc/device-tree/chosen/qemu,graphic-width", F_OK) != 0)
                        ਵਾਪਸ VIRTUALIZATION_POWERVM;

                _cleanup_closedir_ DIR *dir = opendir("/proc/device-tree");
                ਜੇ (!dir) {
                        ਜੇ (errno == ENOENT) {
                                log_debug_errno(errno, "/proc/device-tree/ does not exist");
                                ਵਾਪਸ VIRTUALIZATION_NONE;
                        }
                        ਵਾਪਸ log_debug_errno(errno, "Opening /proc/device-tree/ failed: %m");
                }

                FOREACH_DIRENT(de, dir, ਵਾਪਸ log_debug_errno(errno, "Failed to enumerate /proc/device-tree/ contents: %m"))
                        ਜੇ (ਲੜੀ_ਖੋਜ(de->d_name, "fw-cfg")) {
                                log_debug("Virtualization QEMU: \"fw-cfg\" present in /proc/device-tree/%s", de->d_name);
                                ਵਾਪਸ VIRTUALIZATION_QEMU;
                        }

                _cleanup_free_ ਅੱਖਰ *compat = ਨਿਰਾ;
                r = read_one_ਸਤਰ_file("/proc/device-tree/compatible", &compat);
                ਜੇ (r < 0 && r != -ENOENT)
                        ਵਾਪਸ log_debug_errno(r, "Failed to read /proc/device-tree/compatible: %m");
                ਜੇ (r >= 0) {
                        ਜੇ (streq(compat, "qemu,pseries")) {
                                log_debug("Virtualization %s found in /proc/device-tree/compatible", compat);
                                ਵਾਪਸ VIRTUALIZATION_QEMU;
                        }
                        ਜੇ (streq(compat, "linux,dummy-virt")) {
                                /* https://www.kernel.org/doc/Documentation/devicetree/bindings/arm/linux%2Cdummy-virt.yaml */
                                log_debug("Generic virtualization %s found in /proc/device-tree/compatible", compat);
                                ਵਾਪਸ VIRTUALIZATION_VM_OTHER;
                        }
                }

                log_debug("No virtualization found in /proc/device-tree/*");
                ਵਾਪਸ VIRTUALIZATION_NONE;
        } ਨਹੀਂਤਾਂ ਜੇ (r < 0)
                ਵਾਪਸ r;

        log_debug("Virtualization %s found in /proc/device-tree/hypervisor/compatible", hvtype);
        ਜੇ (streq(hvtype, "linux,kvm"))
                ਵਾਪਸ VIRTUALIZATION_KVM;
        ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀ_ਖੋਜ(hvtype, "xen"))
                ਵਾਪਸ VIRTUALIZATION_XEN;
        ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀ_ਖੋਜ(hvtype, "vmware"))
                ਵਾਪਸ VIRTUALIZATION_VMWARE;
        ਨਹੀਂਤਾਂ
                ਵਾਪਸ VIRTUALIZATION_VM_OTHER;
#ਨਹੀਂਤਾਂ
        log_debug("This platform does not support /proc/device-tree");
        ਵਾਪਸ VIRTUALIZATION_NONE;
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(__i386__) || defined(__x86_64__) || defined(__arm__) || defined(__aarch64__) || defined(__loongarch_lp64) || defined(__riscv)
ਸਥਿਰ Virtualization detect_vm_dmi_vendor(ਖਾਲੀ) {
        ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ dmi_vendors[] = {
                "/sys/class/dmi/id/product_name", /* Test this before sys_vendor to detect KVM over QEMU */
                "/sys/class/dmi/id/sys_vendor",
                "/sys/class/dmi/id/board_vendor",
                "/sys/class/dmi/id/bios_vendor",
                "/sys/class/dmi/id/product_version", /* For Hyper-V VMs test */
                ਨਿਰਾ
        };

        ਸਥਿਰ ਅਚਲ ਬਣਤਰ {
                ਅਚਲ ਅੱਖਰ *vendor;
                Virtualization id;
        } dmi_vendor_ਸਾਰਣੀ[] = {
                { "KVM",                   VIRTUALIZATION_KVM       },
                { "OpenStack",             VIRTUALIZATION_KVM       }, /* Detect OpenStack instance as KVM in non x86 architecture */
                { "KubeVirt",              VIRTUALIZATION_KVM       }, /* Detect KubeVirt instance as KVM in non x86 architecture */
                { "Alibaba Cloud ECS",     VIRTUALIZATION_KVM       }, /* narrower than bare "Alibaba Cloud" sys_vendor */
                { "Amazon EC2",            VIRTUALIZATION_AMAZON    },
                { "QEMU",                  VIRTUALIZATION_QEMU      },
                { "VMware",                VIRTUALIZATION_VMWARE    }, /* https://kb.vmware.com/s/article/1009458 */
                { "VMW",                   VIRTUALIZATION_VMWARE    },
                { "innotek GmbH",          VIRTUALIZATION_ORACLE    },
                { "VirtualBox",            VIRTUALIZATION_ORACLE    },
                { "Oracle Corporation",    VIRTUALIZATION_ORACLE    }, /* Detect VirtualBox on some proprietary systems via the board_vendor */
                { "Xen",                   VIRTUALIZATION_XEN       },
                { "Bochs",                 VIRTUALIZATION_BOCHS     },
                { "Parallels",             VIRTUALIZATION_PARALLELS },
                /* https://wiki.freebsd.org/bhyve */
                { "BHYVE",                 VIRTUALIZATION_BHYVE     },
                { "Hyper-V",               VIRTUALIZATION_MICROSOFT },
                { "Apple Virtualization",  VIRTUALIZATION_APPLE     },
                { "Google Compute Engine", VIRTUALIZATION_GOOGLE    }, /* https://cloud.google.com/run/docs/container-contract#sandbox */
        };
        ਪੂਰਨ r;

        STRV_FOREACH(vendor, dmi_vendors) {
                _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;

                r = read_one_ਸਤਰ_file(*vendor, &s);
                ਜੇ (r == -ENOENT)
                        ਜਾਰੀ;
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                FOREACH_ELEMENT(dmi_vendor, dmi_vendor_ਸਾਰਣੀ)
                        ਜੇ (startswith(s, dmi_vendor->vendor)) {
                                log_debug("Virtualization %s found in DMI (%s)", s, *vendor);
                                ਵਾਪਸ dmi_vendor->id;
                        }
        }
        log_debug("No virtualization found in DMI vendor table.");
        ਵਾਪਸ VIRTUALIZATION_NONE;
}

ਸਥਿਰ ਪੂਰਨ detect_vm_smbios(ਖਾਲੀ) {
        /* The SMBIOS BIOS Characteristics Extension Byte 2 (Section 2.1.2.2 of
         * https://www.dmtf.org/sites/default/files/standards/documents/DSP0134_3.4.0.pdf), specifies that
         * the 4th bit being set indicates a VM. The BIOS Characteristics table is exposed via the kernel in
         * /sys/firmware/dmi/entries/0-0. Note that in the general case, this bit being unset should not
         * imply that the system is running on bare-metal.  For example, QEMU 3.1.0 (with or without KVM)
         * with SeaBIOS does not set this bit. */
        _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;
        size_t readsize;
        ਪੂਰਨ r;

        r = read_ਭਰਿਆ_virtual_file("/sys/firmware/dmi/entries/0-0/raw", &s, &readsize);
        ਜੇ (r < 0) {
                log_debug_errno(r, "Unable to read /sys/firmware/dmi/entries/0-0/raw, "
                                "using the virtualization information found in DMI vendor table, ignoring: %m");
                ਵਾਪਸ SMBIOS_VM_ਦੋਆਧਾਰੀ_ਅੰਕ_UNKNOWN;
        }
        ਜੇ (readsize < 20 || s[1] < 20) {
                /* The spec indicates that byte 1 contains the size of the table, 0x12 + the number of
                 * extension bytes. The data we're interested in is in extension byte 2, which would be at
                 * 0x13. If we didn't read that much data, or if the BIOS indicates that we don't have that
                 * much data, we don't infer anything from the SMBIOS. */
                log_debug("Only read %zu bytes from /sys/firmware/dmi/entries/0-0/raw (expected 20). "
                          "Using the virtualization information found in DMI vendor table.", readsize);
                ਵਾਪਸ SMBIOS_VM_ਦੋਆਧਾਰੀ_ਅੰਕ_UNKNOWN;
        }

        uint8_t byte = (uint8_t) s[19];
        ਜੇ (byte & (1U<<4)) {
                log_debug("DMI BIOS Extension table indicates virtualization.");
                ਵਾਪਸ SMBIOS_VM_ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੈੱਟ;
        }
        log_debug("DMI BIOS Extension table does not indicate virtualization.");
        ਵਾਪਸ SMBIOS_VM_ਦੋਆਧਾਰੀ_ਅੰਕ_UNSET;
}
#ਜੇ_ਅੰਤ /* defined(__i386__) || defined(__x86_64__) || defined(__arm__) || defined(__aarch64__) || defined(__loongarch_lp64) */

ਸਥਿਰ Virtualization detect_vm_dmi(ਖਾਲੀ) {
#ਜੇ defined(__i386__) || defined(__x86_64__) || defined(__arm__) || defined(__aarch64__) || defined(__loongarch_lp64) || defined(__riscv)

        ਪੂਰਨ r;
        r = detect_vm_dmi_vendor();

        /* The DMI vendor tables in /sys/class/dmi/id don't help us distinguish between Amazon EC2
         * virtual machines and bare-metal instances, so we need to look at SMBIOS. */
        ਜੇ (r == VIRTUALIZATION_AMAZON) {
                ਚੋਣ (detect_vm_smbios()) {
                ਹਾਲਤ SMBIOS_VM_ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੈੱਟ:
                        ਵਾਪਸ VIRTUALIZATION_AMAZON;
                ਹਾਲਤ SMBIOS_VM_ਦੋਆਧਾਰੀ_ਅੰਕ_UNSET:
                        ਵਾਪਸ VIRTUALIZATION_NONE;
                ਹਾਲਤ SMBIOS_VM_ਦੋਆਧਾਰੀ_ਅੰਕ_UNKNOWN: {
                        /* The DMI information we are after is only accessible to the root user,
                         * so we fallback to using the product name which is less restricted
                         * to distinguish metal systems from virtualized instances */
                        _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;
                        ਅਚਲ ਅੱਖਰ *e;

                        r = read_ਭਰਿਆ_virtual_file("/sys/class/dmi/id/product_name", &s, ਨਿਰਾ);
                        /* In EC2, virtualized is much more common than metal, so if for some reason
                         * we fail to read the DMI data, assume we are virtualized. */
                        ਜੇ (r < 0) {
                                log_debug_errno(r, "Can't read /sys/class/dmi/id/product_name,"
                                                " assuming virtualized: %m");
                                ਵਾਪਸ VIRTUALIZATION_AMAZON;
                        }
                        e = strstrafter(truncate_nl(s), ".metal");
                        ਜੇ (e && ਵਿੱਚ_ਸੈੱਟ(*e, 0, '-')) {
                                log_debug("DMI product name has '.metal', assuming no virtualization");
                                ਵਾਪਸ VIRTUALIZATION_NONE;
                        } ਨਹੀਂਤਾਂ
                                ਵਾਪਸ VIRTUALIZATION_AMAZON;
                }
                ਮੂਲ:
                        assert_not_reached();
              }
        }

        /* If we haven't identified a VM, but the firmware indicates that there is one, indicate as much. We
         * have no further information about what it is. */
        ਜੇ (r == VIRTUALIZATION_NONE && detect_vm_smbios() == SMBIOS_VM_ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੈੱਟ)
                ਵਾਪਸ VIRTUALIZATION_VM_OTHER;
        ਵਾਪਸ r;
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ VIRTUALIZATION_NONE;
#ਜੇ_ਅੰਤ
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XENFEAT_dom0 11 /* xen/include/public/features.h */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਰਸਤਾ_FEATURES "/sys/hypervisor/properties/features"
/* Returns -errno, or 0 for domU, or 1 for dom0 */
ਸਥਿਰ ਪੂਰਨ detect_vm_xen_dom0(ਖਾਲੀ) {
        _cleanup_free_ ਅੱਖਰ *domcap = ਨਿਰਾ;
        ਪੂਰਨ r;

        r = read_one_ਸਤਰ_file(ਰਸਤਾ_FEATURES, &domcap);
        ਜੇ (r < 0 && r != -ENOENT)
                ਵਾਪਸ r;
        ਜੇ (r >= 0) {
                ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ features;

                /* Here, we need to use sscanf() instead of safe_atoul()
                 * as the string lacks the leading "0x". */
                r = ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(domcap, "%lx", &features);
                ਜੇ (r == 1) {
                        r = !!(features & (1U << XENFEAT_dom0));
                        log_debug("Virtualization XEN, found %s with value %08lx, "
                                  "XENFEAT_dom0 (indicating the 'hardware domain') is%s set.",
                                  ਰਸਤਾ_FEATURES, features, r ? "" : " not");
                        ਵਾਪਸ r;
                }
                log_debug("Virtualization XEN, found %s, unhandled content '%s'",
                          ਰਸਤਾ_FEATURES, domcap);
        }

        r = read_one_ਸਤਰ_file("/proc/xen/capabilities", &domcap);
        ਜੇ (r == -ENOENT) {
                log_debug("Virtualization XEN because /proc/xen/capabilities does not exist");
                ਵਾਪਸ 0;
        }
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਲਈ (ਅਚਲ ਅੱਖਰ *i = domcap;;) {
                _cleanup_free_ ਅੱਖਰ *cap = ਨਿਰਾ;

                r = ਕੱਢੋ_ਪਹਿਲਾ_ਸ਼ਬਦ(&i, &cap, ",", 0);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0) {
                        log_debug("Virtualization XEN DomU found (/proc/xen/capabilities)");
                        ਵਾਪਸ 0;
                }

                ਜੇ (streq(cap, "control_d")) {
                        log_debug("Virtualization XEN Dom0 ignored (/proc/xen/capabilities)");
                        ਵਾਪਸ 1;
                }
        }
}

ਸਥਿਰ Virtualization detect_vm_xen(ਖਾਲੀ) {
        /* The presence of /proc/xen indicates some form of a Xen domain
           The check for Dom0 is handled outside this function */
        ਜੇ (access("/proc/xen", F_OK) < 0) {
                log_debug("Virtualization XEN not found, /proc/xen does not exist");
                ਵਾਪਸ VIRTUALIZATION_NONE;
        }
        log_debug("Virtualization XEN found (/proc/xen exists)");
        ਵਾਪਸ VIRTUALIZATION_XEN;
}

ਸਥਿਰ Virtualization detect_vm_hypervisor(ਖਾਲੀ) {
        _cleanup_free_ ਅੱਖਰ *hvtype = ਨਿਰਾ;
        ਪੂਰਨ r;

        r = read_one_ਸਤਰ_file("/sys/hypervisor/type", &hvtype);
        ਜੇ (r == -ENOENT)
                ਵਾਪਸ VIRTUALIZATION_NONE;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        log_debug("Virtualization %s found in /sys/hypervisor/type", hvtype);

        ਜੇ (streq(hvtype, "xen"))
                ਵਾਪਸ VIRTUALIZATION_XEN;
        ਨਹੀਂਤਾਂ
                ਵਾਪਸ VIRTUALIZATION_VM_OTHER;
}

ਸਥਿਰ Virtualization detect_vm_uml(ਖਾਲੀ) {
        _cleanup_fclose_ FILE *f = ਨਿਰਾ;
        ਪੂਰਨ r;

        /* Detect User-Mode Linux by reading /proc/cpuinfo */
        f = fopen("/proc/cpuinfo", "re");
        ਜੇ (!f) {
                ਜੇ (errno == ENOENT) {
                        log_debug("/proc/cpuinfo not found, assuming no UML virtualization.");
                        ਵਾਪਸ VIRTUALIZATION_NONE;
                }
                ਵਾਪਸ -errno;
        }

        ਲਈ (;;) {
                _cleanup_free_ ਅੱਖਰ *ਸਤਰ = ਨਿਰਾ;
                ਅਚਲ ਅੱਖਰ *t;

                r = read_ਸਤਰ(f, LONG_ਸਤਰ_MAX, &ਸਤਰ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        ਰੋਕੋ;

                t = startswith(ਸਤਰ, "vendor_id\t: ");
                ਜੇ (t) {
                        ਜੇ (startswith(t, "User Mode Linux")) {
                                log_debug("UML virtualization found in /proc/cpuinfo");
                                ਵਾਪਸ VIRTUALIZATION_UML;
                        }

                        ਰੋਕੋ;
                }
        }

        log_debug("UML virtualization not found in /proc/cpuinfo.");
        ਵਾਪਸ VIRTUALIZATION_NONE;
}

ਸਥਿਰ Virtualization detect_vm_zvm(ਖਾਲੀ) {

#ਜੇ defined(__s390__)
        _cleanup_free_ ਅੱਖਰ *t = ਨਿਰਾ;
        ਪੂਰਨ r;

        r = get_proc_ਖੇਤਰ("/proc/sysinfo", "VM00 Control Program", &t);
        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(r, -ENOENT, -ENODATA))
                ਵਾਪਸ VIRTUALIZATION_NONE;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        log_debug("Virtualization %s found in /proc/sysinfo", t);
        ਜੇ (streq(t, "z/VM"))
                ਵਾਪਸ VIRTUALIZATION_ZVM;
        ਨਹੀਂਤਾਂ
                ਵਾਪਸ VIRTUALIZATION_KVM;
#ਨਹੀਂਤਾਂ
        log_debug("This platform does not support /proc/sysinfo");
        ਵਾਪਸ VIRTUALIZATION_NONE;
#ਜੇ_ਅੰਤ
}

/* Returns a short identifier for the various VM implementations */
Virtualization detect_vm(ਖਾਲੀ) {
        ਸਥਿਰ thread_local Virtualization cached_ਮਿਲਿਆ = _VIRTUALIZATION_INVALID;
        bool other = false, hyperv = false;
        ਪੂਰਨ xen_dom0 = 0;
        Virtualization v, dmi;

        ਜੇ (cached_ਮਿਲਿਆ >= 0)
                ਵਾਪਸ cached_ਮਿਲਿਆ;

        /* We have to use the correct order here:
         *
         * → First, try to detect Oracle Virtualbox, Amazon EC2 Nitro, Parallels, and Google Compute Engine,
         *   even if they use KVM, as well as Xen, even if it cloaks as Microsoft Hyper-V. Attempt to detect
         *   UML at this stage too, since it runs as a user-process nested inside other VMs. Also check for
         *   Xen now, because Xen PV mode does not override CPUID when nested inside another hypervisor.
         *
         * → Second, try to detect from CPUID. This will report KVM for whatever software is used even if
         *   info in DMI is overwritten.
         *
         * → Third, try to detect from DMI. */

        dmi = detect_vm_dmi();
        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(dmi,
                   VIRTUALIZATION_ORACLE,
                   VIRTUALIZATION_XEN,
                   VIRTUALIZATION_AMAZON,
                   /* Unable to distinguish a GCE machine from a VM to bare-metal
                    * for non-x86 architectures due to its necessity for cpuid
                    * detection, which functions solely on x86 platforms. Report
                    * as a VM for other architectures. */
#ਜੇ !defined(__i386__) && !defined(__x86_64__)
                   VIRTUALIZATION_GOOGLE,
#ਜੇ_ਅੰਤ
                   VIRTUALIZATION_PARALLELS)) {
                v = dmi;
                ਜਾਓ ਅੰਤ;
        }

        /* Detect UML */
        v = detect_vm_uml();
        ਜੇ (v < 0)
                ਵਾਪਸ v;
        ਜੇ (v != VIRTUALIZATION_NONE)
                ਜਾਓ ਅੰਤ;

        /* Detect Xen */
        v = detect_vm_xen();
        ਜੇ (v < 0)
                ਵਾਪਸ v;
        ਜੇ (v == VIRTUALIZATION_XEN) {
                 /* If we are Dom0, then we expect to not report as a VM. However, as we might be nested
                  * inside another hypervisor which can be detected via the CPUID check, wait to report this
                  * until after the CPUID check. */
                xen_dom0 = detect_vm_xen_dom0();
                ਜੇ (xen_dom0 < 0)
                        ਵਾਪਸ xen_dom0;
                ਜੇ (xen_dom0 == 0)
                        ਜਾਓ ਅੰਤ;
        } ਨਹੀਂਤਾਂ ਜੇ (v != VIRTUALIZATION_NONE)
                assert_not_reached();

        /* Detect from CPUID */
        v = detect_vm_cpuid();
        ਜੇ (v < 0)
                ਵਾਪਸ v;
        ਜੇ (v == VIRTUALIZATION_MICROSOFT)
                /* QEMU sets the CPUID string to hyperv's, in case it provides hyperv enlightenments. Let's
                 * hence not return Microsoft here but just use the other mechanisms first to make a better
                 * decision. */
                hyperv = true;
        ਨਹੀਂਤਾਂ ਜੇ (v == VIRTUALIZATION_VM_OTHER)
                other = true;
        ਨਹੀਂਤਾਂ ਜੇ (v == VIRTUALIZATION_KVM && dmi == VIRTUALIZATION_GOOGLE)
                /* The DMI vendor tables in /sys/class/dmi/id don't help us distinguish between GCE
                 * virtual machines and bare-metal instances, so we need to look at hypervisor. */
                ਵਾਪਸ VIRTUALIZATION_GOOGLE;
        ਨਹੀਂਤਾਂ ਜੇ (v != VIRTUALIZATION_NONE)
                ਜਾਓ ਅੰਤ;

        /* If we are in Dom0 and have not yet finished, finish with the result of detect_vm_cpuid */
        ਜੇ (xen_dom0 > 0)
                ਜਾਓ ਅੰਤ;

        /* Now, let's get back to DMI */
        ਜੇ (dmi < 0)
                ਵਾਪਸ dmi;
        ਜੇ (dmi == VIRTUALIZATION_VM_OTHER)
                other = true;
        ਨਹੀਂਤਾਂ ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(dmi, VIRTUALIZATION_NONE, VIRTUALIZATION_GOOGLE)) {
                /* At this point if GCE has been detected in dmi, do not report as a VM. It should
                 * be a bare-metal machine */
                v = dmi;
                ਜਾਓ ਅੰਤ;
        }

        /* Check high-level hypervisor sysfs file */
        v = detect_vm_hypervisor();
        ਜੇ (v < 0)
                ਵਾਪਸ v;
        ਜੇ (v == VIRTUALIZATION_VM_OTHER)
                other = true;
        ਨਹੀਂਤਾਂ ਜੇ (v != VIRTUALIZATION_NONE)
                ਜਾਓ ਅੰਤ;

        v = detect_vm_device_ਰੁੱਖ();
        ਜੇ (v < 0)
                ਵਾਪਸ v;
        ਜੇ (v == VIRTUALIZATION_VM_OTHER)
                other = true;
        ਨਹੀਂਤਾਂ ਜੇ (v != VIRTUALIZATION_NONE)
                ਜਾਓ ਅੰਤ;

        v = detect_vm_zvm();
        ਜੇ (v < 0)
                ਵਾਪਸ v;

ਅੰਤ:
        /* None of the checks above gave us a clear answer, hence let's now use fallback logic: if hyperv
         * enlightenments are available but the VMM wasn't recognized as anything yet, it's probably
         * Microsoft. */
        ਜੇ (v == VIRTUALIZATION_NONE) {
                ਜੇ (hyperv)
                        v = VIRTUALIZATION_MICROSOFT;
                ਨਹੀਂਤਾਂ ਜੇ (other)
                        v = VIRTUALIZATION_VM_OTHER;
        }

        cached_ਮਿਲਿਆ = v;
        log_debug("Found VM virtualization %s", virtualization_ਮੰਜ਼ਿਲ_string(v));
        ਵਾਪਸ v;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ container_ਸਾਰਣੀ[_VIRTUALIZATION_MAX] = {
        [VIRTUALIZATION_LXC]            = "lxc",
        [VIRTUALIZATION_LXC_LIBVIRT]    = "lxc-libvirt",
        [VIRTUALIZATION_SYSTEMD_NSPAWN] = "systemd-nspawn",
        [VIRTUALIZATION_DOCKER]         = "docker",
        [VIRTUALIZATION_PODMAN]         = "podman",
        [VIRTUALIZATION_RKT]            = "rkt",
        [VIRTUALIZATION_WSL]            = "wsl",
        [VIRTUALIZATION_PROOT]          = "proot",
        [VIRTUALIZATION_POUCH]          = "pouch",
};

DEFINE_ਨਿੱਜੀ_STRING_ਸਾਰਣੀ_LOOKUP_FROM_STRING(container, ਪੂਰਨ);

ਸਥਿਰ ਪੂਰਨ running_ਵਿੱਚ_pidns(ਖਾਲੀ) {
        ਪੂਰਨ r;

        r = namespace_is_init(NAMESPACE_PID);
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to test if in root PID namespace, ignoring: %m");

        ਵਾਪਸ !r;
}

ਸਥਿਰ Virtualization detect_container_files(ਖਾਲੀ) {
        ਸਥਿਰ ਅਚਲ ਬਣਤਰ {
                ਅਚਲ ਅੱਖਰ *file_ਰਸਤਾ;
                Virtualization id;
        } container_file_ਸਾਰਣੀ[] = {
                /* https://github.com/containers/podman/issues/6192 */
                /* https://github.com/containers/podman/issues/3586#issuecomment-661918679 */
                { "/run/.containerenv", VIRTUALIZATION_PODMAN },
                /* https://github.com/moby/moby/issues/18355 */
                /* Docker must be the last in this table, see below. */
                { "/.dockerenv",        VIRTUALIZATION_DOCKER },
        };

        FOREACH_ELEMENT(file, container_file_ਸਾਰਣੀ) {
                ਜੇ (access(file->file_ਰਸਤਾ, F_OK) >= 0)
                        ਵਾਪਸ file->id;

                ਜੇ (errno != ENOENT)
                        log_debug_errno(errno,
                                        "Checking if %s exists failed, ignoring: %m",
                                        file->file_ਰਸਤਾ);
        }

        ਵਾਪਸ VIRTUALIZATION_NONE;
}

Virtualization detect_container(ਖਾਲੀ) {
        ਸਥਿਰ thread_local Virtualization cached_ਮਿਲਿਆ = _VIRTUALIZATION_INVALID;
        _cleanup_free_ ਅੱਖਰ *m = ਨਿਰਾ, *o = ਨਿਰਾ, *p = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *e = ਨਿਰਾ;
        Virtualization v;
        ਪੂਰਨ r;

        ਜੇ (cached_ਮਿਲਿਆ >= 0)
                ਵਾਪਸ cached_ਮਿਲਿਆ;

        /* /proc/vz exists in container and outside of the container, /proc/bc only outside of the container. */
        ਜੇ (access("/proc/vz", F_OK) < 0) {
                ਜੇ (errno != ENOENT)
                        log_debug_errno(errno, "Failed to check if /proc/vz exists, ignoring: %m");
        } ਨਹੀਂਤਾਂ ਜੇ (access("/proc/bc", F_OK) < 0) {
                ਜੇ (errno == ENOENT) {
                        v = VIRTUALIZATION_OPENVZ;
                        ਜਾਓ ਅੰਤ;
                }

                log_debug_errno(errno, "Failed to check if /proc/bc exists, ignoring: %m");
        }

        /* "Official" way of detecting WSL https://github.com/Microsoft/WSL/issues/423#issuecomment-221627364 */
        r = read_one_ਸਤਰ_file("/proc/sys/kernel/osrelease", &o);
        ਜੇ (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/kernel/osrelease, ignoring: %m");
        ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀ_ਖੋਜ(o, "Microsoft") || ਲੜੀ_ਖੋਜ(o, "WSL")) {
                v = VIRTUALIZATION_WSL;
                ਜਾਓ ਅੰਤ;
        }

        /* proot doesn't use PID namespacing, so we can just check if we have a matching tracer for this
         * invocation without worrying about it being elsewhere.
         */
        r = get_proc_ਖੇਤਰ("/proc/self/status", "TracerPid", &p);
        ਜੇ (r < 0)
                log_debug_errno(r, "Failed to read our own trace PID, ignoring: %m");
        ਨਹੀਂਤਾਂ ਜੇ (!streq(p, "0")) {
                pid_t ptrace_pid;

                r = parse_pid(p, &ptrace_pid);
                ਜੇ (r < 0)
                        log_debug_errno(r, "Failed to parse our own tracer PID, ignoring: %m");
                ਨਹੀਂਤਾਂ {
                        _cleanup_free_ ਅੱਖਰ *ptrace_comm = ਨਿਰਾ;
                        ਅਚਲ ਅੱਖਰ *pf;

                        pf = procfs_file_alloca(ptrace_pid, "comm");
                        r = read_one_ਸਤਰ_file(pf, &ptrace_comm);
                        ਜੇ (r < 0)
                                log_debug_errno(r, "Failed to read %s, ignoring: %m", pf);
                        ਨਹੀਂਤਾਂ ਜੇ (startswith(ptrace_comm, "proot")) {
                                v = VIRTUALIZATION_PROOT;
                                ਜਾਓ ਅੰਤ;
                        }
                }
        }

        /* The container manager might have placed this in the /run/host/ hierarchy for us, which is best
         * because we can be consumed just like that, without special privileges. */
        r = read_one_ਸਤਰ_file("/run/host/container-manager", &m);
        ਜੇ (r > 0) {
                e = m;
                ਜਾਓ translate_name;
        }
        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(r, -ENOENT, 0))
                ਵਾਪਸ log_debug_errno(r, "Failed to read /run/host/container-manager: %m");

        ਜੇ (getpid_cached() == 1) {
                /* If we are PID 1 we can just check our own environment variable, and that's authoritative.
                 * We distinguish three cases:
                 * - the variable is not defined → we jump to other checks
                 * - the variable is defined to an empty value → we are not in a container
                 * - anything else → some container, either one of the known ones or "container-other"
                 */
                e = getenv("container");
                ਜੇ (!e)
                        ਜਾਓ check_files;
                ਜੇ (isempty(e)) {
                        v = VIRTUALIZATION_NONE;
                        ਜਾਓ ਅੰਤ;
                }

                ਜਾਓ translate_name;
        }

        /* Otherwise, PID 1 might have dropped this information into a file in /run. This is better than accessing
         * /proc/1/environ, since we don't need CAP_SYS_PTRACE for that. */
        r = read_one_ਸਤਰ_file("/run/systemd/container", &m);
        ਜੇ (r > 0) {
                e = m;
                ਜਾਓ translate_name;
        }
        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(r, -ENOENT, 0))
                ਵਾਪਸ log_debug_errno(r, "Failed to read /run/systemd/container: %m");

        /* Fallback for cases where PID 1 was not systemd (for example, cases where init=/bin/sh is used. */
        r = getenv_for_pid(1, "container", &m);
        ਜੇ (r > 0) {
                e = m;
                ਜਾਓ translate_name;
        }
        ਜੇ (r < 0) /* This only works if we have CAP_SYS_PTRACE, hence let's better ignore failures here */
                log_debug_errno(r, "Failed to read $container of PID 1, ignoring: %m");

check_files:
        /* Check for existence of some well-known files. We only do this after checking
         * for other specific container managers, otherwise we risk mistaking another
         * container manager for Docker: the /.dockerenv file could inadvertently end up
         * in a file system image. */
        v = detect_container_files();
        ਜੇ (v < 0)
                ਵਾਪਸ v;
        ਜੇ (v != VIRTUALIZATION_NONE)
                ਜਾਓ ਅੰਤ;

        /* Finally, the root pid namespace has an hardcoded inode number of 0xEFFFFFFC since kernel 3.8, so
         * if all else fails we can check the inode number of our pid namespace and compare it. */
        ਜੇ (running_ਵਿੱਚ_pidns() > 0) {
                log_debug("Running in a pid namespace, assuming unknown container manager.");
                v = VIRTUALIZATION_CONTAINER_OTHER;
                ਜਾਓ ਅੰਤ;
        }

        /* If none of that worked, give up, assume no container manager. */
        v = VIRTUALIZATION_NONE;
        ਜਾਓ ਅੰਤ;

translate_name:
        ਜੇ (streq(e, "oci")) {
                /* Some images hardcode container=oci, but OCI is not a specific container manager.
                 * Try to detect one based on well-known files. */
                v = detect_container_files();
                ਜੇ (v == VIRTUALIZATION_NONE)
                        v = VIRTUALIZATION_CONTAINER_OTHER;
                ਜਾਓ ਅੰਤ;
        }
        v = container_from_string(e);
        ਜੇ (v < 0)
                v = VIRTUALIZATION_CONTAINER_OTHER;

ਅੰਤ:
        log_debug("Found container virtualization %s.", virtualization_ਮੰਜ਼ਿਲ_string(v));
        cached_ਮਿਲਿਆ = v;
        ਵਾਪਸ v;
}

Virtualization detect_virtualization(ਖਾਲੀ) {
        ਪੂਰਨ v;

        v = detect_container();
        ਜੇ (v != VIRTUALIZATION_NONE)
                ਵਾਪਸ v;

        ਵਾਪਸ detect_vm();
}

ਪੂਰਨ running_ਵਿੱਚ_userns(ਖਾਲੀ) {
        ਪੂਰਨ r;

        r = namespace_is_init(NAMESPACE_USER);
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to test if in root user namespace, ignoring: %m");

        ਵਾਪਸ !r;
}

ਪੂਰਨ running_ਵਿੱਚ_chroot(ਖਾਲੀ) {
        ਪੂਰਨ r;

        /* If we're PID1, /proc may not be mounted (and most likely we're not in a chroot). But PID1 will
         * mount /proc, so all other programs can assume that if /proc is *not* available, we're in some
         * chroot. */

        r = getenv_bool("SYSTEMD_IN_CHROOT");
        ਜੇ (r >= 0)
                ਵਾਪਸ r > 0;
        ਜੇ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_IN_CHROOT, ignoring: %m");

        /* Deprecated but kept for backwards compatibility. */
        ਜੇ (getenv_bool("SYSTEMD_IGNORE_CHROOT") > 0)
                ਵਾਪਸ 0;

        r = inode_same("/proc/1/root", "/", /* flags= */ 0);
        ਜੇ (r == -ENOENT) {
                r = proc_mounted();
                ਜੇ (r == 0) {
                        ਜੇ (getpid_cached() == 1)
                                ਵਾਪਸ false; /* We will mount /proc, assuming we're not in a chroot. */

                        log_debug("/proc/ is not mounted, assuming we're in a chroot.");
                        ਵਾਪਸ true;
                }
                ਜੇ (r > 0) /* If we have fake /proc/, we can't do the check properly. */
                        ਵਾਪਸ -ENOSYS;
        }
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to check if /proc/1/root and / are the same inode: %m");

        ਵਾਪਸ r == 0;
}

#ਜੇ defined(__i386__) || defined(__x86_64__)
ਬਣਤਰ cpuid_ਸਾਰਣੀ_ਇੰਦਰਾਜ {
        uint32_t flag_ਦੋਆਧਾਰੀ_ਅੰਕ;
        ਅਚਲ ਅੱਖਰ *name;
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ cpuid_ਸਾਰਣੀ_ਇੰਦਰਾਜ leaf1_edx[] = {
        {  0, "fpu"     },
        {  1, "vme"     },
        {  2, "de"      },
        {  3, "pse"     },
        {  4, "tsc"     },
        {  5, "msr"     },
        {  6, "pae"     },
        {  7, "mce"     },
        {  8, "cx8"     },
        {  9, "apic"    },
        { 11, "sep"     },
        { 12, "mtrr"    },
        { 13, "pge"     },
        { 14, "mca"     },
        { 15, "cmov"    },
        { 16, "pat"     },
        { 17, "pse36"   },
        { 19, "clflush" },
        { 23, "mmx"     },
        { 24, "fxsr"    },
        { 25, "sse"     },
        { 26, "sse2"    },
        { 28, "ht"      },
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ cpuid_ਸਾਰਣੀ_ਇੰਦਰਾਜ leaf1_ecx[] = {
        {  0, "pni"     },
        {  1, "pclmul"  },
        {  3, "monitor" },
        {  9, "ssse3"   },
        { 12, "fma3"    },
        { 13, "cx16"    },
        { 19, "sse4_1"  },
        { 20, "sse4_2"  },
        { 22, "movbe"   },
        { 23, "popcnt"  },
        { 25, "aes"     },
        { 26, "xsave"   },
        { 27, "osxsave" },
        { 28, "avx"     },
        { 29, "f16c"    },
        { 30, "rdrand"  },
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ cpuid_ਸਾਰਣੀ_ਇੰਦਰਾਜ leaf7_ebx[] = {
        {  3, "bmi1"   },
        {  5, "avx2"   },
        {  8, "bmi2"   },
        { 18, "rdseed" },
        { 19, "adx"    },
        { 29, "sha_ni" },
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ cpuid_ਸਾਰਣੀ_ਇੰਦਰਾਜ leaf81_edx[] = {
        { 11, "syscall" },
        { 27, "rdtscp"  },
        { 29, "lm"      },
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ cpuid_ਸਾਰਣੀ_ਇੰਦਰਾਜ leaf81_ecx[] = {
        {  0, "lahf_lm" },
        {  5, "abm"     },
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ cpuid_ਸਾਰਣੀ_ਇੰਦਰਾਜ leaf87_edx[] = {
        {  8, "constant_tsc" },
};

ਸਥਿਰ bool given_flag_ਵਿੱਚ_ਸੈੱਟ(ਅਚਲ ਅੱਖਰ *flag, ਅਚਲ ਬਣਤਰ cpuid_ਸਾਰਣੀ_ਇੰਦਰਾਜ *ਸੈੱਟ, size_t ਸੈੱਟ_ਮਾਪ, uint32_t val) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਸੈੱਟ);

        ਲਈ (size_t i = 0; i < ਸੈੱਟ_ਮਾਪ; i++) {
                ਜੇ ((UINT32_C(1) << ਸੈੱਟ[i].flag_ਦੋਆਧਾਰੀ_ਅੰਕ) & val &&
                                streq(flag, ਸੈੱਟ[i].name))
                        ਵਾਪਸ true;
        }
        ਵਾਪਸ false;
}

ਸਥਿਰ bool real_has_cpu_with_flag(ਅਚਲ ਅੱਖਰ *flag) {
        uint32_t eax, ebx, ecx, edx;

        ਜੇ (__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
                ਜੇ (given_flag_ਵਿੱਚ_ਸੈੱਟ(flag, leaf1_ecx, ELEMENTSOF(leaf1_ecx), ecx))
                        ਵਾਪਸ true;

                ਜੇ (given_flag_ਵਿੱਚ_ਸੈੱਟ(flag, leaf1_edx, ELEMENTSOF(leaf1_edx), edx))
                        ਵਾਪਸ true;
        }

        ਜੇ (__get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx)) {
                ਜੇ (given_flag_ਵਿੱਚ_ਸੈੱਟ(flag, leaf7_ebx, ELEMENTSOF(leaf7_ebx), ebx))
                        ਵਾਪਸ true;
        }

        ਜੇ (__get_cpuid(0x80000001U, &eax, &ebx, &ecx, &edx)) {
                ਜੇ (given_flag_ਵਿੱਚ_ਸੈੱਟ(flag, leaf81_ecx, ELEMENTSOF(leaf81_ecx), ecx))
                        ਵਾਪਸ true;

                ਜੇ (given_flag_ਵਿੱਚ_ਸੈੱਟ(flag, leaf81_edx, ELEMENTSOF(leaf81_edx), edx))
                        ਵਾਪਸ true;
        }

        ਜੇ (__get_cpuid(0x80000007U, &eax, &ebx, &ecx, &edx))
                ਜੇ (given_flag_ਵਿੱਚ_ਸੈੱਟ(flag, leaf87_edx, ELEMENTSOF(leaf87_edx), edx))
                        ਵਾਪਸ true;

        ਵਾਪਸ false;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(__aarch64__)
ਬਣਤਰ hwcap_ਸਾਰਣੀ_ਇੰਦਰਾਜ {
        uint64_t flag_ਨਕਾਬ;
        ਅਚਲ ਅੱਖਰ *name;
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ hwcap_ਸਾਰਣੀ_ਇੰਦਰਾਜ hwcap[] = {
        { HWCAP_FP,           "fp"          },
        { HWCAP_ASIMD,        "asimd"       },
        { HWCAP_EVTSTRM,      "evtstrm"     },
        { HWCAP_AES,          "aes"         },
        { HWCAP_PMULL,        "pmull"       },
        { HWCAP_SHA1,         "sha1"        },
        { HWCAP_SHA2,         "sha2"        },
        { HWCAP_ਚੱਕਰੀ_ਜਾਂਚ_32,        "crc32"       },
        { HWCAP_ATOMICS,      "atomics"     },
        { HWCAP_FPHP,         "fphp"        },
        { HWCAP_ASIMDHP,      "asimdhp"     },
        { HWCAP_CPUID,        "cpuid"       },
        { HWCAP_ASIMDRDM,     "asimdrdm"    },
        { HWCAP_JSCVT,        "jscvt"       },
        { HWCAP_FCMA,         "fcma"        },
        { HWCAP_LRCPC,        "lrcpc"       },
        { HWCAP_DCPOP,        "dcpop"       },
        { HWCAP_SHA3,         "sha3"        },
        { HWCAP_SM3,          "sm3"         },
        { HWCAP_SM4,          "sm4"         },
        { HWCAP_ASIMDDP,      "asimddp"     },
        { HWCAP_SHA512,       "sha512"      },
        { HWCAP_SVE,          "sve"         },
        { HWCAP_ASIMDFHM,     "asimdfhm"    },
        { HWCAP_DIT,          "dit"         },
        { HWCAP_USCAT,        "uscat"       },
        { HWCAP_ILRCPC,       "ilrcpc"      },
        { HWCAP_FLAGM,        "flagm"       },
        { HWCAP_SSBS,         "ssbs"        },
        { HWCAP_SB,           "sb"          },
        { HWCAP_PACA,         "paca"        },
        { HWCAP_PACG,         "pacg"        },
        { HWCAP_GCS,          "gcs"         },
        { HWCAP_CMPBR,        "cmpbr"       },
        { HWCAP_FPRCVT,       "fprcvt"      },
        { HWCAP_F8MM8,        "f8mm8"       },
        { HWCAP_F8MM4,        "f8mm4"       },
        { HWCAP_SVE_F16MM,    "svef16mm"    },
        { HWCAP_SVE_ELTPERM,  "sveeltperm"  },
        { HWCAP_SVE_AES2,     "sveaes2"     },
        { HWCAP_SVE_BFSCALE,  "svebfscale"  },
        { HWCAP_SVE2P2,       "sve2p2"      },
        { HWCAP_SME2P2,       "sme2p2"      },
        { HWCAP_SME_SBITPERM, "smesbitperm" },
        { HWCAP_SME_AES,      "smeaes"      },
        { HWCAP_SME_SFEXPA,   "smesfexpa"   },
        { HWCAP_SME_STMOP,    "smestmop"    },
        { HWCAP_SME_SMOP4,    "smesmop4"    },
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ hwcap_ਸਾਰਣੀ_ਇੰਦਰਾਜ hwcap2[] = {
        { HWCAP2_DCPODP,      "dcpodp"     },
        { HWCAP2_SVE2,        "sve2"       },
        { HWCAP2_SVEAES,      "sveaes"     },
        { HWCAP2_SVEPMULL,    "svepmull"   },
        { HWCAP2_SVEBITPERM,  "svebitperm" },
        { HWCAP2_SVESHA3,     "svesha3"    },
        { HWCAP2_SVESM4,      "svesm4"     },
        { HWCAP2_FLAGM2,      "flagm2"     },
        { HWCAP2_FRINT,       "frint"      },
        { HWCAP2_SVEI8MM,     "svei8mm"    },
        { HWCAP2_SVEF32MM,    "svef32mm"   },
        { HWCAP2_SVEF64MM,    "svef64mm"   },
        { HWCAP2_SVEBF16,     "svebf16"    },
        { HWCAP2_I8MM,        "i8mm"       },
        { HWCAP2_BF16,        "bf16"       },
        { HWCAP2_DGH,         "dgh"        },
        { HWCAP2_RNG,         "rng"        },
        { HWCAP2_BTI,         "bti"        },
        { HWCAP2_MTE,         "mte"        },
        { HWCAP2_ECV,         "ecv"        },
        { HWCAP2_AFP,         "afp"        },
        { HWCAP2_RPRES,       "rpres"      },
        { HWCAP2_MTE3,        "mte3"       },
        { HWCAP2_SME,         "sme"        },
        { HWCAP2_SME_I16I64,  "smei16i64"  },
        { HWCAP2_SME_F64F64,  "smef64f64"  },
        { HWCAP2_SME_I8I32,   "smei8i32"   },
        { HWCAP2_SME_F16F32,  "smef16f32"  },
        { HWCAP2_SME_B16F32,  "smeb16f32"  },
        { HWCAP2_SME_F32F32,  "smef32f32"  },
        { HWCAP2_SME_FA64,    "smefa64"    },
        { HWCAP2_WFXT,        "wfxt"       },
        { HWCAP2_EBF16,       "ebf16"      },
        { HWCAP2_SVE_EBF16,   "sveebf16"   },
        { HWCAP2_CSSC,        "cssc"       },
        { HWCAP2_RPRFM,       "rprfm"      },
        { HWCAP2_SVE2P1,      "sve2p1"     },
        { HWCAP2_SME2,        "sme2"       },
        { HWCAP2_SME2P1,      "sme2p1"     },
        { HWCAP2_SME_I16I32,  "smei16i32"  },
        { HWCAP2_SME_BI32I32, "smebi32i32" },
        { HWCAP2_SME_B16B16,  "smeb16b16"  },
        { HWCAP2_SME_F16F16,  "smef16f16"  },
        { HWCAP2_MOPS,        "mops"       },
        { HWCAP2_HBC,         "hbc"        },
        { HWCAP2_SVE_B16B16,  "sveb16b16"  },
        { HWCAP2_LRCPC3,      "lrcpc3"     },
        { HWCAP2_LSE128,      "lse128"     },
        { HWCAP2_FPMR,        "fpmr"       },
        { HWCAP2_LUT,         "lut"        },
        { HWCAP2_FAMINMAX,    "faminmax"   },
        { HWCAP2_F8CVT,       "f8cvt"      },
        { HWCAP2_F8FMA,       "f8fma"      },
        { HWCAP2_F8DP4,       "f8dp4"      },
        { HWCAP2_F8DP2,       "f8dp2"      },
        { HWCAP2_F8E4M3,      "f8e4m3"     },
        { HWCAP2_F8E5M2,      "f8e5m2"     },
        { HWCAP2_SME_LUTV2,   "smelutv2"   },
        { HWCAP2_SME_F8F16,   "smef8f16"   },
        { HWCAP2_SME_F8F32,   "smef8f32"   },
        { HWCAP2_SME_SF8FMA,  "smesf8fma"  },
        { HWCAP2_SME_SF8DP4,  "smesf8dp4"  },
        { HWCAP2_SME_SF8DP2,  "smesf8dp2"  },
        { HWCAP2_POE,         "poe"        },
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ hwcap_ਸਾਰਣੀ_ਇੰਦਰਾਜ hwcap3[] = {
        { HWCAP3_MTE_FAR,        "mtefar"       },
        { HWCAP3_MTE_STORE_ONLY, "mtestoreonly" },
        { HWCAP3_LSFE,           "lsfe"         },
        { HWCAP3_LS64,           "ls64"         },
        { HWCAP3_SVE_B16MM,      "sveb16mm"     },
        { HWCAP3_SVE2P3,         "sve2p3"       },
        { HWCAP3_SME_LUT6,       "smelut6"      },
        { HWCAP3_SME2P3,         "sme2p3"       },
        { HWCAP3_F16MM,          "f16mm"        },
        { HWCAP3_F16F32DOT,      "f16f32dot"    },
        { HWCAP3_F16F32MM,       "f16f32mm"     },
        { HWCAP3_SVE_LUT6,       "svelut6"      },
};

ਸਥਿਰ bool given_flag_ਵਿੱਚ_hwcap_ਸੈੱਟ(
                ਅਚਲ ਅੱਖਰ *flag,
                ਅਚਲ ਬਣਤਰ hwcap_ਸਾਰਣੀ_ਇੰਦਰਾਜ *ਸੈੱਟ,
                size_t ਸੈੱਟ_ਮਾਪ,
                ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ val) {

        ਪ੍ਰਤਿਜ੍ਞਾ(ਸੈੱਟ);

        ਲਈ (size_t i = 0; i < ਸੈੱਟ_ਮਾਪ; i++)
                ਜੇ ((ਸੈੱਟ[i].flag_ਨਕਾਬ & val) && streq(flag, ਸੈੱਟ[i].name))
                        ਵਾਪਸ true;

        ਵਾਪਸ false;
}

ਸਥਿਰ bool real_has_cpu_with_flag(ਅਚਲ ਅੱਖਰ *flag) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ val;

        val = getauxval(AT_HWCAP);
        ਜੇ (given_flag_ਵਿੱਚ_hwcap_ਸੈੱਟ(flag, hwcap, ELEMENTSOF(hwcap), val))
                ਵਾਪਸ true;

        val = getauxval(AT_HWCAP2);
        ਜੇ (given_flag_ਵਿੱਚ_hwcap_ਸੈੱਟ(flag, hwcap2, ELEMENTSOF(hwcap2), val))
                ਵਾਪਸ true;

        val = getauxval(AT_HWCAP3);
        ਜੇ (given_flag_ਵਿੱਚ_hwcap_ਸੈੱਟ(flag, hwcap3, ELEMENTSOF(hwcap3), val))
                ਵਾਪਸ true;

        ਵਾਪਸ false;
}
#ਜੇ_ਅੰਤ

bool has_cpu_with_flag(ਅਚਲ ਅੱਖਰ *flag) {
        /* Check the architecture-specific CPU feature interface when available. */
#ਜੇ defined(__i386__) || defined(__x86_64__) || defined(__aarch64__)
        ਵਾਪਸ real_has_cpu_with_flag(flag);
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ false;
#ਜੇ_ਅੰਤ
}

ਪੂਰਨ cpu_ਗਿਆਤ_features(ਅੱਖਰ ***ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

#ਜੇ defined(__i386__) || defined(__x86_64__)
        ਸਥਿਰ ਅਚਲ ਬਣਤਰ {
                ਅਚਲ ਬਣਤਰ cpuid_ਸਾਰਣੀ_ਇੰਦਰਾਜ *ਸਾਰਣੀ;
                size_t n_entries;
        } tables[] = {
                { leaf1_edx,  ELEMENTSOF(leaf1_edx),  },
                { leaf1_ecx,  ELEMENTSOF(leaf1_ecx),  },
                { leaf7_ebx,  ELEMENTSOF(leaf7_ebx),  },
                { leaf81_ecx, ELEMENTSOF(leaf81_ecx), },
                { leaf81_edx, ELEMENTSOF(leaf81_edx), },
                { leaf87_edx, ELEMENTSOF(leaf87_edx), },
        };
#ਜਾਂਜੇ defined(__aarch64__)
        ਸਥਿਰ ਅਚਲ ਬਣਤਰ {
                ਅਚਲ ਬਣਤਰ hwcap_ਸਾਰਣੀ_ਇੰਦਰਾਜ *ਸਾਰਣੀ;
                size_t n_entries;
        } tables[] = {
                { hwcap,  ELEMENTSOF(hwcap),  },
                { hwcap2, ELEMENTSOF(hwcap2), },
                { hwcap3, ELEMENTSOF(hwcap3), },
        };
#ਜੇ_ਅੰਤ

#ਜੇ defined(__i386__) || defined(__x86_64__) || defined(__aarch64__)
        _cleanup_strv_free_ ਅੱਖਰ **features = ਨਿਰਾ;
        size_t n = 0;
        ਪੂਰਨ r;

        FOREACH_ELEMENT(ਸਾਰਣੀ, tables)
                FOREACH_ARRAY(i, ਸਾਰਣੀ->ਸਾਰਣੀ, ਸਾਰਣੀ->n_entries) {
                        r = strv_extend_with_ਮਾਪ(&features, &n, i->name);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                }

        *ret = TAKE_PTR(features);
#ਨਹੀਂਤਾਂ
        *ret = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ virtualization_ਸਾਰਣੀ[_VIRTUALIZATION_MAX] = {
        [VIRTUALIZATION_NONE]            = "none",
        [VIRTUALIZATION_KVM]             = "kvm",
        [VIRTUALIZATION_AMAZON]          = "amazon",
        [VIRTUALIZATION_QEMU]            = "qemu",
        [VIRTUALIZATION_BOCHS]           = "bochs",
        [VIRTUALIZATION_XEN]             = "xen",
        [VIRTUALIZATION_UML]             = "uml",
        [VIRTUALIZATION_VMWARE]          = "vmware",
        [VIRTUALIZATION_ORACLE]          = "oracle",
        [VIRTUALIZATION_MICROSOFT]       = "microsoft",
        [VIRTUALIZATION_ZVM]             = "zvm",
        [VIRTUALIZATION_PARALLELS]       = "parallels",
        [VIRTUALIZATION_BHYVE]           = "bhyve",
        [VIRTUALIZATION_QNX]             = "qnx",
        [VIRTUALIZATION_ACRN]            = "acrn",
        [VIRTUALIZATION_POWERVM]         = "powervm",
        [VIRTUALIZATION_APPLE]           = "apple",
        [VIRTUALIZATION_SRE]             = "sre",
        [VIRTUALIZATION_GOOGLE]          = "google",
        [VIRTUALIZATION_VM_OTHER]        = "vm-other",

        [VIRTUALIZATION_SYSTEMD_NSPAWN]  = "systemd-nspawn",
        [VIRTUALIZATION_LXC_LIBVIRT]     = "lxc-libvirt",
        [VIRTUALIZATION_LXC]             = "lxc",
        [VIRTUALIZATION_OPENVZ]          = "openvz",
        [VIRTUALIZATION_DOCKER]          = "docker",
        [VIRTUALIZATION_PODMAN]          = "podman",
        [VIRTUALIZATION_RKT]             = "rkt",
        [VIRTUALIZATION_WSL]             = "wsl",
        [VIRTUALIZATION_PROOT]           = "proot",
        [VIRTUALIZATION_POUCH]           = "pouch",
        [VIRTUALIZATION_CONTAINER_OTHER] = "container-other",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(virtualization, Virtualization);
