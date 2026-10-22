/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <linux/btrfs.h>
#અન્તર્ભાવ <sys/ioctl.h>
#અન્તર્ભાવ <sys/stat.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "btrfs-util.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fs-util.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "string-util.h"

પૂર્ણાંક btrfs_validate_subvolume_name(અચલ અક્ષર *name) {

        જો (!filename_is_valid(name))
                પાછા -EINVAL;

        જો (strlen(name) > BTRFS_SUBVOL_NAME_MAX)
                પાછા -E2BIG;

        પાછા 0;
}

સ્થિર પૂર્ણાંક extract_subvolume_name(અચલ અક્ષર *path, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *fn = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(ret);

        r = path_extract_filename(path, &fn);
        જો (r < 0)
                પાછા r;

        r = btrfs_validate_subvolume_name(fn);
        જો (r < 0)
                પાછા r;

        *ret = TAKE_PTR(fn);
        પાછા 0;
}

પૂર્ણાંક btrfs_subvol_make(પૂર્ણાંક dir_fd, અચલ અક્ષર *path) {
        સંરચના btrfs_ioctl_vol_args args = {};
        _cleanup_free_ અક્ષર *subvolume = ખાલી, *parent = ખાલી;
        _cleanup_close_ પૂર્ણાંક fd = -EBADF;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(wildcard_fd_is_valid(dir_fd));
        પ્રતિજ્ઞા(!isempty(path));

        r = extract_subvolume_name(path, &subvolume);
        જો (r < 0)
                પાછા r;

        r = path_extract_directory(path, &parent);
        જો (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                પાછા r;

        fd = xopenat(dir_fd, parent ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        જો (fd < 0)
                પાછા fd;

        strncpy(args.name, subvolume, કદ(args.name)-1);

        પાછા RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &args));
}

પૂર્ણાંક btrfs_subvol_make_fallback(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, mode_t mode) {
        mode_t old, combined;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~mode);
        combined = old | ~mode;
        જો (combined != ~mode)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, path);
        umask(old);

        જો (r >= 0)
                પાછા 1; /* subvol worked */
        જો (!ERRNO_IS_NOT_SUPPORTED(r))
                પાછા r;

        જો (mkdirat(dir_fd, path, mode) < 0)
                પાછા -errno;

        પાછા 0; /* plain directory */
}
