# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF']);
(priority-break-system) begin
(priority-break-system) Low thread should have priority 32.  Actual priority: 32.
(priority-break-system) Low thread should have priority 33.  Actual priority: 33.
(priority-break-system) Medium thread should have priority 33.  Actual priority: 33.
(priority-break-system) Medium thread got the lock.
(priority-break-system) High thread got the lock.
(priority-break-system) High thread finished.
(priority-break-system) High thread should have just finished.
(priority-break-system) Middle thread finished.
(priority-break-system) Medium thread should just have finished.
(priority-break-system) Low thread should have priority 31.  Actual priority: 31.
(priority-break-system) end
EOF
pass;
