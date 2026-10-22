/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

प्रकार क्रमागत Virtualization {
        VIRTUALIZATION_NONE = 0,

        VIRTUALIZATION_VM_FIRST,
        VIRTUALIZATION_KVM = VIRTUALIZATION_VM_FIRST,
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
        VIRTUALIZATION_VM_LAST = VIRTUALIZATION_VM_OTHER,

        VIRTUALIZATION_CONTAINER_FIRST,
        VIRTUALIZATION_SYSTEMD_NSPAWN = VIRTUALIZATION_CONTAINER_FIRST,
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
        VIRTUALIZATION_CONTAINER_LAST = VIRTUALIZATION_CONTAINER_OTHER,

        _VIRTUALIZATION_MAX,
        _VIRTUALIZATION_INVALID = -EINVAL,
        _VIRTUALIZATION_ERRNO_MAX = -ERRNO_MAX, /* ensure full range of errno fits into this enum */
} Virtualization;

अटल अंतरभूत bool VIRTUALIZATION_IS_VM(Virtualization x) {
        वापस x >= VIRTUALIZATION_VM_FIRST && x <= VIRTUALIZATION_VM_LAST;
}

अटल अंतरभूत bool VIRTUALIZATION_IS_CONTAINER(Virtualization x) {
        वापस x >= VIRTUALIZATION_CONTAINER_FIRST && x <= VIRTUALIZATION_CONTAINER_LAST;
}

Virtualization detect_vm(व्योम);
Virtualization detect_container(व्योम);
Virtualization detect_virtualization(व्योम);

पूर्णांक running_in_userns(व्योम);
पूर्णांक running_in_chroot(व्योम);

DECLARE_STRING_TABLE_LOOKUP(virtualization, Virtualization);
bool has_cpu_with_flag(स्थिर अक्षर *flag);
पूर्णांक cpu_known_features(अक्षर ***ret);
