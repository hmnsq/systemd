#!/usr/bin/env python3
# SPDX-License-Identifier: LGPL-2.1-or-later

దిగుమతి shlex
దిగుమతి subprocess
దిగుమతి sys
తః collections.abc దిగుమతి Iterator

NAME_గమ్యం_MAGIC = {
    'apparmorfs':      ['AAFS_MAGIC'],
    'adfs':            ['ADFS_SUPER_MAGIC'],
    'affs':            ['AFFS_SUPER_MAGIC'],
    'afs':             ['AFS_FS_MAGIC',
                        'AFS_SUPER_MAGIC'],
    'anon_inodefs':    ['ANON_INODE_FS_MAGIC'],
    'autofs':          ['AUTOFS_SUPER_MAGIC'],
    'balloon-kvm':     ['BALLOON_KVM_MAGIC'],
    'bcachefs':        ['BCACHEFS_SUPER_MAGIC'],
    'bdev':            ['BDEVFS_MAGIC'],
    'binder':          ['BINDERFS_SUPER_MAGIC'],
    'binfmt_misc':     ['BINFMTFS_MAGIC'],
    'bpf':             ['BPF_FS_MAGIC'],
    'btrfs':           ['BTRFS_SUPER_MAGIC'],
    'btrfs_test_fs':   ['BTRFS_TEST_MAGIC'],
    # cpuset's magic got reassigned to cgroupfs
    'cpuset':          ['CGROUP_SUPER_MAGIC'],
    'ceph':            ['CEPH_SUPER_MAGIC'],
    'cgroup2':         ['CGROUP2_SUPER_MAGIC'],
    # note that the cgroupfs magic got reassigned from cpuset
    'cgroup':          ['CGROUP_SUPER_MAGIC'],
    'cifs':            ['CIFS_SUPER_MAGIC',
                        'SMB2_SUPER_MAGIC'],
    'coda':            ['CODA_SUPER_MAGIC'],
    'configfs':        ['CONFIGFS_MAGIC'],
    'cramfs':          ['CRAMFS_MAGIC'],
    'dax':             ['DAXFS_MAGIC'],
    'debugfs':         ['DEBUGFS_MAGIC'],
    'devmem':          ['DEVMEM_MAGIC'],
    'devpts':          ['DEVPTS_SUPER_MAGIC'],
    # devtmpfs is just a special instance of tmpfs, hence it reports its magic
    'devtmpfs':        ['TMPFS_MAGIC'],
    'dmabuf':          ['DMA_BUF_MAGIC'],
    'ecryptfs':        ['ECRYPTFS_SUPER_MAGIC'],
    'efivarfs':        ['EFIVARFS_MAGIC'],
    'efs':             ['EFS_SUPER_MAGIC'],
    'erofs':           ['EROFS_SUPER_MAGIC_V1'],
    # ext2 + ext3 + ext4 use the same magic
    'ext2':            ['EXT2_SUPER_MAGIC'],
    'ext3':            ['EXT3_SUPER_MAGIC'],
    'ext4':            ['EXT4_SUPER_MAGIC'],
    'exfat':           ['EXFAT_SUPER_MAGIC'],
    'f2fs':            ['F2FS_SUPER_MAGIC'],
    'failfs':          ['FAIL_FS_MAGIC'],
    # fuseblk is so closely related to fuse that it shares the same magic
    'fuseblk':         ['FUSE_SUPER_MAGIC'],
    'fuse':            ['FUSE_SUPER_MAGIC'],
    'fusectl':         ['FUSE_CTL_SUPER_MAGIC'],
    # gfs is an old version of gfs2 and reuses the magic
    'gfs':             ['GFS2_MAGIC'],
    'gfs2':            ['GFS2_MAGIC'],
    'gmem':            ['GUEST_MEMFD_MAGIC'],
    'hostfs':          ['HOSTFS_SUPER_MAGIC'],
    'hpfs':            ['HPFS_SUPER_MAGIC'],
    'hugetlbfs':       ['HUGETLBFS_MAGIC'],
    'iso9660':         ['ISOFS_SUPER_MAGIC'],
    'jffs2':           ['JFFS2_SUPER_MAGIC'],
    'minix':           ['MINIX_SUPER_MAGIC',
                        'MINIX_SUPER_MAGIC2',
                        'MINIX2_SUPER_MAGIC',
                        'MINIX2_SUPER_MAGIC2',
                        'MINIX3_SUPER_MAGIC'],
    'mqueue':          ['MQUEUE_MAGIC'],
    # msdos is an older legacy version of vfat, shares the magic
    'msdos':           ['MSDOS_SUPER_MAGIC'],
    # ncp/ncpfs have been removed from the kernel, but ncpfs was the official name
    'ncp':             ['NCP_SUPER_MAGIC'],
    'ncpfs':           ['NCP_SUPER_MAGIC'],
    # nfs is the old version of nfs4, and they share the same magic
    'nfs':             ['NFS_SUPER_MAGIC'],
    'nfs4':            ['NFS_SUPER_MAGIC'],
    'nilfs2':          ['NILFS_SUPER_MAGIC'],
    'nsfs':            ['NSFS_MAGIC'],
    'ntfs':            ['NTFS_SB_MAGIC'],
    'ntfs3':           ['NTFS3_SUPER_MAGIC'],
    'nullfs':          ['NULL_FS_MAGIC'],
    'ocfs2':           ['OCFS2_SUPER_MAGIC'],
    'openpromfs':      ['OPENPROM_SUPER_MAGIC'],
    'orangefs':        ['ORANGEFS_DEVREQ_MAGIC'],
    'overlay':         ['OVERLAYFS_SUPER_MAGIC'],
    'pidfs':           ['PID_FS_MAGIC'],
    'pipefs':          ['PIPEFS_MAGIC'],
    'ppc-cmm':         ['PPC_CMM_MAGIC'],
    'proc':            ['PROC_SUPER_MAGIC'],
    'pstore':          ['PSTOREFS_MAGIC'],
    # pvfs2 is the old version of orangefs
    'pvfs2':           ['ORANGEFS_DEVREQ_MAGIC'],
    'qnx4':            ['QNX4_SUPER_MAGIC'],
    'qnx6':            ['QNX6_SUPER_MAGIC'],
    'ramfs':           ['RAMFS_MAGIC'],
    'resctrl':         ['RDTGROUP_SUPER_MAGIC'],
    'reiserfs':        ['REISERFS_SUPER_MAGIC'],
    'rpc_pipefs':      ['RPC_PIPEFS_SUPER_MAGIC'],
    'secretmem':       ['SECRETMEM_MAGIC'],
    'securityfs':      ['SECURITYFS_MAGIC'],
    'selinuxfs':       ['SELINUX_MAGIC'],
    'shiftfs':         ['SHIFTFS_MAGIC'],
    'smackfs':         ['SMACK_MAGIC'],
    # smb3 is an alias for cifs
    'smb3':            ['CIFS_SUPER_MAGIC'],
    # smbfs was removed from the kernel in 2010, the magic remains
    'smbfs':           ['SMB_SUPER_MAGIC'],
    'sockfs':          ['SOCKFS_MAGIC'],
    'squashfs':        ['SQUASHFS_MAGIC'],
    'sysfs':           ['SYSFS_MAGIC'],
    # note that devtmpfs shares the same magic with tmpfs, given it is just a special named instance of it.
    'tmpfs':           ['TMPFS_MAGIC'],
    'tracefs':         ['TRACEFS_MAGIC'],
    'udf':             ['UDF_SUPER_MAGIC'],
    'usbdevfs':        ['USBDEVICE_SUPER_MAGIC'],
    'vboxsf':          ['VBOXSF_SUPER_MAGIC'],
    # note that msdos shares the same magic (and is the older version)
    'vfat':            ['MSDOS_SUPER_MAGIC'],
    'v9fs':            ['V9FS_MAGIC'],
    'xenfs':           ['XENFS_SUPER_MAGIC'],
    'xfs':             ['XFS_SUPER_MAGIC'],
    'z3fold':          ['Z3FOLD_MAGIC'],
    'zonefs':          ['ZONEFS_MAGIC'],
    'zsmalloc':        ['ZSMALLOC_MAGIC'],
}  # fmt: skip

# System magics are sometimes not unique, because file systems got new
# revisions or got renamed. Let's prefer newer over older here, and thus ignore
# the old names.
OBSOLETE_పేర్లు = {
    'cpuset',    # magic taken over by cgroupfs
    'devtmpfs',  # not a file system of its own, but just a "named superblock" of tmpfs
    'ext2',      # ext4 is the newest revision of ext2 + ext3
    'ext3',
    'fuseblk',   # closely related to fuse; they share a single magic, but the latter is more common
    'gfs',       # magic taken over by gfs2
    'msdos',     # vfat is the newest revision of msdos
    'ncp',       # ncpfs (not ncp) was the last name of the netware 'file_system_type'
                 # name before it was removed in 2018
    'nfs',       # nfs4 is the newest revision of nfs
    'pvfs2',     # orangefs is the new name of pvfs2
    'smb3',      # smb3 is an alias for cifs
}  # fmt: skip

FILESYSTEM_SETS = [
    (
        '@basic-api',
        'Basic filesystem API',
        'cgroup',
        'cgroup2',
        'devpts',
        'devtmpfs',
        'mqueue',
        'proc',
        'sysfs',
    ),
    (
        '@anonymous',
        'Anonymous inodes',
        'anon_inodefs',
        'pipefs',
        'sockfs',
    ),
    (
        '@application',
        'Application virtual filesystems',
        'autofs',
        'fuse',
        'overlay',
    ),
    (
        '@auxiliary-api',
        'Auxiliary filesystem API',
        'binfmt_misc',
        'configfs',
        'efivarfs',
        'fusectl',
        'hugetlbfs',
        'rpc_pipefs',
        'securityfs',
    ),
    (
        '@common-block',
        'Common block device filesystems',
        'btrfs',
        'erofs',
        'exfat',
        'ext4',
        'f2fs',
        'iso9660',
        'ntfs3',
        'squashfs',
        'udf',
        'vfat',
        'xfs',
    ),
    (
        '@historical-block',
        'Historical block device filesystems',
        'ext2',
        'ext3',
        'minix',
    ),
    (
        '@network',
        'Well-known network filesystems',
        'afs',
        'ceph',
        'cifs',
        'gfs',
        'gfs2',
        'ncp',
        'ncpfs',
        'nfs',
        'nfs4',
        'ocfs2',
        'orangefs',
        'pvfs2',
        'smb3',
        'smbfs',
    ),
    (
        '@privileged-api',
        'Privileged filesystem API',
        'bpf',
        'debugfs',
        'pstore',
        'tracefs',
    ),
    (
        '@security',
        'Security/MAC API VFS',
        'apparmorfs',
        'selinuxfs',
        'smackfs',
    ),
    (
        '@temporary',
        'Temporary filesystems',
        'ramfs',
        'tmpfs',
    ),
    (
        '@known',
        'All known filesystems declared in the kernel',
        *NAME_గమ్యం_MAGIC.కీలకాలు(),
    ),
]


నిర్వచనం generate_gperf():
    ముద్రించు('''\
/* SPDX-License-Identifier: LGPL-2.1-or-later */
%{
#if __GNUC__ >= 15
_Pragma("GCC diagnostic ignored \\"-Wzero-as-null-pointer-constant\\"")
#endif
#include <linux/magic.h>

#include "filesystems.h"
#include "stat-util.h"

struct FilesystemMagic {
        const char *name;
        statfs_f_type_t magic[FILESYSTEM_MAGIC_MAX];
};
%}
struct FilesystemMagic;
%language=ANSI-C
%define hash-function-name filesystems_gperf_hash
%define lookup-function-name filesystems_gperf_lookup
%define slot-name name
%readonly-tables
%omit-struct-type
%struct-type
%includes
%%''')
    కొరకు name, magics లో NAME_గమ్యం_MAGIC.వస్తువులు():
        ముద్రించు(f'{name + ",":16} {{{", ".join(magics)}}}')


నిర్వచనం generate_fs_type_గమ్యం_string():
    ముద్రించు('''\
#include <linux/magic.h>
#include "filesystems.h"

/* PROJECT_FILE, which is used by log_xyz() thus also used by assert_not_reached(), cannot be used in
 * generated files, as the build directory may be outside of the source directory. */
#undef PROJECT_FILE
#define PROJECT_FILE __FILE__

const char* fs_type_to_string(statfs_f_type_t magic) {
        switch (magic) {''')

    కొరకు name, magics లో NAME_గమ్యం_MAGIC.వస్తువులు():
        ఒకవేళ name లో OBSOLETE_పేర్లు:
            కొనసాగు
        కొరకు magic లో magics:
            ముద్రించు(f'        case (statfs_f_type_t) {magic}:')
        ముద్రించు(f'                return "{name}";')

    ముద్రించు('''\
        }
        return NULL;
}''')


నిర్వచనం generate_fs_లోపల_సమూహం():
    ముద్రించు('bool fs_in_group(const struct statfs *st, FilesystemGroups fs_group) {')
    ముద్రించు('        switch (fs_group) {')

    కొరకు name, _, *filesystems లో FILESYSTEM_SETS:
        magics = sorted(సమితి(కూడిక((NAME_గమ్యం_MAGIC[fs] కొరకు fs లో filesystems), ప్రారంభం=[])))
        enum = 'FILESYSTEM_SET_' + name[1:].పై().replace('-', '_')
        ముద్రించు(f'        case {enum}:')
        opts = '\n                    || '.join(f'F_TYPE_EQUAL(st->f_type, {magic})' కొరకు magic లో magics)
        ముద్రించు(f'                return {opts};')

    ముద్రించు('        default: assert_not_reached();')
    ముద్రించు('        }')
    ముద్రించు('}')


నిర్వచనం generate_filesystem_sets():
    ముద్రించు('const FilesystemSet filesystem_sets[_FILESYSTEM_SET_MAX] = {')

    కొరకు name, desc, *filesystems లో FILESYSTEM_SETS:
        enum = 'FILESYSTEM_SET_' + name[1:].పై().replace('-', '_')

        ముద్రించు(f'        [{enum}] = {{')
        ముద్రించు(f'                .name = "{name}",')
        ముద్రించు(f'                .help = "{desc}",')
        ముద్రించు('                .value =')
        కొరకు filesystem లో filesystems:
            ముద్రించు(f'                "{filesystem}\\0"')
        ముద్రించు('        },')

    ముద్రించు('};')


నిర్వచనం magic_నిర్వచిస్తుంది(వాదనలు: list[str]) -> Iterator[str]:
    cpp = shlex.విభజించు(వాదనలు[0])
    include_dirs = వాదనలు[1:]
    బహిర్గతం = subprocess.check_output(
        [*cpp, '-dM', '-include', 'linux/magic.h', *include_dirs, '-'],
        stdin=subprocess.DEVNULL,
        పాఠ్యం=నిజం,
    )
    కొరకు పంక్తి లో బహిర్గతం.splitlines():
        _, name, *rest = పంక్తి.విభజించు()
        ఒకవేళ (
            '_MAGIC' లో name
            మరియు rest
            మరియు rest[0].startswith('0x')
            మరియు name
            కాదు లో {
                'STACK_END_MAGIC',
                'MTD_INODE_FS_MAGIC',
                'FUTEXFS_SUPER_MAGIC',
                'CRAMFS_MAGIC_WEND',
            }
        ):
            ఇవ్వు name


నిర్వచనం check(వాదనలు: list[str]) -> ఏమీలేదు:
    kernel_magics = సమితి(magic_నిర్వచిస్తుంది(వాదనలు))
    our_magics = సమితి(కూడిక(NAME_గమ్యం_MAGIC.values(), ప్రారంభం=[]))
    extra = kernel_magics - our_magics
    ఒకవేళ extra:
        sys.exit(f'kernel knows additional filesystem magics: {", ".join(sorted(extra))}')


ఒకవేళ __name__ == '__main__':
    వాదనలు = sys.argv[1:]

    కొరకు i, arg లో enumerate(వాదనలు):
        ఒకవేళ arg == 'gperf':
            generate_gperf()
        లేదావేళ arg == 'fs-type-to-string':
            generate_fs_type_గమ్యం_string()
        లేదావేళ arg == 'filesystem-sets':
            generate_filesystem_sets()
        లేదావేళ arg == 'fs-in-group':
            generate_fs_లోపల_సమూహం()
        లేదావేళ arg == 'check':
            # 'check' must be the last command; pass the remaining arguments to check().
            check(వాదనలు[i + 1 :])
            ఆపు
        లేకపోతే:
            లేపు విలువ_Error
