#ifndef __STRING_PIECE_H__
#define __STRING_PIECE_H__

#include <string>
#include <string.h>
#include <iosfwd> //for ostream forward-declaration
#include <type_traits>

class CStringArg
{
public:
    CStringArg(const char *str): mStr(str)
    {}
    CStringArg(const std::string &str): mStr(str.c_str())
    {}
    const char *c_str()const {return mStr;}
private:
    const char *mStr;
};

class CStringPiece
{
public:
    CStringPiece(): mPtr(NULL), mLength(0)
    {

    }

    CStringPiece(const char* str): mPtr(str), mLength(static_cast<int>(strlen(mPtr)))
    {

    }

    CStringPiece(const unsigned char* str): mPtr(reinterpret_cast<const char*>(str)), mLength(static_cast<int>(strlen(mPtr)))
    {

    }

    CStringPiece(const std::string &string): mPtr(string.data()), mLength(static_cast<int>(string.size()))
    {

    }

    CStringPiece(const char *offset, int len): mPtr(offset), mLength(len)
    {

    }

    const char *data()const{return mPtr;}
    int size()const{return mLength;}
    bool empty(){return mLength == 0;}
    const char *begin()const{return mPtr;}
    const char *end()const{return mPtr + mLength;}
    void clear(){mPtr = NULL; mLength = 0;}
    void set(const char* buffer, int len){mPtr = buffer; mLength = len;}
    void set(const char *str){mPtr = str; mLength = static_cast<int>(strlen(str));}
    void set(const void *str, int len){mPtr = reinterpret_cast<const char*>(str); mLength = len;}
    char operator[](int i)const {return mPtr[i];}
    void removePrefix(int n){mPtr += n; mLength -= n;}
    void removeSuffix(int n){mLength -= n;}
    bool operator==(const CStringPiece &str)const
    {
        return (mLength == str.mLength) && ( memcmp(mPtr, str.mPtr, mLength) == 0);
    }

    bool operator!=(const CStringPiece &str)
    {
        return !(*this == str);
    }

#define STRINGPIECE_BINARY_PREDICATE(cmp,auxcmp)    \
    bool operator cmp(const CStringPiece& x)const {     \
    int r = memcmp(mPtr, x.mPtr, mLength < x.mLength ? mLength : x.mLength);    \
    return ((r auxcmp 0) || ((r == 0) && (mLength cmp x.mLength)));  \
    }
STRINGPIECE_BINARY_PREDICATE(<, <);
STRINGPIECE_BINARY_PREDICATE(<=, <);
STRINGPIECE_BINARY_PREDICATE(>, >);
STRINGPIECE_BINARY_PREDICATE(>=, >);
#undef STRINGPIECE_BINARY_PREDICATE

    int compare(const CStringPiece &x) const
    {
        int r = memcmp(mPtr, x.mPtr, mLength < x.mLength ? mLength : x.mLength);
        if(r == 0)
        {
            if(mLength < x.mLength)
            {
                r = -1;
            }
            else if(mLength > x.mLength)
            {
                r = 1;
            }
        }
        return r;
    }

    std::string asString()const
    {
        return std::string(mPtr, mLength);
    }

    void copyToString(std::string &target)const
    {
        target.assign(mPtr, mLength);
    }

    bool startWith(const CStringPiece &x)const
    {
        return ((mLength >= x.mLength) && (memcmp(mPtr, x.mPtr, x.mLength) == 0));
    }
private:
    const char *mPtr;
    int         mLength;

};

#ifndef HAVE_TYPE_TRAITS
/*
* This make vector<CStringPiece> really fast for some STL implementation
* When a vector resizes, it need to copy old elements to new memory,If the STL knows
* that CStringPiece has a trivial copy constructor, it will directly use memcpy for bulk 
* copying(which is efficient) instead of calling the constructor for each element
* individually(which is inefficient)
*/

template<> 
struct std::is_trivially_default_constructible<CStringPiece>: public std::true_type{};//has trivial default constructor

template<> 
struct std::is_trivially_copy_constructible<CStringPiece>: public std::true_type{};//has trivial default copy constructor

template<> 
struct std::is_trivially_copy_assignable<CStringPiece>: public std::true_type{};//has trivial operator =

template<> 
struct std::is_trivially_destructible<CStringPiece>: public std::true_type{};//has trivial destructor

template<> 
struct std::is_pod<CStringPiece>: public std::true_type{};//plain old data

#endif

std::ostream& operator<<(std::ostream& o, const CStringPiece& piece);
#endif