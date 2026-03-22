#!/bin/bash

## Download usmb2
REPO_URL="https://github.com/Wolf3s/usmb2.git" # For now clone my reposistory, because of EE changes
REPO_FOLDER="modules/network/usmb2"
COMMIT="9ae09c2583dcb79edf6cf929b2d6d33102090ed3"
if test ! -d "$REPO_FOLDER"; then
  git clone $REPO_URL "$REPO_FOLDER" || { exit 1; }
  (cd $REPO_FOLDER && git checkout "$COMMIT" && cd -) || { exit 1; }
else
  (cd "$REPO_FOLDER" && git fetch origin && git checkout "$COMMIT" && cd - )|| exit 1
fi
