#!/bin/sh

extract() {
	tar zxf stcam_dd_archive.tar.gz
	echo "Files were extracted"
}

driver() {
	extract
	cd stcam_dd
	dmesg | grep 'Secure boot enabled' > temp
	if test -s temp
	then
		rm -f temp
		mokutil --test-key ~/.sentech/public_key.der | grep 'already enrolled' > temp
		if test -s temp
		then
			rm -f temp
			make
			/lib/modules/$(uname -r)/build/scripts/sign-file sha256 ~/.sentech/private_key.priv ~/.sentech/public_key.der ./stcam_dd.ko
			make copy
		else
			rm -f temp
			mkdir ~/.sentech 2>/dev/null
			openssl req -x509 -new -nodes -utf8 -sha256 -days 36500  -batch -config configuration.config -outform DER  -out ~/.sentech/public_key.der -keyout ~/.sentech/private_key.priv
			echo "The secure boot is enabled in this system. It is necesary to register the secure boot key.\nPlease enter the same password twice."
			mokutil --import ~/.sentech/public_key.der
			echo "The secure boot key registering window is appeared after reboot the system.\nPlease select [Enroll MOK] then enter the same password\nPlease install the driver again after the secure boot key is registerd"
			exit
		fi
	else
		rm -f temp
		make install
	fi
	cd ..
	echo "The driver was installed"
}

full() {
	driver
	cp lib/stcam_lib.h /usr/local/include/sentech/
	if test -e /usr/local/lib/sentech
	then
		echo
	else
		mkdir /usr/local/lib/sentech
	fi
	uname -a | grep aarch64 > temp
	if test -s temp
	then
		cp lib/libstcam-arm64.so /usr/local/lib/sentech/libstcam.so
	else
	uname -a | grep arm > temp
	if test -s temp
	then
		cp lib/libstcam-arm.so /usr/local/lib/sentech/libstcam.so
	else
	uname -a | grep x86_64 > temp
	if test -s temp
	then
		cp lib/libstcam-x86_64.so /usr/local/lib/sentech/libstcam.so
	else
		cp lib/libstcam-x86.so /usr/local/lib/sentech/libstcam.so
	fi
	fi
	fi
	rm -f temp
	cp lib/sentech.conf /etc/ld.so.conf.d/
	ldconfig
	echo "Library was installed"
	cd simple_capture
	rm -f simple_capture
	make
	cd ..
	cd viewer
	qmake 2>/dev/null || qmake-qt5
	make clean
	make
	cd ..
	echo "Installation procedure was completed"
}

uninstall() {
	rm -rf /usr/local/lib/sentech/
	rm -f /etc/ld.so.conf.d/sentech.conf
	cd stcam_dd
	make uninstall
	make clean
	cd ..
	echo "Do you want to delete all extracted files?"
	echo -n "Select\"y\"to delete all extracted files, select other character to keep the extracted files.: "
	read DEL

	case $DEL in
		"y")
			rm -rf stcam_dd/
			rm -rf lib/
			rm -rf simple_capture/
			rm -rf viewer/
			echo "All extracted files were deleted"
		;;
		*) echo "Extracted files do not delete"
		;;
	esac
	echo "Uninstall was completed"
}

echo "Install Sentech USB Driver. Please select installation mode from below four modes."
echo "1. Device driver and libray install and executable file for sample code generates"
echo "2. Device driver installs only. (Not necessary to use library for the image processing)"
echo "3. File extracts only. (For cross compiler of the embedded system)"
echo "4. Device driver uninstalls"
echo -n "Enter the installation mode: "
read MODE

case $MODE in
	1) full
	;;
	2) driver
	;;
	3) extract
	;;
	4) uninstall
	;;
	*) echo "Enter the number from 1 to 4"
	;;
esac

