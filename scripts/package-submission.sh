#!/usr/bin/env bash
set -euo pipefail

archive="submission.zip"
rm -f "$archive"
zip -r "$archive" . -x './build/*' './.git/*' './submission.zip' './*.log'
python3 scripts/check-package.py "$archive"
echo "Created $archive"
