/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"

تعريفنوع تعداد Virtualization {
        VIRTUALIZATION_NONE = 0,

        VIRTUALIZATION_VM_پہلا,
        VIRTUALIZATION_KVM = VIRTUALIZATION_VM_پہلا,
        VIRTUALIZATION_AMAZON,
        VIRTUALIZATION_QEMU,
        VIRTUALIZATION_BOCHS,
        VIRTUALIZATION_XEN,
        VIRTUALIZATION_UML,
        VIRTUALIZATION_VMWARE,
        VIRTUALIZATION_ORACLE,
        VIRTUALIZATION_MICROSOFT,
        VIRTUALIZATION_ZVM,
        VIRTUALIZATION_PARALLELS,
        VIRTUALIZATION_BHYVE,
        VIRTUALIZATION_QNX,
        VIRTUALIZATION_ACRN,
        VIRTUALIZATION_POWERVM,
        VIRTUALIZATION_APPLE,
        VIRTUALIZATION_SRE,
        VIRTUALIZATION_GOOGLE,
        VIRTUALIZATION_VM_OTHER,
        VIRTUALIZATION_VM_آخری = VIRTUALIZATION_VM_OTHER,

        VIRTUALIZATION_CONTAINER_پہلا,
        VIRTUALIZATION_SYSTEMD_NSPAWN = VIRTUALIZATION_CONTAINER_پہلا,
        VIRTUALIZATION_LXC_LIBVIRT,
        VIRTUALIZATION_LXC,
        VIRTUALIZATION_OPENVZ,
        VIRTUALIZATION_DOCKER,
        VIRTUALIZATION_PODMAN,
        VIRTUALIZATION_RKT,
        VIRTUALIZATION_WSL,
        VIRTUALIZATION_PROOT,
        VIRTUALIZATION_POUCH,
        VIRTUALIZATION_CONTAINER_OTHER,
        VIRTUALIZATION_CONTAINER_آخری = VIRTUALIZATION_CONTAINER_OTHER,

        _VIRTUALIZATION_MAX,
        _VIRTUALIZATION_INVALID = -EINVAL,
        _VIRTUALIZATION_ERRNO_MAX = -ERRNO_MAX, /* ensure full range of errno fits into this enum */
} Virtualization;

ساكن مضمن bool VIRTUALIZATION_IS_VM(Virtualization x) {
        أرجع x >= VIRTUALIZATION_VM_پہلا && x <= VIRTUALIZATION_VM_آخری;
}

ساكن مضمن bool VIRTUALIZATION_IS_CONTAINER(Virtualization x) {
        أرجع x >= VIRTUALIZATION_CONTAINER_پہلا && x <= VIRTUALIZATION_CONTAINER_آخری;
}

Virtualization detect_vm(فراغ);
Virtualization detect_container(فراغ);
Virtualization detect_virtualization(فراغ);

صحيح running_وچ_userns(فراغ);
صحيح running_وچ_chroot(فراغ);

DECLARE_STRING_جدول_LOOKUP(virtualization, Virtualization);
bool has_cpu_with_flag(ثابت حرف *flag);
صحيح cpu_معلوم_features(حرف ***ret);
