#!/bin/zsh
PATH=$(find /sys/class/hwmon/hwmon*/name -exec grep -E '^radeon$' {} + | grep -o '.*/')

if [ $# -eq 0 ] || (( $1 < 35 )); then
	SPEED=50; # ~20%
elif (( $1 < 100 )); then
	SPEED=$(echo "255*$1/100" | /usr/bin/bc);
else
	SPEED=255;
fi

echo 1 > $PATH"pwm1_enable"
echo $SPEED > $PATH"pwm1"
