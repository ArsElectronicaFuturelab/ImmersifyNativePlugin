#pragma once

enum TextureFormat
{
	PVRTC_2BPP_RGB = -127,
	PVRTC_2BPP_RGBA = -127,
	PVRTC_4BPP_RGB = -127,
	PVRTC_4BPP_RGBA = -127,

	Undefined = 0,				//     Alpha-only texture format.
	Alpha8 = 1,					//     A 16 bits/pixel texture format. Texture stores color with an alpha channel.
	ARGB4444 = 2,				//     Color texture format, 8-bits per channel.
	RGB24 = 3,					//     Color with alpha texture format, 8-bits per channel.
	RGBA32 = 4,					//     Color with alpha texture format, 8-bits per channel.
	ARGB32 = 5,					//     A 16 bit color texture format.
	RGB565 = 7,					//     A 16 bit color texture format that only has a red channel.
	R16 = 9,					//     Compressed color texture format.
	DXT1 = 10,					//     Compressed color with alpha channel texture format.
	DXT5 = 12,					//     Color and alpha texture format, 4 bit per channel.
	RGBA4444 = 13,				//     Color with alpha texture format, 8-bits per channel.
	BGRA32 = 14,				//     Scalar (R) texture format, 16 bit floating point.
	RHalf = 15,					//     Two color (RG) texture format, 16 bit floating point per channel.
	RGHalf = 16,				//     RGB color and alpha texture format, 16 bit floating point per channel.
	RGBAHalf = 17,				//     Scalar (R) texture format, 32 bit floating point.
	RFloat = 18,				//     Two color (RG) texture format, 32 bit floating point per channel.
	RGFloat = 19,				//     RGB color and alpha texture format, 32-bit floats per channel.
	RGBAFloat = 20,				//     A format that uses the YUV color space and is often used for video encoding or playback.
	YUY2 = 21,					//     RGB HDR format, with 9 bit mantissa per channel and a 5 bit shared exponent.
	RGB9e5Float = 22,			//     HDR compressed color texture format.
	BC6H = 24,					//     High quality compressed color texture format.
	BC7 = 25,					//     Compressed one channel (R) texture format.
	BC4 = 26,					//     Compressed two-channel (RG) texture format.
	BC5 = 27,					//     Compressed color texture format with Crunch compression for small storage sizes.
	DXT1Crunched = 28,			//     Compressed color with alpha channel texture format with Crunch compression for small storage sizes.
	DXT5Crunched = 29,			//     PowerVR (iOS) 2 bits/pixel compressed color texture format.
	PVRTC_RGB2 = 30,			//     PowerVR (iOS) 2 bits/pixel compressed with alpha channel texture format.
	PVRTC_RGBA2 = 31,			//     PowerVR (iOS) 4 bits/pixel compressed color texture format.
	PVRTC_RGB4 = 32,			//     PowerVR (iOS) 4 bits/pixel compressed with alpha channel texture format.
	PVRTC_RGBA4 = 33,			//     ETC (GLES2.0) 4 bits/pixel compressed RGB texture format.
	ETC_RGB4 = 34,				//     ATC (ATITC) 4 bits/pixel compressed RGB texture format.
	ATC_RGB4 = 35,				//     ATC (ATITC) 8 bits/pixel compressed RGB texture format.
	ATC_RGBA8 = 36,				//     ETC2 EAC (GL ES 3.0) 4 bitspixel compressed unsigned single-channel texture format.
	EAC_R = 41,					//     ETC2 EAC (GL ES 3.0) 4 bitspixel compressed signed single-channel texture format.
	EAC_R_SIGNED = 42,			//     ETC2 EAC (GL ES 3.0) 8 bitspixel compressed unsigned dual-channel (RG) texture format.
	EAC_RG = 43,				//     ETC2 EAC (GL ES 3.0) 8 bitspixel compressed signed dual-channel (RG) texture format.
	EAC_RG_SIGNED = 44,			//     ETC2 (GL ES 3.0) 4 bits/pixel compressed RGB texture format.
	ETC2_RGB = 45,				//     ETC2 (GL ES 3.0) 4 bits/pixel RGB+1-bit alpha texture format.
	ETC2_RGBA1 = 46,			//     ETC2 (GL ES 3.0) 8 bits/pixel compressed RGBA texture format.
	ETC2_RGBA8 = 47,			//     ASTC (4x4 pixel block in 128 bits) compressed RGB texture format.
	ASTC_RGB_4x4 = 48,			//     ASTC (5x5 pixel block in 128 bits) compressed RGB texture format.
	ASTC_RGB_5x5 = 49,			//     ASTC (6x6 pixel block in 128 bits) compressed RGB texture format.
	ASTC_RGB_6x6 = 50,			//     ASTC (8x8 pixel block in 128 bits) compressed RGB texture format.
	ASTC_RGB_8x8 = 51,			//     ASTC (10x10 pixel block in 128 bits) compressed RGB texture format.
	ASTC_RGB_10x10 = 52,		//     ASTC (12x12 pixel block in 128 bits) compressed RGB texture format.
	ASTC_RGB_12x12 = 53,		//     ASTC (4x4 pixel block in 128 bits) compressed RGBA texture format.
	ASTC_RGBA_4x4 = 54,			//     ASTC (5x5 pixel block in 128 bits) compressed RGBA texture format.
	ASTC_RGBA_5x5 = 55,			//     ASTC (6x6 pixel block in 128 bits) compressed RGBA texture format.
	ASTC_RGBA_6x6 = 56,			//     ASTC (8x8 pixel block in 128 bits) compressed RGBA texture format.
	ASTC_RGBA_8x8 = 57,			//     ASTC (10x10 pixel block in 128 bits) compressed RGBA texture format.
	ASTC_RGBA_10x10 = 58,		//     ASTC (12x12 pixel block in 128 bits) compressed RGBA texture format.
	ASTC_RGBA_12x12 = 59,		//     ETC 4 bits/pixel compressed RGB texture format.
	ETC_RGB4_3DS = 60,			//     ETC 4 bitspixel RGB + 4 bitspixel Alpha compressed texture format.
	ETC_RGBA8_3DS = 61,			//     Two color (RG) texture format, 8-bits per channel.
	RG16 = 62,					//     Scalar (R) render texture format, 8 bit fixed point.
	R8 = 63
};

const char *textureFormatToString(TextureFormat format)
{
	switch (format)
	{
	case -127: return "UNSUPPORTED PVRTC_2BPP_XXX(X) FORMAT";
	case Undefined: return "Undefined";
	case Alpha8: return "Alpha8";
	case ARGB4444: return "ARGB4444";
	case RGB24: return "RGB24";
	case RGBA32: return "RGBA32";
	case ARGB32: return "ARGB32";
	case RGB565: return "RGB565";
	case R16: return "R16";
	case DXT1: return "DXT1";
	case DXT5: return "DXT5";
	case RGBA4444: return "RGBA4444";
	case BGRA32: return "BGRA32";
	case RHalf: return "RHalf";
	case RGHalf: return "RGHalf";
	case RGBAHalf: return "RGBAHalf";
	case RFloat: return "RFloat";
	case RGFloat: return "RGFloat";
	case RGBAFloat: return "RGBAFloat";
	case YUY2: return "YUY2";
	case RGB9e5Float: return "RGB9e5Float";
	case BC6H: return "BC6H";
	case BC7: return "BC7";
	case BC4: return "BC4";
	case BC5: return "BC5";
	case DXT1Crunched: return "DXT1Crunched";
	case DXT5Crunched: return "DXT5Crunched";
	case PVRTC_RGB2: return "PVRTC_RGB2";
	case PVRTC_RGBA2: return "PVRTC_RGBA2";
	case PVRTC_RGB4: return "PVRTC_RGB4";
	case PVRTC_RGBA4: return "PVRTC_RGBA4";
	case ETC_RGB4: return "ETC_RGB4";
	case ATC_RGB4: return "ATC_RGB4";
	case ATC_RGBA8: return "ATC_RGBA8";
	case EAC_R: return "EAC_R";
	case EAC_R_SIGNED: return "EAC_R_SIGNED";
	case EAC_RG: return "EAC_RG";
	case EAC_RG_SIGNED: return "EAC_RG_SIGNED";
	case ETC2_RGB: return "ETC2_RGB";
	case ETC2_RGBA1: return "ETC2_RGBA1";
	case ETC2_RGBA8: return "ETC2_RGBA8";
	case ASTC_RGB_4x4: return "ASTC_RGB_4x4";
	case ASTC_RGB_5x5: return "ASTC_RGB_5x5";
	case ASTC_RGB_6x6: return "ASTC_RGB_6x6";
	case ASTC_RGB_8x8: return "ASTC_RGB_8x8";
	case ASTC_RGB_10x10: return "ASTC_RGB_10x10";
	case ASTC_RGB_12x12: return "ASTC_RGB_12x12";
	case ASTC_RGBA_4x4: return "ASTC_RGBA_4x4";
	case ASTC_RGBA_5x5: return "ASTC_RGBA_5x5";
	case ASTC_RGBA_6x6: return "ASTC_RGBA_6x6";
	case ASTC_RGBA_8x8: return "ASTC_RGBA_8x8";
	case ASTC_RGBA_10x10: return "ASTC_RGBA_10x10";
	case ASTC_RGBA_12x12: return "ASTC_RGBA_12x12";
	case ETC_RGB4_3DS: return "ETC_RGB4_3DS";
	case ETC_RGBA8_3DS: return "ETC_RGBA8_3DS";
	case RG16: return "RG16";
	case R8: return "R8";
	}
	return "UNKNOWN";
}