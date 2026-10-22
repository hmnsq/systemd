/* SPDX-License-Identifier: LGPL-2.1-or-later */

#اگر defined(__i386__) || defined(__x86_64__)
#شامل <cpuid.h>
#اگر_ختم
#اگر defined(__aarch64__)
#شامل <sys/auxv.h>
#اگر_ختم
#شامل <stdlib.h>
#شامل <threads.h>
#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "dirent-util.h"        /* IWYU pragma: keep */
#شامل "env-util.h"
#شامل "extract-word.h"
#شامل "fd-util.h"
#شامل "fileio.h"
#شامل "log.h"
#شامل "namespace-util.h"
#شامل "parse-util.h"
#شامل "process-util.h"
#شامل "stat-util.h"
#شامل "string-table.h"
#شامل "string-util.h"
#شامل "strv.h"
#شامل "virt.h"

شمار {
      SMBIOS_VM_ثنائی_ہندسہ_سیٹ,
      SMBIOS_VM_ثنائی_ہندسہ_UNSET,
      SMBIOS_VM_ثنائی_ہندسہ_UNKNOWN,
};

ساکن Virtualization detect_vm_cpuid(خالی) {

        /* CPUID is an x86 specific interface. */
#اگر defined(__i386__) || defined(__x86_64__)

        ساکن ثابت ساخت {
                ثابت حرف sig[13];
                Virtualization id;
        } vm_جدول[] = {
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
        اگر (__get_cpuid(1, &eax, &ebx, &ecx, &edx) == 0)
                واپس VIRTUALIZATION_NONE;

        hypervisor = ecx & 0x80000000U;

        اگر (hypervisor) {
                اتحاد {
                        uint32_t sig32[3];
                        حرف متن[13];
                } sig = {};

                /* There is a hypervisor, see what it is */
                __cpuid(0x40000000U, eax, ebx, ecx, edx);

                sig.sig32[0] = ebx;
                sig.sig32[1] = ecx;
                sig.sig32[2] = edx;

                log_debug("Virtualization found, CPUID=%s", sig.متن);

                FOREACH_ELEMENT(vm, vm_جدول)
                        اگر (memcmp_nn(sig.متن, حجم(sig.متن),
                                      vm->sig, حجم(vm->sig)) == 0)
                                واپس vm->id;

                log_debug("Unknown virtualization with CPUID=%s. Add to vm_table[]?", sig.متن);
                واپس VIRTUALIZATION_VM_OTHER;
        }
#اگر_ختم
        log_debug("No virtualization found in CPUID");

        واپس VIRTUALIZATION_NONE;
}

ساکن Virtualization detect_vm_device_درخت(خالی) {
#اگر defined(__arm__) || defined(__aarch64__) || defined(__powerpc__) || defined(__powerpc64__) || defined(__riscv)
        _cleanup_free_ حرف *hvtype = تہی;
        صحیح r;

        r = read_one_سطر_file("/proc/device-tree/hypervisor/compatible", &hvtype);
        اگر (r == -ENOENT) {
                اگر (access("/proc/device-tree/ibm,partition-name", F_OK) == 0 &&
                    access("/proc/device-tree/hmc-managed?", F_OK) == 0 &&
                    access("/proc/device-tree/chosen/qemu,graphic-width", F_OK) != 0)
                        واپس VIRTUALIZATION_POWERVM;

                _cleanup_closedir_ DIR *dir = opendir("/proc/device-tree");
                اگر (!dir) {
                        اگر (errno == ENOENT) {
                                log_debug_errno(errno, "/proc/device-tree/ does not exist");
                                واپس VIRTUALIZATION_NONE;
                        }
                        واپس log_debug_errno(errno, "Opening /proc/device-tree/ failed: %m");
                }

                FOREACH_DIRENT(de, dir, واپس log_debug_errno(errno, "Failed to enumerate /proc/device-tree/ contents: %m"))
                        اگر (لڑی_تلاش(de->d_name, "fw-cfg")) {
                                log_debug("Virtualization QEMU: \"fw-cfg\" present in /proc/device-tree/%s", de->d_name);
                                واپس VIRTUALIZATION_QEMU;
                        }

                _cleanup_free_ حرف *compat = تہی;
                r = read_one_سطر_file("/proc/device-tree/compatible", &compat);
                اگر (r < 0 && r != -ENOENT)
                        واپس log_debug_errno(r, "Failed to read /proc/device-tree/compatible: %m");
                اگر (r >= 0) {
                        اگر (streq(compat, "qemu,pseries")) {
                                log_debug("Virtualization %s found in /proc/device-tree/compatible", compat);
                                واپس VIRTUALIZATION_QEMU;
                        }
                        اگر (streq(compat, "linux,dummy-virt")) {
                                /* https://www.kernel.org/doc/Documentation/devicetree/bindings/arm/linux%2Cdummy-virt.yaml */
                                log_debug("Generic virtualization %s found in /proc/device-tree/compatible", compat);
                                واپس VIRTUALIZATION_VM_OTHER;
                        }
                }

                log_debug("No virtualization found in /proc/device-tree/*");
                واپس VIRTUALIZATION_NONE;
        } ورنہ اگر (r < 0)
                واپس r;

        log_debug("Virtualization %s found in /proc/device-tree/hypervisor/compatible", hvtype);
        اگر (streq(hvtype, "linux,kvm"))
                واپس VIRTUALIZATION_KVM;
        ورنہ اگر (لڑی_تلاش(hvtype, "xen"))
                واپس VIRTUALIZATION_XEN;
        ورنہ اگر (لڑی_تلاش(hvtype, "vmware"))
                واپس VIRTUALIZATION_VMWARE;
        ورنہ
                واپس VIRTUALIZATION_VM_OTHER;
#ورنہ
        log_debug("This platform does not support /proc/device-tree");
        واپس VIRTUALIZATION_NONE;
#اگر_ختم
}

#اگر defined(__i386__) || defined(__x86_64__) || defined(__arm__) || defined(__aarch64__) || defined(__loongarch_lp64) || defined(__riscv)
ساکن Virtualization detect_vm_dmi_vendor(خالی) {
        ساکن ثابت حرف* ثابت dmi_vendors[] = {
                "/sys/class/dmi/id/product_name", /* Test this before sys_vendor to detect KVM over QEMU */
                "/sys/class/dmi/id/sys_vendor",
                "/sys/class/dmi/id/board_vendor",
                "/sys/class/dmi/id/bios_vendor",
                "/sys/class/dmi/id/product_version", /* For Hyper-V VMs test */
                تہی
        };

        ساکن ثابت ساخت {
                ثابت حرف *vendor;
                Virtualization id;
        } dmi_vendor_جدول[] = {
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
        صحیح r;

        STRV_FOREACH(vendor, dmi_vendors) {
                _cleanup_free_ حرف *s = تہی;

                r = read_one_سطر_file(*vendor, &s);
                اگر (r == -ENOENT)
                        جاری;
                اگر (r < 0)
                        واپس r;

                FOREACH_ELEMENT(dmi_vendor, dmi_vendor_جدول)
                        اگر (startswith(s, dmi_vendor->vendor)) {
                                log_debug("Virtualization %s found in DMI (%s)", s, *vendor);
                                واپس dmi_vendor->id;
                        }
        }
        log_debug("No virtualization found in DMI vendor table.");
        واپس VIRTUALIZATION_NONE;
}

ساکن صحیح detect_vm_smbios(خالی) {
        /* The SMBIOS BIOS Characteristics Extension Byte 2 (Section 2.1.2.2 of
         * https://www.dmtf.org/sites/default/files/standards/documents/DSP0134_3.4.0.pdf), specifies that
         * the 4th bit being set indicates a VM. The BIOS Characteristics table is exposed via the kernel in
         * /sys/firmware/dmi/entries/0-0. Note that in the general case, this bit being unset should not
         * imply that the system is running on bare-metal.  For example, QEMU 3.1.0 (with or without KVM)
         * with SeaBIOS does not set this bit. */
        _cleanup_free_ حرف *s = تہی;
        size_t readsize;
        صحیح r;

        r = read_بھرا_virtual_file("/sys/firmware/dmi/entries/0-0/raw", &s, &readsize);
        اگر (r < 0) {
                log_debug_errno(r, "Unable to read /sys/firmware/dmi/entries/0-0/raw, "
                                "using the virtualization information found in DMI vendor table, ignoring: %m");
                واپس SMBIOS_VM_ثنائی_ہندسہ_UNKNOWN;
        }
        اگر (readsize < 20 || s[1] < 20) {
                /* The spec indicates that byte 1 contains the size of the table, 0x12 + the number of
                 * extension bytes. The data we're interested in is in extension byte 2, which would be at
                 * 0x13. If we didn't read that much data, or if the BIOS indicates that we don't have that
                 * much data, we don't infer anything from the SMBIOS. */
                log_debug("Only read %zu bytes from /sys/firmware/dmi/entries/0-0/raw (expected 20). "
                          "Using the virtualization information found in DMI vendor table.", readsize);
                واپس SMBIOS_VM_ثنائی_ہندسہ_UNKNOWN;
        }

        uint8_t byte = (uint8_t) s[19];
        اگر (byte & (1U<<4)) {
                log_debug("DMI BIOS Extension table indicates virtualization.");
                واپس SMBIOS_VM_ثنائی_ہندسہ_سیٹ;
        }
        log_debug("DMI BIOS Extension table does not indicate virtualization.");
        واپس SMBIOS_VM_ثنائی_ہندسہ_UNSET;
}
#اگر_ختم /* defined(__i386__) || defined(__x86_64__) || defined(__arm__) || defined(__aarch64__) || defined(__loongarch_lp64) */

ساکن Virtualization detect_vm_dmi(خالی) {
#اگر defined(__i386__) || defined(__x86_64__) || defined(__arm__) || defined(__aarch64__) || defined(__loongarch_lp64) || defined(__riscv)

        صحیح r;
        r = detect_vm_dmi_vendor();

        /* The DMI vendor tables in /sys/class/dmi/id don't help us distinguish between Amazon EC2
         * virtual machines and bare-metal instances, so we need to look at SMBIOS. */
        اگر (r == VIRTUALIZATION_AMAZON) {
                انتخاب (detect_vm_smbios()) {
                حالت SMBIOS_VM_ثنائی_ہندسہ_سیٹ:
                        واپس VIRTUALIZATION_AMAZON;
                حالت SMBIOS_VM_ثنائی_ہندسہ_UNSET:
                        واپس VIRTUALIZATION_NONE;
                حالت SMBIOS_VM_ثنائی_ہندسہ_UNKNOWN: {
                        /* The DMI information we are after is only accessible to the root user,
                         * so we fallback to using the product name which is less restricted
                         * to distinguish metal systems from virtualized instances */
                        _cleanup_free_ حرف *s = تہی;
                        ثابت حرف *e;

                        r = read_بھرا_virtual_file("/sys/class/dmi/id/product_name", &s, تہی);
                        /* In EC2, virtualized is much more common than metal, so if for some reason
                         * we fail to read the DMI data, assume we are virtualized. */
                        اگر (r < 0) {
                                log_debug_errno(r, "Can't read /sys/class/dmi/id/product_name,"
                                                " assuming virtualized: %m");
                                واپس VIRTUALIZATION_AMAZON;
                        }
                        e = strstrafter(truncate_nl(s), ".metal");
                        اگر (e && اندر_سیٹ(*e, 0, '-')) {
                                log_debug("DMI product name has '.metal', assuming no virtualization");
                                واپس VIRTUALIZATION_NONE;
                        } ورنہ
                                واپس VIRTUALIZATION_AMAZON;
                }
                طےشدہ:
                        assert_not_reached();
              }
        }

        /* If we haven't identified a VM, but the firmware indicates that there is one, indicate as much. We
         * have no further information about what it is. */
        اگر (r == VIRTUALIZATION_NONE && detect_vm_smbios() == SMBIOS_VM_ثنائی_ہندسہ_سیٹ)
                واپس VIRTUALIZATION_VM_OTHER;
        واپس r;
#ورنہ
        واپس VIRTUALIZATION_NONE;
#اگر_ختم
}

#تعریف_کرو XENFEAT_dom0 11 /* xen/include/public/features.h */
#تعریف_کرو راستہ_FEATURES "/sys/hypervisor/properties/features"
/* Returns -errno, or 0 for domU, or 1 for dom0 */
ساکن صحیح detect_vm_xen_dom0(خالی) {
        _cleanup_free_ حرف *domcap = تہی;
        صحیح r;

        r = read_one_سطر_file(راستہ_FEATURES, &domcap);
        اگر (r < 0 && r != -ENOENT)
                واپس r;
        اگر (r >= 0) {
                غیرعلامتی طویل features;

                /* Here, we need to use sscanf() instead of safe_atoul()
                 * as the string lacks the leading "0x". */
                r = لڑی_سے_پڑھو(domcap, "%lx", &features);
                اگر (r == 1) {
                        r = !!(features & (1U << XENFEAT_dom0));
                        log_debug("Virtualization XEN, found %s with value %08lx, "
                                  "XENFEAT_dom0 (indicating the 'hardware domain') is%s set.",
                                  راستہ_FEATURES, features, r ? "" : " not");
                        واپس r;
                }
                log_debug("Virtualization XEN, found %s, unhandled content '%s'",
                          راستہ_FEATURES, domcap);
        }

        r = read_one_سطر_file("/proc/xen/capabilities", &domcap);
        اگر (r == -ENOENT) {
                log_debug("Virtualization XEN because /proc/xen/capabilities does not exist");
                واپس 0;
        }
        اگر (r < 0)
                واپس r;

        برائے (ثابت حرف *i = domcap;;) {
                _cleanup_free_ حرف *cap = تہی;

                r = نکالو_پہلا_لفظ(&i, &cap, ",", 0);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0) {
                        log_debug("Virtualization XEN DomU found (/proc/xen/capabilities)");
                        واپس 0;
                }

                اگر (streq(cap, "control_d")) {
                        log_debug("Virtualization XEN Dom0 ignored (/proc/xen/capabilities)");
                        واپس 1;
                }
        }
}

ساکن Virtualization detect_vm_xen(خالی) {
        /* The presence of /proc/xen indicates some form of a Xen domain
           The check for Dom0 is handled outside this function */
        اگر (access("/proc/xen", F_OK) < 0) {
                log_debug("Virtualization XEN not found, /proc/xen does not exist");
                واپس VIRTUALIZATION_NONE;
        }
        log_debug("Virtualization XEN found (/proc/xen exists)");
        واپس VIRTUALIZATION_XEN;
}

ساکن Virtualization detect_vm_hypervisor(خالی) {
        _cleanup_free_ حرف *hvtype = تہی;
        صحیح r;

        r = read_one_سطر_file("/sys/hypervisor/type", &hvtype);
        اگر (r == -ENOENT)
                واپس VIRTUALIZATION_NONE;
        اگر (r < 0)
                واپس r;

        log_debug("Virtualization %s found in /sys/hypervisor/type", hvtype);

        اگر (streq(hvtype, "xen"))
                واپس VIRTUALIZATION_XEN;
        ورنہ
                واپس VIRTUALIZATION_VM_OTHER;
}

ساکن Virtualization detect_vm_uml(خالی) {
        _cleanup_fclose_ FILE *f = تہی;
        صحیح r;

        /* Detect User-Mode Linux by reading /proc/cpuinfo */
        f = fopen("/proc/cpuinfo", "re");
        اگر (!f) {
                اگر (errno == ENOENT) {
                        log_debug("/proc/cpuinfo not found, assuming no UML virtualization.");
                        واپس VIRTUALIZATION_NONE;
                }
                واپس -errno;
        }

        برائے (;;) {
                _cleanup_free_ حرف *سطر = تہی;
                ثابت حرف *t;

                r = read_سطر(f, LONG_سطر_MAX, &سطر);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        توڑو;

                t = startswith(سطر, "vendor_id\t: ");
                اگر (t) {
                        اگر (startswith(t, "User Mode Linux")) {
                                log_debug("UML virtualization found in /proc/cpuinfo");
                                واپس VIRTUALIZATION_UML;
                        }

                        توڑو;
                }
        }

        log_debug("UML virtualization not found in /proc/cpuinfo.");
        واپس VIRTUALIZATION_NONE;
}

ساکن Virtualization detect_vm_zvm(خالی) {

#اگر defined(__s390__)
        _cleanup_free_ حرف *t = تہی;
        صحیح r;

        r = get_proc_میدان("/proc/sysinfo", "VM00 Control Program", &t);
        اگر (اندر_سیٹ(r, -ENOENT, -ENODATA))
                واپس VIRTUALIZATION_NONE;
        اگر (r < 0)
                واپس r;

        log_debug("Virtualization %s found in /proc/sysinfo", t);
        اگر (streq(t, "z/VM"))
                واپس VIRTUALIZATION_ZVM;
        ورنہ
                واپس VIRTUALIZATION_KVM;
#ورنہ
        log_debug("This platform does not support /proc/sysinfo");
        واپس VIRTUALIZATION_NONE;
#اگر_ختم
}

/* Returns a short identifier for the various VM implementations */
Virtualization detect_vm(خالی) {
        ساکن thread_local Virtualization cached_ملا = _VIRTUALIZATION_INVALID;
        bool other = false, hyperv = false;
        صحیح xen_dom0 = 0;
        Virtualization v, dmi;

        اگر (cached_ملا >= 0)
                واپس cached_ملا;

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
        اگر (اندر_سیٹ(dmi,
                   VIRTUALIZATION_ORACLE,
                   VIRTUALIZATION_XEN,
                   VIRTUALIZATION_AMAZON,
                   /* Unable to distinguish a GCE machine from a VM to bare-metal
                    * for non-x86 architectures due to its necessity for cpuid
                    * detection, which functions solely on x86 platforms. Report
                    * as a VM for other architectures. */
#اگر !defined(__i386__) && !defined(__x86_64__)
                   VIRTUALIZATION_GOOGLE,
#اگر_ختم
                   VIRTUALIZATION_PARALLELS)) {
                v = dmi;
                جاؤ اختتام;
        }

        /* Detect UML */
        v = detect_vm_uml();
        اگر (v < 0)
                واپس v;
        اگر (v != VIRTUALIZATION_NONE)
                جاؤ اختتام;

        /* Detect Xen */
        v = detect_vm_xen();
        اگر (v < 0)
                واپس v;
        اگر (v == VIRTUALIZATION_XEN) {
                 /* If we are Dom0, then we expect to not report as a VM. However, as we might be nested
                  * inside another hypervisor which can be detected via the CPUID check, wait to report this
                  * until after the CPUID check. */
                xen_dom0 = detect_vm_xen_dom0();
                اگر (xen_dom0 < 0)
                        واپس xen_dom0;
                اگر (xen_dom0 == 0)
                        جاؤ اختتام;
        } ورنہ اگر (v != VIRTUALIZATION_NONE)
                assert_not_reached();

        /* Detect from CPUID */
        v = detect_vm_cpuid();
        اگر (v < 0)
                واپس v;
        اگر (v == VIRTUALIZATION_MICROSOFT)
                /* QEMU sets the CPUID string to hyperv's, in case it provides hyperv enlightenments. Let's
                 * hence not return Microsoft here but just use the other mechanisms first to make a better
                 * decision. */
                hyperv = true;
        ورنہ اگر (v == VIRTUALIZATION_VM_OTHER)
                other = true;
        ورنہ اگر (v == VIRTUALIZATION_KVM && dmi == VIRTUALIZATION_GOOGLE)
                /* The DMI vendor tables in /sys/class/dmi/id don't help us distinguish between GCE
                 * virtual machines and bare-metal instances, so we need to look at hypervisor. */
                واپس VIRTUALIZATION_GOOGLE;
        ورنہ اگر (v != VIRTUALIZATION_NONE)
                جاؤ اختتام;

        /* If we are in Dom0 and have not yet finished, finish with the result of detect_vm_cpuid */
        اگر (xen_dom0 > 0)
                جاؤ اختتام;

        /* Now, let's get back to DMI */
        اگر (dmi < 0)
                واپس dmi;
        اگر (dmi == VIRTUALIZATION_VM_OTHER)
                other = true;
        ورنہ اگر (!اندر_سیٹ(dmi, VIRTUALIZATION_NONE, VIRTUALIZATION_GOOGLE)) {
                /* At this point if GCE has been detected in dmi, do not report as a VM. It should
                 * be a bare-metal machine */
                v = dmi;
                جاؤ اختتام;
        }

        /* Check high-level hypervisor sysfs file */
        v = detect_vm_hypervisor();
        اگر (v < 0)
                واپس v;
        اگر (v == VIRTUALIZATION_VM_OTHER)
                other = true;
        ورنہ اگر (v != VIRTUALIZATION_NONE)
                جاؤ اختتام;

        v = detect_vm_device_درخت();
        اگر (v < 0)
                واپس v;
        اگر (v == VIRTUALIZATION_VM_OTHER)
                other = true;
        ورنہ اگر (v != VIRTUALIZATION_NONE)
                جاؤ اختتام;

        v = detect_vm_zvm();
        اگر (v < 0)
                واپس v;

اختتام:
        /* None of the checks above gave us a clear answer, hence let's now use fallback logic: if hyperv
         * enlightenments are available but the VMM wasn't recognized as anything yet, it's probably
         * Microsoft. */
        اگر (v == VIRTUALIZATION_NONE) {
                اگر (hyperv)
                        v = VIRTUALIZATION_MICROSOFT;
                ورنہ اگر (other)
                        v = VIRTUALIZATION_VM_OTHER;
        }

        cached_ملا = v;
        log_debug("Found VM virtualization %s", virtualization_مقصد_string(v));
        واپس v;
}

ساکن ثابت حرف *ثابت container_جدول[_VIRTUALIZATION_MAX] = {
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

DEFINE_نجی_STRING_جدول_LOOKUP_FROM_STRING(container, صحیح);

ساکن صحیح running_اندر_pidns(خالی) {
        صحیح r;

        r = namespace_is_init(NAMESPACE_PID);
        اگر (r < 0)
                واپس log_debug_errno(r, "Failed to test if in root PID namespace, ignoring: %m");

        واپس !r;
}

ساکن Virtualization detect_container_files(خالی) {
        ساکن ثابت ساخت {
                ثابت حرف *file_راستہ;
                Virtualization id;
        } container_file_جدول[] = {
                /* https://github.com/containers/podman/issues/6192 */
                /* https://github.com/containers/podman/issues/3586#issuecomment-661918679 */
                { "/run/.containerenv", VIRTUALIZATION_PODMAN },
                /* https://github.com/moby/moby/issues/18355 */
                /* Docker must be the last in this table, see below. */
                { "/.dockerenv",        VIRTUALIZATION_DOCKER },
        };

        FOREACH_ELEMENT(file, container_file_جدول) {
                اگر (access(file->file_راستہ, F_OK) >= 0)
                        واپس file->id;

                اگر (errno != ENOENT)
                        log_debug_errno(errno,
                                        "Checking if %s exists failed, ignoring: %m",
                                        file->file_راستہ);
        }

        واپس VIRTUALIZATION_NONE;
}

Virtualization detect_container(خالی) {
        ساکن thread_local Virtualization cached_ملا = _VIRTUALIZATION_INVALID;
        _cleanup_free_ حرف *m = تہی, *o = تہی, *p = تہی;
        ثابت حرف *e = تہی;
        Virtualization v;
        صحیح r;

        اگر (cached_ملا >= 0)
                واپس cached_ملا;

        /* /proc/vz exists in container and outside of the container, /proc/bc only outside of the container. */
        اگر (access("/proc/vz", F_OK) < 0) {
                اگر (errno != ENOENT)
                        log_debug_errno(errno, "Failed to check if /proc/vz exists, ignoring: %m");
        } ورنہ اگر (access("/proc/bc", F_OK) < 0) {
                اگر (errno == ENOENT) {
                        v = VIRTUALIZATION_OPENVZ;
                        جاؤ اختتام;
                }

                log_debug_errno(errno, "Failed to check if /proc/bc exists, ignoring: %m");
        }

        /* "Official" way of detecting WSL https://github.com/Microsoft/WSL/issues/423#issuecomment-221627364 */
        r = read_one_سطر_file("/proc/sys/kernel/osrelease", &o);
        اگر (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/kernel/osrelease, ignoring: %m");
        ورنہ اگر (لڑی_تلاش(o, "Microsoft") || لڑی_تلاش(o, "WSL")) {
                v = VIRTUALIZATION_WSL;
                جاؤ اختتام;
        }

        /* proot doesn't use PID namespacing, so we can just check if we have a matching tracer for this
         * invocation without worrying about it being elsewhere.
         */
        r = get_proc_میدان("/proc/self/status", "TracerPid", &p);
        اگر (r < 0)
                log_debug_errno(r, "Failed to read our own trace PID, ignoring: %m");
        ورنہ اگر (!streq(p, "0")) {
                pid_t ptrace_pid;

                r = parse_pid(p, &ptrace_pid);
                اگر (r < 0)
                        log_debug_errno(r, "Failed to parse our own tracer PID, ignoring: %m");
                ورنہ {
                        _cleanup_free_ حرف *ptrace_comm = تہی;
                        ثابت حرف *pf;

                        pf = procfs_file_alloca(ptrace_pid, "comm");
                        r = read_one_سطر_file(pf, &ptrace_comm);
                        اگر (r < 0)
                                log_debug_errno(r, "Failed to read %s, ignoring: %m", pf);
                        ورنہ اگر (startswith(ptrace_comm, "proot")) {
                                v = VIRTUALIZATION_PROOT;
                                جاؤ اختتام;
                        }
                }
        }

        /* The container manager might have placed this in the /run/host/ hierarchy for us, which is best
         * because we can be consumed just like that, without special privileges. */
        r = read_one_سطر_file("/run/host/container-manager", &m);
        اگر (r > 0) {
                e = m;
                جاؤ translate_name;
        }
        اگر (!اندر_سیٹ(r, -ENOENT, 0))
                واپس log_debug_errno(r, "Failed to read /run/host/container-manager: %m");

        اگر (getpid_cached() == 1) {
                /* If we are PID 1 we can just check our own environment variable, and that's authoritative.
                 * We distinguish three cases:
                 * - the variable is not defined → we jump to other checks
                 * - the variable is defined to an empty value → we are not in a container
                 * - anything else → some container, either one of the known ones or "container-other"
                 */
                e = getenv("container");
                اگر (!e)
                        جاؤ check_files;
                اگر (isempty(e)) {
                        v = VIRTUALIZATION_NONE;
                        جاؤ اختتام;
                }

                جاؤ translate_name;
        }

        /* Otherwise, PID 1 might have dropped this information into a file in /run. This is better than accessing
         * /proc/1/environ, since we don't need CAP_SYS_PTRACE for that. */
        r = read_one_سطر_file("/run/systemd/container", &m);
        اگر (r > 0) {
                e = m;
                جاؤ translate_name;
        }
        اگر (!اندر_سیٹ(r, -ENOENT, 0))
                واپس log_debug_errno(r, "Failed to read /run/systemd/container: %m");

        /* Fallback for cases where PID 1 was not systemd (for example, cases where init=/bin/sh is used. */
        r = getenv_for_pid(1, "container", &m);
        اگر (r > 0) {
                e = m;
                جاؤ translate_name;
        }
        اگر (r < 0) /* This only works if we have CAP_SYS_PTRACE, hence let's better ignore failures here */
                log_debug_errno(r, "Failed to read $container of PID 1, ignoring: %m");

check_files:
        /* Check for existence of some well-known files. We only do this after checking
         * for other specific container managers, otherwise we risk mistaking another
         * container manager for Docker: the /.dockerenv file could inadvertently end up
         * in a file system image. */
        v = detect_container_files();
        اگر (v < 0)
                واپس v;
        اگر (v != VIRTUALIZATION_NONE)
                جاؤ اختتام;

        /* Finally, the root pid namespace has an hardcoded inode number of 0xEFFFFFFC since kernel 3.8, so
         * if all else fails we can check the inode number of our pid namespace and compare it. */
        اگر (running_اندر_pidns() > 0) {
                log_debug("Running in a pid namespace, assuming unknown container manager.");
                v = VIRTUALIZATION_CONTAINER_OTHER;
                جاؤ اختتام;
        }

        /* If none of that worked, give up, assume no container manager. */
        v = VIRTUALIZATION_NONE;
        جاؤ اختتام;

translate_name:
        اگر (streq(e, "oci")) {
                /* Some images hardcode container=oci, but OCI is not a specific container manager.
                 * Try to detect one based on well-known files. */
                v = detect_container_files();
                اگر (v == VIRTUALIZATION_NONE)
                        v = VIRTUALIZATION_CONTAINER_OTHER;
                جاؤ اختتام;
        }
        v = container_from_string(e);
        اگر (v < 0)
                v = VIRTUALIZATION_CONTAINER_OTHER;

اختتام:
        log_debug("Found container virtualization %s.", virtualization_مقصد_string(v));
        cached_ملا = v;
        واپس v;
}

Virtualization detect_virtualization(خالی) {
        صحیح v;

        v = detect_container();
        اگر (v != VIRTUALIZATION_NONE)
                واپس v;

        واپس detect_vm();
}

صحیح running_اندر_userns(خالی) {
        صحیح r;

        r = namespace_is_init(NAMESPACE_USER);
        اگر (r < 0)
                واپس log_debug_errno(r, "Failed to test if in root user namespace, ignoring: %m");

        واپس !r;
}

صحیح running_اندر_chroot(خالی) {
        صحیح r;

        /* If we're PID1, /proc may not be mounted (and most likely we're not in a chroot). But PID1 will
         * mount /proc, so all other programs can assume that if /proc is *not* available, we're in some
         * chroot. */

        r = getenv_bool("SYSTEMD_IN_CHROOT");
        اگر (r >= 0)
                واپس r > 0;
        اگر (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_IN_CHROOT, ignoring: %m");

        /* Deprecated but kept for backwards compatibility. */
        اگر (getenv_bool("SYSTEMD_IGNORE_CHROOT") > 0)
                واپس 0;

        r = inode_same("/proc/1/root", "/", /* flags= */ 0);
        اگر (r == -ENOENT) {
                r = proc_mounted();
                اگر (r == 0) {
                        اگر (getpid_cached() == 1)
                                واپس false; /* We will mount /proc, assuming we're not in a chroot. */

                        log_debug("/proc/ is not mounted, assuming we're in a chroot.");
                        واپس true;
                }
                اگر (r > 0) /* If we have fake /proc/, we can't do the check properly. */
                        واپس -ENOSYS;
        }
        اگر (r < 0)
                واپس log_debug_errno(r, "Failed to check if /proc/1/root and / are the same inode: %m");

        واپس r == 0;
}

#اگر defined(__i386__) || defined(__x86_64__)
ساخت cpuid_جدول_اندراج {
        uint32_t flag_ثنائی_ہندسہ;
        ثابت حرف *name;
};

ساکن ثابت ساخت cpuid_جدول_اندراج leaf1_edx[] = {
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

ساکن ثابت ساخت cpuid_جدول_اندراج leaf1_ecx[] = {
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

ساکن ثابت ساخت cpuid_جدول_اندراج leaf7_ebx[] = {
        {  3, "bmi1"   },
        {  5, "avx2"   },
        {  8, "bmi2"   },
        { 18, "rdseed" },
        { 19, "adx"    },
        { 29, "sha_ni" },
};

ساکن ثابت ساخت cpuid_جدول_اندراج leaf81_edx[] = {
        { 11, "syscall" },
        { 27, "rdtscp"  },
        { 29, "lm"      },
};

ساکن ثابت ساخت cpuid_جدول_اندراج leaf81_ecx[] = {
        {  0, "lahf_lm" },
        {  5, "abm"     },
};

ساکن ثابت ساخت cpuid_جدول_اندراج leaf87_edx[] = {
        {  8, "constant_tsc" },
};

ساکن bool given_flag_اندر_سیٹ(ثابت حرف *flag, ثابت ساخت cpuid_جدول_اندراج *سیٹ, size_t سیٹ_سائز, uint32_t val) {
        أكد(سیٹ);

        برائے (size_t i = 0; i < سیٹ_سائز; i++) {
                اگر ((UINT32_C(1) << سیٹ[i].flag_ثنائی_ہندسہ) & val &&
                                streq(flag, سیٹ[i].name))
                        واپس true;
        }
        واپس false;
}

ساکن bool real_has_cpu_with_flag(ثابت حرف *flag) {
        uint32_t eax, ebx, ecx, edx;

        اگر (__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
                اگر (given_flag_اندر_سیٹ(flag, leaf1_ecx, ELEMENTSOF(leaf1_ecx), ecx))
                        واپس true;

                اگر (given_flag_اندر_سیٹ(flag, leaf1_edx, ELEMENTSOF(leaf1_edx), edx))
                        واپس true;
        }

        اگر (__get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx)) {
                اگر (given_flag_اندر_سیٹ(flag, leaf7_ebx, ELEMENTSOF(leaf7_ebx), ebx))
                        واپس true;
        }

        اگر (__get_cpuid(0x80000001U, &eax, &ebx, &ecx, &edx)) {
                اگر (given_flag_اندر_سیٹ(flag, leaf81_ecx, ELEMENTSOF(leaf81_ecx), ecx))
                        واپس true;

                اگر (given_flag_اندر_سیٹ(flag, leaf81_edx, ELEMENTSOF(leaf81_edx), edx))
                        واپس true;
        }

        اگر (__get_cpuid(0x80000007U, &eax, &ebx, &ecx, &edx))
                اگر (given_flag_اندر_سیٹ(flag, leaf87_edx, ELEMENTSOF(leaf87_edx), edx))
                        واپس true;

        واپس false;
}
#اگر_ختم

#اگر defined(__aarch64__)
ساخت hwcap_جدول_اندراج {
        uint64_t flag_نقاب;
        ثابت حرف *name;
};

ساکن ثابت ساخت hwcap_جدول_اندراج hwcap[] = {
        { HWCAP_FP,           "fp"          },
        { HWCAP_ASIMD,        "asimd"       },
        { HWCAP_EVTSTRM,      "evtstrm"     },
        { HWCAP_AES,          "aes"         },
        { HWCAP_PMULL,        "pmull"       },
        { HWCAP_SHA1,         "sha1"        },
        { HWCAP_SHA2,         "sha2"        },
        { HWCAP_چکری_جانچ_32,        "crc32"       },
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

ساکن ثابت ساخت hwcap_جدول_اندراج hwcap2[] = {
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

ساکن ثابت ساخت hwcap_جدول_اندراج hwcap3[] = {
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

ساکن bool given_flag_اندر_hwcap_سیٹ(
                ثابت حرف *flag,
                ثابت ساخت hwcap_جدول_اندراج *سیٹ,
                size_t سیٹ_سائز,
                غیرعلامتی طویل val) {

        أكد(سیٹ);

        برائے (size_t i = 0; i < سیٹ_سائز; i++)
                اگر ((سیٹ[i].flag_نقاب & val) && streq(flag, سیٹ[i].name))
                        واپس true;

        واپس false;
}

ساکن bool real_has_cpu_with_flag(ثابت حرف *flag) {
        غیرعلامتی طویل val;

        val = getauxval(AT_HWCAP);
        اگر (given_flag_اندر_hwcap_سیٹ(flag, hwcap, ELEMENTSOF(hwcap), val))
                واپس true;

        val = getauxval(AT_HWCAP2);
        اگر (given_flag_اندر_hwcap_سیٹ(flag, hwcap2, ELEMENTSOF(hwcap2), val))
                واپس true;

        val = getauxval(AT_HWCAP3);
        اگر (given_flag_اندر_hwcap_سیٹ(flag, hwcap3, ELEMENTSOF(hwcap3), val))
                واپس true;

        واپس false;
}
#اگر_ختم

bool has_cpu_with_flag(ثابت حرف *flag) {
        /* Check the architecture-specific CPU feature interface when available. */
#اگر defined(__i386__) || defined(__x86_64__) || defined(__aarch64__)
        واپس real_has_cpu_with_flag(flag);
#ورنہ
        واپس false;
#اگر_ختم
}

صحیح cpu_معلوم_features(حرف ***ret) {
        أكد(ret);

#اگر defined(__i386__) || defined(__x86_64__)
        ساکن ثابت ساخت {
                ثابت ساخت cpuid_جدول_اندراج *جدول;
                size_t n_entries;
        } tables[] = {
                { leaf1_edx,  ELEMENTSOF(leaf1_edx),  },
                { leaf1_ecx,  ELEMENTSOF(leaf1_ecx),  },
                { leaf7_ebx,  ELEMENTSOF(leaf7_ebx),  },
                { leaf81_ecx, ELEMENTSOF(leaf81_ecx), },
                { leaf81_edx, ELEMENTSOF(leaf81_edx), },
                { leaf87_edx, ELEMENTSOF(leaf87_edx), },
        };
#وگرنہ_اگر defined(__aarch64__)
        ساکن ثابت ساخت {
                ثابت ساخت hwcap_جدول_اندراج *جدول;
                size_t n_entries;
        } tables[] = {
                { hwcap,  ELEMENTSOF(hwcap),  },
                { hwcap2, ELEMENTSOF(hwcap2), },
                { hwcap3, ELEMENTSOF(hwcap3), },
        };
#اگر_ختم

#اگر defined(__i386__) || defined(__x86_64__) || defined(__aarch64__)
        _cleanup_strv_free_ حرف **features = تہی;
        size_t n = 0;
        صحیح r;

        FOREACH_ELEMENT(جدول, tables)
                FOREACH_ARRAY(i, جدول->جدول, جدول->n_entries) {
                        r = strv_extend_with_سائز(&features, &n, i->name);
                        اگر (r < 0)
                                واپس r;
                }

        *ret = TAKE_PTR(features);
#ورنہ
        *ret = تہی;
#اگر_ختم
        واپس 0;
}

ساکن ثابت حرف *ثابت virtualization_جدول[_VIRTUALIZATION_MAX] = {
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

DEFINE_STRING_جدول_LOOKUP(virtualization, Virtualization);
