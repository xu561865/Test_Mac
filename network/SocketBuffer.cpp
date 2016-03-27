#include "SocketConst.h"
#include "SocketBuffer.h"
#include "SocketClient.h"


SocketBuffer::SocketBuffer(int capacity)
{
    SK_ASSERT(capacity > 0);
    
	buffer = new char[capacity];
    
	position = 0;
	this->capacity = capacity;
	this->limit = capacity;
}

SocketBuffer::SocketBuffer(char* data, int offset, int size)
{
    SK_ASSERT(data && (size > 0));
    
	buffer = new char[size];
    memcpy(buffer, data + offset, size);
    
	position = 0;
	this->capacity = size;
	this->limit = this->capacity;
}

SocketBuffer::~SocketBuffer()
{
	SK_SAFE_DELETE_ARRAY( buffer);
}

int SocketBuffer::remaining()
{
	return limit - position;
}

void SocketBuffer::put(const char* data,int offset,int len)
{
    SK_ASSERT(data && len > 0);
    
	if(position + len > capacity)
    {
		printf("error -SocketBuffer::put(const char* bytes,int offset,int len)---position=%d,len=%d,capacity=%d\n",position,len,capacity);
		return;
	}
    
	memcpy(buffer + position, data + offset, len);
	position += len;
}

void SocketBuffer::putBoolean(bool b)
{
    if( position + 1> capacity )
    {
        printf("error putBoolean position+len> limit------------position=%d,len=%d,capacity=%d\n",position,1,capacity);
        
        return;
    }
    
    if(b)
        buffer[position] =(1 & 0xff);
    else
        buffer[position]=(0 & 0xff);
    position+=1;
}

void SocketBuffer::putByte(signed char n)
{
	if(position + 1 > capacity)
    {
		printf("error SocketBuffer::putByte----position=%d,len=%d,capacity=%d\n",position,1,capacity);
		return;
	}
    
	buffer[position] = n;
	position++;
}

void SocketBuffer::put(int n)
{
	if(position + 1 > capacity)
    {
		printf("error SocketBuffer::put---position=%d,len=%d,capacity=%d\n",position,1,capacity);
		return;
	}
    
	buffer[position] = (char)n;
	position++;
}

void SocketBuffer::putInt(int n)
{
	if(position + 4 > capacity)
    {
		printf("error SocketBuffer::putInt--position=%d,len=%d,capacity=%d\n",position,4,capacity);
		return;
	}
    
	for(int i = 0 ; i < 4 ; i++)
    {
		buffer[position] = (char)((n >> (8 *(3 - i))) & 0xFF);
		position++;
	}
}

void SocketBuffer::setIntAt(int n,int index)
{
	if(index + 4 > capacity)
    {
		printf("error SocketBuffer::setIntAt--index=%d,len=%d,capacity=%d\n",index,4,capacity);
		return;
	}
    
	for(int i = 0 ; i < 4 ; i++)
    {
		buffer[index] = (char)((n >> (8 *(3 - i))) & 0xFF);
		index++;
	}
}

void SocketBuffer::putFloat(float n)
{
	if(position + 4 > capacity)
    {
		printf("error -SocketBuffer::putFloat---position=%d,len=%d,capacity=%d\n",position,4,capacity);
		return;
	}
    
	unsigned char* pBytes;
	pBytes = (unsigned char*)&n;

	buffer[position++] = pBytes[3];
	buffer[position++] = pBytes[2];
	buffer[position++] = pBytes[1];
	buffer[position++] = pBytes[0];
}

void SocketBuffer::putShort(short n)
{
	if(position + 2 > capacity)
    {
		printf("error -SocketBuffer::putShort---position=%d,len=%d,capacity=%d\n",position,2,capacity);
		return;
	}
    
	for(int i = 0 ; i < 2 ; i++)
    {
		buffer[position] = (char)((n >> (8 *(1 - i))) & 0xFF);
		position++;
	}
}

void SocketBuffer::putLong(long long n)
{
	if(position + 8 > capacity)
    {
		printf("error SocketBuffer::putLong-position=%d,len=%d,capacity=%d\n",position,8,capacity);
		return;
	}
	
	for(int i = 0 ; i < 8 ; i++)
    {
		buffer[position] = (char)((n >> (8 *(7 - i))) & 0xFF);
		position++;
	}
}

void SocketBuffer::putUTF(const char* str)
{
	short len = strlen(str);
	if(position + 2 + len > capacity)
    {
		printf("error SocketBuffer::putUTF----position=%d,len=%d,capacity=%d\n",position,2+len,capacity);
		return;
	}
	putShort(len);
	if( len>0)
    {
		put(str,0,len);
	}
}

void SocketBuffer::putUTF(const std::string& str)
{
	putUTF(str.c_str());
}

void SocketBuffer::putArray(const std::vector<signed char>& a)
{
	putInt(a.size());
	for(int i = 0;i < a.size(); ++i)
    {
		put(a[i]);
	}	
}

void SocketBuffer::putArray(const std::vector<bool>& a)
{
	putInt(a.size());
	for(int i=0; i< a.size(); ++i)
    {
		put(a[i]);
	}	
}

void SocketBuffer::putArray(const std::vector<short>& a)
{
	putInt(a.size());
	for(int i=0; i< a.size(); ++i)
    {
		putShort(a[i]);
	}
}

void SocketBuffer::putArray(const std::vector<int>& a)
{
	putInt(a.size());
	for(int i=0; i< a.size(); ++i)
    {
		putInt(a[i]);
	}
}

void SocketBuffer::putArray(const std::vector<long long>& a)
{
	putInt(a.size());

	for(int i=0; i< a.size(); ++i)
    {
		putLong(a[i]);
	}
}

void SocketBuffer::putArray(const std::vector<std::string>& a)
{
	putInt(a.size());
	
	for(int i=0; i< a.size(); ++i)
    {
		putUTF(a[i]);
	}
}

void SocketBuffer::getUTF(std::string& str)
{
	short len = getShort();		
	if( len>0)
    {
		str.append(buffer+position,len);
		position+=len;
	}
    else
    {
		str="";
	}
}

std::string SocketBuffer::getUTF()
{
    std::string str;
	getUTF(str);		
	return str;
}

int SocketBuffer::getPosition()
{
	return position;
}

void SocketBuffer::setPosition(int p)
{
	if(p > limit)
    {
		printf("error SocketBuffer::setPosition p> limit------------p=%d,limit=%d\n",p,limit);
	}
	position = p;
}

int SocketBuffer::getLimit()
{
	return limit;
}

int SocketBuffer::getCapacity()
{
	return this->capacity;
}

char* SocketBuffer::getBuffer()
{
	return buffer;
}

char* SocketBuffer::toByteArray()
{
	char* tmp = new char[position];
	memcpy(tmp,buffer,position);
	return tmp;
}

signed char SocketBuffer::getByte()
{
	if(position + 1 > limit)
    {
		printf("error SocketBuffer::getByte() position+1> limit------------position=%d,limit=%d\n",position,limit);
		return 0;
	}
	return buffer[position++];
}

bool SocketBuffer::getBoolean()
{
	if(position + 1 > limit)
    {
		printf("error SocketBuffer::getBoolean() position+1> limit------------position=%d,limit=%d\n",position,limit);
		return false;
	}
	return buffer[position++]!=0;
}

void SocketBuffer::getAsBytes(signed char* bytes)
{
    for(int i = 0 ; i < 4 ; i++)
    {
        bytes[i]=  buffer[position];
        position++;
    }
}

int SocketBuffer::getInt()
{
    if(position + 4 > limit)
    {
        printf("error SocketBuffer::getInt() position+4> limit------------position=%d,limit=%d\n",position,limit);
        return 0;
    }
    int rt = 0;
    for(int i = 0 ; i < 4 ; i++)
    {
        rt |=  ((buffer[position] & 0xFF) << (8 *(3 - i)));
        position++;
    }
    return rt;
}

float SocketBuffer::getFloat()
{
    if(position + 4 > limit)
    {
        printf("error SocketBuffer::getFloat() position+4> limit------------position=%d,limit=%d\n",position,limit);
        return 0;
    }
    float floatValue;
    unsigned char* pBytes;
    pBytes = (unsigned char*)&floatValue;
    pBytes[3] = buffer[position++];
    pBytes[2] = buffer[position++];
    pBytes[1] = buffer[position++];
    pBytes[0] = buffer[position++];
    return floatValue;
}

short SocketBuffer::getShort()
{
    if(position + 2 > limit)
    {
        printf("error SocketBuffer::getShort() position+2> limit------------position=%d,limit=%d\n",position,limit);
        return 0;
    }
    short n = 0;
    for(int i = 0 ; i < 2 ; i++)
    {
        n |= ((buffer[position] & 0xFF) << (8 *(1 - i)));
        position++;
    }
    return n;
}

long long SocketBuffer::getLong()
{
    if( position + 8 > limit )
    {
        printf("error SocketBuffer::getLong() position+8> limit------------position=%d,limit=%d\n",position,limit);
        return 0;
    }
    long long n = 0;
    for(int i = 0 ; i < 8 ; i++)
    {
        long long tmp = (buffer[position] & 0xFF);
        n |= (tmp << (8 *(7 - i)));
        
        position++;
    }
    return n;
}

void SocketBuffer::getArray(std::vector<signed char>& a)
{
    int size = getInt();
    a.resize(size);
    for(int i = 0; i < size; ++i)
    {
        a[i] = getByte();
    }
}

void SocketBuffer::getArray(std::vector<bool>& a)
{
    int size = getInt();
    a.resize(size);
    for(int i = 0; i < size; ++i)
    {
        a[i] = getBoolean();
    }
}

void SocketBuffer::getArray(std::vector<short>& a)
{
    int size = getInt();
    a.resize(size);
    for(int i = 0; i < size; ++i)
    {
        a[i] = getShort();
    }
}

void SocketBuffer::getArray(std::vector<int>& a)
{
    int size = getInt();
    a.resize(size);
    for(int i = 0; i < size; ++i)
    {
        a[i] = getInt();
    }
}

void SocketBuffer::getArray(std::vector<long long>& a)
{
    int size = getInt();
    a.resize(size);
    for(int i = 0; i < size; ++i)
    {
        a[i] = getLong();
    }
}

void SocketBuffer::getArray(std::vector<std::string>& a)
{
    int size = getInt();
    getArray(a, size);
}

void SocketBuffer::getArray(std::vector<signed char>& a, int size)
{
    a.resize(size);
    for(int i = 0; i < size; ++i)
    {
        a[i] = getByte();
    }
}

void SocketBuffer::getArray(std::vector<bool>& a, int size)
{
    a.resize(size);
    for(int i = 0; i < size; ++i)
    {
        a[i] = getBoolean();
    }
}

void SocketBuffer::getArray(std::vector<short>& a, int size)
{
    a.resize(size);
    for(int i = 0; i < size; ++i)
    {
        a[i] = getShort();
    }
}

void SocketBuffer::getArray(std::vector<int>& a, int size)
{
    a.resize(size);
    for(int i = 0; i < size; ++i)
    {
        a[i] = getInt();
    }
}

void SocketBuffer::getArray(std::vector<long long>& a, int size)
{
    a.resize(size);
    for(int i = 0; i < size;++i)
    {
        a[i] = getLong();
    }
}

void SocketBuffer::getArray(std::vector<std::string>& a, int size)
{
    a.resize(size);
    for(int i = 0; i < size; ++i)
    {
        getUTF(a[i]);
    }
}

void SocketBuffer::get(char* bytes, int size)
{
	get(bytes, 0, size);
}

void SocketBuffer::get(char* bytes, int offset, int len)
{
	if(position + len > limit)
    {
		memset(bytes+offset, 0, len );
		printf("error SocketBuffer::get(char* bytes,int offset,int len) position+len> limit------------position=%d,len=%d,limit=%d\n",position,len,limit);
		return;
	}
	memcpy(bytes+offset,buffer+position,len);
	position += len;
}

int readIntFromBuffer(signed char* buffer, int position)
{
    int rt = 0;
    for(int i = 0; i < 4; i++)
    {
        rt |=  ((buffer[position] & 0xFF) << (8 *(3 - i)));
        position++;
    }
    return rt; 
}

int SocketBuffer::getLength(int offset)
{
    int lengthPos = position + offset;
    signed char* pos = new signed char[4];
    for(int i = 0; i < 4 ; i++)
    {
        pos[i] = buffer[lengthPos+i];
    }
    
    for(int i = 0; i < 73; i ++)
    {
        printf("%d," , buffer[i]);
    }
    
    printf("\n %s \n" , buffer);
    return SocketClient::bytesToInt(pos);
}


/**
 * makes a buffer ready for a new sequence of channel-read or relative put operations: It sets the limit to the capacity and the position to zero.
 */
void SocketBuffer::clear()
{
	position = 0;
	this->limit = capacity;
}

/**
 * flip() makes a buffer ready for a new sequence of channel-write or relative get operations: It sets the limit to the current position and then sets the position to zero.
 */
void SocketBuffer::flip()
{
	this->limit = position;
	position = 0;
}

void SocketBuffer::compact()
{
	if(position > 0)
    {
		for(int i = position; i < limit ; i++)
        {
			buffer[i-position] = buffer[i];
		}
	}
	position = limit - position;
	limit = this->capacity;
}

void SocketBuffer::rewind()
{
	position = 0;
}

//这个方法只能用于打开的是文本文件的时候
std::string SocketBuffer::getLine()
{
    if( position >= capacity)
    {
        return "";
    }
    
	std::string line;
	for (int i = position; i<capacity; i++)
    {
		if (buffer[i] == '\n')
        {
			line.append(buffer + position, i - position);
			position = i + 1;
			return line;
		}
	}
	line.append(buffer + position, capacity - position);
	position = capacity + 1;
	return line;
}


