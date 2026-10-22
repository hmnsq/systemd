/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

#అన్తర్భావ "sd-id128.h"

#అన్తర్భావ "../fundamental/efivars.h"        /* IWYU pragma: export */

#నిర్వచించు EFI_VENDOR_LOADER       SD_ID128_MAKE(4a,67,b0,82,0a,4c,41,cf,b6,c7,44,0b,29,bb,8c,4f)
#నిర్వచించు EFI_VENDOR_LOADER_STR   SD_ID128_MAKE_UUID_STR(4a,67,b0,82,0a,4c,41,cf,b6,c7,44,0b,29,bb,8c,4f)
#నిర్వచించు EFI_VENDOR_GLOBAL       SD_ID128_MAKE(8b,e4,df,61,93,ca,11,d2,aa,0d,00,e0,98,03,2b,8c)
#నిర్వచించు EFI_VENDOR_GLOBAL_STR   SD_ID128_MAKE_UUID_STR(8b,e4,df,61,93,ca,11,d2,aa,0d,00,e0,98,03,2b,8c)
#నిర్వచించు EFI_VENDOR_DATABASE     SD_ID128_MAKE(d7,19,b2,cb,3d,3a,45,96,a3,bc,da,d0,0e,67,65,6f)
#నిర్వచించు EFI_VENDOR_DATABASE_STR SD_ID128_MAKE_UUID_STR(d7,19,b2,cb,3d,3a,45,96,a3,bc,da,d0,0e,67,65,6f)
#నిర్వచించు EFI_VENDOR_SYSTEMD      SD_ID128_MAKE(8c,f2,64,4b,4b,0b,42,8f,93,87,6d,87,60,50,dc,67)
#నిర్వచించు EFI_VENDOR_SYSTEMD_STR  SD_ID128_MAKE_UUID_STR(8c,f2,64,4b,4b,0b,42,8f,93,87,6d,87,60,50,dc,67)
#నిర్వచించు EFI_VENDOR_SHIMLOCK     SD_ID128_MAKE(60,5d,ab,50,e0,46,43,00,ab,b6,3d,d8,10,dd,8b,23)
#నిర్వచించు EFI_VENDOR_SHIMLOCK_STR SD_ID128_MAKE_UUID_STR(60,5d,ab,50,e0,46,43,00,ab,b6,3d,d8,10,dd,8b,23)

#నిర్వచించు EFI_VARIABLE_NON_VOLATILE                          UINT32_C(0x00000001)
#నిర్వచించు EFI_VARIABLE_BOOTSERVICE_ACCESS                    UINT32_C(0x00000002)
#నిర్వచించు EFI_VARIABLE_RUNTIME_ACCESS                        UINT32_C(0x00000004)
#నిర్వచించు EFI_VARIABLE_సమయం_BASED_AUTHENTICATED_WRITE_ACCESS UINT32_C(0x00000020)

/* Note that the <lowercaseuuid>-<varname> naming scheme is an efivarfs convention, i.e. part of the Linux
 * API file system implementation for EFI. EFI itself processes UIDS in binary form.
 */

#నిర్వచించు EFI_VENDOR_VARIABLE_STR(vendor, name) name "-" vendor

#నిర్వచించు EFI_GLOBAL_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_GLOBAL_STR, name)
#నిర్వచించు EFI_LOADER_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_LOADER_STR, name)
#నిర్వచించు EFI_SYSTEMD_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_SYSTEMD_STR, name)
#నిర్వచించు EFI_SHIMLOCK_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_SHIMLOCK_STR, name)

#నిర్వచించు EFIVAR_మార్గం(variable) "/sys/firmware/efi/efivars/" variable
#నిర్వచించు EFIVAR_CACHE_మార్గం(variable) "/run/systemd/efivars/" variable

#ఒకవేళ ENABLE_EFI

పూర్ణాంకం efi_get_variable(స్థిరరాశి అక్షరం *variable, uint32_t *attribute, శూన్య **ret_విలువ, size_t *ret_కొలత);
పూర్ణాంకం efi_get_variable_string(స్థిరరాశి అక్షరం *variable, అక్షరం **ret);
పూర్ణాంకం efi_get_variable_మార్గం(స్థిరరాశి అక్షరం *variable, అక్షరం **ret);
పూర్ణాంకం efi_సమితి_variable(స్థిరరాశి అక్షరం *variable, స్థిరరాశి శూన్య *విలువ, size_t కొలత) _nonnull_if_nonzero_(2, 3);
పూర్ణాంకం efi_సమితి_variable_string(స్థిరరాశి అక్షరం *variable, స్థిరరాశి అక్షరం *విలువ);

bool సమితి_efi_boot(bool b);
bool is_efi_boot(శూన్య);
bool is_efi_secure_boot(శూన్య);
Secure_Boot_బహుళకం efi_get_secure_boot_బహుళకం(శూన్య);

#లేకపోతే

స్థిర అంతఃవరుస పూర్ణాంకం efi_get_variable(స్థిరరాశి అక్షరం *variable, uint32_t *attribute, శూన్య **విలువ, size_t *కొలత) {
        తిరిగి -EOPNOTSUPP;
}

స్థిర అంతఃవరుస పూర్ణాంకం efi_get_variable_string(స్థిరరాశి అక్షరం *variable, అక్షరం **ret) {
        తిరిగి -EOPNOTSUPP;
}

స్థిర అంతఃవరుస పూర్ణాంకం efi_get_variable_మార్గం(స్థిరరాశి అక్షరం *variable, అక్షరం **ret) {
        తిరిగి -EOPNOTSUPP;
}

స్థిర అంతఃవరుస పూర్ణాంకం efi_సమితి_variable(స్థిరరాశి అక్షరం *variable, స్థిరరాశి శూన్య *విలువ, size_t కొలత) {
        తిరిగి -EOPNOTSUPP;
}

స్థిర అంతఃవరుస పూర్ణాంకం efi_సమితి_variable_string(స్థిరరాశి అక్షరం *variable, స్థిరరాశి అక్షరం *p) {
        తిరిగి -EOPNOTSUPP;
}

స్థిర అంతఃవరుస bool సమితి_efi_boot(bool b) {
        తిరిగి false;
}

స్థిర అంతఃవరుస bool is_efi_boot(శూన్య) {
        తిరిగి false;
}

స్థిర అంతఃవరుస bool is_efi_secure_boot(శూన్య) {
        తిరిగి false;
}

స్థిర అంతఃవరుస Secure_Boot_బహుళకం efi_get_secure_boot_బహుళకం(శూన్య) {
        తిరిగి SECURE_BOOT_UNKNOWN;
}
#అయితే_ముగింపు

అక్షరం *efi_tilt_backslashes(అక్షరం *s);
