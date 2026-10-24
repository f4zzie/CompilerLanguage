#!/usr/bin/env bash
set -euo pipefail

archive="submission.zip"
rm -f "$archive"
zip -r "$archive" . -x './build/*' './.git/*' './submission.zip' './*.log'
echo "Created $archive"
