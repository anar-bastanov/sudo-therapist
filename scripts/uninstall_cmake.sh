#!/usr/bin/env bash
set -euo pipefail

if grep -R "pam_sudo_therapist\.so" /etc/pam.d >/dev/null 2>&1; then
  echo "Disable sudo-therapist first."
  exit 1
fi

sudo sh -c 'while IFS= read -r f; do rm -f "$f"; done < build/install_manifest.txt'
sudo rm -f /etc/sudo-therapist.conf
