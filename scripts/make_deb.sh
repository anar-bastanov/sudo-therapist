#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT="$ROOT/dist"
WORK="$OUT/deb-src"

rm -rf "$WORK"
mkdir -p "$WORK"

rsync -a \
  --exclude '.git' \
  --exclude 'dump' \
  --exclude 'build' \
  --exclude 'dist' \
  "$ROOT/" "$WORK/"

cp -a "$WORK/packaging/debian" "$WORK/debian"

cd "$WORK"
dpkg-buildpackage -us -uc -b

echo
echo "Artifacts are in: $OUT"
