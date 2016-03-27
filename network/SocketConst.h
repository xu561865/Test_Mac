#ifndef _UTIL_H_
#define _UTIL_H_

#include <pthread.h>
#include <assert.h>

#define  Float_MAX_VALUE  3.4028235E+38F
#define  Float_MIN_VALUE  1.4E-45F;

typedef signed char byte;

#define SK_SAFE_DELETE(p)           do { if(p) { delete (p); (p) = 0; } } while(0)
#define SK_SAFE_DELETE_ARRAY(p)     do { if(p) { delete[] (p); (p) = 0; } } while(0)
#define SK_SAFE_DELETE_VEC(x)   {for(int i = 0; i < x->size();i++){delete (x+i);}delete[] x;}

#define SK_ASSERT(e) assert(e)


template<typename _RandomAccessIterator>
inline void safe_delete_vector(_RandomAccessIterator __first, _RandomAccessIterator __last)
{
	for (_RandomAccessIterator it = __first;it!= __last ; ++it)
    {
		if( (*it)!=NULL)
        {
			delete *it;
			*it = NULL;
		}
	}
}

const char* fullpathFromRelatePath(const char* relatePath);


class MyLock
{
public:
	MyLock(pthread_mutex_t* _mutex_t)
    {
        this->mutex_t = _mutex_t;
        pthread_mutex_lock(mutex_t);
    }
    
	~MyLock()
    {
        pthread_mutex_unlock(mutex_t);
    }
    
private:
    pthread_mutex_t* mutex_t;
};


#endif
