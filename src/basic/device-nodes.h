/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪೂರ್ಣಾಙ್ಕ encode_devnode_name(ಧ್ರುವ ವರ್ಣ *str, ವರ್ಣ *str_enc, size_t len);
ಪೂರ್ಣಾಙ್ಕ allow_listed_char_for_devnode(ವರ್ಣ c, ಧ್ರುವ ವರ್ಣ *additional);

ಪೂರ್ಣಾಙ್ಕ devnode_same(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *b);
