/* SPDX-License-Identifier: LGPL-2.1-or-later */

#إذا defined(__i386__) || defined(__x86_64__)
#ضمن <cpuid.h>
#endif
#إذا defined(__aarch64__)
#ضمن <sys/auxv.h>
#endif
#ضمن <stdlib.h>
#ضمن <threads.h>
#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "dirent-util.h"        /* IWYU pragma: keep */
#ضمن "env-util.h"
#ضمن "extract-word.h"
#ضمن "fd-util.h"
#ضمن "fileio.h"
#ضمن "log.h"
#ضمن "namespace-util.h"
#ضمن "parse-util.h"
#ضمن "process-util.h"
#ضمن "stat-util.h"
#ضمن "string-table.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "virt.h"

تعداد {
      SMBIOS_VM_BIT_SET,
      SMBIOS_VM_BIT_UNSET,
      SMBIOS_VM_BIT_UNKNOWN,
};

ساكن Virtualization detect_vm_cpuid(فراغ) {

        /* CPUID is an x86 specific interface. */
#إذا defined(__i386__) || defined(__x86_64__)

        ساكن ثابت بنية {
                ثابت حرف sig[13];
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
        إذا (__get_cpuid(1, &eax, &ebx, &ecx, &edx) == 0)
                أرجع VIRTUALIZATION_NONE;

        hypervisor = ecx & 0x80000000U;

        إذا (hypervisor) {
                اتحاد {
                        uint32_t sig32[3];
                        حرف text[13];
                } sig = {};

                /* There is a hypervisor, see what it is */
                __cpuid(0x40000000U, eax, ebx, ecx, edx);

                sig.sig32[0] = ebx;
                sig.sig32[1] = ecx;
                sig.sig32[2] = edx;

                log_debug("Virtualization found, CPUID=%s", sig.text);

                FOREACH_ELEMENT(vm, vm_table)
                        إذا (memcmp_nn(sig.text, حجم(sig.text),
                                      vm->sig, حجم(vm->sig)) == 0)
                                أرجع vm->id;

                log_debug("Unknown virtualization with CPUID=%s. Add to vm_table[]?", sig.text);
                أرجع VIRTUALIZATION_VM_OTHER;
        }
#endif
        log_debug("No virtualization found in CPUID");

        أرجع VIRTUALIZATION_NONE;
}

ساكن Virtualization detect_vm_device_درخت(فراغ) {
#إذا defined(__arm__) || defined(__aarch64__) || defined(__powerpc__) || defined(__powerpc64__) || defined(__riscv)
        _cleanup_free_ حرف *hvtype = عدم;
        صحيح r;

        r = read_one_سطر_file("/proc/device-tree/hypervisor/compatible", &hvtype);
        إذا (r == -ENOENT) {
                إذا (access("/proc/device-tree/ibm,partition-name", F_OK) == 0 &&
                    access("/proc/device-tree/hmc-managed?", F_OK) == 0 &&
                    access("/proc/device-tree/chosen/qemu,graphic-width", F_OK) != 0)
                        أرجع VIRTUALIZATION_POWERVM;

                _cleanup_closedir_ DIR *dir = opendir("/proc/device-tree");
                إذا (!dir) {
                        إذا (errno == ENOENT) {
                                log_debug_errno(errno, "/proc/device-tree/ does not exist");
                                أرجع VIRTUALIZATION_NONE;
                        }
                        أرجع log_debug_errno(errno, "Opening /proc/device-tree/ failed: %m");
                }

                FOREACH_DIRENT(de, dir, أرجع log_debug_errno(errno, "Failed to enumerate /proc/device-tree/ contents: %m"))
                        إذا (strstr(de->d_name, "fw-cfg")) {
                                log_debug("Virtualization QEMU: \"fw-cfg\" present in /proc/device-tree/%s", de->d_name);
                                أرجع VIRTUALIZATION_QEMU;
                        }

                _cleanup_free_ حرف *compat = عدم;
                r = read_one_سطر_file("/proc/device-tree/compatible", &compat);
                إذا (r < 0 && r != -ENOENT)
                        أرجع log_debug_errno(r, "Failed to read /proc/device-tree/compatible: %m");
                إذا (r >= 0) {
                        إذا (streq(compat, "qemu,pseries")) {
                                log_debug("Virtualization %s found in /proc/device-tree/compatible", compat);
                                أرجع VIRTUALIZATION_QEMU;
                        }
                        إذا (streq(compat, "linux,dummy-virt")) {
                                /* https://www.kernel.org/doc/Documentation/devicetree/bindings/arm/linux%2Cdummy-virt.yaml */
                                log_debug("Generic virtualization %s found in /proc/device-tree/compatible", compat);
                                أرجع VIRTUALIZATION_VM_OTHER;
                        }
                }

                log_debug("No virtualization found in /proc/device-tree/*");
                أرجع VIRTUALIZATION_NONE;
        } وإلا إذا (r < 0)
                أرجع r;

        log_debug("Virtualization %s found in /proc/device-tree/hypervisor/compatible", hvtype);
        إذا (streq(hvtype, "linux,kvm"))
                أرجع VIRTUALIZATION_KVM;
        وإلا إذا (strstr(hvtype, "xen"))
                أرجع VIRTUALIZATION_XEN;
        وإلا إذا (strstr(hvtype, "vmware"))
                أرجع VIRTUALIZATION_VMWARE;
        وإلا
                أرجع VIRTUALIZATION_VM_OTHER;
#وإلا
        log_debug("This platform does not support /proc/device-tree");
        أرجع VIRTUALIZATION_NONE;
#endif
}

#إذا defined(__i386__) || defined(__x86_64__) || defined(__arm__) || defined(__aarch64__) || defined(__loongarch_lp64) || defined(__riscv)
ساكن Virtualization detect_vm_dmi_vendor(فراغ) {
        ساكن ثابت حرف* ثابت dmi_vendors[] = {
                "/sys/class/dmi/id/product_name", /* Test this before sys_vendor to detect KVM over QEMU */
                "/sys/class/dmi/id/sys_vendor",
                "/sys/class/dmi/id/board_vendor",
                "/sys/class/dmi/id/bios_vendor",
                "/sys/class/dmi/id/product_version", /* For Hyper-V VMs test */
                عدم
        };

        ساكن ثابت بنية {
                ثابت حرف *vendor;
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
        صحيح r;

        STRV_FOREACH(vendor, dmi_vendors) {
                _cleanup_free_ حرف *s = عدم;

                r = read_one_سطر_file(*vendor, &s);
                إذا (r == -ENOENT)
                        استمر;
                إذا (r < 0)
                        أرجع r;

                FOREACH_ELEMENT(dmi_vendor, dmi_vendor_table)
                        إذا (startswith(s, dmi_vendor->vendor)) {
                                log_debug("Virtualization %s found in DMI (%s)", s, *vendor);
                                أرجع dmi_vendor->id;
                        }
        }
        log_debug("No virtualization found in DMI vendor table.");
        أرجع VIRTUALIZATION_NONE;
}

ساكن صحيح detect_vm_smbios(فراغ) {
        /* The SMBIOS BIOS Characteristics Extension Byte 2 (Section 2.1.2.2 of
         * https://www.dmtf.org/sites/default/files/standards/documents/DSP0134_3.4.0.pdf), specifies that
         * the 4th bit being set indicates a VM. The BIOS Characteristics table is exposed via the kernel in
         * /sys/firmware/dmi/entries/0-0. Note that in the general case, this bit being unset should not
         * imply that the system is running on bare-metal.  For example, QEMU 3.1.0 (with or without KVM)
         * with SeaBIOS does not set this bit. */
        _cleanup_free_ حرف *s = عدم;
        size_t readsize;
        صحيح r;

        r = read_full_virtual_file("/sys/firmware/dmi/entries/0-0/raw", &s, &readsize);
        إذا (r < 0) {
                log_debug_errno(r, "Unable to read /sys/firmware/dmi/entries/0-0/raw, "
                                "using the virtualization information found in DMI vendor table, ignoring: %m");
                أرجع SMBIOS_VM_BIT_UNKNOWN;
        }
        إذا (readsize < 20 || s[1] < 20) {
                /* The spec indicates that byte 1 contains the size of the table, 0x12 + the number of
                 * extension bytes. The data we're interested in is in extension byte 2, which would be at
                 * 0x13. If we didn't read that much data, or if the BIOS indicates that we don't have that
                 * much data, we don't infer anything from the SMBIOS. */
                log_debug("Only read %zu bytes from /sys/firmware/dmi/entries/0-0/raw (expected 20). "
                          "Using the virtualization information found in DMI vendor table.", readsize);
                أرجع SMBIOS_VM_BIT_UNKNOWN;
        }

        uint8_t byte = (uint8_t) s[19];
        إذا (byte & (1U<<4)) {
                log_debug("DMI BIOS Extension table indicates virtualization.");
                أرجع SMBIOS_VM_BIT_SET;
        }
        log_debug("DMI BIOS Extension table does not indicate virtualization.");
        أرجع SMBIOS_VM_BIT_UNSET;
}
#endif /* defined(__i386__) || defined(__x86_64__) || defined(__arm__) || defined(__aarch64__) || defined(__loongarch_lp64) */

ساكن Virtualization detect_vm_dmi(فراغ) {
#إذا defined(__i386__) || defined(__x86_64__) || defined(__arm__) || defined(__aarch64__) || defined(__loongarch_lp64) || defined(__riscv)

        صحيح r;
        r = detect_vm_dmi_vendor();

        /* The DMI vendor tables in /sys/class/dmi/id don't help us distinguish between Amazon EC2
         * virtual machines and bare-metal instances, so we need to look at SMBIOS. */
        إذا (r == VIRTUALIZATION_AMAZON) {
                بدل (detect_vm_smbios()) {
                حالة SMBIOS_VM_BIT_SET:
                        أرجع VIRTUALIZATION_AMAZON;
                حالة SMBIOS_VM_BIT_UNSET:
                        أرجع VIRTUALIZATION_NONE;
                حالة SMBIOS_VM_BIT_UNKNOWN: {
                        /* The DMI information we are after is only accessible to the root user,
                         * so we fallback to using the product name which is less restricted
                         * to distinguish metal systems from virtualized instances */
                        _cleanup_free_ حرف *s = عدم;
                        ثابت حرف *e;

                        r = read_full_virtual_file("/sys/class/dmi/id/product_name", &s, عدم);
                        /* In EC2, virtualized is much more common than metal, so if for some reason
                         * we fail to read the DMI data, assume we are virtualized. */
                        إذا (r < 0) {
                                log_debug_errno(r, "Can't read /sys/class/dmi/id/product_name,"
                                                " assuming virtualized: %m");
                                أرجع VIRTUALIZATION_AMAZON;
                        }
                        e = strstrafter(truncate_nl(s), ".metal");
                        إذا (e && IN_SET(*e, 0, '-')) {
                                log_debug("DMI product name has '.metal', assuming no virtualization");
                                أرجع VIRTUALIZATION_NONE;
                        } وإلا
                                أرجع VIRTUALIZATION_AMAZON;
                }
                افتراضي:
                        assert_not_reached();
              }
        }

        /* If we haven't identified a VM, but the firmware indicates that there is one, indicate as much. We
         * have no further information about what it is. */
        إذا (r == VIRTUALIZATION_NONE && detect_vm_smbios() == SMBIOS_VM_BIT_SET)
                أرجع VIRTUALIZATION_VM_OTHER;
        أرجع r;
#وإلا
        أرجع VIRTUALIZATION_NONE;
#endif
}

#define XENFEAT_dom0 11 /* xen/include/public/features.h */
#define PATH_FEATURES "/sys/hypervisor/properties/features"
/* Returns -errno, or 0 for domU, or 1 for dom0 */
ساكن صحيح detect_vm_xen_dom0(فراغ) {
        _cleanup_free_ حرف *domcap = عدم;
        صحيح r;

        r = read_one_سطر_file(PATH_FEATURES, &domcap);
        إذا (r < 0 && r != -ENOENT)
                أرجع r;
        إذا (r >= 0) {
                unsigned long features;

                /* Here, we need to use sscanf() instead of safe_atoul()
                 * as the string lacks the leading "0x". */
                r = sscanf(domcap, "%lx", &features);
                إذا (r == 1) {
                        r = !!(features & (1U << XENFEAT_dom0));
                        log_debug("Virtualization XEN, found %s with value %08lx, "
                                  "XENFEAT_dom0 (indicating the 'hardware domain') is%s set.",
                                  PATH_FEATURES, features, r ? "" : " not");
                        أرجع r;
                }
                log_debug("Virtualization XEN, found %s, unhandled content '%s'",
                          PATH_FEATURES, domcap);
        }

        r = read_one_سطر_file("/proc/xen/capabilities", &domcap);
        إذا (r == -ENOENT) {
                log_debug("Virtualization XEN because /proc/xen/capabilities does not exist");
                أرجع 0;
        }
        إذا (r < 0)
                أرجع r;

        لكل (ثابت حرف *i = domcap;;) {
                _cleanup_free_ حرف *cap = عدم;

                r = extract_first_word(&i, &cap, ",", 0);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0) {
                        log_debug("Virtualization XEN DomU found (/proc/xen/capabilities)");
                        أرجع 0;
                }

                إذا (streq(cap, "control_d")) {
                        log_debug("Virtualization XEN Dom0 ignored (/proc/xen/capabilities)");
                        أرجع 1;
                }
        }
}

ساكن Virtualization detect_vm_xen(فراغ) {
        /* The presence of /proc/xen indicates some form of a Xen domain
           The check for Dom0 is handled outside this function */
        إذا (access("/proc/xen", F_OK) < 0) {
                log_debug("Virtualization XEN not found, /proc/xen does not exist");
                أرجع VIRTUALIZATION_NONE;
        }
        log_debug("Virtualization XEN found (/proc/xen exists)");
        أرجع VIRTUALIZATION_XEN;
}

ساكن Virtualization detect_vm_hypervisor(فراغ) {
        _cleanup_free_ حرف *hvtype = عدم;
        صحيح r;

        r = read_one_سطر_file("/sys/hypervisor/type", &hvtype);
        إذا (r == -ENOENT)
                أرجع VIRTUALIZATION_NONE;
        إذا (r < 0)
                أرجع r;

        log_debug("Virtualization %s found in /sys/hypervisor/type", hvtype);

        إذا (streq(hvtype, "xen"))
                أرجع VIRTUALIZATION_XEN;
        وإلا
                أرجع VIRTUALIZATION_VM_OTHER;
}

ساكن Virtualization detect_vm_uml(فراغ) {
        _cleanup_fclose_ FILE *f = عدم;
        صحيح r;

        /* Detect User-Mode Linux by reading /proc/cpuinfo */
        f = fopen("/proc/cpuinfo", "re");
        إذا (!f) {
                إذا (errno == ENOENT) {
                        log_debug("/proc/cpuinfo not found, assuming no UML virtualization.");
                        أرجع VIRTUALIZATION_NONE;
                }
                أرجع -errno;
        }

        لكل (;;) {
                _cleanup_free_ حرف *سطر = عدم;
                ثابت حرف *t;

                r = read_سطر(f, LONG_سطر_MAX, &سطر);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        اكسر;

                t = startswith(سطر, "vendor_id\t: ");
                إذا (t) {
                        إذا (startswith(t, "User Mode Linux")) {
                                log_debug("UML virtualization found in /proc/cpuinfo");
                                أرجع VIRTUALIZATION_UML;
                        }

                        اكسر;
                }
        }

        log_debug("UML virtualization not found in /proc/cpuinfo.");
        أرجع VIRTUALIZATION_NONE;
}

ساكن Virtualization detect_vm_zvm(فراغ) {

#إذا defined(__s390__)
        _cleanup_free_ حرف *t = عدم;
        صحيح r;

        r = get_proc_field("/proc/sysinfo", "VM00 Control Program", &t);
        إذا (IN_SET(r, -ENOENT, -ENODATA))
                أرجع VIRTUALIZATION_NONE;
        إذا (r < 0)
                أرجع r;

        log_debug("Virtualization %s found in /proc/sysinfo", t);
        إذا (streq(t, "z/VM"))
                أرجع VIRTUALIZATION_ZVM;
        وإلا
                أرجع VIRTUALIZATION_KVM;
#وإلا
        log_debug("This platform does not support /proc/sysinfo");
        أرجع VIRTUALIZATION_NONE;
#endif
}

/* Returns a short identifier for the various VM implementations */
Virtualization detect_vm(فراغ) {
        ساكن thread_local Virtualization cached_found = _VIRTUALIZATION_INVALID;
        bool other = false, hyperv = false;
        صحيح xen_dom0 = 0;
        Virtualization v, dmi;

        إذا (cached_found >= 0)
                أرجع cached_found;

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
        إذا (IN_SET(dmi,
                   VIRTUALIZATION_ORACLE,
                   VIRTUALIZATION_XEN,
                   VIRTUALIZATION_AMAZON,
                   /* Unable to distinguish a GCE machine from a VM to bare-metal
                    * for non-x86 architectures due to its necessity for cpuid
                    * detection, which functions solely on x86 platforms. Report
                    * as a VM for other architectures. */
#إذا !defined(__i386__) && !defined(__x86_64__)
                   VIRTUALIZATION_GOOGLE,
#endif
                   VIRTUALIZATION_PARALLELS)) {
                v = dmi;
                اذهب finish;
        }

        /* Detect UML */
        v = detect_vm_uml();
        إذا (v < 0)
                أرجع v;
        إذا (v != VIRTUALIZATION_NONE)
                اذهب finish;

        /* Detect Xen */
        v = detect_vm_xen();
        إذا (v < 0)
                أرجع v;
        إذا (v == VIRTUALIZATION_XEN) {
                 /* If we are Dom0, then we expect to not report as a VM. However, as we might be nested
                  * inside another hypervisor which can be detected via the CPUID check, wait to report this
                  * until after the CPUID check. */
                xen_dom0 = detect_vm_xen_dom0();
                إذا (xen_dom0 < 0)
                        أرجع xen_dom0;
                إذا (xen_dom0 == 0)
                        اذهب finish;
        } وإلا إذا (v != VIRTUALIZATION_NONE)
                assert_not_reached();

        /* Detect from CPUID */
        v = detect_vm_cpuid();
        إذا (v < 0)
                أرجع v;
        إذا (v == VIRTUALIZATION_MICROSOFT)
                /* QEMU sets the CPUID string to hyperv's, in case it provides hyperv enlightenments. Let's
                 * hence not return Microsoft here but just use the other mechanisms first to make a better
                 * decision. */
                hyperv = true;
        وإلا إذا (v == VIRTUALIZATION_VM_OTHER)
                other = true;
        وإلا إذا (v == VIRTUALIZATION_KVM && dmi == VIRTUALIZATION_GOOGLE)
                /* The DMI vendor tables in /sys/class/dmi/id don't help us distinguish between GCE
                 * virtual machines and bare-metal instances, so we need to look at hypervisor. */
                أرجع VIRTUALIZATION_GOOGLE;
        وإلا إذا (v != VIRTUALIZATION_NONE)
                اذهب finish;

        /* If we are in Dom0 and have not yet finished, finish with the result of detect_vm_cpuid */
        إذا (xen_dom0 > 0)
                اذهب finish;

        /* Now, let's get back to DMI */
        إذا (dmi < 0)
                أرجع dmi;
        إذا (dmi == VIRTUALIZATION_VM_OTHER)
                other = true;
        وإلا إذا (!IN_SET(dmi, VIRTUALIZATION_NONE, VIRTUALIZATION_GOOGLE)) {
                /* At this point if GCE has been detected in dmi, do not report as a VM. It should
                 * be a bare-metal machine */
                v = dmi;
                اذهب finish;
        }

        /* Check high-level hypervisor sysfs file */
        v = detect_vm_hypervisor();
        إذا (v < 0)
                أرجع v;
        إذا (v == VIRTUALIZATION_VM_OTHER)
                other = true;
        وإلا إذا (v != VIRTUALIZATION_NONE)
                اذهب finish;

        v = detect_vm_device_درخت();
        إذا (v < 0)
                أرجع v;
        إذا (v == VIRTUALIZATION_VM_OTHER)
                other = true;
        وإلا إذا (v != VIRTUALIZATION_NONE)
                اذهب finish;

        v = detect_vm_zvm();
        إذا (v < 0)
                أرجع v;

finish:
        /* None of the checks above gave us a clear answer, hence let's now use fallback logic: if hyperv
         * enlightenments are available but the VMM wasn't recognized as anything yet, it's probably
         * Microsoft. */
        إذا (v == VIRTUALIZATION_NONE) {
                إذا (hyperv)
                        v = VIRTUALIZATION_MICROSOFT;
                وإلا إذا (other)
                        v = VIRTUALIZATION_VM_OTHER;
        }

        cached_found = v;
        log_debug("Found VM virtualization %s", virtualization_to_string(v));
        أرجع v;
}

ساكن ثابت حرف *ثابت container_table[_VIRTUALIZATION_MAX] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_FROM_STRING(container, صحيح);

ساكن صحيح running_in_pidns(فراغ) {
        صحيح r;

        r = namespace_is_init(NAMESPACE_PID);
        إذا (r < 0)
                أرجع log_debug_errno(r, "Failed to test if in root PID namespace, ignoring: %m");

        أرجع !r;
}

ساكن Virtualization detect_container_files(فراغ) {
        ساكن ثابت بنية {
                ثابت حرف *file_path;
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
                إذا (access(file->file_path, F_OK) >= 0)
                        أرجع file->id;

                إذا (errno != ENOENT)
                        log_debug_errno(errno,
                                        "Checking if %s exists failed, ignoring: %m",
                                        file->file_path);
        }

        أرجع VIRTUALIZATION_NONE;
}

Virtualization detect_container(فراغ) {
        ساكن thread_local Virtualization cached_found = _VIRTUALIZATION_INVALID;
        _cleanup_free_ حرف *m = عدم, *o = عدم, *p = عدم;
        ثابت حرف *e = عدم;
        Virtualization v;
        صحيح r;

        إذا (cached_found >= 0)
                أرجع cached_found;

        /* /proc/vz exists in container and outside of the container, /proc/bc only outside of the container. */
        إذا (access("/proc/vz", F_OK) < 0) {
                إذا (errno != ENOENT)
                        log_debug_errno(errno, "Failed to check if /proc/vz exists, ignoring: %m");
        } وإلا إذا (access("/proc/bc", F_OK) < 0) {
                إذا (errno == ENOENT) {
                        v = VIRTUALIZATION_OPENVZ;
                        اذهب finish;
                }

                log_debug_errno(errno, "Failed to check if /proc/bc exists, ignoring: %m");
        }

        /* "Official" way of detecting WSL https://github.com/Microsoft/WSL/issues/423#issuecomment-221627364 */
        r = read_one_سطر_file("/proc/sys/kernel/osrelease", &o);
        إذا (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/kernel/osrelease, ignoring: %m");
        وإلا إذا (strstr(o, "Microsoft") || strstr(o, "WSL")) {
                v = VIRTUALIZATION_WSL;
                اذهب finish;
        }

        /* proot doesn't use PID namespacing, so we can just check if we have a matching tracer for this
         * invocation without worrying about it being elsewhere.
         */
        r = get_proc_field("/proc/self/status", "TracerPid", &p);
        إذا (r < 0)
                log_debug_errno(r, "Failed to read our own trace PID, ignoring: %m");
        وإلا إذا (!streq(p, "0")) {
                pid_t ptrace_pid;

                r = parse_pid(p, &ptrace_pid);
                إذا (r < 0)
                        log_debug_errno(r, "Failed to parse our own tracer PID, ignoring: %m");
                وإلا {
                        _cleanup_free_ حرف *ptrace_comm = عدم;
                        ثابت حرف *pf;

                        pf = procfs_file_alloca(ptrace_pid, "comm");
                        r = read_one_سطر_file(pf, &ptrace_comm);
                        إذا (r < 0)
                                log_debug_errno(r, "Failed to read %s, ignoring: %m", pf);
                        وإلا إذا (startswith(ptrace_comm, "proot")) {
                                v = VIRTUALIZATION_PROOT;
                                اذهب finish;
                        }
                }
        }

        /* The container manager might have placed this in the /run/host/ hierarchy for us, which is best
         * because we can be consumed just like that, without special privileges. */
        r = read_one_سطر_file("/run/host/container-manager", &m);
        إذا (r > 0) {
                e = m;
                اذهب translate_name;
        }
        إذا (!IN_SET(r, -ENOENT, 0))
                أرجع log_debug_errno(r, "Failed to read /run/host/container-manager: %m");

        إذا (getpid_cached() == 1) {
                /* If we are PID 1 we can just check our own environment variable, and that's authoritative.
                 * We distinguish three cases:
                 * - the variable is not defined → we jump to other checks
                 * - the variable is defined to an empty value → we are not in a container
                 * - anything else → some container, either one of the known ones or "container-other"
                 */
                e = getenv("container");
                إذا (!e)
                        اذهب check_files;
                إذا (isempty(e)) {
                        v = VIRTUALIZATION_NONE;
                        اذهب finish;
                }

                اذهب translate_name;
        }

        /* Otherwise, PID 1 might have dropped this information into a file in /run. This is better than accessing
         * /proc/1/environ, since we don't need CAP_SYS_PTRACE for that. */
        r = read_one_سطر_file("/run/systemd/container", &m);
        إذا (r > 0) {
                e = m;
                اذهب translate_name;
        }
        إذا (!IN_SET(r, -ENOENT, 0))
                أرجع log_debug_errno(r, "Failed to read /run/systemd/container: %m");

        /* Fallback for cases where PID 1 was not systemd (for example, cases where init=/bin/sh is used. */
        r = getenv_for_pid(1, "container", &m);
        إذا (r > 0) {
                e = m;
                اذهب translate_name;
        }
        إذا (r < 0) /* This only works if we have CAP_SYS_PTRACE, hence let's better ignore failures here */
                log_debug_errno(r, "Failed to read $container of PID 1, ignoring: %m");

check_files:
        /* Check for existence of some well-known files. We only do this after checking
         * for other specific container managers, otherwise we risk mistaking another
         * container manager for Docker: the /.dockerenv file could inadvertently end up
         * in a file system image. */
        v = detect_container_files();
        إذا (v < 0)
                أرجع v;
        إذا (v != VIRTUALIZATION_NONE)
                اذهب finish;

        /* Finally, the root pid namespace has an hardcoded inode number of 0xEFFFFFFC since kernel 3.8, so
         * if all else fails we can check the inode number of our pid namespace and compare it. */
        إذا (running_in_pidns() > 0) {
                log_debug("Running in a pid namespace, assuming unknown container manager.");
                v = VIRTUALIZATION_CONTAINER_OTHER;
                اذهب finish;
        }

        /* If none of that worked, give up, assume no container manager. */
        v = VIRTUALIZATION_NONE;
        اذهب finish;

translate_name:
        إذا (streq(e, "oci")) {
                /* Some images hardcode container=oci, but OCI is not a specific container manager.
                 * Try to detect one based on well-known files. */
                v = detect_container_files();
                إذا (v == VIRTUALIZATION_NONE)
                        v = VIRTUALIZATION_CONTAINER_OTHER;
                اذهب finish;
        }
        v = container_from_string(e);
        إذا (v < 0)
                v = VIRTUALIZATION_CONTAINER_OTHER;

finish:
        log_debug("Found container virtualization %s.", virtualization_to_string(v));
        cached_found = v;
        أرجع v;
}

Virtualization detect_virtualization(فراغ) {
        صحيح v;

        v = detect_container();
        إذا (v != VIRTUALIZATION_NONE)
                أرجع v;

        أرجع detect_vm();
}

صحيح running_in_userns(فراغ) {
        صحيح r;

        r = namespace_is_init(NAMESPACE_USER);
        إذا (r < 0)
                أرجع log_debug_errno(r, "Failed to test if in root user namespace, ignoring: %m");

        أرجع !r;
}

صحيح running_in_chroot(فراغ) {
        صحيح r;

        /* If we're PID1, /proc may not be mounted (and most likely we're not in a chroot). But PID1 will
         * mount /proc, so all other programs can assume that if /proc is *not* available, we're in some
         * chroot. */

        r = getenv_bool("SYSTEMD_IN_CHROOT");
        إذا (r >= 0)
                أرجع r > 0;
        إذا (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_IN_CHROOT, ignoring: %m");

        /* Deprecated but kept for backwards compatibility. */
        إذا (getenv_bool("SYSTEMD_IGNORE_CHROOT") > 0)
                أرجع 0;

        r = inode_same("/proc/1/root", "/", /* flags= */ 0);
        إذا (r == -ENOENT) {
                r = proc_mounted();
                إذا (r == 0) {
                        إذا (getpid_cached() == 1)
                                أرجع false; /* We will mount /proc, assuming we're not in a chroot. */

                        log_debug("/proc/ is not mounted, assuming we're in a chroot.");
                        أرجع true;
                }
                إذا (r > 0) /* If we have fake /proc/, we can't do the check properly. */
                        أرجع -ENOSYS;
        }
        إذا (r < 0)
                أرجع log_debug_errno(r, "Failed to check if /proc/1/root and / are the same inode: %m");

        أرجع r == 0;
}

#إذا defined(__i386__) || defined(__x86_64__)
بنية cpuid_table_entry {
        uint32_t flag_bit;
        ثابت حرف *name;
};

ساكن ثابت بنية cpuid_table_entry leaf1_edx[] = {
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

ساكن ثابت بنية cpuid_table_entry leaf1_ecx[] = {
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

ساكن ثابت بنية cpuid_table_entry leaf7_ebx[] = {
        {  3, "bmi1"   },
        {  5, "avx2"   },
        {  8, "bmi2"   },
        { 18, "rdseed" },
        { 19, "adx"    },
        { 29, "sha_ni" },
};

ساكن ثابت بنية cpuid_table_entry leaf81_edx[] = {
        { 11, "syscall" },
        { 27, "rdtscp"  },
        { 29, "lm"      },
};

ساكن ثابت بنية cpuid_table_entry leaf81_ecx[] = {
        {  0, "lahf_lm" },
        {  5, "abm"     },
};

ساكن ثابت بنية cpuid_table_entry leaf87_edx[] = {
        {  8, "constant_tsc" },
};

ساكن bool given_flag_in_set(ثابت حرف *flag, ثابت بنية cpuid_table_entry *set, size_t set_size, uint32_t val) {
        أكد(set);

        لكل (size_t i = 0; i < set_size; i++) {
                إذا ((UINT32_C(1) << set[i].flag_bit) & val &&
                                streq(flag, set[i].name))
                        أرجع true;
        }
        أرجع false;
}

ساكن bool real_has_cpu_with_flag(ثابت حرف *flag) {
        uint32_t eax, ebx, ecx, edx;

        إذا (__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
                إذا (given_flag_in_set(flag, leaf1_ecx, ELEMENTSOF(leaf1_ecx), ecx))
                        أرجع true;

                إذا (given_flag_in_set(flag, leaf1_edx, ELEMENTSOF(leaf1_edx), edx))
                        أرجع true;
        }

        إذا (__get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx)) {
                إذا (given_flag_in_set(flag, leaf7_ebx, ELEMENTSOF(leaf7_ebx), ebx))
                        أرجع true;
        }

        إذا (__get_cpuid(0x80000001U, &eax, &ebx, &ecx, &edx)) {
                إذا (given_flag_in_set(flag, leaf81_ecx, ELEMENTSOF(leaf81_ecx), ecx))
                        أرجع true;

                إذا (given_flag_in_set(flag, leaf81_edx, ELEMENTSOF(leaf81_edx), edx))
                        أرجع true;
        }

        إذا (__get_cpuid(0x80000007U, &eax, &ebx, &ecx, &edx))
                إذا (given_flag_in_set(flag, leaf87_edx, ELEMENTSOF(leaf87_edx), edx))
                        أرجع true;

        أرجع false;
}
#endif

#إذا defined(__aarch64__)
بنية hwcap_table_entry {
        uint64_t flag_mask;
        ثابت حرف *name;
};

ساكن ثابت بنية hwcap_table_entry hwcap[] = {
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

ساكن ثابت بنية hwcap_table_entry hwcap2[] = {
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

ساكن ثابت بنية hwcap_table_entry hwcap3[] = {
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

ساكن bool given_flag_in_hwcap_set(
                ثابت حرف *flag,
                ثابت بنية hwcap_table_entry *set,
                size_t set_size,
                unsigned long val) {

        أكد(set);

        لكل (size_t i = 0; i < set_size; i++)
                إذا ((set[i].flag_mask & val) && streq(flag, set[i].name))
                        أرجع true;

        أرجع false;
}

ساكن bool real_has_cpu_with_flag(ثابت حرف *flag) {
        unsigned long val;

        val = getauxval(AT_HWCAP);
        إذا (given_flag_in_hwcap_set(flag, hwcap, ELEMENTSOF(hwcap), val))
                أرجع true;

        val = getauxval(AT_HWCAP2);
        إذا (given_flag_in_hwcap_set(flag, hwcap2, ELEMENTSOF(hwcap2), val))
                أرجع true;

        val = getauxval(AT_HWCAP3);
        إذا (given_flag_in_hwcap_set(flag, hwcap3, ELEMENTSOF(hwcap3), val))
                أرجع true;

        أرجع false;
}
#endif

bool has_cpu_with_flag(ثابت حرف *flag) {
        /* Check the architecture-specific CPU feature interface when available. */
#إذا defined(__i386__) || defined(__x86_64__) || defined(__aarch64__)
        أرجع real_has_cpu_with_flag(flag);
#وإلا
        أرجع false;
#endif
}

صحيح cpu_known_features(حرف ***ret) {
        أكد(ret);

#إذا defined(__i386__) || defined(__x86_64__)
        ساكن ثابت بنية {
                ثابت بنية cpuid_table_entry *table;
                size_t n_entries;
        } tables[] = {
                { leaf1_edx,  ELEMENTSOF(leaf1_edx),  },
                { leaf1_ecx,  ELEMENTSOF(leaf1_ecx),  },
                { leaf7_ebx,  ELEMENTSOF(leaf7_ebx),  },
                { leaf81_ecx, ELEMENTSOF(leaf81_ecx), },
                { leaf81_edx, ELEMENTSOF(leaf81_edx), },
                { leaf87_edx, ELEMENTSOF(leaf87_edx), },
        };
#وإلاإذا defined(__aarch64__)
        ساكن ثابت بنية {
                ثابت بنية hwcap_table_entry *table;
                size_t n_entries;
        } tables[] = {
                { hwcap,  ELEMENTSOF(hwcap),  },
                { hwcap2, ELEMENTSOF(hwcap2), },
                { hwcap3, ELEMENTSOF(hwcap3), },
        };
#endif

#إذا defined(__i386__) || defined(__x86_64__) || defined(__aarch64__)
        _cleanup_strv_free_ حرف **features = عدم;
        size_t n = 0;
        صحيح r;

        FOREACH_ELEMENT(table, tables)
                FOREACH_ARRAY(i, table->table, table->n_entries) {
                        r = strv_extend_with_size(&features, &n, i->name);
                        إذا (r < 0)
                                أرجع r;
                }

        *ret = TAKE_PTR(features);
#وإلا
        *ret = عدم;
#endif
        أرجع 0;
}

ساكن ثابت حرف *ثابت virtualization_table[_VIRTUALIZATION_MAX] = {
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
