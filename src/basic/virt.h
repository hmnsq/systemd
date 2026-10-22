/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା Virtualization {
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

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool VIRTUALIZATION_IS_VM(Virtualization x) {
        ଫେରନ୍ତୁ x >= VIRTUALIZATION_VM_FIRST && x <= VIRTUALIZATION_VM_LAST;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool VIRTUALIZATION_IS_CONTAINER(Virtualization x) {
        ଫେରନ୍ତୁ x >= VIRTUALIZATION_CONTAINER_FIRST && x <= VIRTUALIZATION_CONTAINER_LAST;
}

Virtualization detect_vm(ଶୂନ୍ୟ);
Virtualization detect_container(ଶୂନ୍ୟ);
Virtualization detect_virtualization(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା running_in_userns(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା running_in_chroot(ଶୂନ୍ୟ);

DECLARE_STRING_TABLE_LOOKUP(virtualization, Virtualization);
bool has_cpu_with_flag(ଅଚଳ ଅକ୍ଷର *flag);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା cpu_known_features(ଅକ୍ଷର ***ret);
