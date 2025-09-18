/*
 * C
 *
 *	Copyright 2023-2025 MicroEJ Corp. All rights reserved.
 *	Use of this source code is governed by a BSD-style license that can be found with this software.
 */
#include <embUnit/embUnit.h>
#include "Outputter.h"
#include "TextUIRunner.h"
#include "XMLOutputter.h"

int main(int argc, const char *argv[]) {
	TextUIRunner_setOutputter(XMLOutputter_outputter());
	TextUIRunner_start();
	// Please add your tests here
	TextUIRunner_end();
	return 0;
}
