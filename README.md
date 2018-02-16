# pintos-anon

Latest version from Stanford repo. used in CMSC 326 at Bard College.

# To get started

- These directions assume that you have all the necessary other
  packages (e.g., gcc, gdb, qemu, etc) installed properly.  If not,
  read the "Installing Pintos" section of the Pinto Manual.

- move the main pintos directory to a reasonable location (e.g., mkdir
  ~/cmsc326; mv pintos ~/cmsc326)

- edit your .bashrc file to add the pintos/src/utils directory to your path

export PATH="$PATH:/home/YOURID/cmsc326/pintos/src/utils"

- edit your .bashrc file, setting the GDBMACROS environment variable to "pintos/src/misc/gdb-macros"

GDBMACROS="/home/YOURID/cmsc326/pintos/src/misc/gdb-macros"

- Execute your changes: "source ~/.bashrc"

- Change directory to pintos/src/utils and type "make"

- Change directory to pintos/src/threads and type "make"

- Change directory to pintos/src/threads.  Now run a test program
  "pintos run alarm-multiple"

Pintos should run and load several threads in a QEMU terminal.