#!/bin/sh
# Installs Anuga Viewer as a user application in Ubuntu

# Make sure only root can run our script
if [ "$(id -u)" != "0" ]; then
   echo "This script must be run as root. Please use sudo." 1>&2
   exit 1
fi

echo "Adding Anuga app files to the appropriate locations..."
cp distros/anuga_viewer.desktop /usr/share/applications/.
cp bin/anuga_viewer /usr/bin/anuga_viewer
mkdir -p /usr/local/share/anuga_viewer
cp distros/icon.png /usr/local/share/anuga_viewer/.
echo
echo "Complete! Anuga Viewer should now appear in your Applications list."
echo

