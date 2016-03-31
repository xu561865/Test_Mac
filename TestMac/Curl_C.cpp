//
//  Curl_C.cpp
//  TestMac
//
//  Created by 徐以 on 4/1/16.
//  Copyright © 2016 xuyi. All rights reserved.
//
#include "Curl_C.h"

#include <curl/curl.h>
#include <stdlib.h>
#include <iostream>


Curl_C* Curl_C::sharedInstance()
{
    static Curl_C* s_pCurl = nullptr;
    if(!s_pCurl)
    {
        s_pCurl = new Curl_C();
    }
    
    return s_pCurl;
}

void Curl_C::testUrl()
{
    CURL *curl;			//定义CURL类型的指针
    CURLcode res;		//定义CURLcode类型的变量
    
    curl = curl_easy_init();	//初始化一个CURL类型的指针
    if(curl!=NULL)
    {
        //设置curl选项. 其中CURLOPT_URL是让用户指定url. argv[1]中存放的命令行传进来的网址
        curl_easy_setopt(curl, CURLOPT_URL, "www.chinaunix1.net");
        //调用curl_easy_perform 执行我们的设置.并进行相关的操作. 在这里只在屏幕上显示出来.
        res = curl_easy_perform(curl);
        if (res == CURLE_OK)
        {
            std::cout<<"success"<<std::endl;
        }
        else
        {
            std::cout<<"failed"<<std::endl;
        }
        
        //清除curl操作.
        curl_easy_cleanup(curl);
    }
}