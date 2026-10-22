/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಅಕ್ಷರ* bus_label_escape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s);
ಅಕ್ಷರ* bus_label_unescape_n(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *f, size_t l);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ* bus_label_unescape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *f) {
        ಹಿಂತಿರುಗಿ bus_label_unescape_n(f, SIZE_MAX);
}
