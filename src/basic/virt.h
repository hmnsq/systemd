/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ Virtualization {
        VIRTUALIZATION_NONE = 0,

        VIRTUALIZATION_VM_ਪਹਿਲਾ,
        VIRTUALIZATION_KVM = VIRTUALIZATION_VM_ਪਹਿਲਾ,
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
        VIRTUALIZATION_VM_ਆਖਰੀ = VIRTUALIZATION_VM_OTHER,

        VIRTUALIZATION_CONTAINER_ਪਹਿਲਾ,
        VIRTUALIZATION_SYSTEMD_NSPAWN = VIRTUALIZATION_CONTAINER_ਪਹਿਲਾ,
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
        VIRTUALIZATION_CONTAINER_ਆਖਰੀ = VIRTUALIZATION_CONTAINER_OTHER,

        _VIRTUALIZATION_MAX,
        _VIRTUALIZATION_INVALID = -EINVAL,
        _VIRTUALIZATION_ERRNO_MAX = -ERRNO_MAX, /* ensure full range of errno fits into this enum */
} Virtualization;

ਸਥਿਰ ਸਤਰਅੰਦਰ bool VIRTUALIZATION_IS_VM(Virtualization x) {
        ਵਾਪਸ x >= VIRTUALIZATION_VM_ਪਹਿਲਾ && x <= VIRTUALIZATION_VM_ਆਖਰੀ;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool VIRTUALIZATION_IS_CONTAINER(Virtualization x) {
        ਵਾਪਸ x >= VIRTUALIZATION_CONTAINER_ਪਹਿਲਾ && x <= VIRTUALIZATION_CONTAINER_ਆਖਰੀ;
}

Virtualization detect_vm(ਖਾਲੀ);
Virtualization detect_container(ਖਾਲੀ);
Virtualization detect_virtualization(ਖਾਲੀ);

ਪੂਰਨ running_ਵਿੱਚ_userns(ਖਾਲੀ);
ਪੂਰਨ running_ਵਿੱਚ_chroot(ਖਾਲੀ);

DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(virtualization, Virtualization);
bool has_cpu_with_flag(ਅਚਲ ਅੱਖਰ *flag);
ਪੂਰਨ cpu_ਗਿਆਤ_features(ਅੱਖਰ ***ret);
