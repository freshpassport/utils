#!/bin/sh

. lib_disk

TMPFS_ROOT="/tmp/disk"
SAS_HEALTH_STR="SMART Health Status: OK"
SATA_HEALTH_STR="SMART overall-health self-assessment test result: PASSED"

update_health()
{
	local disk_dev="$1"

	[ -z "$disk_dev" ] && return

	local disk_type=$(disk_type "$disk_dev")
	local health_result="GOOD"

	if [ "$disk_type" = "SAS" ]; then
		# 更新SAS磁盘健康状态
		if `2>/dev/null smartctl -H "/dev/$disk_dev" | grep "$SAS_HEALTH_STR" >/dev/null`; then
			health_result="GOOD"
		else
			health_result="ALARM"
		fi
	else
		# 更新SATA磁盘健康状态
		if `2>/dev/null smartctl -H "/dev/$disk_dev" | grep "$SATA_HEALTH_STR" >/dev/null`; then
			health_result="GOOD"
		else
			health_result="ALARM"
		fi
	fi

	local DISK_DIR="$TMPFS_ROOT/by-dev/$disk_dev/smart"

	mkdir -p "$DISK_DIR"
	echo "$health_result" > "$DISK_DIR/health"
}

update_sata_smartinfo()
{
	local disk_dev="$1"
	local tmp_file="/tmp/.upd_sata_smartinfo"

	2>/dev/null smartctl -A "/dev/$disk_dev" > "$tmp_file"

	local DISK_DIR="$TMPFS_ROOT/by-dev/$disk_dev/smart"
	mkdir -p "$DISK_DIR"

	echo "---------- (SATA)$disk_dev ---------"

	# smart/read_error
	value=$(2>/dev/null grep "1 Raw_Read_Error_Rate" "$tmp_file" | awk '{printf("%d(%d)", $4, $6)}')
	echo "$value" > "$DISK_DIR/read_err"
	echo "read_error: $value"

	# smart/realct_sect_cnt
	value=$(2>/dev/null grep "5 Reallocated_Sector_Ct" "$tmp_file" | awk '{printf("%d", $NF)}')
	echo "$value" > "$DISK_DIR/realct_sect_cnt"
	echo "realct_sect_cnt: $value"

	# smart/seek_err
	value=$(2>/dev/null grep "7 Seek_Error_Rate" "$tmp_file" | awk '{printf("%d(%d)", $4, $6)}')
	echo "$value" > "$DISK_DIR/seek_err"
	echo "seek_err: $value"

	# smart/spin_retry_cnt
	value=$(2>/dev/null grep "10 Spin_Retry_Count" "$tmp_file" | awk '{printf("%d(%d)", $4, $6)}')
	echo "$value" > "$DISK_DIR/spin_retry_cnt"
	echo "spin_retry_cnt: $value"

	# smart/power_on_hours
	value=$(2>/dev/null grep "9 Power_On_Hours" "$tmp_file" | awk '{printf("%d", $NF)}')
	echo "$value" > "$DISK_DIR/power_on_hours"
	echo "power_on_hours: $value"

	# smart/degrees_celsius
	value=$(2>/dev/null grep "194 Temperature_Celsius" "$tmp_file" | awk '{printf("%d", $10)}')
	echo "$value" > "$DISK_DIR/degrees_celsius"
	echo "degrees_celsius: $value"

	# smart/curr_pending_sect
	value=$(2>/dev/null grep "197 Current_Pending_Sector" "$tmp_file" | awk '{printf("%d", $NF)}')
	echo "$value" > "$DISK_DIR/curr_pending_sect"
	echo "curr_pending_sect: $value"

	# smart/offline_uncorrect
	value=$(2>/dev/null grep "198 Offline_Uncorrectable" "$tmp_file" | awk '{printf("%d", $NF)}')
	echo "$value" > "$DISK_DIR/offline_uncorrect"
	echo "offline_uncorrect: $value"

	# smart/spin_up_time
	value=$(2>/dev/null grep "3 Spin_Up_Time" "$tmp_file" | awk '{printf("%d(%d)", $4, $6)}')
	echo "$value" > "$DISK_DIR/spin_up_time"
	echo "spin_up_time: $value"

	rm -f "$tmp_file"
}

update_sas_smartinfo()
{
	local disk_dev="$1"
	local tmp_file="/tmp/.upd_sas_smartinfo"

	2>/dev/null smartctl -A "/dev/$disk_dev" > "$tmp_file"

	local DISK_DIR="$TMPFS_ROOT/by-dev/$disk_dev/smart"
	mkdir -p "$DISK_DIR"

	echo "---------- (SAS) $disk_dev ---------"

	# smart/read_error
	echo "N/A" > "$DISK_DIR/read_error"

	# smart/realct_sect_cnt
	echo "N/A" > "$DISK_DIR/realct_sect_cnt"

	# smart/seek_err
	echo "N/A" > "$DISK_DIR/seek_err"

	# smart/spin_retry_cnt
	echo "N/A" > "$DISK_DIR/spin_retry_cnt"

	# smart/power_on_hours
	value=$(2>/dev/null grep "number of hours powered up" "$tmp_file" | awk '{printf("%.2f", $NF)}')
	echo "$value" > "$DISK_DIR/power_on_hours"
	echo "power_on_hours: $value"

	# smart/degrees_celsius
	value=$(2>/dev/null grep "Current Drive Temperature:" "$tmp_file" | awk '{printf("%d", $(NF-1))}')
	echo "$value" > "$DISK_DIR/degrees_celsius"
	echo "degrees_celsius: $value"

	# smart/curr_pending_sect
	echo "N/A" > "$DISK_DIR/curr_pending_sect"

	# smart/offline_uncorrect
	echo "N/A" > "$DISK_DIR/offline_uncorrect"

	# smart/spin_up_time
	echo "N/A" > "$DISK_DIR/spin_up_time"

	rm -f "$tmp_file"
}

update_smartinfo()
{
	local disk_dev="$1"

	[ -z "$disk_dev" ] && return

	local disk_type=$(disk_type "$disk_dev")
	if [ "$disk_type" = "SAS" ]; then
		update_sas_smartinfo "$disk_dev"
	else
		update_sata_smartinfo "$disk_dev"
	fi
}


# main
while read disk_dev
do
	update_health "$disk_dev"
	update_smartinfo "$disk_dev"
done<<EOF
$(ls "$TMPFS_ROOT/by-dev")
EOF

