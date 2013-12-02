option1=$1
option2=$2
option3=$3

qemu-system-x86_64 $option1 $option2 $option3 -curses -cdrom $USER.iso -hda $USER.img -gdb tcp::3334

