// Original C++ code by https://github.com/init-01
// Visual Studio, DLL\LIB adaptation and refactoring by Nick "Flashback" Skolozdra https://github.com/Nick-IronTower

#include <cstdint>
#include <cstring>
#include <iostream>
#include <fstream>
#include <cassert>

// libwebp from https://developers.google.com/speed/webp/download
#include "../libwebp/include/webp/types.h"
#include "../libwebp/include/webp/demux.h"

// Gif.h from https://github.com/charlietangora/gif-h
#include "gif.h"

#include "FbWebp2Gif.h"

using namespace std;

int main(int argc, char **argv)
{
	if (argc > 1)
		ConvertWebp2Gif(argv[1]);
	else
		cout << "Please provide file path as an argument.";
	
	return 0;
}

extern "C" {

	bool ConvertWebp2Gif(char* Filename, bool EnableLogging)
	{
		ofstream logstream;
		if (EnableLogging)
		{
			logstream.open("webp2gif.log", ios::ate | ios::app);
			logstream << "Start." << endl;
			logstream << "Webp path: " << Filename << endl;
		}

		//Make Gif filename
		string webp_name = Filename;
		string gif_name = webp_name;

		const string webp_ext = ".webp";
		const string gif_ext = ".gif";

		string::size_type ext_pos = webp_name.rfind(webp_ext);
		if (ext_pos == string::npos)
			gif_name.append(".gif");
		else
			gif_name.replace(ext_pos, webp_ext.length(), gif_ext);

		//Open WebP File
		ifstream webpfile;
		ios_base::iostate exceptionMask = webpfile.exceptions() | std::ios::failbit;
		webpfile.exceptions(exceptionMask);
		try
		{
			webpfile.open(webp_name.c_str(), ios::binary | ios::in | ios::ate);
		}
		catch (const std::exception& e)
		{
			cerr << e.what() << endl;
			char error_str[256];
			strerror_s(error_str, 256, errno);
			cerr << error_str << endl;
			
			if (EnableLogging)
				logstream << "Exception during Webp opening: " << e.what() << " ; " << error_str << endl;
			return false;
		}

		if (EnableLogging)
			logstream << "Webp opened successfully." << endl;

		//Initialize Anim WebP Decoder Options
		WebPAnimDecoderOptions webp_dec_options;
		WebPAnimDecoderOptionsInit(&webp_dec_options);
		webp_dec_options.color_mode = MODE_RGBA;
		webp_dec_options.use_threads = true;

		//Read WebP File to webp_data's buffer
		WebPData webp_data;

		webp_data.size = webpfile.tellg();
		webp_data.bytes = new uint8_t[webp_data.size];
		webpfile.seekg(0, ios::beg);
		webpfile.read((char*)webp_data.bytes, webp_data.size);
		webpfile.close();

		if (EnableLogging)
			logstream << "Webp read successfully." << endl;

		//Initialize Anim WebP Decoder
		WebPAnimDecoder* webp_dec = WebPAnimDecoderNew(&webp_data, &webp_dec_options);

		//Get Anim WebP Info
		WebPAnimInfo webp_anim_info;
		WebPAnimDecoderGetInfo(webp_dec, &webp_anim_info);

		int curr_timestamp = 0;
		int prev_timestamp = 0;

		GifWriter gif_file;
		bool gif_writer_initialized = false;

		//Cycle through WebP and Write Gif
		while (WebPAnimDecoderHasMoreFrames(webp_dec))
		{
			uint8_t* rgba_buffer;
			WebPAnimDecoderGetNext(webp_dec, &rgba_buffer, &curr_timestamp);

			if (!gif_writer_initialized)
			{
				//Initialize Gif Writer
				//FB: We're doing it here to capture proper timing between frames and resolve "1 loop" bug
				GifBegin(&gif_file, gif_name.c_str(), webp_anim_info.canvas_width, webp_anim_info.canvas_height, curr_timestamp / 10);
				gif_writer_initialized = true;
			}

			GifWriteFrame(&gif_file, rgba_buffer, webp_anim_info.canvas_width, webp_anim_info.canvas_height, curr_timestamp / 10 - prev_timestamp / 10);	//WebPAnimDecoder uses 1/1000 sec to delay but GifWriteFrame uses 1/100 sec to delay so divide by 10
			prev_timestamp = curr_timestamp;
		}
		
		GifEnd(&gif_file);

		if (EnableLogging)
			logstream << "Gif written successfully." << endl;

		//Cleanup
		WebPAnimDecoderReset(webp_dec);
		WebPAnimDecoderDelete(webp_dec);
		WebPDataClear(&webp_data);

		if (EnableLogging)
			logstream << "Cleared, exiting." << endl;
		return true;
	}
}