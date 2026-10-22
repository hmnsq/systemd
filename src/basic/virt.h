/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

రకనిర్వచనం గణన Virtualization {
        VIRTUALIZATION_NONE = 0,

        VIRTUALIZATION_VM_మొదటి,
        VIRTUALIZATION_KVM = VIRTUALIZATION_VM_మొదటి,
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
        VIRTUALIZATION_VM_చివరి = VIRTUALIZATION_VM_OTHER,

        VIRTUALIZATION_CONTAINER_మొదటి,
        VIRTUALIZATION_SYSTEMD_NSPAWN = VIRTUALIZATION_CONTAINER_మొదటి,
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
        VIRTUALIZATION_CONTAINER_చివరి = VIRTUALIZATION_CONTAINER_OTHER,

        _VIRTUALIZATION_MAX,
        _VIRTUALIZATION_INVALID = -EINVAL,
        _VIRTUALIZATION_ERRNO_MAX = -ERRNO_MAX, /* ensure full range of errno fits into this enum */
} Virtualization;

స్థిర అంతఃవరుస bool VIRTUALIZATION_IS_VM(Virtualization x) {
        తిరిగి x >= VIRTUALIZATION_VM_మొదటి && x <= VIRTUALIZATION_VM_చివరి;
}

స్థిర అంతఃవరుస bool VIRTUALIZATION_IS_CONTAINER(Virtualization x) {
        తిరిగి x >= VIRTUALIZATION_CONTAINER_మొదటి && x <= VIRTUALIZATION_CONTAINER_చివరి;
}

Virtualization detect_vm(శూన్య);
Virtualization detect_container(శూన్య);
Virtualization detect_virtualization(శూన్య);

పూర్ణాంకం running_లోపల_userns(శూన్య);
పూర్ణాంకం running_లోపల_chroot(శూన్య);

DECLARE_STRING_పట్టిక_LOOKUP(virtualization, Virtualization);
bool has_cpu_with_flag(స్థిరరాశి అక్షరం *flag);
పూర్ణాంకం cpu_తెలిసిన_features(అక్షరం ***ret);
