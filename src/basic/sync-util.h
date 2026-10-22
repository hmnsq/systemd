/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

पूर्णाङ्क fsync_directory_सम्बन्धि_file(पूर्णाङ्क fd);
पूर्णाङ्क fsync_पूर्णम्(पूर्णाङ्क fd);

पूर्णाङ्क fsync_मार्गः_at(पूर्णाङ्क at_fd, ध्रुव वर्ण *मार्गः);
पूर्णाङ्क fsync_parent_at(पूर्णाङ्क at_fd, ध्रुव वर्ण *मार्गः);
पूर्णाङ्क fsync_मार्गः_and_parent_at(पूर्णाङ्क at_fd, ध्रुव वर्ण *मार्गः);

पूर्णाङ्क syncfs_मार्गः(पूर्णाङ्क at_fd, ध्रुव वर्ण *मार्गः);
