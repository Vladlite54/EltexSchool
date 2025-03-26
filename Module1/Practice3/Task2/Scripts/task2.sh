#!/bin/bash

SCRIPT_NAME=$(basename "$0")
SCRIPT_NAME="${SCRIPT_NAME%.sh}"

REPORT_FILE="report_${SCRIPT_NAME}.log"

PID=$$

DATE_TIME=$(date "+%Y-%m-%d %H:%M:%S")

echo "[$PID] $DATE_TIME Скрипт запущен" >> "$REPORT_FILE"

SLEEP_TIME=$((RANDOM % 1771 + 30))

sleep "$SLEEP_TIME"

DATE_TIME=$(date "+%Y-%m-%d %H:%M:%S")

RUNTIME_MINUTES=$((SLEEP_TIME / 60))

echo "[$PID] $DATE_TIME Скрипт завершился, работал $RUNTIME_MINUTES минут" >> "$REPORT_FILE"
