#!/bin/bash

## Download smb2man
REPO_URL="https://github.com/rickgaiser/libsmb2.git"
REPO_FOLDER="modules/network/libsmb2"
COMMIT="8a5aa0839427f79868bd5d63e1cee14c96731640"
if test ! -d "$REPO_FOLDER"; then
  git clone $REPO_URL "$REPO_FOLDER" || { exit 1; }
  (cd $REPO_FOLDER && git checkout "$COMMIT" && cd -) || { exit 1; }
else
  (cd "$REPO_FOLDER" && git fetch origin && git checkout "$COMMIT" && cd - )|| exit 1
fi
