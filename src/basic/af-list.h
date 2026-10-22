/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಧ್ರುವ ವರ್ಣ* af_to_name(ಪೂರ್ಣಾಙ್ಕ id) _const_;
ಪೂರ್ಣಾಙ್ಕ af_from_name(ಧ್ರುವ ವರ್ಣ *name) _pure_;

ಧ್ರುವ ವರ್ಣ* af_to_name_short(ಪೂರ್ಣಾಙ್ಕ id) _const_;

ಧ್ರುವ ವರ್ಣ* af_to_ipv4_ipv6(ಪೂರ್ಣಾಙ್ಕ id) _const_;
ಪೂರ್ಣಾಙ್ಕ af_from_ipv4_ipv6(ಧ್ರುವ ವರ್ಣ *af) _pure_;

ಪೂರ್ಣಾಙ್ಕ af_max(ಶೂನ್ಯ) _const_;
