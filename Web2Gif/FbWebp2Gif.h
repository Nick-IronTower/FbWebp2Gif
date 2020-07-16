#pragma once

extern "C" {
	__declspec(dllexport) bool ConvertWebp2Gif(char* Filename, bool EnableLogging=false);
}
