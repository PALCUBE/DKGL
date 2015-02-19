// ExtDeps.h
// DK
#pragma once

#ifdef DKLIB_EXTDEPS_ZLIB
#include "ExtDeps/zlib/zlib.h"
#include "ExtDeps/zlib/zip.h"
#include "ExtDeps/zlib/unzip.h"
	#ifdef _WIN32
	#include "ExtDeps/zlib/iowin32.h"
	#endif
#endif

#ifdef DKLIB_EXTDEPS_LIBXML
#define LIBXML_STATIC
#include "ExtDeps/libxml2/include/libxml/globals.h"
#include "ExtDeps/libxml2/include/libxml/xmlerror.h"
#include "ExtDeps/libxml2/include/libxml/parser.h"
#include "ExtDeps/libxml2/include/libxml/tree.h"
#include "ExtDeps/libxml2/include/libxml/debugXML.h"
#include "ExtDeps/libxml2/include/libxml/xmlmemory.h"
#include "ExtDeps/libxml2/include/libxml/HTMLparser.h"
#include "ExtDeps/libxml2/include/libxml/HTMLtree.h"
#include "ExtDeps/libxml2/include/libxml/nanohttp.h"
#include "ExtDeps/libxml2/include/libxml/nanoftp.h"
#endif

#ifdef DKLIB_EXTDEPS_LIBOGG
#include <ogg/ogg.h>
#endif

#ifdef DKLIB_EXTDEPS_LIBFLAC
#include "ExtDeps/libFLAC/include/FLAC/stream_decoder.h"
#endif

#ifdef DKLIB_EXTDEPS_LIBVORBIS
#include "ExtDeps/libvorbis/include/vorbis/codec.h"
#include "ExtDeps/libvorbis/include/vorbis/vorbisfile.h"
#endif

#ifdef DKLIB_EXTDEPS_CXIMAGE
#include "ExtDeps/CxImage/ximage.h"
#include "ExtDeps/CxImage/ximabmp.h"
#include "ExtDeps/CxImage/ximagif.h"
#include "ExtDeps/CxImage/ximaico.h"
#include "ExtDeps/CxImage/ximajas.h"
#include "ExtDeps/CxImage/ximajbg.h"
#include "ExtDeps/CxImage/ximajpg.h"
#include "ExtDeps/CxImage/ximamng.h"
#include "ExtDeps/CxImage/ximapcx.h"
#include "ExtDeps/CxImage/ximapng.h"
#include "ExtDeps/CxImage/ximapsd.h"
#include "ExtDeps/CxImage/ximaraw.h"
#include "ExtDeps/CxImage/ximaska.h"
#include "ExtDeps/CxImage/ximatga.h"
#include "ExtDeps/CxImage/ximatif.h"
#include "ExtDeps/CxImage/ximawbmp.h"
#include "ExtDeps/CxImage/ximawmf.h"
#endif

#ifdef DKLIB_EXTDEPS_FREETYPE
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_BITMAP_H
#include FT_OUTLINE_H
#include FT_STROKER_H
#include FT_BBOX_H
#endif

#ifdef DKLIB_EXTDEPS_SQLITE3
#include "ExtDeps/sqlite/sqlite3.h
#endif