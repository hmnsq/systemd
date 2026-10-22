/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ Virtualization {
        VIRTUALIZATION_NONE = 0,

        VIRTUALIZATION_VM_ಮೊದಲ,
        VIRTUALIZATION_KVM = VIRTUALIZATION_VM_ಮೊದಲ,
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
        VIRTUALIZATION_VM_ಕೊನೆಯ = VIRTUALIZATION_VM_OTHER,

        VIRTUALIZATION_CONTAINER_ಮೊದಲ,
        VIRTUALIZATION_SYSTEMD_NSPAWN = VIRTUALIZATION_CONTAINER_ಮೊದಲ,
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
        VIRTUALIZATION_CONTAINER_ಕೊನೆಯ = VIRTUALIZATION_CONTAINER_OTHER,

        _VIRTUALIZATION_MAX,
        _VIRTUALIZATION_INVALID = -EINVAL,
        _VIRTUALIZATION_ERRNO_MAX = -ERRNO_MAX, /* ensure full range of errno fits into this enum */
} Virtualization;

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool VIRTUALIZATION_IS_VM(Virtualization x) {
        ಹಿಂತಿರುಗಿ x >= VIRTUALIZATION_VM_ಮೊದಲ && x <= VIRTUALIZATION_VM_ಕೊನೆಯ;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool VIRTUALIZATION_IS_CONTAINER(Virtualization x) {
        ಹಿಂತಿರುಗಿ x >= VIRTUALIZATION_CONTAINER_ಮೊದಲ && x <= VIRTUALIZATION_CONTAINER_ಕೊನೆಯ;
}

Virtualization detect_vm(ಶೂನ್ಯ);
Virtualization detect_container(ಶೂನ್ಯ);
Virtualization detect_virtualization(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ running_ಒಳ_userns(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ running_ಒಳ_chroot(ಶೂನ್ಯ);

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(virtualization, Virtualization);
bool has_cpu_with_flag(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *flag);
ಪೂರ್ಣಾಂಕ cpu_ತಿಳಿದ_features(ಅಕ್ಷರ ***ret);
