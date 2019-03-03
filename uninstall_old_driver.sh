rmmod Sentech -s 2>/dev/null || echo
rm -rf /usr/local/include/Sentech/
rm -rf /lib/modules/$(uname -r)/extra/Sentech/
rm -f /usr/local/bin/usbsentech*
rm -f /lib/udev/rules.d/50-udev-my.rules 
rm -f /lib/udev/rules.d/50-udev-sentech-script.rules 


