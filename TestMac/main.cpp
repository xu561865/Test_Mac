//
//  main.cpp
//  Test2
//
//  Created by xuyi on 15/8/17.
//  Copyright (c) 2015年 xuyi. All rights reserved.
//

#include <iostream>
#include "ClassA.h"
#include <string>
#include "json.h"
#include "Function.h"
#include "Hash_C.h"
#include "MI_C.h"
#include "AutoPool_C.h"
#include "CString_C.h"
#include <math.h>
#include <map>
#include <set>

#include "Stl_C.h"

class Base1
{
public:
    static const int m = 99;
    virtual void run1()
    {
        printf("BaseB1");
    }
};

class Base2
{
public:
    virtual void run2()
    {
        printf("BaseB2");

    }
};

class AA : public Base1, public Base2
{
public:
    virtual void run2() override
    {
        printf("AA");
    }
};

int solution(int A[], int N) {
    // write your code in C99 (gcc 4.8.2)
    
    if(N < 3)
    {
        return -1;
    }
    
    int first = -1;
    int second =-1;
    int third = -1;
    int depth = 0;
    
    
    for(int i = 0; i < N - 1; i++)
    {
        if(A[i] > A[i + 1])
        {
            if(first != -1 && second != -1 && third != -1)
            {
                int tmpDepth = first - second > third - second ? first - second : third - second;
                depth = depth < tmpDepth ? tmpDepth : depth;
                
                first = second = third = -1;
            }
            if(first == -1)
            {
                first = i;
            }
            
            second = i + 1;
        }
        
        if(A[i] < A[i + 1])
        {
            third = i + 1;
        }
    }
    
    return depth;
}

int solution(std::vector<int> &A, int D);


bool checkRoute(std::vector<int>& A, int D)
{
    // generate B
    std::vector<int> B;
    B.push_back(0);
    for(auto i : A)
    {
        B.push_back(i);
    }
    B.push_back(0);
    
    
    // generate C
    std::vector<int> C;
    for(int i = 0; i < B.size(); i++)
    {
        if(B[i] == 0)
        {
            C.push_back(i);
        }
    }
    
    bool isPassed = true;
    for(int i = 0; i < C.size() - 1; i++)
    {
        if(D < C[i + 1] - C[i])
        {
            isPassed = false;
            break;
        }
    }
    
    return isPassed;
}

int solution_new(std::vector<int> &A, int D)
{
    if(D > A.size())
    {
        return 0;
    }
    
    
    // set max time
    int max_time = 0;
    for(auto i : A)
    {
        if(i > 0)
        {
            max_time = max_time < i ? i : max_time;
        }
    }
    
    if(max_time <= 0)
    {
        if(checkRoute(A, D))
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        uint32_t use_time = 0;
        
        do
        {
            if(checkRoute(A, D))
            {
                return use_time;
            }
            else
            {
                // 台阶上升
                for(int j = 0; j < A.size(); j++)
                {
                    if(A[j] > 0)
                    {
                        A[j]--;
                    }
                }
            }
        }while(use_time++ != max_time);
    }
    
    return -1;
}

int solution_2(std::vector<int> &A)
{
    int break_index_1 = -1;
    int break_index_2 = -1;
    
    /*
    for(int i = 1; i < A.size() -1; i++)
    {
        if(break_index_1 == -1)
        {
            break_index_1 = i;
        }
        else if(abs(break_index_1 - i) > 1 && break_index_2 == -1)
        {
            break_index_2 = i;
        }
        else if(break_index_1 != -1 && break_index_2 != -1)
        {
            int &big_index = A[break_index_1] <= A[break_index_2] ? break_index_2 : break_index_1;
            
            if(A[i] >= A[big_index]) continue;
            else
            {
                int &nearest_index = break_index_1 < break_index_2 ? break_index_2 : break_index_1;
                if(big_index == nearest_index)      //大值在后
                {
                    big_index = i;      //替换大值
                }
                else if(i - nearest_index > 1)      //小值在后(small_index == nearest_index)，小值与i不相邻
                {
                    big_index = i;      //替换大值
                }
                else if(i - nearest_index == 1 && A[i] < A[nearest_index])     //小值在后(small_index == nearest_index)，小值与i相邻，A[i]比小值更小
                {
                    nearest_index = i;
                }
            }
        }
    }
    
    */
    
    
    
    return A[break_index_1] + A[break_index_2];
}

void print_m(std::map<int, int>& m)
{
    printf("\n");
    for(auto i : m)
    {
        printf("%d ", i.second);
    }
    printf("\n");
}

int solution_length_A(std::vector<int> &A)
{
    std::map<int, int> m_small;
    std::map<int, int> m_big;
    
    int small_first_index, small_last_index, big_first_index, big_last_index, small_length, big_length;

    small_length = big_length = 0;
    
    for(int i = 0; i < A.size(); i++)
    {
        if(m_small.find(A[i]) == m_small.end())
        {
            m_small[A[i]] = i;
        }
        
        m_big[A[i]] = i;
    }
    
     //test
     printf("small");
     print_m(m_small);
     printf("big");
     print_m(m_big);
     //
     
    small_first_index = small_last_index = m_small.begin()->second;
    for(auto j : m_small)
    {
        if(small_first_index > j.second)
        {
            small_first_index = j.second;
        }
        else if(small_last_index < j.second)
        {
            small_last_index = j.second;
        }
    }
    
    for(int k = small_first_index; k < m_small.size(); k++)
    {
        if(small_first_index + 1 < m_small.size() && A[small_first_index] == A[small_first_index + 1])
        {
            small_first_index = small_first_index + 1;
        }
        else
        {
            break;
        }
    }
    
    small_length = small_last_index - small_first_index + 1;
    
    big_first_index = big_last_index = m_big.begin()->second;
    for(auto j : m_big)
    {
        if(big_first_index > j.second)
        {
            big_first_index = j.second;
        }
        else if(big_last_index < j.second)
        {
            big_last_index = j.second;
        }
    }
    
    for(int k = big_last_index; k >= 0; k--)
    {
        if(big_last_index - 1 >= 0 && A[big_last_index] == A[big_last_index - 1])
        {
            big_last_index = big_last_index - 1;
        }
        else
        {
            break;
        }
    }
    
    small_length = small_last_index - small_first_index + 1;
    big_length = big_last_index - big_first_index + 1;
    
    //test
    if(small_length < big_length)
    {
        printf("first: %d, last: %d", small_first_index, small_last_index);
    }
    else
    {
        printf("first: %d, last: %d", big_first_index, big_last_index);
    }
    //
    
    return small_length < big_length ? small_length : big_length;
}

int solution_length(std::vector<int> &A)
{
    std::map<int, int> m_small;
    std::map<int, int> m_big;
    
    int small_first_index, small_last_index, big_first_index, big_last_index, small_length, big_length;
    small_last_index = big_first_index = -1;
    small_first_index = 0;
    big_last_index = (int)A.size() - 1;
    small_length = big_length = 0;
    
    int tmp_big_first_index = -1;
    int tmp_big_last_index = -1;
    
    for(int i = 0; i < A.size(); i++)
    {
        if(m_small.find(A[i]) == m_small.end())
        {
            m_small[A[i]] = i;
            
            if(small_last_index == -1)
            {
                small_last_index = i;
            }
            else
            {
                small_last_index = small_last_index > i ? small_last_index : i;
            }
        }
        
        m_big[A[i]] = i;
        if(big_first_index == -1)
        {
            big_first_index = i;
        }
        else
        {
            // 相邻
            if(big_first_index + 1 == i)
            {
                if(A[big_first_index] == A[i])
                {
                    big_first_index = i;
                }
                else if(tmp_big_first_index == -1)
                {
                    tmp_big_first_index = i;
                }
            }
            else if(A[big_first_index] == A[i])
            {
                big_first_index = tmp_big_last_index;
                tmp_big_last_index = -1;
            }
        }
        
        // 最小连续多个处理
        if(small_first_index + 1 == i && A[small_first_index] == A[i])
        {
            small_first_index = i;
        }
        
        // 最大连续多个处理
        if(tmp_big_last_index == -1 && A[i] == A[big_last_index])
        {
            tmp_big_last_index = i;
        }
        else if(tmp_big_last_index != -1 && A[i] == A[big_last_index])
        {
            if(tmp_big_last_index + 1 != i)
            {
                tmp_big_last_index = i;
            }
        }
    }
    
    big_last_index = tmp_big_last_index;
    
    small_length = small_last_index - small_first_index + 1;
    big_length = big_last_index - big_first_index + 1;
    
    //test
    if(small_length < big_length)
    {
        printf("first: %d, last: %d", small_first_index, small_last_index);
    }
    else
    {
        printf("first: %d, last: %d", big_first_index, big_last_index);
    }
    //
    
    return small_length < big_length ? small_length : big_length;
}

std::vector<int> solution(std::vector<int> &A)
{
    int X = 0;
    for(int i = 0; i < A.size(); i++)
    {
        X += A[i] * pow(-2, i);
    }
    
    printf("%d\n", X);
    
    std::vector<int> ret;
    
    int Y = X + 1;
    int rate = ceil(log2(abs(Y)));
    int sum = 0;
    int rateY = 0;
    
    //负数
    if(Y < 0)
    {
        if(rate % 2 == 0)
        {
            sum = pow(-2, rate + 1);
            rateY = rate + 2;
        }
        else
        {
            sum = pow(-2, rate);
            rateY = rate + 1;
        }
    }
    else if(Y == 0)
    {
        ret[0] = 0;
    }
    // 正数
    else
    {
        if(rate % 2 == 0)
        {
            sum = pow(-2, rate);
            rateY = rate + 1;
        }
        else
        {
            sum = pow(-2, rate + 1);
            rateY = rate + 2;
        }
    }
    
    ret.resize(rateY, 0);
    ret[0] = Y % 2 == 0 ? 0 : 1;
    ret[ret.size() - 1] = 1;
    
    for(int i = rate; i > 0; i--)
    {
        if(sum > Y)
        {
            ret[i] = (i % 2 == 0) ? 0 : 1;
        }
        else if(sum < Y)
        {
            ret[i] = (i % 2 == 0) ? 1 : 0;
        }
        else
        {
            break;
        }
        
        sum += pow(-2, i) * ret[i];
    }
    
    printf("sum : %d\n", sum);
    
    return ret;
}

int solution_brackets(std::string &S)
{
    int ret = 0;
    
    /*
    int count_open_brackets = 0;
    int count_close_brackets = 0;
    
    printf("length: %d \n", S.length());
    
    for(int i = 0; i < S.length(); i++)
    {
        if(S[i] == '(')
        {
            count_open_brackets++;
        }
        
        if(S[i] == ')')
        {
            count_close_brackets++;
        }
    }
    
    int tmp_count_open_brackets = 0;
    int tmp_count_close_brackets = 0;
    for(int i = 0; i < S.length(); i++)
    {
        if(S[i] == '(')
        {
            tmp_count_open_brackets++;
        }
        
        if(S[i] == ')')
        {
            tmp_count_close_brackets++;
        }
        
        if(tmp_count_open_brackets == count_close_brackets - tmp_count_close_brackets)
        {
            ret = i + 1;
            
            return ret;
        }
    }
     */
    
    // (())))(
    printf("%d\n", S.length());
    
    std::string::iterator iter_begin = S.begin();
    std::string::iterator iter_end = S.end() - 1;
    
    
    while (iter_begin < iter_end)
    {
        while((iter_begin < iter_end) && ('(' != *iter_begin))
        {
            printf("%c ", *iter_begin);
            iter_begin++;
//            ret++;
        }
        
        while((iter_begin < iter_end) && (')' != *iter_end))
        {
            printf("%c ", *iter_end);
            iter_end--;
        }
        
        if(iter_begin < iter_end)
        {
            iter_begin++;
//            ret++;
            
            if(iter_begin < iter_end)
            {
                iter_end--;
            }
        }
    }
    
    return iter_begin - S.begin();
//    return ret == 0 ? 0 : ret + 1;
}

int find_ones(uint32_t n)
{
    int j = 0;
    int i = 0;
    while(n > 0)
    {
        // 当前位为 0，直接判断下一位
        int remander = n % (int)pow(10, i + 1);
        if(remander == 0)
        {
            i++;
            continue;
        }
        
        // 当前位为 1，且为最高位
        n -= (int)pow(10, i);
        if(n == 0)
        {
            j++;
            i++;
        }
        
        // 当前位为 1，但不是最高位
        remander = n % (int)pow(10, i + 1);
        if(n > 0 && remander == 0)
        {
            j++;
            i++;
        }
        else    // 当前位不为 1
        {
            n -= remander;
            i++;
        }
    }
    
    return j;
}

int main(int argc, const char * argv[]) {
    printf("find : %d\n", find_ones(12091));
    return 0;
    
    std::vector<std::string> s=
        {
            "((((())))))))))))))(()",       //0     15
            "()((()()()()()(((())))(()))()()",  //1
            "((()))))((((()))",     //2
            "(())))(",      //3
            ")(",       //4
            "(",        //5
            "" ,     //6
            "()" ,   //7
            "((((((((", // 8
            "))))))",   // 9
        };
    printf("solution: %d \n", solution_brackets(s[1]));;
    
    return 0;
    
    std::vector<int> A1 = {1, 3, 7, 2, 3, 5};
    std::vector<int> A2 = {3, 4, 5, 6, 7, 8};
    std::vector<int> A3 = {9, 1, 2, 7, 3};
    std::vector<int> A4 = {5, 2, 4, 6, 3, 7};
    std::vector<int> A5 = {9, 9, 8, 3, 2, 4, 8};
    std::vector<int> A6 = {1, 2, 1, 2, 3, 1, 2, 3, 4};
    std::vector<int> A7 = {1, 1, 1, 2, 2, 3, 3, 3, 4};
    std::vector<int> A8 = {1, 2, 3, 4, 1, 2, 3, 1, 2};
    std::vector<int> A9 = {7, 3 ,7, 3, 1, 3, 4, 1};
    std::vector<int> A10 = {1, 2, 3, 4, 1, 2};
    std::vector<int> A11 = {1, 1, 2, 3, 4, 1, 2};
    std::vector<int> A12 = {1};
    std::vector<int> A13 = {1, 1};
    std::vector<int> A14 = {1, 1, 2, 3, 3};
    std::vector<int> A15 = {1, 2};
    std::vector<int> A16 = {1, 3, 4, 1, 2, 1, 3, 2, 1};
    
    std::vector<int> M1 = {1, 0, 1};    // 5
    std::vector<int> M2 = {1, 1, 1};    // 3
    std::vector<int> M3 = {1, 1, 1, 0, 1};    // 19
    
    for(auto i : solution(M3))
    {
        printf("%d ", i);
    }


#ifdef T1
    ClassA a;
    Json::Value val = a.translateJson(std::string("aabbcc"));
    Json::Value va;
    va = val;
#endif
    
#ifdef FUN1
    //test fun
    FunctionClass fun;
    fun.setFun();
    fun.getFun("name")();
#endif
    
#ifdef HASHTEST
    //test hash
    Hash_C c;
    User *user = new User();
    User *user1 = new User();
    c.addHashTable(user);
    c.addHashTable(user1);
    //c.delHashTable(user);
    c.showHashTable();
#endif

#ifdef MITEST
    A a;
    MI_C mic;
    mic.runFun(&a, fun_micro(C::funC));
#endif
    
#ifdef AUTOPOOL
    AutoPool_C autoPool;
    autoPool.ReleaseObject();
#endif
    
#define STREAM
#ifdef STREAM
    CString_C strc;
    strc.test_CString();
    
    
#endif
    
    std::cout << "Hello, World!\n";
    return 0;
}


int solution(std::vector<int> &A, int D)
{
    if(D > A.size())
    {
        return 0;
    }
    
    
    // set max time
    int max_time = -1;
    for(auto i : A)
    {
        if(i >= 0)
        {
            max_time = max_time < i ? i : max_time;
        }
    }
    
    int tmpMaxTime = max_time;
    
    while(tmpMaxTime > -1)
    {
        bool canJump = true;
        
        int begin = -1;
        int first = -1;
        int second = -1;
        for(int i = 0; i < A.size(); i++)
        {
            if(A[i] == 0 && begin == -1)
            {
                if(i > D - 1)
                {
                    break;  //第一步即不可跳
                }
                else
                {
                    begin = 0;
                }
            }
            
            if(A[i] == 0 && first == -1)
            {
                first = i;
                if(first >= A.size() - D)
                {
                    
                    return max_time - tmpMaxTime;
                }
            }
            else if(A[i] == 0 && first != -1 && second == -1)
            {
                second = i;
                if(second - first >= D)
                {
                    canJump = false;
                    break;
                }
                
                if(second >= A.size() - D)
                {
                    return max_time - tmpMaxTime;
                }
            }
            
            if(first != -1 && second != -1)
            {
                if(second - first >= D)
                {
                    canJump = false;
                    break;
                }
                else
                {
                    first = second;
                    
                    second = -1;
                }
            }
        }
        
        if(canJump && second != -1 && second >= A.size() - D)
        {
            return max_time - tmpMaxTime;
        }
        

        
        // 台阶上升
        for(int j = 0; j < A.size(); j++)
        {
            if(A[j] > 0)
            {
                A[j]--;
            }
        }
        
        tmpMaxTime--;
    }
    
    if(tmpMaxTime <= -1)
    {
        return -1;
    }
    else
    {
        return max_time - tmpMaxTime;
    }
}



