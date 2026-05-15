#!/bin/bash

echo "Sky-OS Başlatılıyor..."

# Arka planda yapay zeka asistan motorunu uyandır
python3 /src/core/sky-assistant.py "Sistem başarıyla açıldı, kaptana selam çak!" &

# Masaüstü ortamını (XFCE) ekrana bas
xfce4-session
