/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

வகைவரை எண்ணிக்கை Virtualization {
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

நிலையான உள்வரி bool VIRTUALIZATION_IS_VM(Virtualization x) {
        திரும்பு x >= VIRTUALIZATION_VM_FIRST && x <= VIRTUALIZATION_VM_LAST;
}

நிலையான உள்வரி bool VIRTUALIZATION_IS_CONTAINER(Virtualization x) {
        திரும்பு x >= VIRTUALIZATION_CONTAINER_FIRST && x <= VIRTUALIZATION_CONTAINER_LAST;
}

Virtualization detect_vm(வெறுமை);
Virtualization detect_container(வெறுமை);
Virtualization detect_virtualization(வெறுமை);

முழு running_in_userns(வெறுமை);
முழு running_in_chroot(வெறுமை);

DECLARE_STRING_TABLE_LOOKUP(virtualization, Virtualization);
bool has_cpu_with_flag(மாறிலி எழுத்து *flag);
முழு cpu_known_features(எழுத்து ***ret);
