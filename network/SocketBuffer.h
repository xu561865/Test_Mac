#ifndef BYTE_BUFFER_H
#define BYTE_BUFFER_H
#include <string>
#include <vector>

int readIntFromBuffer(char* buffer,int position);

class SocketBuffer
{
	char* buffer;
	
	int position;
	int capacity;
	int limit;
	int mark;
public:
	
	SocketBuffer(int capacity);
	SocketBuffer(char* data,int offset,int length);
	~SocketBuffer();
	
	int remaining();
	void setIntAt(int n,int index);
    
    //put
	void put(const char* bytes,int offset,int len);
	void put(int n);
	void putBoolean(bool b);
	void putByte(signed char b);
	void putShort(short n);
	void putFloat(float n);
	void putInt(int n);
	void putLong(long long n);
	void putUTF(const char* str);
    void putUTF(const std::string& str);
    void putArray(const std::vector<signed char>& a);
    void putArray(const std::vector<bool>& a);
    void putArray(const std::vector<short>& a);
    void putArray(const std::vector<int>& a);
    void putArray(const std::vector<long long>& a);
    void putArray(const std::vector<std::string>& a);
    
    //get
    signed char getByte();
    bool getBoolean();
    int getInt();
    float getFloat();
    short getShort();
    long long getLong();
    void get(char* bytes,int size);
    void get(char* bytes,int offset,int len);
    void getArray(std::vector<signed char>& a);
    void getArray(std::vector<bool>& a);
    void getArray(std::vector<short>& a);
    void getArray(std::vector<int>& a);
    void getArray(std::vector<long long>& a);
    void getArray(std::vector<std::string>& a);
	void getArray(std::vector<signed char>& a,int len);
	void getArray(std::vector<bool>& a,int len);
	void getArray(std::vector<short>& a,int len);
	void getArray(std::vector<int>& a,int len);
	void getArray(std::vector<long long>& a,int len);
	void getArray(std::vector<std::string>& a,int len);
    void getUTF(std::string& str);
    
    std::string getUTF();
	int getPosition();
	void setPosition(int p);
	int getLimit();
	int getCapacity();
    
	char* getBuffer();
	char* toByteArray();
	
	//这个方法只能用于打开的是文本文件的时候
    std::string getLine();
	
	/**
	 * makes a buffer ready for a new sequence of channel-read or relative put operations: It sets the limit to the capacity and the position to zero.
	 */
    void clear();
    int getLength(int offset);
    void getAsBytes(signed char* bytes);
	
    /**
    * flip() makes a buffer ready for a new sequence of channel-write or relative get operations: It sets the limit to the current position and then sets the position to zero.
    */
    void flip();
    void compact();
    void rewind();
};


#endif
