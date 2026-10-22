/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

#شامل "sd-id128.h"

#شامل "../fundamental/efivars.h"        /* IWYU pragma: export */

#تعریف_کرو EFI_VENDOR_LOADER       SD_ID128_MAKE(4a,67,b0,82,0a,4c,41,cf,b6,c7,44,0b,29,bb,8c,4f)
#تعریف_کرو EFI_VENDOR_LOADER_STR   SD_ID128_MAKE_UUID_STR(4a,67,b0,82,0a,4c,41,cf,b6,c7,44,0b,29,bb,8c,4f)
#تعریف_کرو EFI_VENDOR_GLOBAL       SD_ID128_MAKE(8b,e4,df,61,93,ca,11,d2,aa,0d,00,e0,98,03,2b,8c)
#تعریف_کرو EFI_VENDOR_GLOBAL_STR   SD_ID128_MAKE_UUID_STR(8b,e4,df,61,93,ca,11,d2,aa,0d,00,e0,98,03,2b,8c)
#تعریف_کرو EFI_VENDOR_DATABASE     SD_ID128_MAKE(d7,19,b2,cb,3d,3a,45,96,a3,bc,da,d0,0e,67,65,6f)
#تعریف_کرو EFI_VENDOR_DATABASE_STR SD_ID128_MAKE_UUID_STR(d7,19,b2,cb,3d,3a,45,96,a3,bc,da,d0,0e,67,65,6f)
#تعریف_کرو EFI_VENDOR_SYSTEMD      SD_ID128_MAKE(8c,f2,64,4b,4b,0b,42,8f,93,87,6d,87,60,50,dc,67)
#تعریف_کرو EFI_VENDOR_SYSTEMD_STR  SD_ID128_MAKE_UUID_STR(8c,f2,64,4b,4b,0b,42,8f,93,87,6d,87,60,50,dc,67)
#تعریف_کرو EFI_VENDOR_SHIMLOCK     SD_ID128_MAKE(60,5d,ab,50,e0,46,43,00,ab,b6,3d,d8,10,dd,8b,23)
#تعریف_کرو EFI_VENDOR_SHIMLOCK_STR SD_ID128_MAKE_UUID_STR(60,5d,ab,50,e0,46,43,00,ab,b6,3d,d8,10,dd,8b,23)

#تعریف_کرو EFI_VARIABLE_NON_VOLATILE                          UINT32_C(0x00000001)
#تعریف_کرو EFI_VARIABLE_BOOTSERVICE_ACCESS                    UINT32_C(0x00000002)
#تعریف_کرو EFI_VARIABLE_RUNTIME_ACCESS                        UINT32_C(0x00000004)
#تعریف_کرو EFI_VARIABLE_وقت_BASED_AUTHENTICATED_WRITE_ACCESS UINT32_C(0x00000020)

/* Note that the <lowercaseuuid>-<varname> naming scheme is an efivarfs convention, i.e. part of the Linux
 * API file system implementation for EFI. EFI itself processes UIDS in binary form.
 */

#تعریف_کرو EFI_VENDOR_VARIABLE_STR(vendor, name) name "-" vendor

#تعریف_کرو EFI_GLOBAL_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_GLOBAL_STR, name)
#تعریف_کرو EFI_LOADER_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_LOADER_STR, name)
#تعریف_کرو EFI_SYSTEMD_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_SYSTEMD_STR, name)
#تعریف_کرو EFI_SHIMLOCK_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_SHIMLOCK_STR, name)

#تعریف_کرو EFIVAR_راستہ(variable) "/sys/firmware/efi/efivars/" variable
#تعریف_کرو EFIVAR_CACHE_راستہ(variable) "/run/systemd/efivars/" variable

#اگر ENABLE_EFI

صحیح efi_get_variable(ثابت حرف *variable, uint32_t *attribute, خالی **ret_قدر, size_t *ret_سائز);
صحیح efi_get_variable_string(ثابت حرف *variable, حرف **ret);
صحیح efi_get_variable_راستہ(ثابت حرف *variable, حرف **ret);
صحیح efi_سیٹ_variable(ثابت حرف *variable, ثابت خالی *قدر, size_t سائز) _nonnull_if_nonzero_(2, 3);
صحیح efi_سیٹ_variable_string(ثابت حرف *variable, ثابت حرف *قدر);

bool سیٹ_efi_boot(bool b);
bool is_efi_boot(خالی);
bool is_efi_secure_boot(خالی);
Secure_Boot_منوال efi_get_secure_boot_منوال(خالی);

#ورنہ

ساکن اندرونی صحیح efi_get_variable(ثابت حرف *variable, uint32_t *attribute, خالی **قدر, size_t *سائز) {
        واپس -EOPNOTSUPP;
}

ساکن اندرونی صحیح efi_get_variable_string(ثابت حرف *variable, حرف **ret) {
        واپس -EOPNOTSUPP;
}

ساکن اندرونی صحیح efi_get_variable_راستہ(ثابت حرف *variable, حرف **ret) {
        واپس -EOPNOTSUPP;
}

ساکن اندرونی صحیح efi_سیٹ_variable(ثابت حرف *variable, ثابت خالی *قدر, size_t سائز) {
        واپس -EOPNOTSUPP;
}

ساکن اندرونی صحیح efi_سیٹ_variable_string(ثابت حرف *variable, ثابت حرف *p) {
        واپس -EOPNOTSUPP;
}

ساکن اندرونی bool سیٹ_efi_boot(bool b) {
        واپس false;
}

ساکن اندرونی bool is_efi_boot(خالی) {
        واپس false;
}

ساکن اندرونی bool is_efi_secure_boot(خالی) {
        واپس false;
}

ساکن اندرونی Secure_Boot_منوال efi_get_secure_boot_منوال(خالی) {
        واپس SECURE_BOOT_UNKNOWN;
}
#اگر_ختم

حرف *efi_tilt_backslashes(حرف *s);
