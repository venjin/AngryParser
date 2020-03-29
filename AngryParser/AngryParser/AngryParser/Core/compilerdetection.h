#ifndef AE_CompilerDetection_H_
#define AE_CompilerDetection_H_

//#define ANGRYPARSER_EXPORTS

#if defined(_WIN32)
#if defined(ANGRYPARSER_EXPORTS)
#define AngryParser_API __declspec(dllexport)
#else
#define AngryParser_API __declspec(dllimport)
#endif
#endif


#if !defined(AngryParser_API)
#if defined (__GNUC__) && (__GNUC__ >= 4)
#define AngryParser_API __attribute__ ((visibility ("default")))
#else
#define AngryParser_API
#endif
#endif

#define NS_BEGINE namespace AngryParser {

#define NS_END }


#endif
