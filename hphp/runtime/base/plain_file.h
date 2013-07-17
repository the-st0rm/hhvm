/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010-2013 Facebook, Inc. (http://www.facebook.com)     |
   | Copyright (c) 1998-2010 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
*/

#ifndef incl_HPHP_PLAIN_FILE_H_
#define incl_HPHP_PLAIN_FILE_H_

#include "hphp/runtime/base/file.h"
#include "hphp/runtime/base/execution_context.h"

namespace HPHP {
///////////////////////////////////////////////////////////////////////////////

/**
 * A file system file that's nothing but ordinary. A simple FILE* wrapper.
 */
class PlainFile : public File {
public:
  DECLARE_OBJECT_ALLOCATION(PlainFile);

  explicit PlainFile(FILE *stream = nullptr, bool nonblocking = false);
  explicit PlainFile(int fd, bool nonblocking = false);
  virtual ~PlainFile();

  static StaticString s_class_name;
  // overriding ResourceData
  CStrRef o_getClassNameHook() const { return s_class_name; }

  // implementing File
  virtual bool open(CStrRef filename, CStrRef mode);
  virtual bool close();
  virtual int64_t readImpl(char *buffer, int64_t length);
  virtual int getc();
  virtual String read(int64_t length = 0);
  virtual int64_t writeImpl(const char *buffer, int64_t length);
  virtual bool seekable() { return true;}
  virtual bool seek(int64_t offset, int whence = SEEK_SET);
  virtual int64_t tell();
  virtual bool eof();
  virtual bool rewind();
  virtual bool flush();
  virtual bool truncate(int64_t size);

  FILE *getStream() { return m_stream;}
  virtual const char *getStreamType() const { return "STDIO";}

protected:
  FILE *m_stream;
  bool m_eof;
  char *m_buffer;       // For setbuffer.  Needed to reduce mmap
                        // contention due to how glibc allocates memory
                        // for buffered io.

  bool closeImpl();
};

/**
 * This is wrapper for fds that cannot be closed.
 */
class BuiltinFile : public PlainFile {
public:
  explicit BuiltinFile(FILE *stream) : PlainFile(stream, true) {}
  explicit BuiltinFile(int fd) : PlainFile(fd, true) {}
  virtual ~BuiltinFile();
  virtual bool close();
};

/**
 * A request-local wrapper for the three standard files:
 * STDIN, STDOUT, and STDERR.
 */
class BuiltinFiles : public RequestEventHandler {
public:
  virtual void requestInit();
  virtual void requestShutdown();

  static CVarRef GetSTDIN();
  static CVarRef GetSTDOUT();
  static CVarRef GetSTDERR();

private:
  Variant m_stdin;
  Variant m_stdout;
  Variant m_stderr;
};

///////////////////////////////////////////////////////////////////////////////
}

#endif // incl_HPHP_PLAIN_FILE_H_
