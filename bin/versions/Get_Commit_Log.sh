#!/usr/bin/env bash

# version variable
Version="0.1.8"

# This shell script will fetch the commits between the current head and Main branch and list them with --no-merge and --oneline
git log --no-merges --oneline main..HEAD > Dev_Log_$Version.txt

# This will create an overview summary of file changes:
git merge --no-commit --no-ff main > Dev_Log_Overview_$Version.txt
#remove the virtual merge
git merge --abort

echo "Commits between main and HEAD have been logged in Dev_Log_$Version.txt"