#pragma once

#ifndef _MR_TEXT_H_
#define _MR_TEXT_H_

//#define MR_USE_RAW_TEXT
//#define MR_USE_UTF8_TEXT
//#define MR_USE_UTF16_TEXT
//#define MR_USE_UTF32_TEXT

#ifdef MR_USE_RAW_TEXT
#define MR_TEXT(text) R#text
#elifdef MR_USE_UTF8_TEXT
#define MR_TEXT(text) u8#text
#elifdef MR_USE_UTF16_TEXT
#define MR_TEXT(text) u#text
#elifdef MR_USE_UTF32_TEXT
#define MR_TEXT(text) U#text
#else
#define MR_TEXT(text) text
#endif

#endif // _MR_TEXT_H_
