/* Copyright (C) 2010 MoSync AB

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <string>
#include <cctype>
#include "package.h"
#include "util.h"

// Maximum line length: 80 characters.
static const char* sUsage =
"This program turns compiled MoSync programs into packages that can be\n"
"installed on various portable devices.\n"
"\n"
"Usage: package <OPTIONS>\n"
"\n"
"Options:\n"
" -h, --help                   Print this information.\n"
" -p, --program <file>         Input: compiled program file.\n"
" -r, --resource <file>        Input: compiled resource file.\n"
" -i, --icon <file>            Input: MoSync icon definition file (XML).\n"
"     --cpp-output <path>      Input: Absolute path to a directory containing\n"
"                              'rebuild.build.cpp' and 'data_section.bin',\n"
"                              generated by pipe-tool -cpp.\n"
"                              Used by the iOS packager.\n"
" -t, --type <profiletype>     Input: Type of profile database,\n"
"                              either \"device\" and \"platform\" based.\n"
"                              Default: device.\n"
" -m, --model <vendor>/<model> Output: target model.\n"
"             <family>/<variant>\n"
" -d, --dst <path>             Output: target directory.\n"
" -n, --name <name>            Output: application name.\n"
"     --vendor <name>          Output: application vendor's name.\n"
"     --version <version>      Output: application version.\n"
"                              Format: major[.minor][.micro][.qualifier]\n"
"                              Some parts may be ignored by some platforms.\n"
"     --debug                  Output: use debug runtime.\n"
"     --s60v3uid <8-digit hex> Output: Symbian UID, 3rd edition.\n"
"     --s60v2uid <8-digit hex> Output: Symbian UID, 2nd edition.\n"
"     --s60cert <file>         PEM X.509 certificate for Symbian signing.\n"
"     --s60key <file>          PEM X.509 private key for Symbian signing.\n"
"     --s60pass <password>     Password to the Symbian private key.\n"
"     --ios-cert <name>        Output: name of an iOS developer certificate.\n"
"     --ios-sdk <path>         Output: path to an iOS SDK. Only useful on OSX.\n"
"     --ios-project-only       Output: don't compile the generated iOS project.\n"
"                              Required on non-OSX hosts.\n"
"     --android-package <pkg>  Android package name\n"
"                              (need to be unique for Android Market).\n"
"     --android-version-code <version>\n"
"                              Android version code\n"
"                              (used in androidmanifest.xml).\n"
"     --android-keystore <file>\n"
"                              Keystore for signing (Android only).\n"
"     --android-storepass <password>\n"
"                              Keystore password (Android only).\n"
"     --android-alias <alias>  Key alias (Android only).\n"
"     --android-keypass <password>\n"
"                              Key password (Android only).\n"
"     --javame-keystore <file> Keystore for signing (JavaME only).\n"
"     --javame-storepass <password>"
"                              Keystore password (JavaME only).\n"
"     --javame-alias <alias>   Key alias (JavaME only).\n"
"     --javame-keypass <password>"
"                              Key password (JavaME only).\n"
" -s, --silent                 Output: Supress additional output, e.g. warnings.\n"
"\n"
"Environment variables used:\n"
" MOSYNCDIR                    Path to the MoSync installation directory.\n"
;

static bool streq(const char* a, const char* b) {
	return strcmp(a, b) == 0;
}

static void setString(int& i, int argc, const char** argv, const char*& dst) {
	i++;
	if(i == argc) {
		printf("Too few arguments!\n");
		exit(1);
	}
	dst = argv[i];
}

static void onExit() {
	printf("onExit\n");
}

int main(int argc, const char** argv) {
	SETTINGS s;
	memset(&s, 0, sizeof(s));

	atexit(&onExit);

	if(argc == 1) {
		printf("%s", sUsage);
		return 1;
	}

	for(int i=1; i<argc; i++) {
		if(streq(argv[i], "-h") || streq(argv[i], "--help")) {
			printf("%s", sUsage);
			return 1;
		} else if(streq(argv[i], "-p") || streq(argv[i], "--program")) {
			setString(i, argc, argv, s.program);
		} else if(streq(argv[i], "-r") || streq(argv[i], "--resource")) {
			setString(i, argc, argv, s.resource);
		} else if(streq(argv[i], "-i") || streq(argv[i], "--icon")) {
			setString(i, argc, argv, s.icon);
		} else if(streq(argv[i], "-m") || streq(argv[i], "--model")) {
			setString(i, argc, argv, s.model);
		} else if(streq(argv[i], "-d") || streq(argv[i], "--dst")) {
			setString(i, argc, argv, s.dst);
		} else if(streq(argv[i], "-n") || streq(argv[i], "--name")) {
			setString(i, argc, argv, s.name);
		} else if(streq(argv[i], "-t") || streq(argv[i], "--type")) {
			setString(i, argc, argv, s.profileType);
		} else if(streq(argv[i], "--vendor")) {
			setString(i, argc, argv, s.vendor);
		} else if(streq(argv[i], "--version")) {
			setString(i, argc, argv, s.version);
		} else if(streq(argv[i], "--permissions")) {
			setString(i, argc, argv, s.permissions);
		} else if(streq(argv[i], "--s60v3uid")) {
			setString(i, argc, argv, s.s60v3uid);
		} else if(streq(argv[i], "--s60v2uid")) {
			setString(i, argc, argv, s.s60v2uid);
		} else if(streq(argv[i], "--s60cert")) {
			setString(i, argc, argv, s.s60cert);
		} else if(streq(argv[i], "--s60key")) {
			setString(i, argc, argv, s.s60key);
		} else if(streq(argv[i], "--s60pass")) {
			setString(i, argc, argv, s.s60pass);
		} else if(streq(argv[i], "--debug")) {
			s.debug = true;
		} else if(streq(argv[i], "--nfc")) { // NFC specific
			setString(i, argc, argv, s.nfc);
		} else if(streq(argv[i], "--ios-cert")) { // iOS specific
			setString(i, argc, argv, s.iOSCert);
		} else if(streq(argv[i], "--ios-sdk")) { // iOS specific
			setString(i, argc, argv, s.iOSSdk);
		} else if(streq(argv[i], "--ios-xcode-target")) { // iOS specific
			setString(i, argc, argv, s.iOSXcodeTarget);
		} else if(streq(argv[i], "--ios-project-only")) { // iOS specific
			s.iOSgenerateOnly = true;
		} else if(streq(argv[i], "--cpp-output")) {
			setString(i, argc, argv, s.cppOutputDir);
		} else if(streq(argv[i], "--android-package")) {
			setString(i, argc, argv, s.androidPackage);
		} else if(streq(argv[i], "--android-version-code")) {
			setString(i, argc, argv, s.androidVersionCode);
		} else if(streq(argv[i], "--android-keystore")) {
			setString(i, argc, argv, s.androidKeystore);
		} else if(streq(argv[i], "--android-storepass")) {
			setString(i, argc, argv, s.androidStorePass);
		} else if(streq(argv[i], "--android-alias")) {
			setString(i, argc, argv, s.androidAlias);
		} else if(streq(argv[i], "--android-keypass")) {
			setString(i, argc, argv, s.androidKeyPass);
		} else if(streq(argv[i], "--javame-keystore")) {
			setString(i, argc, argv, s.javameKeystore);
		} else if(streq(argv[i], "--javame-storepass")) {
			setString(i, argc, argv, s.javameStorePass);
		} else if(streq(argv[i], "--javame-alias")) {
			setString(i, argc, argv, s.javameAlias);
		} else if(streq(argv[i], "--javame-keypass")) {
			setString(i, argc, argv, s.javameKeyPass);
		} else if(streq(argv[i], "-s") || streq(argv[i], "--silent")) {
			s.silent = true;
		} else if(streq(argv[i], "--show-passwords")) {
			s.showPasswords = true;
		} else {
			printf("Unknown argument: '%s'\n", argv[i]);
			return 1;
		}
	}

	package(s);
	return 0;
}

void testProgram(const SETTINGS& s) {
	if(!s.program) {
		printf("Must specify program!\n");
		exit(1);
	}
}
void testModel(const SETTINGS& s) {
	if(!s.model) {
		printf("Must specify model!\n");
		exit(1);
	}
}
void testDst(const SETTINGS& s) {
	if(!s.dst) {
		printf("Must specify target directory!\n");
		exit(1);
	}
	if (!existsDir(s.dst)) {
		printf("Destination directory must exist! (Try 'mkdir %s')\n", s.dst);
		exit(1);
	}
}
void testName(const SETTINGS& s) {
	if(!s.name) {
		printf("Must specify name!\n");
		exit(1);
	}
}
void testProfileType(const SETTINGS& s) {
	if (s.profileType) {
		if (strcmp(s.profileType, "device") &&
			strcmp(s.profileType, "platform")) {
			printf("Profile type must be \"device\" or \"platform\"\n");
			exit(1);
		}
	}
}
void testVendor(const SETTINGS& s) {
	if(!s.vendor) {
		printf("Must specify vendor!\n");
		exit(1);
	}
}
void testVersion(const SETTINGS& s) {
	if (!s.version) {
		printf("Must specify version!\n");
		exit(1);
	}
}
static void testUid(const char* uid) {
	if(strlen(uid) == 8) {
		bool good = true;
		for(int i=0; i<8; i++) {
			if(!isxdigit(uid[i]))
				good = false;
		}
		if(good)
			return;
	}
	printf("Invalid UID format!\n");
	exit(1);
}
void testS60v3Uid(const SETTINGS& s) {
	if(!s.s60v3uid) {
		printf("Must specify s60v3uid!\n");
		exit(1);
	}
	testUid(s.s60v3uid);
}
void testS60v2Uid(const SETTINGS& s) {
	if(!s.s60v2uid) {
		printf("Must specify s60v2uid!\n");
		exit(1);
	}
	testUid(s.s60v2uid);
}
void testIOSCert(const SETTINGS& s) {
	if(!s.iOSCert) {
		printf("Must specify iOS certificate!\n");
		exit(1);
	}
}
void testIOSSdk(const SETTINGS& s) {
	if(!s.iOSSdk) {
		printf("Must specify iOS SDK!\n");
		exit(1);
	}
}
void testCppOutputDir(const SETTINGS& s) {
	if(!s.cppOutputDir) {
		printf("Must specify C++ output directory!\n");
		exit(1);
	}
}

void testAndroidPackage(const SETTINGS& s) {
	if (!s.androidPackage) {
		printf("Must specify Android package!\n");
		exit(1);
	}

	// NOTE: We support only ASCII package names!
	bool valid = true;
	bool isStart = true;
	bool isSeparator = false;
	int packageCount = 1;
	int len = strlen(s.androidPackage);
	for (int i = 0; i < len; i++) {
		char ch = s.androidPackage[i];

		bool wasSeparator = isSeparator;
		isSeparator = ch == '.';

		// Not two . in a row
		valid &= !isSeparator || !wasSeparator;

		valid &= isStart ?
				isJavaIdentifierStart(ch) :
				isJavaIdentifierPart(ch) || isSeparator;

		// After every ., new package part
		isStart = isSeparator;
		if (isSeparator) {
			packageCount++;
		}
	}

	// Must not end with .
	valid &= !isSeparator;

	// At least 2 package parts.
	valid &= packageCount > 1;

	if (!valid) {
		printf("Invalid Android package name (%s). Android package names:\n", s.androidPackage);
		printf("* Have at least two package parts\n");
		printf("* Package parts are separated by the '.' character\n");
		printf("* Package parts cannot be empty (ie not two consecutive '.')\n");
		printf("* The first character of a package part is a letter, '$', or '_'\n");
		printf("* Each of the rest of the package part's characters may also be a digit\n");
		printf("* Only ASCII package names are supported\n");
		exit(1);
	}
}

bool isJavaIdentifierStart(char ch) {
	return isalpha(ch) || ch == '_' || ch == '$';
}

bool isJavaIdentifierPart(char ch) {
	return (isJavaIdentifierStart(ch)) || isdigit(ch);
}

void testAndroidVersionCode(const SETTINGS& s) {
	if (!s.androidVersionCode) {
		printf("Must specify Android version code!\n");
		exit(1);
	}
	if (atoi(s.androidVersionCode) < 1) {
		printf("Android version code must be a number > 0!\n");
		exit(1);
	}
}

void testJavaMESigning(const SETTINGS& s) {
	if (!(s.javameAlias && s.javameKeyPass && s.javameKeystore && s.javameStorePass) &&
		(s.javameAlias || s.javameKeyPass || s.javameKeystore || s.javameStorePass)) {
		printf("JavaME signing options must either all be specified or none!\n");
		exit(1);
	}
}

const char* mosyncdir() {
	static const char* md = NULL;
	if(!md) {
		md	= getenv("MOSYNCDIR");
		if(!md) {
			printf("MOSYNCDIR is not set!\n");
			exit(1);
		}
	}
	return md;
}

void sh(const char* cmd, bool hideOutput, const char* shownCmdLine) {
	if (!shownCmdLine) {
		printf("%s\n", cmd);
	} else {
		printf("%s\n", shownCmdLine);
	}

	fflush(stdout);
	fflush(stderr);

	std::string finalCmd(cmd);
	if(hideOutput) {
		// Supress output from stdout and stderr
#ifdef WIN32
		finalCmd += "> nul 2>&1";
#else
		finalCmd += "> /dev/null 2>&1";
#endif
	}

	int res = system(finalCmd.c_str());

	fflush(stdout);
	fflush(stderr);
	if(res != 0) {
		printf("System error %i\n", res);
		exit(res);
	}
}
