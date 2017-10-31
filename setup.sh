. git-tools/funcs.sh

# uh...
local=/home/code/git-tools
remote=https://github.com/cyisfor/git-tools.git
dest=git-tools
clonepull

local=/home/code/note
remote=https://github.com/cyisfor/note.git
dest=note
clonepull

local=/home/code/libxmlfixes
remote=https://github.com/cyisfor/libxmlfixes.git
dest=libxmlfixes
clonepull

uplink libxmlfixes libxml2
uplink libxmlfixes coolmake


