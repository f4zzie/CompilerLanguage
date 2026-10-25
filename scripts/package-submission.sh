#!/usr/bin/env bash
set -euo pipefail

archive="submission.zip"
rm -f "$archive"
zip -r "$archive" . -x './build/*' './.git/*' './submission.zip' './submission.zip.sha256' './*.log' './gmon.out' './*.prof' './.cache/*' '*__pycache__/*' '*.pyc' '*.DS_Store'
python3 scripts/check-package.py "$archive"
sha256sum "$archive" > "$archive.sha256"
echo "Created $archive"
echo "Recorded $archive.sha256"
