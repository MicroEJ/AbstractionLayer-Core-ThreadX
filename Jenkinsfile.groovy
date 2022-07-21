// Copyright 2021-2022 MicroEJ Corp. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found with this software.

buildWithMMM {
  MODULE_FILENAME= 'module.ivy'
  DOCKER_IMAGE = 'artifactory.cross:18084/microej/sdk-cco:latest'
  ANT_PROPERTIES = '-b=${IVY_SETTINGS_MODULES_REPO_PATH}'
}
