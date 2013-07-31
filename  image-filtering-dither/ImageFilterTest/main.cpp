
#include "ImageSaver.h"

#if defined(UNICODE) || defined(_UNICODE)
#	define _tofstream std::wofstream
#	define _tostream  std::wostream
#	define _tcout     std::wcout
#	define _tcin	  std::wcin
#	define _tstring   std::wstring
#else
#	define _tofstream std::ofstream
#	define _tostream  std::ostream
#	define _tcout     std::cout
#	define _tcin	  std::cin
#	define _tstring   std::string
#endif

#define _TSTR_EQ(_Str1, _Str2, _Strict) ( ((_Str1) == (_Str2)) || ( (_Str1) != NULL && (_Str2) != NULL && ((_Strict) == 0 ? _tcscmp(_Str1, _Str2) : _tcsncmp(_Str1, _Str2, (_Strict))) == 0 ) )
inline int argv_index_of(int argc, _TCHAR *argv[], const _TCHAR *param_tag, bool strict, int start_index)
{
	for (int i = max(0, start_index); i < argc; ++i)
	{
		if (_TSTR_EQ(argv[i], param_tag, (strict ? min(_tcslen(argv[i]), _tcslen(param_tag)) : 0))) return i;
	}
	return 0;	
}
inline int argv_index_of(int argc, _TCHAR *argv[], const _TCHAR *param_tag, bool strict)
{
	return argv_index_of(argc, argv, param_tag, strict, 1);
}

inline const _tstring TIL_GetErrorEx()
{
#if defined(UNICODE) || defined(_UNICODE)
	std::string mbtil_error = til::TIL_GetError();
	_tstring wctil_error(mbtil_error.length(), L'\0');
	MultiByteToWideChar(CP_ACP, MB_COMPOSITE, mbtil_error.data(), mbtil_error.length(), &wctil_error[0], mbtil_error.length());
	return wctil_error;
#else
	return til::TIL_GetError();
#endif
}

#define PARAM_MIN_COUNT 3
int _tmain(int argc, _TCHAR *argv[])
{
	int ret_result = 0;

	if (argc < PARAM_MIN_COUNT) return ret_result;
	std::vector<_tstring> program_log;

	int param_index = 1;

	_tstring wcfile_name(argv[param_index]);

	std::string mbfile_name(WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, argv[param_index], _tcslen(argv[param_index]), NULL, 0, NULL, NULL), '\0');
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, argv[param_index], mbfile_name.length(), &mbfile_name[0], mbfile_name.length(), NULL, NULL); 

	til::TIL_Init();

	til::Image *image[] = { NULL, NULL, NULL };
	try
	{
		image[0] = til::TIL_Load(mbfile_name.data(), TIL_FILE_ADDWORKINGDIR | TIL_DEPTH_R8G8B8);
		if (image[0] == NULL) throw TIL_GetErrorEx();

		if ((param_index = max(0, argv_index_of(argc, argv, _T("/oq"), true, 2))) != 0)
		{
			image[1] = til::TIL_Load(mbfile_name.data(), TIL_FILE_ADDWORKINGDIR | TIL_DEPTH_R8G8B8);
			if (image[1] == NULL) throw TIL_GetErrorEx();

			til::byte quant[3];
			for (int i = 0; i < 3; ++i)
				quant[i] = _tstoi(argv[param_index + i + 1]);
			ipo::RGBLinearQuantizationFilter *filter = new ipo::RGBLinearQuantizationFilter(RGB_QUANTS(quant[0], quant[1], quant[2]));
			filter->Apply(*image[1]);
			delete filter;

			TCHAR temp[25];
			til::uint64 tt = ipo::ImageTransformationAnalysis::FindStandardDeviation(*image[0], *image[1]);
			_i64tot_s(tt, temp, _countof(temp), 10);
			program_log.push_back(_tstring(_T("Linear quantization error: ")).append(temp));

			ImageSaver::SaveAsBmp(*image[1], _tstring(_T("oq_result_")).append(wcfile_name).data());
		}
		if ((param_index = max(0, argv_index_of(argc, argv, _T("/op"), true, 2))) != 0)
		{
			image[2] = til::TIL_Load(mbfile_name.data(), TIL_FILE_ADDWORKINGDIR | TIL_DEPTH_R8G8B8);
			if (image[2] == NULL) throw TIL_GetErrorEx();

			int color_number = _tstoi(argv[param_index + 1]);
			ipo::RGBPriorityQuantizationFilter *filter = new ipo::RGBPriorityQuantizationFilter(color_number);
			filter->Apply(*image[2]);
			delete filter;

			TCHAR temp[25];
			_i64tot_s(ipo::ImageTransformationAnalysis::FindStandardDeviation(*image[0], *image[2]), temp, _countof(temp), 10);
			program_log.push_back(_tstring(_T("Priority quantization error: ")).append(temp));

			ImageSaver::SaveAsBmp(*image[2], _tstring(_T("op_result_")).append(wcfile_name).data());
		}
		if ((param_index = argv_index_of(argc, argv, _T("-d:"), false, param_index)) != 0)
		{
			float mask[8];
			for (int i = 0; i < 8; i++)	mask[i] = static_cast<float>(_tstof(argv[param_index + i + 1]));
			til::byte movement = DITH_MOVE_HORIZONTAL | DITH_STARTPOINT_REMAIN;
			if (param_index + 9 < argc) {
				if (_TSTR_EQ(argv[param_index + 9], _T("vertical"), 10))	movement = DITH_MOVE_VERTICAL;
				if (_TSTR_EQ(argv[param_index + 9], _T("change"), 10))		movement = DITH_STARTPOINT_CHANGE;
			}
			if (param_index + 10 < argc) {
				if (_TSTR_EQ(argv[param_index + 10], _T("vertical"), 10))	movement |= DITH_MOVE_VERTICAL;
				if (_TSTR_EQ(argv[param_index + 10], _T("change"), 10))		movement |= DITH_STARTPOINT_CHANGE;
			}

			ipo::Dither *dither = new ipo::Dither(*image[0], mask, movement);
			if (image[1] != NULL)
			{
				dither->Apply(*image[1]);

				TCHAR temp[25];
				til::uint64 tt = ipo::ImageTransformationAnalysis::FindStandardDeviation(*image[0], *image[1]);
				_i64tot_s(tt, temp, _countof(temp), 10);
				program_log.push_back(_tstring(_T("Linear quantization error after dithering: ")).append(temp));

				ImageSaver::SaveAsBmp(*image[1], _tstring(_T("oq_dith_result_")).append(wcfile_name).data());
			}				
			if (image[2] != NULL)
			{
				dither->Apply(*image[2]);

				TCHAR temp[25];
				_i64tot_s(ipo::ImageTransformationAnalysis::FindStandardDeviation(*image[0], *image[2]), temp, _countof(temp), 10);
				program_log.push_back(_tstring(_T("Priority quantization error after dithering: ")).append(temp));

				ImageSaver::SaveAsBmp(*image[2], _tstring(_T("op_dith_result_")).append(wcfile_name).data());
			}
			delete dither;		
		}
	}
	catch (_tstring error_message)
	{
		program_log.push_back(error_message);
		ret_result = -1;
	}

	_tostream *output = &_tcout;

	_tofstream *out_file = NULL;
	if (param_index = argv_index_of(argc, argv, _T(">"), true, param_index) != 0)
	{
		out_file = new _tofstream(argv[param_index + 1]);
		output = out_file;
	}

	for (std::vector<_tstring>::size_type i = 0, max_i = program_log.size(); i < max_i; ++i) (*output) << program_log[i] << std::endl;

	if (out_file != NULL)
	{
		out_file->close();
		delete out_file;
	}

	for (std::size_t i = 0, max_i = _countof(image); i < max_i; ++i) til::TIL_Release(image[i]);

	til::TIL_ShutDown();

	return ret_result;
}
