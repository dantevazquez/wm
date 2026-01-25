#!/bin/bash
sxhkd &
# Start window manager piped to lemonbar
exec ~/wm/wm | lemonbar -p -f "JetBrainsMono Nerd Font:size=12"
