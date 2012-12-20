
#include "ImageSaver.h"

#if defined(UNICODE) || defined(_UNICODE)
#define _tofstream std::wofstream
#define _tostream  std::wostream
#define _tcout     std::wcout
#define _tcin	   std::wcin
#define _tstring   std::wstring
#else
#define _tofstream std::ofstream
#define _tostream  std::ostream
#define _tcout     std::cout
#define _tcin	   std::cin
#define _tstring   std::string
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
	if (argc < PARAM_MIN_COUNT) return 0;
	std::vector<_tstring> program_log;

	std::string mbfile_name(_tcslen(argv[1]), '\0');
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, argv[1], mbfile_name.length(),  &mbfile_name[0], mbfile_name.length(), NULL, NULL); 

	til::TIL_Init();
	til::Image *ref_image = til::TIL_Load(mbfile_name.data(), TIL_FILE_ADDWORKINGDIR | TIL_DEPTH_R8G8B8);

	int param_index = 0;
	if (ref_image != NULL)
	{
		if ((param_index = max(param_index, argv_index_of(argc, argv, _T("/oq"), true, 2))) != 0)
		{
			// Parse here number of sRGB dimensions subdivisions
		}
		if ((param_index = max(param_index, argv_index_of(argc, argv, _T("/op"), true, 2))) != 0)
		{
			// Parse here number of sRGB dimensions subdivisions
		}
		if ((param_index = argv_index_of(argc, argv, _T("-d"), false, param_index)) != 0)
		{
			// Copying an image instead of opening a new one, might be a great desicion
		}
	}
	else
	{
		program_log.push_back(TIL_GetErrorEx());
		return -1;
	}

	_tostream output = _tcout;

	_tofstream *out_file = NULL;
	if (param_index = argv_index_of(argc, argv, _T(">"), true, param_index) != 0)
	{
		out_file = new _tofstream(argv[param_index + 1]);
		output = *out_file;
	}

	for (std::vector<_tstring>::size_type i = 0, max_i = program_log.size(); i < max_i; ++i) output << program_log[i];

	if (out_file != NULL)
	{
		out_file->close();
		delete out_file;
	}

	til::TIL_Release(ref_image);
	til::TIL_ShutDown();

	return 0;
}
