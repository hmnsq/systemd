/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <net/if.h>

రకనిర్వచనం గణన {
        FORMAT_IFNAME_IFINDEX              = 1 << 0,
        FORMAT_IFNAME_IFINDEX_WITH_PERCENT = (1 << 1) | FORMAT_IFNAME_IFINDEX,
} FormatIfnameFlag;

పూర్ణాంకం format_ifname_నిండిన(పూర్ణాంకం ifindex, FormatIfnameFlag flag, అక్షరం buf[స్థిర IF_NAMESIZE]);
పూర్ణాంకం format_ifname_నిండిన_alloc(పూర్ణాంకం ifindex, FormatIfnameFlag flag, అక్షరం **ret);

స్థిర అంతఃవరుస పూర్ణాంకం format_ifname(పూర్ణాంకం ifindex, అక్షరం buf[స్థిర IF_NAMESIZE]) {
        తిరిగి format_ifname_నిండిన(ifindex, 0, buf);
}
స్థిర అంతఃవరుస పూర్ణాంకం format_ifname_alloc(పూర్ణాంకం ifindex, అక్షరం **ret) {
        తిరిగి format_ifname_నిండిన_alloc(ifindex, 0, ret);
}

స్థిర అంతఃవరుస అక్షరం* format_ifname_నిండిన_2(పూర్ణాంకం ifindex, FormatIfnameFlag flag, అక్షరం buf[స్థిర IF_NAMESIZE]) {
        (శూన్య) format_ifname_నిండిన(ifindex, flag, buf);
        తిరిగి buf;
}

#నిర్వచించు FORMAT_IFNAME_నిండిన(index, flag) format_ifname_నిండిన_2(index, flag, (అక్షరం[IF_NAMESIZE]){})
#నిర్వచించు FORMAT_IFNAME(index) format_ifname_నిండిన_2(index, 0, (అక్షరం[IF_NAMESIZE]){})
