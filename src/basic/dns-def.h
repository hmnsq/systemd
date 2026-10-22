/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

/* Length of a single label, with all escaping removed, excluding any trailing dot or NUL byte */
#నిర్వచించు DNS_LABEL_MAX 63

/* Worst case length of a single label, with all escaping applied and room for a trailing NUL byte. */
#నిర్వచించు DNS_LABEL_ESCAPED_MAX (DNS_LABEL_MAX*4+1)

/* Maximum length of a full hostname, consisting of a series of unescaped labels, and no trailing dot or NUL byte */
#నిర్వచించు DNS_HOSTNAME_MAX 253

/* Maximum length of a full hostname, on the wire, including the final NUL byte */
#నిర్వచించు DNS_WIRE_FORMAT_HOSTNAME_MAX 255

/* Maximum number of labels per valid hostname */
#నిర్వచించు DNS_N_LABELS_MAX 127

/* RFC 1035 § 4.1.4 name compression: the two high bits tag a 16-bit field as a pointer, the
 * remaining 14 bits carry the offset, which is hence also the last position a name a pointer can
 * reference may start at. */
#నిర్వచించు DNS_COMPRESSION_సూచిక_FLAG UINT16_C(0xC000)
#నిర్వచించు DNS_COMPRESSION_స్థానభ్రంశం_MAX UINT16_C(0x3FFF)
assert_cc((DNS_COMPRESSION_సూచిక_FLAG & DNS_COMPRESSION_స్థానభ్రంశం_MAX) == 0);
assert_cc((DNS_COMPRESSION_సూచిక_FLAG | DNS_COMPRESSION_స్థానభ్రంశం_MAX) == UINT16_MAX);
/* The tag is the top two bits, which is what lets a reader take it from the first octet alone. */
assert_cc((DNS_COMPRESSION_సూచిక_FLAG & UINT16_C(0xFF)) == 0);
