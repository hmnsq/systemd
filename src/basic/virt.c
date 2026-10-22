/* SPDX-License-Identifier: LGPL-2.1-or-later */

#যদি defined(__i386__) || defined(__x86_64__)
#অন্তর্ভাব <cpuid.h>
#endif
#যদি defined(__aarch64__)
#অন্তর্ভাব <sys/auxv.h>
#endif
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <threads.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "dirent-util.h"        /* IWYU pragma: keep */
#অন্তর্ভাব "env-util.h"
#অন্তর্ভাব "extract-word.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "fileio.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "namespace-util.h"
#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "process-util.h"
#অন্তর্ভাব "stat-util.h"
#অন্তর্ভাব "string-table.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"
#অন্তর্ভাব "virt.h"

গণনা {
      SMBIOS_VM_BIT_SET,
      SMBIOS_VM_BIT_UNSET,
      SMBIOS_VM_BIT_UNKNOWN,
};

স্থির Virtualization detect_vm_cpuid(শূন্য) {

        /* CPUID is an x86 specific interface. */
#যদি defined(__i386__) || defined(__x86_64__)

        স্থির ধ্রুব সংরচনা {
                ধ্রুব বর্ণ sig[13];
                Virtualization id;
        } vm_table[] = {
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
        যদি (__get_cpuid(1, &eax, &ebx, &ecx, &edx) == 0)
                প্রত্যাগম VIRTUALIZATION_NONE;

        hypervisor = ecx & 0x80000000U;

        যদি (hypervisor) {
                সঙ্ঘ {
                        uint32_t sig32[3];
                        বর্ণ text[13];
                } sig = {};

                /* There is a hypervisor, see what it is */
                __cpuid(0x40000000U, eax, ebx, ecx, edx);

                sig.sig32[0] = ebx;
                sig.sig32[1] = ecx;
                sig.sig32[2] = edx;

                log_debug("Virtualization found, CPUID=%s", sig.text);

                FOREACH_ELEMENT(vm, vm_table)
                        যদি (memcmp_nn(sig.text, পরিমাণ(sig.text),
                                      vm->sig, পরিমাণ(vm->sig)) == 0)
                                প্রত্যাগম vm->id;

                log_debug("Unknown virtualization with CPUID=%s. Add to vm_table[]?", sig.text);
                প্রত্যাগম VIRTUALIZATION_VM_OTHER;
        }
#endif
        log_debug("No virtualization found in CPUID");

        প্রত্যাগম VIRTUALIZATION_NONE;
}

স্থির Virtualization detect_vm_device_বৃক্ষ(শূন্য) {
#যদি defined(__arm__) || defined(__aarch64__) || defined(__powerpc__) || defined(__powerpc64__) || defined(__riscv)
        _cleanup_free_ বর্ণ *hvtype = রিক্ত;
        পূর্ণাঙ্ক r;

        r = read_one_সারি_file("/proc/device-tree/hypervisor/compatible", &hvtype);
        যদি (r == -ENOENT) {
                যদি (access("/proc/device-tree/ibm,partition-name", F_OK) == 0 &&
                    access("/proc/device-tree/hmc-managed?", F_OK) == 0 &&
                    access("/proc/device-tree/chosen/qemu,graphic-width", F_OK) != 0)
                        প্রত্যাগম VIRTUALIZATION_POWERVM;

                _cleanup_closedir_ DIR *dir = opendir("/proc/device-tree");
                যদি (!dir) {
                        যদি (errno == ENOENT) {
                                log_debug_errno(errno, "/proc/device-tree/ does not exist");
                                প্রত্যাগম VIRTUALIZATION_NONE;
                        }
                        প্রত্যাগম log_debug_errno(errno, "Opening /proc/device-tree/ failed: %m");
                }

                FOREACH_DIRENT(de, dir, প্রত্যাগম log_debug_errno(errno, "Failed to enumerate /proc/device-tree/ contents: %m"))
                        যদি (strstr(de->d_name, "fw-cfg")) {
                                log_debug("Virtualization QEMU: \"fw-cfg\" present in /proc/device-tree/%s", de->d_name);
                                প্রত্যাগম VIRTUALIZATION_QEMU;
                        }

                _cleanup_free_ বর্ণ *compat = রিক্ত;
                r = read_one_সারি_file("/proc/device-tree/compatible", &compat);
                যদি (r < 0 && r != -ENOENT)
                        প্রত্যাগম log_debug_errno(r, "Failed to read /proc/device-tree/compatible: %m");
                যদি (r >= 0) {
                        যদি (streq(compat, "qemu,pseries")) {
                                log_debug("Virtualization %s found in /proc/device-tree/compatible", compat);
                                প্রত্যাগম VIRTUALIZATION_QEMU;
                        }
                        যদি (streq(compat, "linux,dummy-virt")) {
                                /* https://www.kernel.org/doc/Documentation/devicetree/bindings/arm/linux%2Cdummy-virt.yaml */
                                log_debug("Generic virtualization %s found in /proc/device-tree/compatible", compat);
                                প্রত্যাগম VIRTUALIZATION_VM_OTHER;
                        }
                }

                log_debug("No virtualization found in /proc/device-tree/*");
                প্রত্যাগম VIRTUALIZATION_NONE;
        } অন্যথা যদি (r < 0)
                প্রত্যাগম r;

        log_debug("Virtualization %s found in /proc/device-tree/hypervisor/compatible", hvtype);
        যদি (streq(hvtype, "linux,kvm"))
                প্রত্যাগম VIRTUALIZATION_KVM;
        অন্যথা যদি (strstr(hvtype, "xen"))
                প্রত্যাগম VIRTUALIZATION_XEN;
        অন্যথা যদি (strstr(hvtype, "vmware"))
                প্রত্যাগম VIRTUALIZATION_VMWARE;
        অন্যথা
                প্রত্যাগম VIRTUALIZATION_VM_OTHER;
#অন্যথা
        log_debug("This platform does not support /proc/device-tree");
        প্রত্যাগম VIRTUALIZATION_NONE;
#endif
}

#যদি defined(__i386__) || defined(__x86_64__) || defined(__arm__) || defined(__aarch64__) || defined(__loongarch_lp64) || defined(__riscv)
স্থির Virtualization detect_vm_dmi_vendor(শূন্য) {
        স্থির ধ্রুব বর্ণ* ধ্রুব dmi_vendors[] = {
                "/sys/class/dmi/id/product_name", /* Test this before sys_vendor to detect KVM over QEMU */
                "/sys/class/dmi/id/sys_vendor",
                "/sys/class/dmi/id/board_vendor",
                "/sys/class/dmi/id/bios_vendor",
                "/sys/class/dmi/id/product_version", /* For Hyper-V VMs test */
                রিক্ত
        };

        স্থির ধ্রুব সংরচনা {
                ধ্রুব বর্ণ *vendor;
                Virtualization id;
        } dmi_vendor_table[] = {
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
        পূর্ণাঙ্ক r;

        STRV_FOREACH(vendor, dmi_vendors) {
                _cleanup_free_ বর্ণ *s = রিক্ত;

                r = read_one_সারি_file(*vendor, &s);
                যদি (r == -ENOENT)
                        অনুবর্ত;
                যদি (r < 0)
                        প্রত্যাগম r;

                FOREACH_ELEMENT(dmi_vendor, dmi_vendor_table)
                        যদি (startswith(s, dmi_vendor->vendor)) {
                                log_debug("Virtualization %s found in DMI (%s)", s, *vendor);
                                প্রত্যাগম dmi_vendor->id;
                        }
        }
        log_debug("No virtualization found in DMI vendor table.");
        প্রত্যাগম VIRTUALIZATION_NONE;
}

স্থির পূর্ণাঙ্ক detect_vm_smbios(শূন্য) {
        /* The SMBIOS BIOS Characteristics Extension Byte 2 (Section 2.1.2.2 of
         * https://www.dmtf.org/sites/default/files/standards/documents/DSP0134_3.4.0.pdf), specifies that
         * the 4th bit being set indicates a VM. The BIOS Characteristics table is exposed via the kernel in
         * /sys/firmware/dmi/entries/0-0. Note that in the general case, this bit being unset should not
         * imply that the system is running on bare-metal.  For example, QEMU 3.1.0 (with or without KVM)
         * with SeaBIOS does not set this bit. */
        _cleanup_free_ বর্ণ *s = রিক্ত;
        size_t readsize;
        পূর্ণাঙ্ক r;

        r = read_full_virtual_file("/sys/firmware/dmi/entries/0-0/raw", &s, &readsize);
        যদি (r < 0) {
                log_debug_errno(r, "Unable to read /sys/firmware/dmi/entries/0-0/raw, "
                                "using the virtualization information found in DMI vendor table, ignoring: %m");
                প্রত্যাগম SMBIOS_VM_BIT_UNKNOWN;
        }
        যদি (readsize < 20 || s[1] < 20) {
                /* The spec indicates that byte 1 contains the size of the table, 0x12 + the number of
                 * extension bytes. The data we're interested in is in extension byte 2, which would be at
                 * 0x13. If we didn't read that much data, or if the BIOS indicates that we don't have that
                 * much data, we don't infer anything from the SMBIOS. */
                log_debug("Only read %zu bytes from /sys/firmware/dmi/entries/0-0/raw (expected 20). "
                          "Using the virtualization information found in DMI vendor table.", readsize);
                প্রত্যাগম SMBIOS_VM_BIT_UNKNOWN;
        }

        uint8_t byte = (uint8_t) s[19];
        যদি (byte & (1U<<4)) {
                log_debug("DMI BIOS Extension table indicates virtualization.");
                প্রত্যাগম SMBIOS_VM_BIT_SET;
        }
        log_debug("DMI BIOS Extension table does not indicate virtualization.");
        প্রত্যাগম SMBIOS_VM_BIT_UNSET;
}
#endif /* defined(__i386__) || defined(__x86_64__) || defined(__arm__) || defined(__aarch64__) || defined(__loongarch_lp64) */

স্থির Virtualization detect_vm_dmi(শূন্য) {
#যদি defined(__i386__) || defined(__x86_64__) || defined(__arm__) || defined(__aarch64__) || defined(__loongarch_lp64) || defined(__riscv)

        পূর্ণাঙ্ক r;
        r = detect_vm_dmi_vendor();

        /* The DMI vendor tables in /sys/class/dmi/id don't help us distinguish between Amazon EC2
         * virtual machines and bare-metal instances, so we need to look at SMBIOS. */
        যদি (r == VIRTUALIZATION_AMAZON) {
                বিকল্প (detect_vm_smbios()) {
                পক্ষ SMBIOS_VM_BIT_SET:
                        প্রত্যাগম VIRTUALIZATION_AMAZON;
                পক্ষ SMBIOS_VM_BIT_UNSET:
                        প্রত্যাগম VIRTUALIZATION_NONE;
                পক্ষ SMBIOS_VM_BIT_UNKNOWN: {
                        /* The DMI information we are after is only accessible to the root user,
                         * so we fallback to using the product name which is less restricted
                         * to distinguish metal systems from virtualized instances */
                        _cleanup_free_ বর্ণ *s = রিক্ত;
                        ধ্রুব বর্ণ *e;

                        r = read_full_virtual_file("/sys/class/dmi/id/product_name", &s, রিক্ত);
                        /* In EC2, virtualized is much more common than metal, so if for some reason
                         * we fail to read the DMI data, assume we are virtualized. */
                        যদি (r < 0) {
                                log_debug_errno(r, "Can't read /sys/class/dmi/id/product_name,"
                                                " assuming virtualized: %m");
                                প্রত্যাগম VIRTUALIZATION_AMAZON;
                        }
                        e = strstrafter(truncate_nl(s), ".metal");
                        যদি (e && IN_SET(*e, 0, '-')) {
                                log_debug("DMI product name has '.metal', assuming no virtualization");
                                প্রত্যাগম VIRTUALIZATION_NONE;
                        } অন্যথা
                                প্রত্যাগম VIRTUALIZATION_AMAZON;
                }
                স্বভাব:
                        assert_not_reached();
              }
        }

        /* If we haven't identified a VM, but the firmware indicates that there is one, indicate as much. We
         * have no further information about what it is. */
        যদি (r == VIRTUALIZATION_NONE && detect_vm_smbios() == SMBIOS_VM_BIT_SET)
                প্রত্যাগম VIRTUALIZATION_VM_OTHER;
        প্রত্যাগম r;
#অন্যথা
        প্রত্যাগম VIRTUALIZATION_NONE;
#endif
}

#define XENFEAT_dom0 11 /* xen/include/public/features.h */
#define PATH_FEATURES "/sys/hypervisor/properties/features"
/* Returns -errno, or 0 for domU, or 1 for dom0 */
স্থির পূর্ণাঙ্ক detect_vm_xen_dom0(শূন্য) {
        _cleanup_free_ বর্ণ *domcap = রিক্ত;
        পূর্ণাঙ্ক r;

        r = read_one_সারি_file(PATH_FEATURES, &domcap);
        যদি (r < 0 && r != -ENOENT)
                প্রত্যাগম r;
        যদি (r >= 0) {
                unsigned long features;

                /* Here, we need to use sscanf() instead of safe_atoul()
                 * as the string lacks the leading "0x". */
                r = sscanf(domcap, "%lx", &features);
                যদি (r == 1) {
                        r = !!(features & (1U << XENFEAT_dom0));
                        log_debug("Virtualization XEN, found %s with value %08lx, "
                                  "XENFEAT_dom0 (indicating the 'hardware domain') is%s set.",
                                  PATH_FEATURES, features, r ? "" : " not");
                        প্রত্যাগম r;
                }
                log_debug("Virtualization XEN, found %s, unhandled content '%s'",
                          PATH_FEATURES, domcap);
        }

        r = read_one_সারি_file("/proc/xen/capabilities", &domcap);
        যদি (r == -ENOENT) {
                log_debug("Virtualization XEN because /proc/xen/capabilities does not exist");
                প্রত্যাগম 0;
        }
        যদি (r < 0)
                প্রত্যাগম r;

        প্রতি (ধ্রুব বর্ণ *i = domcap;;) {
                _cleanup_free_ বর্ণ *cap = রিক্ত;

                r = extract_first_word(&i, &cap, ",", 0);
                যদি (r < 0)
                        প্রত্যাগম r;
                যদি (r == 0) {
                        log_debug("Virtualization XEN DomU found (/proc/xen/capabilities)");
                        প্রত্যাগম 0;
                }

                যদি (streq(cap, "control_d")) {
                        log_debug("Virtualization XEN Dom0 ignored (/proc/xen/capabilities)");
                        প্রত্যাগম 1;
                }
        }
}

স্থির Virtualization detect_vm_xen(শূন্য) {
        /* The presence of /proc/xen indicates some form of a Xen domain
           The check for Dom0 is handled outside this function */
        যদি (access("/proc/xen", F_OK) < 0) {
                log_debug("Virtualization XEN not found, /proc/xen does not exist");
                প্রত্যাগম VIRTUALIZATION_NONE;
        }
        log_debug("Virtualization XEN found (/proc/xen exists)");
        প্রত্যাগম VIRTUALIZATION_XEN;
}

স্থির Virtualization detect_vm_hypervisor(শূন্য) {
        _cleanup_free_ বর্ণ *hvtype = রিক্ত;
        পূর্ণাঙ্ক r;

        r = read_one_সারি_file("/sys/hypervisor/type", &hvtype);
        যদি (r == -ENOENT)
                প্রত্যাগম VIRTUALIZATION_NONE;
        যদি (r < 0)
                প্রত্যাগম r;

        log_debug("Virtualization %s found in /sys/hypervisor/type", hvtype);

        যদি (streq(hvtype, "xen"))
                প্রত্যাগম VIRTUALIZATION_XEN;
        অন্যথা
                প্রত্যাগম VIRTUALIZATION_VM_OTHER;
}

স্থির Virtualization detect_vm_uml(শূন্য) {
        _cleanup_fclose_ FILE *f = রিক্ত;
        পূর্ণাঙ্ক r;

        /* Detect User-Mode Linux by reading /proc/cpuinfo */
        f = fopen("/proc/cpuinfo", "re");
        যদি (!f) {
                যদি (errno == ENOENT) {
                        log_debug("/proc/cpuinfo not found, assuming no UML virtualization.");
                        প্রত্যাগম VIRTUALIZATION_NONE;
                }
                প্রত্যাগম -errno;
        }

        প্রতি (;;) {
                _cleanup_free_ বর্ণ *সারি = রিক্ত;
                ধ্রুব বর্ণ *t;

                r = read_সারি(f, LONG_সারি_MAX, &সারি);
                যদি (r < 0)
                        প্রত্যাগম r;
                যদি (r == 0)
                        ভঙ্গ;

                t = startswith(সারি, "vendor_id\t: ");
                যদি (t) {
                        যদি (startswith(t, "User Mode Linux")) {
                                log_debug("UML virtualization found in /proc/cpuinfo");
                                প্রত্যাগম VIRTUALIZATION_UML;
                        }

                        ভঙ্গ;
                }
        }

        log_debug("UML virtualization not found in /proc/cpuinfo.");
        প্রত্যাগম VIRTUALIZATION_NONE;
}

স্থির Virtualization detect_vm_zvm(শূন্য) {

#যদি defined(__s390__)
        _cleanup_free_ বর্ণ *t = রিক্ত;
        পূর্ণাঙ্ক r;

        r = get_proc_field("/proc/sysinfo", "VM00 Control Program", &t);
        যদি (IN_SET(r, -ENOENT, -ENODATA))
                প্রত্যাগম VIRTUALIZATION_NONE;
        যদি (r < 0)
                প্রত্যাগম r;

        log_debug("Virtualization %s found in /proc/sysinfo", t);
        যদি (streq(t, "z/VM"))
                প্রত্যাগম VIRTUALIZATION_ZVM;
        অন্যথা
                প্রত্যাগম VIRTUALIZATION_KVM;
#অন্যথা
        log_debug("This platform does not support /proc/sysinfo");
        প্রত্যাগম VIRTUALIZATION_NONE;
#endif
}

/* Returns a short identifier for the various VM implementations */
Virtualization detect_vm(শূন্য) {
        স্থির thread_local Virtualization cached_found = _VIRTUALIZATION_INVALID;
        bool other = false, hyperv = false;
        পূর্ণাঙ্ক xen_dom0 = 0;
        Virtualization v, dmi;

        যদি (cached_found >= 0)
                প্রত্যাগম cached_found;

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
        যদি (IN_SET(dmi,
                   VIRTUALIZATION_ORACLE,
                   VIRTUALIZATION_XEN,
                   VIRTUALIZATION_AMAZON,
                   /* Unable to distinguish a GCE machine from a VM to bare-metal
                    * for non-x86 architectures due to its necessity for cpuid
                    * detection, which functions solely on x86 platforms. Report
                    * as a VM for other architectures. */
#যদি !defined(__i386__) && !defined(__x86_64__)
                   VIRTUALIZATION_GOOGLE,
#endif
                   VIRTUALIZATION_PARALLELS)) {
                v = dmi;
                গচ্ছ finish;
        }

        /* Detect UML */
        v = detect_vm_uml();
        যদি (v < 0)
                প্রত্যাগম v;
        যদি (v != VIRTUALIZATION_NONE)
                গচ্ছ finish;

        /* Detect Xen */
        v = detect_vm_xen();
        যদি (v < 0)
                প্রত্যাগম v;
        যদি (v == VIRTUALIZATION_XEN) {
                 /* If we are Dom0, then we expect to not report as a VM. However, as we might be nested
                  * inside another hypervisor which can be detected via the CPUID check, wait to report this
                  * until after the CPUID check. */
                xen_dom0 = detect_vm_xen_dom0();
                যদি (xen_dom0 < 0)
                        প্রত্যাগম xen_dom0;
                যদি (xen_dom0 == 0)
                        গচ্ছ finish;
        } অন্যথা যদি (v != VIRTUALIZATION_NONE)
                assert_not_reached();

        /* Detect from CPUID */
        v = detect_vm_cpuid();
        যদি (v < 0)
                প্রত্যাগম v;
        যদি (v == VIRTUALIZATION_MICROSOFT)
                /* QEMU sets the CPUID string to hyperv's, in case it provides hyperv enlightenments. Let's
                 * hence not return Microsoft here but just use the other mechanisms first to make a better
                 * decision. */
                hyperv = true;
        অন্যথা যদি (v == VIRTUALIZATION_VM_OTHER)
                other = true;
        অন্যথা যদি (v == VIRTUALIZATION_KVM && dmi == VIRTUALIZATION_GOOGLE)
                /* The DMI vendor tables in /sys/class/dmi/id don't help us distinguish between GCE
                 * virtual machines and bare-metal instances, so we need to look at hypervisor. */
                প্রত্যাগম VIRTUALIZATION_GOOGLE;
        অন্যথা যদি (v != VIRTUALIZATION_NONE)
                গচ্ছ finish;

        /* If we are in Dom0 and have not yet finished, finish with the result of detect_vm_cpuid */
        যদি (xen_dom0 > 0)
                গচ্ছ finish;

        /* Now, let's get back to DMI */
        যদি (dmi < 0)
                প্রত্যাগম dmi;
        যদি (dmi == VIRTUALIZATION_VM_OTHER)
                other = true;
        অন্যথা যদি (!IN_SET(dmi, VIRTUALIZATION_NONE, VIRTUALIZATION_GOOGLE)) {
                /* At this point if GCE has been detected in dmi, do not report as a VM. It should
                 * be a bare-metal machine */
                v = dmi;
                গচ্ছ finish;
        }

        /* Check high-level hypervisor sysfs file */
        v = detect_vm_hypervisor();
        যদি (v < 0)
                প্রত্যাগম v;
        যদি (v == VIRTUALIZATION_VM_OTHER)
                other = true;
        অন্যথা যদি (v != VIRTUALIZATION_NONE)
                গচ্ছ finish;

        v = detect_vm_device_বৃক্ষ();
        যদি (v < 0)
                প্রত্যাগম v;
        যদি (v == VIRTUALIZATION_VM_OTHER)
                other = true;
        অন্যথা যদি (v != VIRTUALIZATION_NONE)
                গচ্ছ finish;

        v = detect_vm_zvm();
        যদি (v < 0)
                প্রত্যাগম v;

finish:
        /* None of the checks above gave us a clear answer, hence let's now use fallback logic: if hyperv
         * enlightenments are available but the VMM wasn't recognized as anything yet, it's probably
         * Microsoft. */
        যদি (v == VIRTUALIZATION_NONE) {
                যদি (hyperv)
                        v = VIRTUALIZATION_MICROSOFT;
                অন্যথা যদি (other)
                        v = VIRTUALIZATION_VM_OTHER;
        }

        cached_found = v;
        log_debug("Found VM virtualization %s", virtualization_to_string(v));
        প্রত্যাগম v;
}

স্থির ধ্রুব বর্ণ *ধ্রুব container_table[_VIRTUALIZATION_MAX] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_FROM_STRING(container, পূর্ণাঙ্ক);

স্থির পূর্ণাঙ্ক running_in_pidns(শূন্য) {
        পূর্ণাঙ্ক r;

        r = namespace_is_init(NAMESPACE_PID);
        যদি (r < 0)
                প্রত্যাগম log_debug_errno(r, "Failed to test if in root PID namespace, ignoring: %m");

        প্রত্যাগম !r;
}

স্থির Virtualization detect_container_files(শূন্য) {
        স্থির ধ্রুব সংরচনা {
                ধ্রুব বর্ণ *file_path;
                Virtualization id;
        } container_file_table[] = {
                /* https://github.com/containers/podman/issues/6192 */
                /* https://github.com/containers/podman/issues/3586#issuecomment-661918679 */
                { "/run/.containerenv", VIRTUALIZATION_PODMAN },
                /* https://github.com/moby/moby/issues/18355 */
                /* Docker must be the last in this table, see below. */
                { "/.dockerenv",        VIRTUALIZATION_DOCKER },
        };

        FOREACH_ELEMENT(file, container_file_table) {
                যদি (access(file->file_path, F_OK) >= 0)
                        প্রত্যাগম file->id;

                যদি (errno != ENOENT)
                        log_debug_errno(errno,
                                        "Checking if %s exists failed, ignoring: %m",
                                        file->file_path);
        }

        প্রত্যাগম VIRTUALIZATION_NONE;
}

Virtualization detect_container(শূন্য) {
        স্থির thread_local Virtualization cached_found = _VIRTUALIZATION_INVALID;
        _cleanup_free_ বর্ণ *m = রিক্ত, *o = রিক্ত, *p = রিক্ত;
        ধ্রুব বর্ণ *e = রিক্ত;
        Virtualization v;
        পূর্ণাঙ্ক r;

        যদি (cached_found >= 0)
                প্রত্যাগম cached_found;

        /* /proc/vz exists in container and outside of the container, /proc/bc only outside of the container. */
        যদি (access("/proc/vz", F_OK) < 0) {
                যদি (errno != ENOENT)
                        log_debug_errno(errno, "Failed to check if /proc/vz exists, ignoring: %m");
        } অন্যথা যদি (access("/proc/bc", F_OK) < 0) {
                যদি (errno == ENOENT) {
                        v = VIRTUALIZATION_OPENVZ;
                        গচ্ছ finish;
                }

                log_debug_errno(errno, "Failed to check if /proc/bc exists, ignoring: %m");
        }

        /* "Official" way of detecting WSL https://github.com/Microsoft/WSL/issues/423#issuecomment-221627364 */
        r = read_one_সারি_file("/proc/sys/kernel/osrelease", &o);
        যদি (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/kernel/osrelease, ignoring: %m");
        অন্যথা যদি (strstr(o, "Microsoft") || strstr(o, "WSL")) {
                v = VIRTUALIZATION_WSL;
                গচ্ছ finish;
        }

        /* proot doesn't use PID namespacing, so we can just check if we have a matching tracer for this
         * invocation without worrying about it being elsewhere.
         */
        r = get_proc_field("/proc/self/status", "TracerPid", &p);
        যদি (r < 0)
                log_debug_errno(r, "Failed to read our own trace PID, ignoring: %m");
        অন্যথা যদি (!streq(p, "0")) {
                pid_t ptrace_pid;

                r = parse_pid(p, &ptrace_pid);
                যদি (r < 0)
                        log_debug_errno(r, "Failed to parse our own tracer PID, ignoring: %m");
                অন্যথা {
                        _cleanup_free_ বর্ণ *ptrace_comm = রিক্ত;
                        ধ্রুব বর্ণ *pf;

                        pf = procfs_file_alloca(ptrace_pid, "comm");
                        r = read_one_সারি_file(pf, &ptrace_comm);
                        যদি (r < 0)
                                log_debug_errno(r, "Failed to read %s, ignoring: %m", pf);
                        অন্যথা যদি (startswith(ptrace_comm, "proot")) {
                                v = VIRTUALIZATION_PROOT;
                                গচ্ছ finish;
                        }
                }
        }

        /* The container manager might have placed this in the /run/host/ hierarchy for us, which is best
         * because we can be consumed just like that, without special privileges. */
        r = read_one_সারি_file("/run/host/container-manager", &m);
        যদি (r > 0) {
                e = m;
                গচ্ছ translate_name;
        }
        যদি (!IN_SET(r, -ENOENT, 0))
                প্রত্যাগম log_debug_errno(r, "Failed to read /run/host/container-manager: %m");

        যদি (getpid_cached() == 1) {
                /* If we are PID 1 we can just check our own environment variable, and that's authoritative.
                 * We distinguish three cases:
                 * - the variable is not defined → we jump to other checks
                 * - the variable is defined to an empty value → we are not in a container
                 * - anything else → some container, either one of the known ones or "container-other"
                 */
                e = getenv("container");
                যদি (!e)
                        গচ্ছ check_files;
                যদি (isempty(e)) {
                        v = VIRTUALIZATION_NONE;
                        গচ্ছ finish;
                }

                গচ্ছ translate_name;
        }

        /* Otherwise, PID 1 might have dropped this information into a file in /run. This is better than accessing
         * /proc/1/environ, since we don't need CAP_SYS_PTRACE for that. */
        r = read_one_সারি_file("/run/systemd/container", &m);
        যদি (r > 0) {
                e = m;
                গচ্ছ translate_name;
        }
        যদি (!IN_SET(r, -ENOENT, 0))
                প্রত্যাগম log_debug_errno(r, "Failed to read /run/systemd/container: %m");

        /* Fallback for cases where PID 1 was not systemd (for example, cases where init=/bin/sh is used. */
        r = getenv_for_pid(1, "container", &m);
        যদি (r > 0) {
                e = m;
                গচ্ছ translate_name;
        }
        যদি (r < 0) /* This only works if we have CAP_SYS_PTRACE, hence let's better ignore failures here */
                log_debug_errno(r, "Failed to read $container of PID 1, ignoring: %m");

check_files:
        /* Check for existence of some well-known files. We only do this after checking
         * for other specific container managers, otherwise we risk mistaking another
         * container manager for Docker: the /.dockerenv file could inadvertently end up
         * in a file system image. */
        v = detect_container_files();
        যদি (v < 0)
                প্রত্যাগম v;
        যদি (v != VIRTUALIZATION_NONE)
                গচ্ছ finish;

        /* Finally, the root pid namespace has an hardcoded inode number of 0xEFFFFFFC since kernel 3.8, so
         * if all else fails we can check the inode number of our pid namespace and compare it. */
        যদি (running_in_pidns() > 0) {
                log_debug("Running in a pid namespace, assuming unknown container manager.");
                v = VIRTUALIZATION_CONTAINER_OTHER;
                গচ্ছ finish;
        }

        /* If none of that worked, give up, assume no container manager. */
        v = VIRTUALIZATION_NONE;
        গচ্ছ finish;

translate_name:
        যদি (streq(e, "oci")) {
                /* Some images hardcode container=oci, but OCI is not a specific container manager.
                 * Try to detect one based on well-known files. */
                v = detect_container_files();
                যদি (v == VIRTUALIZATION_NONE)
                        v = VIRTUALIZATION_CONTAINER_OTHER;
                গচ্ছ finish;
        }
        v = container_from_string(e);
        যদি (v < 0)
                v = VIRTUALIZATION_CONTAINER_OTHER;

finish:
        log_debug("Found container virtualization %s.", virtualization_to_string(v));
        cached_found = v;
        প্রত্যাগম v;
}

Virtualization detect_virtualization(শূন্য) {
        পূর্ণাঙ্ক v;

        v = detect_container();
        যদি (v != VIRTUALIZATION_NONE)
                প্রত্যাগম v;

        প্রত্যাগম detect_vm();
}

পূর্ণাঙ্ক running_in_userns(শূন্য) {
        পূর্ণাঙ্ক r;

        r = namespace_is_init(NAMESPACE_USER);
        যদি (r < 0)
                প্রত্যাগম log_debug_errno(r, "Failed to test if in root user namespace, ignoring: %m");

        প্রত্যাগম !r;
}

পূর্ণাঙ্ক running_in_chroot(শূন্য) {
        পূর্ণাঙ্ক r;

        /* If we're PID1, /proc may not be mounted (and most likely we're not in a chroot). But PID1 will
         * mount /proc, so all other programs can assume that if /proc is *not* available, we're in some
         * chroot. */

        r = getenv_bool("SYSTEMD_IN_CHROOT");
        যদি (r >= 0)
                প্রত্যাগম r > 0;
        যদি (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_IN_CHROOT, ignoring: %m");

        /* Deprecated but kept for backwards compatibility. */
        যদি (getenv_bool("SYSTEMD_IGNORE_CHROOT") > 0)
                প্রত্যাগম 0;

        r = inode_same("/proc/1/root", "/", /* flags= */ 0);
        যদি (r == -ENOENT) {
                r = proc_mounted();
                যদি (r == 0) {
                        যদি (getpid_cached() == 1)
                                প্রত্যাগম false; /* We will mount /proc, assuming we're not in a chroot. */

                        log_debug("/proc/ is not mounted, assuming we're in a chroot.");
                        প্রত্যাগম true;
                }
                যদি (r > 0) /* If we have fake /proc/, we can't do the check properly. */
                        প্রত্যাগম -ENOSYS;
        }
        যদি (r < 0)
                প্রত্যাগম log_debug_errno(r, "Failed to check if /proc/1/root and / are the same inode: %m");

        প্রত্যাগম r == 0;
}

#যদি defined(__i386__) || defined(__x86_64__)
সংরচনা cpuid_table_entry {
        uint32_t flag_bit;
        ধ্রুব বর্ণ *name;
};

স্থির ধ্রুব সংরচনা cpuid_table_entry leaf1_edx[] = {
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

স্থির ধ্রুব সংরচনা cpuid_table_entry leaf1_ecx[] = {
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

স্থির ধ্রুব সংরচনা cpuid_table_entry leaf7_ebx[] = {
        {  3, "bmi1"   },
        {  5, "avx2"   },
        {  8, "bmi2"   },
        { 18, "rdseed" },
        { 19, "adx"    },
        { 29, "sha_ni" },
};

স্থির ধ্রুব সংরচনা cpuid_table_entry leaf81_edx[] = {
        { 11, "syscall" },
        { 27, "rdtscp"  },
        { 29, "lm"      },
};

স্থির ধ্রুব সংরচনা cpuid_table_entry leaf81_ecx[] = {
        {  0, "lahf_lm" },
        {  5, "abm"     },
};

স্থির ধ্রুব সংরচনা cpuid_table_entry leaf87_edx[] = {
        {  8, "constant_tsc" },
};

স্থির bool given_flag_in_set(ধ্রুব বর্ণ *flag, ধ্রুব সংরচনা cpuid_table_entry *set, size_t set_size, uint32_t val) {
        প্রতিজ্ঞা(set);

        প্রতি (size_t i = 0; i < set_size; i++) {
                যদি ((UINT32_C(1) << set[i].flag_bit) & val &&
                                streq(flag, set[i].name))
                        প্রত্যাগম true;
        }
        প্রত্যাগম false;
}

স্থির bool real_has_cpu_with_flag(ধ্রুব বর্ণ *flag) {
        uint32_t eax, ebx, ecx, edx;

        যদি (__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
                যদি (given_flag_in_set(flag, leaf1_ecx, ELEMENTSOF(leaf1_ecx), ecx))
                        প্রত্যাগম true;

                যদি (given_flag_in_set(flag, leaf1_edx, ELEMENTSOF(leaf1_edx), edx))
                        প্রত্যাগম true;
        }

        যদি (__get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx)) {
                যদি (given_flag_in_set(flag, leaf7_ebx, ELEMENTSOF(leaf7_ebx), ebx))
                        প্রত্যাগম true;
        }

        যদি (__get_cpuid(0x80000001U, &eax, &ebx, &ecx, &edx)) {
                যদি (given_flag_in_set(flag, leaf81_ecx, ELEMENTSOF(leaf81_ecx), ecx))
                        প্রত্যাগম true;

                যদি (given_flag_in_set(flag, leaf81_edx, ELEMENTSOF(leaf81_edx), edx))
                        প্রত্যাগম true;
        }

        যদি (__get_cpuid(0x80000007U, &eax, &ebx, &ecx, &edx))
                যদি (given_flag_in_set(flag, leaf87_edx, ELEMENTSOF(leaf87_edx), edx))
                        প্রত্যাগম true;

        প্রত্যাগম false;
}
#endif

#যদি defined(__aarch64__)
সংরচনা hwcap_table_entry {
        uint64_t flag_mask;
        ধ্রুব বর্ণ *name;
};

স্থির ধ্রুব সংরচনা hwcap_table_entry hwcap[] = {
        { HWCAP_FP,           "fp"          },
        { HWCAP_ASIMD,        "asimd"       },
        { HWCAP_EVTSTRM,      "evtstrm"     },
        { HWCAP_AES,          "aes"         },
        { HWCAP_PMULL,        "pmull"       },
        { HWCAP_SHA1,         "sha1"        },
        { HWCAP_SHA2,         "sha2"        },
        { HWCAP_CRC32,        "crc32"       },
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

স্থির ধ্রুব সংরচনা hwcap_table_entry hwcap2[] = {
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

স্থির ধ্রুব সংরচনা hwcap_table_entry hwcap3[] = {
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

স্থির bool given_flag_in_hwcap_set(
                ধ্রুব বর্ণ *flag,
                ধ্রুব সংরচনা hwcap_table_entry *set,
                size_t set_size,
                unsigned long val) {

        প্রতিজ্ঞা(set);

        প্রতি (size_t i = 0; i < set_size; i++)
                যদি ((set[i].flag_mask & val) && streq(flag, set[i].name))
                        প্রত্যাগম true;

        প্রত্যাগম false;
}

স্থির bool real_has_cpu_with_flag(ধ্রুব বর্ণ *flag) {
        unsigned long val;

        val = getauxval(AT_HWCAP);
        যদি (given_flag_in_hwcap_set(flag, hwcap, ELEMENTSOF(hwcap), val))
                প্রত্যাগম true;

        val = getauxval(AT_HWCAP2);
        যদি (given_flag_in_hwcap_set(flag, hwcap2, ELEMENTSOF(hwcap2), val))
                প্রত্যাগম true;

        val = getauxval(AT_HWCAP3);
        যদি (given_flag_in_hwcap_set(flag, hwcap3, ELEMENTSOF(hwcap3), val))
                প্রত্যাগম true;

        প্রত্যাগম false;
}
#endif

bool has_cpu_with_flag(ধ্রুব বর্ণ *flag) {
        /* Check the architecture-specific CPU feature interface when available. */
#যদি defined(__i386__) || defined(__x86_64__) || defined(__aarch64__)
        প্রত্যাগম real_has_cpu_with_flag(flag);
#অন্যথা
        প্রত্যাগম false;
#endif
}

পূর্ণাঙ্ক cpu_known_features(বর্ণ ***ret) {
        প্রতিজ্ঞা(ret);

#যদি defined(__i386__) || defined(__x86_64__)
        স্থির ধ্রুব সংরচনা {
                ধ্রুব সংরচনা cpuid_table_entry *table;
                size_t n_entries;
        } tables[] = {
                { leaf1_edx,  ELEMENTSOF(leaf1_edx),  },
                { leaf1_ecx,  ELEMENTSOF(leaf1_ecx),  },
                { leaf7_ebx,  ELEMENTSOF(leaf7_ebx),  },
                { leaf81_ecx, ELEMENTSOF(leaf81_ecx), },
                { leaf81_edx, ELEMENTSOF(leaf81_edx), },
                { leaf87_edx, ELEMENTSOF(leaf87_edx), },
        };
#নোচেদ্যদি defined(__aarch64__)
        স্থির ধ্রুব সংরচনা {
                ধ্রুব সংরচনা hwcap_table_entry *table;
                size_t n_entries;
        } tables[] = {
                { hwcap,  ELEMENTSOF(hwcap),  },
                { hwcap2, ELEMENTSOF(hwcap2), },
                { hwcap3, ELEMENTSOF(hwcap3), },
        };
#endif

#যদি defined(__i386__) || defined(__x86_64__) || defined(__aarch64__)
        _cleanup_strv_free_ বর্ণ **features = রিক্ত;
        size_t n = 0;
        পূর্ণাঙ্ক r;

        FOREACH_ELEMENT(table, tables)
                FOREACH_ARRAY(i, table->table, table->n_entries) {
                        r = strv_extend_with_size(&features, &n, i->name);
                        যদি (r < 0)
                                প্রত্যাগম r;
                }

        *ret = TAKE_PTR(features);
#অন্যথা
        *ret = রিক্ত;
#endif
        প্রত্যাগম 0;
}

স্থির ধ্রুব বর্ণ *ধ্রুব virtualization_table[_VIRTUALIZATION_MAX] = {
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

DEFINE_STRING_TABLE_LOOKUP(virtualization, Virtualization);
