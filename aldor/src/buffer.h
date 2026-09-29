#ifndef _BUFFER_H_
#define _BUFFER_H_
# include "axlport.h"
#define BUF_INIT_SIZE 64
#ifdef __cplusplus
# include <cstddef>
# include <type_traits>
class Buffer {
public:
 Buffer() = default; constexpr Buffer(std::nullptr_t) noexcept:rep_(nullptr){}
 static Buffer create(); static Buffer capture(MutString,Length); static Buffer fromPointer(void * p) noexcept { return Buffer(reinterpret_cast<Rep *>(p)); } static const Buffer nil;
 bool isNull()const noexcept{return rep_==nullptr;} explicit operator bool()const noexcept{return !isNull();}
 void free(); MutString liberate(); MutString chars()const noexcept; UByte *bytes() const noexcept; Length size()const noexcept; Length position()const noexcept;
 void start()noexcept; void skip(Length)noexcept; void back()noexcept; UByte next()const noexcept; UByte get1()noexcept;
 Buffer need(Length); Buffer grow(Length); Buffer add1(int); int add1Char(int); Buffer addn(const char*,Length); MutString getn(Length); MutString gets();
 int putc(int); int putcTimes(int,int); int puts(String); int puti(int); int printf(const char*,...); int vprintf(const char*,va_list); int print(FILE*)const;
 UByte readUByte(); UShort readUShort(); ULong readULong(); int writeUByte(UByte); int writeUShort(UShort); int writeULong(ULong);
 SFloat readSFloat(); DFloat readDFloat(); int writeSFloat(SFloat); int writeDFloat(DFloat);
 MutString readChars(int); int writeChars(int,String); MutString readString(); int writeString(String); MutString getString(); Buffer readBuffer(); int writeBuffer(Buffer);
 friend bool operator==(Buffer a,Buffer b)noexcept{return a.rep_==b.rep_;} friend bool operator!=(Buffer a,Buffer b)noexcept{return !(a==b);}
 friend bool operator==(Buffer a,std::nullptr_t)noexcept{return a.isNull();} friend bool operator!=(Buffer a,std::nullptr_t)noexcept{return !a.isNull();}
 void setPosition(Length n) noexcept;
private: struct Rep; explicit constexpr Buffer(Rep*r)noexcept:rep_(r){} Rep*rep_;
};
static_assert(sizeof(Buffer)==sizeof(void*)); static_assert(std::is_trivially_copyable_v<Buffer>);
#define BUF_START(b) ((b).start())
#define BUF_ADD1(b,c) ((b).add1Char(c))
#define BUF_GET1(b) ((b).get1())
#define BUF_BACK1(b) ((b).back())
#define BUF_NEXT1(b) ((b).next())
#define BUF_PUT_CHARS(b,s,n) ((b).addn((s),(n)))
#define BUF_GET_CHARS(b,s,n) strncpy((s),(b).getn(n),(n))
#define BUF_PUT_BYTE(b,c) ((b).add1Char(UNBYTE1(c)))
#define BUF_PUT_HINT(b,h) ((b).add1Char(HBYTE0(h)),(b).add1Char(HBYTE1(h)))
#define BUF_PUT_SINT(b,i) ((b).add1Char(BYTE0(i)),(b).add1Char(BYTE1(i)),(b).add1Char(BYTE2(i)),(b).add1Char(BYTE3(i)))
#define BUF_GET_BYTE(b,i) ((i)=static_cast<decltype(i)>((b).get1()))
#define BUF_GET_HINT(b,i) { MutString _s=(b).getn(HINT_BYTES); (i)=UNBYTE2(_s[0],_s[1]); }
#define BUF_GET_SINT(b,i) { MutString _s=(b).getn(SINT_BYTES); (i)=UNBYTE4(_s[0],_s[1],_s[2],_s[3]); }
#else
typedef struct BufferStruct {
        Length          pos;            /* amount used == next position */
        Length          argc;           /* amount available */
        UByte           *argv;
} *Buffer;

extern Buffer   bufNew          (void);
extern void     bufFree         (Buffer);

extern Buffer   bufCapture      (MutString, Length);
        /* Form buffer from given string. */
extern MutString   bufLiberate     (Buffer);
        /* Free buffer but keep chars. */

#define         bufChars(s)     ((MutString) ((s)->argv))
#define         bufSize(s)      ((s)->argc)
#define         bufPosition(s)  ((s)->pos)
#define         bufSkip(s,n)    (bufPosition(s) += (n))

extern Buffer   bufNeed         (Buffer, Length n);
        /* Indicate the available size must be at least n. */

extern Buffer   bufGrow         (Buffer, Length inc);
        /* Change the available size by inc. */

extern Buffer   bufAdd1         (Buffer, int c);
extern int      bufAdd1Char     (Buffer, int c);
        /* Add the character 'c' at the end of the buffer. */

extern Buffer   bufAddn         (Buffer, const char *s, Length n);
        /* Add the first n characters of s to the end of the buffer. */

extern MutString   bufGetn         (Buffer, Length n);
        /* Advance past n characters and return pointer to them. */

extern MutString   bufGets         (Buffer);
        /* Advance past a null-terminated string and return it. */

extern int      bufPutc         (Buffer, int c);
        /* Add the character to the buffer, pushing ahead a NULL terminator. */

extern int      bufPutcTimes(Buffer b, int c, int n);
        /* Add n character to the buffer, pushing ahead a NULL terminator. */

extern int      bufPuts         (Buffer, String s);
        /* Add the characters of s to the buffer, pushing ahead a NULL terminator. */

extern int      bufPuti         (Buffer, int i);
        /* Add a text representation of i to the buffer. */

extern int      bufPrintf       (Buffer, const char *fmt, ...);
extern int      bufVPrintf      (Buffer, const char *fmt, va_list);
        /* Use printf-style formatting onto the end of a buffer.  */

extern int      bufPrint        (FILE *, Buffer);
        /* Print buffer in readable form. */

/*
 * Macros for growing a buffer one character at a time.
 *
 * The names are capitalized since some of these evaluate args more than once.
 */
#define BUF_START(b)  ((b)->pos = 0)
#define BUF_ADD1(b,c) ((b)->pos < (b)->argc \
                        ? (b)->argv[(b)->pos++] = (c) : bufAdd1Char(b,c))
#define BUF_GET1(b)   ((b)->argv[(b)->pos++])
#define BUF_BACK1(b)  ((b)->pos--)
#define BUF_NEXT1(b)  ((b)->argv[(b)->pos])

        /*
         * To put the null-terminated string "hi" into a buffer do:
         *   BUF_START(b);
         *   BUF_ADD1(b,'h'); BUF_ADD1(b,'i'); BUF_ADD1(0);
         * To concatenate "lo":
         *   BUF_BACK1(b);
         *   BUF_ADD1(b,'l'); BUF_ADD1(b,'o'); BUF_ADD1(b,0);
         */

/*
 * Macros for putting and getting characters to a buffer.
 */

#define BUF_PUT_CHARS(buf, s, cc)                       \
        bufAddn(buf, s, cc)

#define BUF_GET_CHARS(buf, s, cc)                       \
        strncpy(s, bufGetn(buf, cc), cc)

/*
 * Macros for putting and getting integers as byte sequences.
 */

#define BUF_PUT_BYTE(b,c)                               \
        (BUF_ADD1(b,UNBYTE1(c)))

#define BUF_PUT_HINT(b,h)                               \
        (BUF_ADD1(b,HBYTE0(h)),BUF_ADD1(b,HBYTE1(h)))

#define BUF_PUT_SINT(b,i)                               \
        (BUF_ADD1(b,BYTE0(i)), BUF_ADD1(b,BYTE1(i)),    \
         BUF_ADD1(b,BYTE2(i)), BUF_ADD1(b,BYTE3(i)))

#ifdef __cplusplus
#define BUF_GET_BYTE(b, i)                              \
        ((i) = static_cast<decltype(i)>(BUF_GET1(b)))
#else
#define BUF_GET_BYTE(b, i)                              \
        ((i) = BUF_GET1(b))
#endif

#define BUF_GET_HINT(b, i) {                            \
        MutString  _s = bufGetn(b, HINT_BYTES);            \
        (i) = UNBYTE2(_s[0],_s[1]);                     \
}

#define BUF_GET_SINT(b, i) {                            \
        MutString  _s = bufGetn(b, SINT_BYTES);            \
        (i) = UNBYTE4(_s[0],_s[1],_s[2],_s[3]); \
}

/* Save integers in standard byte order. */
extern UByte    bufRdUByte      (Buffer buf);
extern UShort   bufRdUShort     (Buffer buf);
extern ULong    bufRdULong      (Buffer buf);

extern int      bufWrUByte      (Buffer buf, UByte b);
extern int      bufWrUShort     (Buffer buf, UShort s);
extern int      bufWrULong      (Buffer buf, ULong l);

/* Save floating-point numbers in IEEE format. */
extern SFloat   bufRdSFloat     (Buffer buf);
extern DFloat   bufRdDFloat     (Buffer buf);

extern int      bufWrSFloat     (Buffer buf, SFloat s);
extern int      bufWrDFloat     (Buffer buf, DFloat d);

/* Save a given number of characters in ASCII format. */
extern MutString   bufRdChars      (Buffer buf, int cc);
extern int      bufWrChars      (Buffer buf, int cc, String s);

/* Save strings in ASCII format, length included. */
extern MutString   bufRdString     (Buffer buf);
extern int      bufWrString     (Buffer buf, String s);

/* Read an unknown number of ASCII characters. */
extern MutString   bufGetString    (Buffer);

/* Save data in binary format. */
extern Buffer   bufRdBuffer     (Buffer buf);
extern int      bufWrBuffer     (Buffer buf, Buffer b);

#endif
#endif
