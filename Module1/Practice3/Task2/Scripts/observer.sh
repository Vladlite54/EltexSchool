#!/bin/bash

CONFIG_FILE="scripts.conf"

LOG_FILE="observer.log"

log() {
  DATE_TIME=$(date "+%Y-%m-%d %H:%M:%S")
  echo "$DATE_TIME $1" >> "$LOG_FILE"
}

if [ ! -f "$CONFIG_FILE" ]; then
  log "Ошибка: Файл конфигурации '$CONFIG_FILE' не найден."
  exit 1
fi

while IFS= read -r script; do
  script=$(echo "$script" | tr -d '[:space:]')

  if [ -z "$script" ] || [ "${script:0:1}" == "#" ]; then
    continue
  fi

  if pgrep -x "$script" > /dev/null; then
    log "Скрипт '$script' уже запущен."
  else
    nohup "$script" &> /dev/null &
    log "Скрипт '$script' запущен (nohup)."
  fi
done < "$CONFIG_FILE"
