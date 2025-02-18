#!/bin/bash

# version variable
Version="1.0.0"

# This shell script will fetch the commits between the current head and Main branch and list them with --no-merge and --oneline
git log --no-merges --oneline Main..HEAD > Dev_Log_$Version.txt

echo "Commits between Main and HEAD have been logged in Dev_Log_$Version.txt"