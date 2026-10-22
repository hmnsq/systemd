/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ఒకవేళ defined(__i386__) || defined(__x86_64__)
#అన్తర్భావ <cpuid.h>
#అయితే_ముగింపు
#ఒకవేళ defined(__aarch64__)
#అన్తర్భావ <sys/auxv.h>
#అయితే_ముగింపు
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <threads.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "dirent-util.h"        /* IWYU pragma: keep */
#అన్తర్భావ "env-util.h"
#అన్తర్భావ "extract-word.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fileio.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "namespace-util.h"
#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "process-util.h"
#అన్తర్భావ "stat-util.h"
#అన్తర్భావ "string-table.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"
#అన్తర్భావ "virt.h"

గణన {
      SMBIOS_VM_బిట్టు_సమితి,
      SMBIOS_VM_బిట్టు_UNSET,
      SMBIOS_VM_బిట్టు_UNKNOWN,
};

స్థిర Virtualization detect_vm_cpuid(శూన్య) {

        /* CPUID is an x86 specific interface. */
#ఒకవేళ defined(__i386__) || defined(__x86_64__)

        స్థిర స్థిరరాశి నిర్మాణం {
                స్థిరరాశి అక్షరం sig[13];
                Virtualization id;
        } vm_పట్టిక[] = {
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
        ఒకవేళ (__get_cpuid(1, &eax, &ebx, &ecx, &edx) == 0)
                తిరిగి VIRTUALIZATION_NONE;

        hypervisor = ecx & 0x80000000U;

        ఒకవేళ (hypervisor) {
                సంఘం {
                        uint32_t sig32[3];
                        అక్షరం పాఠ్యం[13];
                } sig = {};

                /* There is a hypervisor, see what it is */
                __cpuid(0x40000000U, eax, ebx, ecx, edx);

                sig.sig32[0] = ebx;
                sig.sig32[1] = ecx;
                sig.sig32[2] = edx;

                log_debug("Virtualization found, CPUID=%s", sig.పాఠ్యం);

                FOREACH_ELEMENT(vm, vm_పట్టిక)
                        ఒకవేళ (memcmp_nn(sig.పాఠ్యం, పరిమాణం(sig.పాఠ్యం),
                                      vm->sig, పరిమాణం(vm->sig)) == 0)
                                తిరిగి vm->id;

                log_debug("Unknown virtualization with CPUID=%s. Add to vm_table[]?", sig.పాఠ్యం);
                తిరిగి VIRTUALIZATION_VM_OTHER;
        }
#అయితే_ముగింపు
        log_debug("No virtualization found in CPUID");

        తిరిగి VIRTUALIZATION_NONE;
}

స్థిర Virtualization detect_vm_device_వృక్షం(శూన్య) {
#ఒకవేళ defined(__arm__) || defined(__aarch64__) || defined(__powerpc__) || defined(__powerpc64__) || defined(__riscv)
        _cleanup_free_ అక్షరం *hvtype = ఖాళీ;
        పూర్ణాంకం r;

        r = read_one_పంక్తి_file("/proc/device-tree/hypervisor/compatible", &hvtype);
        ఒకవేళ (r == -ENOENT) {
                ఒకవేళ (access("/proc/device-tree/ibm,partition-name", F_OK) == 0 &&
                    access("/proc/device-tree/hmc-managed?", F_OK) == 0 &&
                    access("/proc/device-tree/chosen/qemu,graphic-width", F_OK) != 0)
                        తిరిగి VIRTUALIZATION_POWERVM;

                _cleanup_closedir_ DIR *dir = opendir("/proc/device-tree");
                ఒకవేళ (!dir) {
                        ఒకవేళ (errno == ENOENT) {
                                log_debug_errno(errno, "/proc/device-tree/ does not exist");
                                తిరిగి VIRTUALIZATION_NONE;
                        }
                        తిరిగి log_debug_errno(errno, "Opening /proc/device-tree/ failed: %m");
                }

                FOREACH_DIRENT(de, dir, తిరిగి log_debug_errno(errno, "Failed to enumerate /proc/device-tree/ contents: %m"))
                        ఒకవేళ (అక్షరశ్రేణి_శోధన(de->d_name, "fw-cfg")) {
                                log_debug("Virtualization QEMU: \"fw-cfg\" present in /proc/device-tree/%s", de->d_name);
                                తిరిగి VIRTUALIZATION_QEMU;
                        }

                _cleanup_free_ అక్షరం *compat = ఖాళీ;
                r = read_one_పంక్తి_file("/proc/device-tree/compatible", &compat);
                ఒకవేళ (r < 0 && r != -ENOENT)
                        తిరిగి log_debug_errno(r, "Failed to read /proc/device-tree/compatible: %m");
                ఒకవేళ (r >= 0) {
                        ఒకవేళ (streq(compat, "qemu,pseries")) {
                                log_debug("Virtualization %s found in /proc/device-tree/compatible", compat);
                                తిరిగి VIRTUALIZATION_QEMU;
                        }
                        ఒకవేళ (streq(compat, "linux,dummy-virt")) {
                                /* https://www.kernel.org/doc/Documentation/devicetree/bindings/arm/linux%2Cdummy-virt.yaml */
                                log_debug("Generic virtualization %s found in /proc/device-tree/compatible", compat);
                                తిరిగి VIRTUALIZATION_VM_OTHER;
                        }
                }

                log_debug("No virtualization found in /proc/device-tree/*");
                తిరిగి VIRTUALIZATION_NONE;
        } లేకపోతే ఒకవేళ (r < 0)
                తిరిగి r;

        log_debug("Virtualization %s found in /proc/device-tree/hypervisor/compatible", hvtype);
        ఒకవేళ (streq(hvtype, "linux,kvm"))
                తిరిగి VIRTUALIZATION_KVM;
        లేకపోతే ఒకవేళ (అక్షరశ్రేణి_శోధన(hvtype, "xen"))
                తిరిగి VIRTUALIZATION_XEN;
        లేకపోతే ఒకవేళ (అక్షరశ్రేణి_శోధన(hvtype, "vmware"))
                తిరిగి VIRTUALIZATION_VMWARE;
        లేకపోతే
                తిరిగి VIRTUALIZATION_VM_OTHER;
#లేకపోతే
        log_debug("This platform does not support /proc/device-tree");
        తిరిగి VIRTUALIZATION_NONE;
#అయితే_ముగింపు
}

#ఒకవేళ defined(__i386__) || defined(__x86_64__) || defined(__arm__) || defined(__aarch64__) || defined(__loongarch_lp64) || defined(__riscv)
స్థిర Virtualization detect_vm_dmi_vendor(శూన్య) {
        స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి dmi_vendors[] = {
                "/sys/class/dmi/id/product_name", /* Test this before sys_vendor to detect KVM over QEMU */
                "/sys/class/dmi/id/sys_vendor",
                "/sys/class/dmi/id/board_vendor",
                "/sys/class/dmi/id/bios_vendor",
                "/sys/class/dmi/id/product_version", /* For Hyper-V VMs test */
                ఖాళీ
        };

        స్థిర స్థిరరాశి నిర్మాణం {
                స్థిరరాశి అక్షరం *vendor;
                Virtualization id;
        } dmi_vendor_పట్టిక[] = {
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
        పూర్ణాంకం r;

        STRV_FOREACH(vendor, dmi_vendors) {
                _cleanup_free_ అక్షరం *s = ఖాళీ;

                r = read_one_పంక్తి_file(*vendor, &s);
                ఒకవేళ (r == -ENOENT)
                        కొనసాగు;
                ఒకవేళ (r < 0)
                        తిరిగి r;

                FOREACH_ELEMENT(dmi_vendor, dmi_vendor_పట్టిక)
                        ఒకవేళ (startswith(s, dmi_vendor->vendor)) {
                                log_debug("Virtualization %s found in DMI (%s)", s, *vendor);
                                తిరిగి dmi_vendor->id;
                        }
        }
        log_debug("No virtualization found in DMI vendor table.");
        తిరిగి VIRTUALIZATION_NONE;
}

స్థిర పూర్ణాంకం detect_vm_smbios(శూన్య) {
        /* The SMBIOS BIOS Characteristics Extension Byte 2 (Section 2.1.2.2 of
         * https://www.dmtf.org/sites/default/files/standards/documents/DSP0134_3.4.0.pdf), specifies that
         * the 4th bit being set indicates a VM. The BIOS Characteristics table is exposed via the kernel in
         * /sys/firmware/dmi/entries/0-0. Note that in the general case, this bit being unset should not
         * imply that the system is running on bare-metal.  For example, QEMU 3.1.0 (with or without KVM)
         * with SeaBIOS does not set this bit. */
        _cleanup_free_ అక్షరం *s = ఖాళీ;
        size_t readsize;
        పూర్ణాంకం r;

        r = read_నిండిన_virtual_file("/sys/firmware/dmi/entries/0-0/raw", &s, &readsize);
        ఒకవేళ (r < 0) {
                log_debug_errno(r, "Unable to read /sys/firmware/dmi/entries/0-0/raw, "
                                "using the virtualization information found in DMI vendor table, ignoring: %m");
                తిరిగి SMBIOS_VM_బిట్టు_UNKNOWN;
        }
        ఒకవేళ (readsize < 20 || s[1] < 20) {
                /* The spec indicates that byte 1 contains the size of the table, 0x12 + the number of
                 * extension bytes. The data we're interested in is in extension byte 2, which would be at
                 * 0x13. If we didn't read that much data, or if the BIOS indicates that we don't have that
                 * much data, we don't infer anything from the SMBIOS. */
                log_debug("Only read %zu bytes from /sys/firmware/dmi/entries/0-0/raw (expected 20). "
                          "Using the virtualization information found in DMI vendor table.", readsize);
                తిరిగి SMBIOS_VM_బిట్టు_UNKNOWN;
        }

        uint8_t byte = (uint8_t) s[19];
        ఒకవేళ (byte & (1U<<4)) {
                log_debug("DMI BIOS Extension table indicates virtualization.");
                తిరిగి SMBIOS_VM_బిట్టు_సమితి;
        }
        log_debug("DMI BIOS Extension table does not indicate virtualization.");
        తిరిగి SMBIOS_VM_బిట్టు_UNSET;
}
#అయితే_ముగింపు /* defined(__i386__) || defined(__x86_64__) || defined(__arm__) || defined(__aarch64__) || defined(__loongarch_lp64) */

స్థిర Virtualization detect_vm_dmi(శూన్య) {
#ఒకవేళ defined(__i386__) || defined(__x86_64__) || defined(__arm__) || defined(__aarch64__) || defined(__loongarch_lp64) || defined(__riscv)

        పూర్ణాంకం r;
        r = detect_vm_dmi_vendor();

        /* The DMI vendor tables in /sys/class/dmi/id don't help us distinguish between Amazon EC2
         * virtual machines and bare-metal instances, so we need to look at SMBIOS. */
        ఒకవేళ (r == VIRTUALIZATION_AMAZON) {
                ఎంపిక (detect_vm_smbios()) {
                స్థితి SMBIOS_VM_బిట్టు_సమితి:
                        తిరిగి VIRTUALIZATION_AMAZON;
                స్థితి SMBIOS_VM_బిట్టు_UNSET:
                        తిరిగి VIRTUALIZATION_NONE;
                స్థితి SMBIOS_VM_బిట్టు_UNKNOWN: {
                        /* The DMI information we are after is only accessible to the root user,
                         * so we fallback to using the product name which is less restricted
                         * to distinguish metal systems from virtualized instances */
                        _cleanup_free_ అక్షరం *s = ఖాళీ;
                        స్థిరరాశి అక్షరం *e;

                        r = read_నిండిన_virtual_file("/sys/class/dmi/id/product_name", &s, ఖాళీ);
                        /* In EC2, virtualized is much more common than metal, so if for some reason
                         * we fail to read the DMI data, assume we are virtualized. */
                        ఒకవేళ (r < 0) {
                                log_debug_errno(r, "Can't read /sys/class/dmi/id/product_name,"
                                                " assuming virtualized: %m");
                                తిరిగి VIRTUALIZATION_AMAZON;
                        }
                        e = strstrafter(truncate_nl(s), ".metal");
                        ఒకవేళ (e && లోపల_సమితి(*e, 0, '-')) {
                                log_debug("DMI product name has '.metal', assuming no virtualization");
                                తిరిగి VIRTUALIZATION_NONE;
                        } లేకపోతే
                                తిరిగి VIRTUALIZATION_AMAZON;
                }
                అప్రమేయ:
                        assert_not_reached();
              }
        }

        /* If we haven't identified a VM, but the firmware indicates that there is one, indicate as much. We
         * have no further information about what it is. */
        ఒకవేళ (r == VIRTUALIZATION_NONE && detect_vm_smbios() == SMBIOS_VM_బిట్టు_సమితి)
                తిరిగి VIRTUALIZATION_VM_OTHER;
        తిరిగి r;
#లేకపోతే
        తిరిగి VIRTUALIZATION_NONE;
#అయితే_ముగింపు
}

#నిర్వచించు XENFEAT_dom0 11 /* xen/include/public/features.h */
#నిర్వచించు మార్గం_FEATURES "/sys/hypervisor/properties/features"
/* Returns -errno, or 0 for domU, or 1 for dom0 */
స్థిర పూర్ణాంకం detect_vm_xen_dom0(శూన్య) {
        _cleanup_free_ అక్షరం *domcap = ఖాళీ;
        పూర్ణాంకం r;

        r = read_one_పంక్తి_file(మార్గం_FEATURES, &domcap);
        ఒకవేళ (r < 0 && r != -ENOENT)
                తిరిగి r;
        ఒకవేళ (r >= 0) {
                గుర్తులేని దీర్ఘ features;

                /* Here, we need to use sscanf() instead of safe_atoul()
                 * as the string lacks the leading "0x". */
                r = అక్షరశ్రేణి_చదువు(domcap, "%lx", &features);
                ఒకవేళ (r == 1) {
                        r = !!(features & (1U << XENFEAT_dom0));
                        log_debug("Virtualization XEN, found %s with value %08lx, "
                                  "XENFEAT_dom0 (indicating the 'hardware domain') is%s set.",
                                  మార్గం_FEATURES, features, r ? "" : " not");
                        తిరిగి r;
                }
                log_debug("Virtualization XEN, found %s, unhandled content '%s'",
                          మార్గం_FEATURES, domcap);
        }

        r = read_one_పంక్తి_file("/proc/xen/capabilities", &domcap);
        ఒకవేళ (r == -ENOENT) {
                log_debug("Virtualization XEN because /proc/xen/capabilities does not exist");
                తిరిగి 0;
        }
        ఒకవేళ (r < 0)
                తిరిగి r;

        కొరకు (స్థిరరాశి అక్షరం *i = domcap;;) {
                _cleanup_free_ అక్షరం *cap = ఖాళీ;

                r = వెలికితీయి_మొదటి_మాట(&i, &cap, ",", 0);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0) {
                        log_debug("Virtualization XEN DomU found (/proc/xen/capabilities)");
                        తిరిగి 0;
                }

                ఒకవేళ (streq(cap, "control_d")) {
                        log_debug("Virtualization XEN Dom0 ignored (/proc/xen/capabilities)");
                        తిరిగి 1;
                }
        }
}

స్థిర Virtualization detect_vm_xen(శూన్య) {
        /* The presence of /proc/xen indicates some form of a Xen domain
           The check for Dom0 is handled outside this function */
        ఒకవేళ (access("/proc/xen", F_OK) < 0) {
                log_debug("Virtualization XEN not found, /proc/xen does not exist");
                తిరిగి VIRTUALIZATION_NONE;
        }
        log_debug("Virtualization XEN found (/proc/xen exists)");
        తిరిగి VIRTUALIZATION_XEN;
}

స్థిర Virtualization detect_vm_hypervisor(శూన్య) {
        _cleanup_free_ అక్షరం *hvtype = ఖాళీ;
        పూర్ణాంకం r;

        r = read_one_పంక్తి_file("/sys/hypervisor/type", &hvtype);
        ఒకవేళ (r == -ENOENT)
                తిరిగి VIRTUALIZATION_NONE;
        ఒకవేళ (r < 0)
                తిరిగి r;

        log_debug("Virtualization %s found in /sys/hypervisor/type", hvtype);

        ఒకవేళ (streq(hvtype, "xen"))
                తిరిగి VIRTUALIZATION_XEN;
        లేకపోతే
                తిరిగి VIRTUALIZATION_VM_OTHER;
}

స్థిర Virtualization detect_vm_uml(శూన్య) {
        _cleanup_fclose_ FILE *f = ఖాళీ;
        పూర్ణాంకం r;

        /* Detect User-Mode Linux by reading /proc/cpuinfo */
        f = fopen("/proc/cpuinfo", "re");
        ఒకవేళ (!f) {
                ఒకవేళ (errno == ENOENT) {
                        log_debug("/proc/cpuinfo not found, assuming no UML virtualization.");
                        తిరిగి VIRTUALIZATION_NONE;
                }
                తిరిగి -errno;
        }

        కొరకు (;;) {
                _cleanup_free_ అక్షరం *పంక్తి = ఖాళీ;
                స్థిరరాశి అక్షరం *t;

                r = read_పంక్తి(f, LONG_పంక్తి_MAX, &పంక్తి);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        ఆపు;

                t = startswith(పంక్తి, "vendor_id\t: ");
                ఒకవేళ (t) {
                        ఒకవేళ (startswith(t, "User Mode Linux")) {
                                log_debug("UML virtualization found in /proc/cpuinfo");
                                తిరిగి VIRTUALIZATION_UML;
                        }

                        ఆపు;
                }
        }

        log_debug("UML virtualization not found in /proc/cpuinfo.");
        తిరిగి VIRTUALIZATION_NONE;
}

స్థిర Virtualization detect_vm_zvm(శూన్య) {

#ఒకవేళ defined(__s390__)
        _cleanup_free_ అక్షరం *t = ఖాళీ;
        పూర్ణాంకం r;

        r = get_proc_క్షేత్రం("/proc/sysinfo", "VM00 Control Program", &t);
        ఒకవేళ (లోపల_సమితి(r, -ENOENT, -ENODATA))
                తిరిగి VIRTUALIZATION_NONE;
        ఒకవేళ (r < 0)
                తిరిగి r;

        log_debug("Virtualization %s found in /proc/sysinfo", t);
        ఒకవేళ (streq(t, "z/VM"))
                తిరిగి VIRTUALIZATION_ZVM;
        లేకపోతే
                తిరిగి VIRTUALIZATION_KVM;
#లేకపోతే
        log_debug("This platform does not support /proc/sysinfo");
        తిరిగి VIRTUALIZATION_NONE;
#అయితే_ముగింపు
}

/* Returns a short identifier for the various VM implementations */
Virtualization detect_vm(శూన్య) {
        స్థిర thread_local Virtualization cached_దొరికింది = _VIRTUALIZATION_INVALID;
        bool other = false, hyperv = false;
        పూర్ణాంకం xen_dom0 = 0;
        Virtualization v, dmi;

        ఒకవేళ (cached_దొరికింది >= 0)
                తిరిగి cached_దొరికింది;

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
        ఒకవేళ (లోపల_సమితి(dmi,
                   VIRTUALIZATION_ORACLE,
                   VIRTUALIZATION_XEN,
                   VIRTUALIZATION_AMAZON,
                   /* Unable to distinguish a GCE machine from a VM to bare-metal
                    * for non-x86 architectures due to its necessity for cpuid
                    * detection, which functions solely on x86 platforms. Report
                    * as a VM for other architectures. */
#ఒకవేళ !defined(__i386__) && !defined(__x86_64__)
                   VIRTUALIZATION_GOOGLE,
#అయితే_ముగింపు
                   VIRTUALIZATION_PARALLELS)) {
                v = dmi;
                వెళ్ళు ముగింపు;
        }

        /* Detect UML */
        v = detect_vm_uml();
        ఒకవేళ (v < 0)
                తిరిగి v;
        ఒకవేళ (v != VIRTUALIZATION_NONE)
                వెళ్ళు ముగింపు;

        /* Detect Xen */
        v = detect_vm_xen();
        ఒకవేళ (v < 0)
                తిరిగి v;
        ఒకవేళ (v == VIRTUALIZATION_XEN) {
                 /* If we are Dom0, then we expect to not report as a VM. However, as we might be nested
                  * inside another hypervisor which can be detected via the CPUID check, wait to report this
                  * until after the CPUID check. */
                xen_dom0 = detect_vm_xen_dom0();
                ఒకవేళ (xen_dom0 < 0)
                        తిరిగి xen_dom0;
                ఒకవేళ (xen_dom0 == 0)
                        వెళ్ళు ముగింపు;
        } లేకపోతే ఒకవేళ (v != VIRTUALIZATION_NONE)
                assert_not_reached();

        /* Detect from CPUID */
        v = detect_vm_cpuid();
        ఒకవేళ (v < 0)
                తిరిగి v;
        ఒకవేళ (v == VIRTUALIZATION_MICROSOFT)
                /* QEMU sets the CPUID string to hyperv's, in case it provides hyperv enlightenments. Let's
                 * hence not return Microsoft here but just use the other mechanisms first to make a better
                 * decision. */
                hyperv = true;
        లేకపోతే ఒకవేళ (v == VIRTUALIZATION_VM_OTHER)
                other = true;
        లేకపోతే ఒకవేళ (v == VIRTUALIZATION_KVM && dmi == VIRTUALIZATION_GOOGLE)
                /* The DMI vendor tables in /sys/class/dmi/id don't help us distinguish between GCE
                 * virtual machines and bare-metal instances, so we need to look at hypervisor. */
                తిరిగి VIRTUALIZATION_GOOGLE;
        లేకపోతే ఒకవేళ (v != VIRTUALIZATION_NONE)
                వెళ్ళు ముగింపు;

        /* If we are in Dom0 and have not yet finished, finish with the result of detect_vm_cpuid */
        ఒకవేళ (xen_dom0 > 0)
                వెళ్ళు ముగింపు;

        /* Now, let's get back to DMI */
        ఒకవేళ (dmi < 0)
                తిరిగి dmi;
        ఒకవేళ (dmi == VIRTUALIZATION_VM_OTHER)
                other = true;
        లేకపోతే ఒకవేళ (!లోపల_సమితి(dmi, VIRTUALIZATION_NONE, VIRTUALIZATION_GOOGLE)) {
                /* At this point if GCE has been detected in dmi, do not report as a VM. It should
                 * be a bare-metal machine */
                v = dmi;
                వెళ్ళు ముగింపు;
        }

        /* Check high-level hypervisor sysfs file */
        v = detect_vm_hypervisor();
        ఒకవేళ (v < 0)
                తిరిగి v;
        ఒకవేళ (v == VIRTUALIZATION_VM_OTHER)
                other = true;
        లేకపోతే ఒకవేళ (v != VIRTUALIZATION_NONE)
                వెళ్ళు ముగింపు;

        v = detect_vm_device_వృక్షం();
        ఒకవేళ (v < 0)
                తిరిగి v;
        ఒకవేళ (v == VIRTUALIZATION_VM_OTHER)
                other = true;
        లేకపోతే ఒకవేళ (v != VIRTUALIZATION_NONE)
                వెళ్ళు ముగింపు;

        v = detect_vm_zvm();
        ఒకవేళ (v < 0)
                తిరిగి v;

ముగింపు:
        /* None of the checks above gave us a clear answer, hence let's now use fallback logic: if hyperv
         * enlightenments are available but the VMM wasn't recognized as anything yet, it's probably
         * Microsoft. */
        ఒకవేళ (v == VIRTUALIZATION_NONE) {
                ఒకవేళ (hyperv)
                        v = VIRTUALIZATION_MICROSOFT;
                లేకపోతే ఒకవేళ (other)
                        v = VIRTUALIZATION_VM_OTHER;
        }

        cached_దొరికింది = v;
        log_debug("Found VM virtualization %s", virtualization_గమ్యం_string(v));
        తిరిగి v;
}

స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి container_పట్టిక[_VIRTUALIZATION_MAX] = {
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

DEFINE_వ్యక్తిగత_STRING_పట్టిక_LOOKUP_FROM_STRING(container, పూర్ణాంకం);

స్థిర పూర్ణాంకం running_లోపల_pidns(శూన్య) {
        పూర్ణాంకం r;

        r = namespace_is_init(NAMESPACE_PID);
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to test if in root PID namespace, ignoring: %m");

        తిరిగి !r;
}

స్థిర Virtualization detect_container_files(శూన్య) {
        స్థిర స్థిరరాశి నిర్మాణం {
                స్థిరరాశి అక్షరం *file_మార్గం;
                Virtualization id;
        } container_file_పట్టిక[] = {
                /* https://github.com/containers/podman/issues/6192 */
                /* https://github.com/containers/podman/issues/3586#issuecomment-661918679 */
                { "/run/.containerenv", VIRTUALIZATION_PODMAN },
                /* https://github.com/moby/moby/issues/18355 */
                /* Docker must be the last in this table, see below. */
                { "/.dockerenv",        VIRTUALIZATION_DOCKER },
        };

        FOREACH_ELEMENT(file, container_file_పట్టిక) {
                ఒకవేళ (access(file->file_మార్గం, F_OK) >= 0)
                        తిరిగి file->id;

                ఒకవేళ (errno != ENOENT)
                        log_debug_errno(errno,
                                        "Checking if %s exists failed, ignoring: %m",
                                        file->file_మార్గం);
        }

        తిరిగి VIRTUALIZATION_NONE;
}

Virtualization detect_container(శూన్య) {
        స్థిర thread_local Virtualization cached_దొరికింది = _VIRTUALIZATION_INVALID;
        _cleanup_free_ అక్షరం *m = ఖాళీ, *o = ఖాళీ, *p = ఖాళీ;
        స్థిరరాశి అక్షరం *e = ఖాళీ;
        Virtualization v;
        పూర్ణాంకం r;

        ఒకవేళ (cached_దొరికింది >= 0)
                తిరిగి cached_దొరికింది;

        /* /proc/vz exists in container and outside of the container, /proc/bc only outside of the container. */
        ఒకవేళ (access("/proc/vz", F_OK) < 0) {
                ఒకవేళ (errno != ENOENT)
                        log_debug_errno(errno, "Failed to check if /proc/vz exists, ignoring: %m");
        } లేకపోతే ఒకవేళ (access("/proc/bc", F_OK) < 0) {
                ఒకవేళ (errno == ENOENT) {
                        v = VIRTUALIZATION_OPENVZ;
                        వెళ్ళు ముగింపు;
                }

                log_debug_errno(errno, "Failed to check if /proc/bc exists, ignoring: %m");
        }

        /* "Official" way of detecting WSL https://github.com/Microsoft/WSL/issues/423#issuecomment-221627364 */
        r = read_one_పంక్తి_file("/proc/sys/kernel/osrelease", &o);
        ఒకవేళ (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/kernel/osrelease, ignoring: %m");
        లేకపోతే ఒకవేళ (అక్షరశ్రేణి_శోధన(o, "Microsoft") || అక్షరశ్రేణి_శోధన(o, "WSL")) {
                v = VIRTUALIZATION_WSL;
                వెళ్ళు ముగింపు;
        }

        /* proot doesn't use PID namespacing, so we can just check if we have a matching tracer for this
         * invocation without worrying about it being elsewhere.
         */
        r = get_proc_క్షేత్రం("/proc/self/status", "TracerPid", &p);
        ఒకవేళ (r < 0)
                log_debug_errno(r, "Failed to read our own trace PID, ignoring: %m");
        లేకపోతే ఒకవేళ (!streq(p, "0")) {
                pid_t ptrace_pid;

                r = parse_pid(p, &ptrace_pid);
                ఒకవేళ (r < 0)
                        log_debug_errno(r, "Failed to parse our own tracer PID, ignoring: %m");
                లేకపోతే {
                        _cleanup_free_ అక్షరం *ptrace_comm = ఖాళీ;
                        స్థిరరాశి అక్షరం *pf;

                        pf = procfs_file_alloca(ptrace_pid, "comm");
                        r = read_one_పంక్తి_file(pf, &ptrace_comm);
                        ఒకవేళ (r < 0)
                                log_debug_errno(r, "Failed to read %s, ignoring: %m", pf);
                        లేకపోతే ఒకవేళ (startswith(ptrace_comm, "proot")) {
                                v = VIRTUALIZATION_PROOT;
                                వెళ్ళు ముగింపు;
                        }
                }
        }

        /* The container manager might have placed this in the /run/host/ hierarchy for us, which is best
         * because we can be consumed just like that, without special privileges. */
        r = read_one_పంక్తి_file("/run/host/container-manager", &m);
        ఒకవేళ (r > 0) {
                e = m;
                వెళ్ళు translate_name;
        }
        ఒకవేళ (!లోపల_సమితి(r, -ENOENT, 0))
                తిరిగి log_debug_errno(r, "Failed to read /run/host/container-manager: %m");

        ఒకవేళ (getpid_cached() == 1) {
                /* If we are PID 1 we can just check our own environment variable, and that's authoritative.
                 * We distinguish three cases:
                 * - the variable is not defined → we jump to other checks
                 * - the variable is defined to an empty value → we are not in a container
                 * - anything else → some container, either one of the known ones or "container-other"
                 */
                e = getenv("container");
                ఒకవేళ (!e)
                        వెళ్ళు check_files;
                ఒకవేళ (isempty(e)) {
                        v = VIRTUALIZATION_NONE;
                        వెళ్ళు ముగింపు;
                }

                వెళ్ళు translate_name;
        }

        /* Otherwise, PID 1 might have dropped this information into a file in /run. This is better than accessing
         * /proc/1/environ, since we don't need CAP_SYS_PTRACE for that. */
        r = read_one_పంక్తి_file("/run/systemd/container", &m);
        ఒకవేళ (r > 0) {
                e = m;
                వెళ్ళు translate_name;
        }
        ఒకవేళ (!లోపల_సమితి(r, -ENOENT, 0))
                తిరిగి log_debug_errno(r, "Failed to read /run/systemd/container: %m");

        /* Fallback for cases where PID 1 was not systemd (for example, cases where init=/bin/sh is used. */
        r = getenv_for_pid(1, "container", &m);
        ఒకవేళ (r > 0) {
                e = m;
                వెళ్ళు translate_name;
        }
        ఒకవేళ (r < 0) /* This only works if we have CAP_SYS_PTRACE, hence let's better ignore failures here */
                log_debug_errno(r, "Failed to read $container of PID 1, ignoring: %m");

check_files:
        /* Check for existence of some well-known files. We only do this after checking
         * for other specific container managers, otherwise we risk mistaking another
         * container manager for Docker: the /.dockerenv file could inadvertently end up
         * in a file system image. */
        v = detect_container_files();
        ఒకవేళ (v < 0)
                తిరిగి v;
        ఒకవేళ (v != VIRTUALIZATION_NONE)
                వెళ్ళు ముగింపు;

        /* Finally, the root pid namespace has an hardcoded inode number of 0xEFFFFFFC since kernel 3.8, so
         * if all else fails we can check the inode number of our pid namespace and compare it. */
        ఒకవేళ (running_లోపల_pidns() > 0) {
                log_debug("Running in a pid namespace, assuming unknown container manager.");
                v = VIRTUALIZATION_CONTAINER_OTHER;
                వెళ్ళు ముగింపు;
        }

        /* If none of that worked, give up, assume no container manager. */
        v = VIRTUALIZATION_NONE;
        వెళ్ళు ముగింపు;

translate_name:
        ఒకవేళ (streq(e, "oci")) {
                /* Some images hardcode container=oci, but OCI is not a specific container manager.
                 * Try to detect one based on well-known files. */
                v = detect_container_files();
                ఒకవేళ (v == VIRTUALIZATION_NONE)
                        v = VIRTUALIZATION_CONTAINER_OTHER;
                వెళ్ళు ముగింపు;
        }
        v = container_from_string(e);
        ఒకవేళ (v < 0)
                v = VIRTUALIZATION_CONTAINER_OTHER;

ముగింపు:
        log_debug("Found container virtualization %s.", virtualization_గమ్యం_string(v));
        cached_దొరికింది = v;
        తిరిగి v;
}

Virtualization detect_virtualization(శూన్య) {
        పూర్ణాంకం v;

        v = detect_container();
        ఒకవేళ (v != VIRTUALIZATION_NONE)
                తిరిగి v;

        తిరిగి detect_vm();
}

పూర్ణాంకం running_లోపల_userns(శూన్య) {
        పూర్ణాంకం r;

        r = namespace_is_init(NAMESPACE_USER);
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to test if in root user namespace, ignoring: %m");

        తిరిగి !r;
}

పూర్ణాంకం running_లోపల_chroot(శూన్య) {
        పూర్ణాంకం r;

        /* If we're PID1, /proc may not be mounted (and most likely we're not in a chroot). But PID1 will
         * mount /proc, so all other programs can assume that if /proc is *not* available, we're in some
         * chroot. */

        r = getenv_bool("SYSTEMD_IN_CHROOT");
        ఒకవేళ (r >= 0)
                తిరిగి r > 0;
        ఒకవేళ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_IN_CHROOT, ignoring: %m");

        /* Deprecated but kept for backwards compatibility. */
        ఒకవేళ (getenv_bool("SYSTEMD_IGNORE_CHROOT") > 0)
                తిరిగి 0;

        r = inode_same("/proc/1/root", "/", /* flags= */ 0);
        ఒకవేళ (r == -ENOENT) {
                r = proc_mounted();
                ఒకవేళ (r == 0) {
                        ఒకవేళ (getpid_cached() == 1)
                                తిరిగి false; /* We will mount /proc, assuming we're not in a chroot. */

                        log_debug("/proc/ is not mounted, assuming we're in a chroot.");
                        తిరిగి true;
                }
                ఒకవేళ (r > 0) /* If we have fake /proc/, we can't do the check properly. */
                        తిరిగి -ENOSYS;
        }
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to check if /proc/1/root and / are the same inode: %m");

        తిరిగి r == 0;
}

#ఒకవేళ defined(__i386__) || defined(__x86_64__)
నిర్మాణం cpuid_పట్టిక_నమోదు {
        uint32_t flag_బిట్టు;
        స్థిరరాశి అక్షరం *name;
};

స్థిర స్థిరరాశి నిర్మాణం cpuid_పట్టిక_నమోదు leaf1_edx[] = {
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

స్థిర స్థిరరాశి నిర్మాణం cpuid_పట్టిక_నమోదు leaf1_ecx[] = {
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

స్థిర స్థిరరాశి నిర్మాణం cpuid_పట్టిక_నమోదు leaf7_ebx[] = {
        {  3, "bmi1"   },
        {  5, "avx2"   },
        {  8, "bmi2"   },
        { 18, "rdseed" },
        { 19, "adx"    },
        { 29, "sha_ni" },
};

స్థిర స్థిరరాశి నిర్మాణం cpuid_పట్టిక_నమోదు leaf81_edx[] = {
        { 11, "syscall" },
        { 27, "rdtscp"  },
        { 29, "lm"      },
};

స్థిర స్థిరరాశి నిర్మాణం cpuid_పట్టిక_నమోదు leaf81_ecx[] = {
        {  0, "lahf_lm" },
        {  5, "abm"     },
};

స్థిర స్థిరరాశి నిర్మాణం cpuid_పట్టిక_నమోదు leaf87_edx[] = {
        {  8, "constant_tsc" },
};

స్థిర bool given_flag_లోపల_సమితి(స్థిరరాశి అక్షరం *flag, స్థిరరాశి నిర్మాణం cpuid_పట్టిక_నమోదు *సమితి, size_t సమితి_కొలత, uint32_t val) {
        ప్రతిజ్ఞా(సమితి);

        కొరకు (size_t i = 0; i < సమితి_కొలత; i++) {
                ఒకవేళ ((UINT32_C(1) << సమితి[i].flag_బిట్టు) & val &&
                                streq(flag, సమితి[i].name))
                        తిరిగి true;
        }
        తిరిగి false;
}

స్థిర bool real_has_cpu_with_flag(స్థిరరాశి అక్షరం *flag) {
        uint32_t eax, ebx, ecx, edx;

        ఒకవేళ (__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
                ఒకవేళ (given_flag_లోపల_సమితి(flag, leaf1_ecx, ELEMENTSOF(leaf1_ecx), ecx))
                        తిరిగి true;

                ఒకవేళ (given_flag_లోపల_సమితి(flag, leaf1_edx, ELEMENTSOF(leaf1_edx), edx))
                        తిరిగి true;
        }

        ఒకవేళ (__get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx)) {
                ఒకవేళ (given_flag_లోపల_సమితి(flag, leaf7_ebx, ELEMENTSOF(leaf7_ebx), ebx))
                        తిరిగి true;
        }

        ఒకవేళ (__get_cpuid(0x80000001U, &eax, &ebx, &ecx, &edx)) {
                ఒకవేళ (given_flag_లోపల_సమితి(flag, leaf81_ecx, ELEMENTSOF(leaf81_ecx), ecx))
                        తిరిగి true;

                ఒకవేళ (given_flag_లోపల_సమితి(flag, leaf81_edx, ELEMENTSOF(leaf81_edx), edx))
                        తిరిగి true;
        }

        ఒకవేళ (__get_cpuid(0x80000007U, &eax, &ebx, &ecx, &edx))
                ఒకవేళ (given_flag_లోపల_సమితి(flag, leaf87_edx, ELEMENTSOF(leaf87_edx), edx))
                        తిరిగి true;

        తిరిగి false;
}
#అయితే_ముగింపు

#ఒకవేళ defined(__aarch64__)
నిర్మాణం hwcap_పట్టిక_నమోదు {
        uint64_t flag_ముసుగు;
        స్థిరరాశి అక్షరం *name;
};

స్థిర స్థిరరాశి నిర్మాణం hwcap_పట్టిక_నమోదు hwcap[] = {
        { HWCAP_FP,           "fp"          },
        { HWCAP_ASIMD,        "asimd"       },
        { HWCAP_EVTSTRM,      "evtstrm"     },
        { HWCAP_AES,          "aes"         },
        { HWCAP_PMULL,        "pmull"       },
        { HWCAP_SHA1,         "sha1"        },
        { HWCAP_SHA2,         "sha2"        },
        { HWCAP_చక్రీయ_తనిఖీ_32,        "crc32"       },
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

స్థిర స్థిరరాశి నిర్మాణం hwcap_పట్టిక_నమోదు hwcap2[] = {
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

స్థిర స్థిరరాశి నిర్మాణం hwcap_పట్టిక_నమోదు hwcap3[] = {
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

స్థిర bool given_flag_లోపల_hwcap_సమితి(
                స్థిరరాశి అక్షరం *flag,
                స్థిరరాశి నిర్మాణం hwcap_పట్టిక_నమోదు *సమితి,
                size_t సమితి_కొలత,
                గుర్తులేని దీర్ఘ val) {

        ప్రతిజ్ఞా(సమితి);

        కొరకు (size_t i = 0; i < సమితి_కొలత; i++)
                ఒకవేళ ((సమితి[i].flag_ముసుగు & val) && streq(flag, సమితి[i].name))
                        తిరిగి true;

        తిరిగి false;
}

స్థిర bool real_has_cpu_with_flag(స్థిరరాశి అక్షరం *flag) {
        గుర్తులేని దీర్ఘ val;

        val = getauxval(AT_HWCAP);
        ఒకవేళ (given_flag_లోపల_hwcap_సమితి(flag, hwcap, ELEMENTSOF(hwcap), val))
                తిరిగి true;

        val = getauxval(AT_HWCAP2);
        ఒకవేళ (given_flag_లోపల_hwcap_సమితి(flag, hwcap2, ELEMENTSOF(hwcap2), val))
                తిరిగి true;

        val = getauxval(AT_HWCAP3);
        ఒకవేళ (given_flag_లోపల_hwcap_సమితి(flag, hwcap3, ELEMENTSOF(hwcap3), val))
                తిరిగి true;

        తిరిగి false;
}
#అయితే_ముగింపు

bool has_cpu_with_flag(స్థిరరాశి అక్షరం *flag) {
        /* Check the architecture-specific CPU feature interface when available. */
#ఒకవేళ defined(__i386__) || defined(__x86_64__) || defined(__aarch64__)
        తిరిగి real_has_cpu_with_flag(flag);
#లేకపోతే
        తిరిగి false;
#అయితే_ముగింపు
}

పూర్ణాంకం cpu_తెలిసిన_features(అక్షరం ***ret) {
        ప్రతిజ్ఞా(ret);

#ఒకవేళ defined(__i386__) || defined(__x86_64__)
        స్థిర స్థిరరాశి నిర్మాణం {
                స్థిరరాశి నిర్మాణం cpuid_పట్టిక_నమోదు *పట్టిక;
                size_t n_entries;
        } tables[] = {
                { leaf1_edx,  ELEMENTSOF(leaf1_edx),  },
                { leaf1_ecx,  ELEMENTSOF(leaf1_ecx),  },
                { leaf7_ebx,  ELEMENTSOF(leaf7_ebx),  },
                { leaf81_ecx, ELEMENTSOF(leaf81_ecx), },
                { leaf81_edx, ELEMENTSOF(leaf81_edx), },
                { leaf87_edx, ELEMENTSOF(leaf87_edx), },
        };
#లేదావేళ defined(__aarch64__)
        స్థిర స్థిరరాశి నిర్మాణం {
                స్థిరరాశి నిర్మాణం hwcap_పట్టిక_నమోదు *పట్టిక;
                size_t n_entries;
        } tables[] = {
                { hwcap,  ELEMENTSOF(hwcap),  },
                { hwcap2, ELEMENTSOF(hwcap2), },
                { hwcap3, ELEMENTSOF(hwcap3), },
        };
#అయితే_ముగింపు

#ఒకవేళ defined(__i386__) || defined(__x86_64__) || defined(__aarch64__)
        _cleanup_strv_free_ అక్షరం **features = ఖాళీ;
        size_t n = 0;
        పూర్ణాంకం r;

        FOREACH_ELEMENT(పట్టిక, tables)
                FOREACH_ARRAY(i, పట్టిక->పట్టిక, పట్టిక->n_entries) {
                        r = strv_extend_with_కొలత(&features, &n, i->name);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                }

        *ret = TAKE_PTR(features);
#లేకపోతే
        *ret = ఖాళీ;
#అయితే_ముగింపు
        తిరిగి 0;
}

స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి virtualization_పట్టిక[_VIRTUALIZATION_MAX] = {
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

DEFINE_STRING_పట్టిక_LOOKUP(virtualization, Virtualization);
