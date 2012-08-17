#!/usr/bin/env sh

# !!! 此脚本仅在磁盘上线时执行一次
local single_disk="$1"

TMPFS_ROOT="/tmp/disk"

# 写缓存，默认打开
# 取值范围：0,关闭 1,打开
DFT_WCE="1"

# 待机模式默认时间30秒
# 可取值范围：disable, 30, 60, 300, 1800
DFT_STANDBY="disable"

# 默认预读模式：开启
# 取值：0,关闭 1,开启
DFT_RDAHEAD="1"

# 命令队列：默认值1024，SATA磁盘无法设置
DFT_QUEUE="1024"

actual_standby()
{
	local standby="$1"
	local actual_standby="0"

	case "$standby" in
		"30")
			actual_standby="6"
			;;
		"60")
			actual_standby="10"
			;;
		"300")
			actual_standby="60"
			;;
		"1800")
			actual_standby="241"
			;;
		*)
			actual_standby="0"
	esac

	echo "$actual_standby"
}

update_disk()
{
	local disk_dev="$1"

	[ -z "$disk_dev" ] && return

	local DISK_DIR="$TMPFS_ROOT/by-dev/$disk_dev"
	mkdir -p "$DISK_DIR"

	echo "------- $disk_dev -------"

	# 接口类型 SAS/SATA
	if `2>/dev/null smartctl -i "/dev/$disk_dev" | grep "Transport protocol: SAS" >/dev/null`; then
		value="SAS"
	else
		value="SATA"
	fi
	echo "$value" > "$DISK_DIR/interface_type"
	echo "INTERFACE: $value"

	# 写缓存(不区分SAS/SATA)

	# 设置默认值
	2>/dev/null sdparm --set WCE="$DFT_WCE" "/dev/$disk_dev" >/dev/null

	# 检查设置结果
	if `2>/dev/null sdparm --get WCE "/dev/$disk_dev" | grep "WCE         1" >/dev/null`; then
		value="enable"
	else
		value="disable"  # SATA盘加转接板无法设置，应该显示真实情况更合理
	fi
	echo "$value" > "$DISK_DIR/cache_switch"
	echo "WRITE CACHE: $value"

	## 待机模式

	# 每次设置默认待机时间，然后更新到tmpfs
	standby=$(actual_standby "$DFT_STANDBY")
	if `2>/dev/null hdparm -S "$standby" "/dev/$disk_dev" | grep "setting standby to $standby" >/dev/null`; then
		value="$DFT_STANDBY"
	else
		value="disable"  # SAS盘无法设置，应该显示真实情况更合理
	fi
	echo "$value" > "$DISK_DIR/standby"
	echo "STANDBY: $value"

	## 预读模式

	# 设置默认预读模式
	2>/dev/null hdparm -A "$DFT_RDAHEAD" "/dev/$disk_dev" >/dev/null

	# 检查设置结果
	if `2>/dev/null hdparm -A "/dev/$disk_dev" | grep "(on)" >/dev/null`; then
		value="enable"
	else
		value="disable"  # SAS盘无法设置，应该显示真实情况更合理
	fi
	echo "$value" > "$DISK_DIR/ahead_switch"
	echo "READ AHEAD: $value"

	## 命令队列

	# 设置默认值
	2>/dev/null hdparm -Q "$DFT_QUEUE" "/dev/$disk_dev" >/dev/null
	queue=$(2>/dev/null hdparm -Q "/dev/$disk_dev" | grep "queue_depth" | awk '{print $NF}')
	if [ "$queue" = "0" ] || [ -z "$queue" ]; then
		value="disable"
	else
		value="enable"
	fi
	echo "$value" > "$DISK_DIR/queue_switch"
	echo "COMMAND QUEUE: $value"

}

# 更新指定磁盘
[ ! -z "$single_disk" ] && update_disk "$single_disk" && exit

# 更新所有磁盘
while read disk_dev
do
	update_disk "$disk_dev"
done<<EOF
$(ls "$TMPFS_ROOT/by-dev")
EOF
