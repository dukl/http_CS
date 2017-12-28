/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2015, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/ 
/* <DESC>
 * Shows how the write callback function can be used to download data into a
 * chunk of memory instead of storing it in a file.
 * </DESC>
 */ 
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h" 
#include <curl/curl.h>


struct MemoryStruct {
  char *memory;
  size_t size;
};
 
static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  mem->memory = realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
    /* out of memory! */ 
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}
 
int main(void)
{
  cJSON * root,*fmt;
  char * data;

  CURL *curl_handle;
  CURLcode res; 
  struct MemoryStruct chunk;
 
  chunk.memory = malloc(1);  
  chunk.size = 0;    
 
  root = cJSON_CreateObject();
  cJSON_AddItemToObject(root,"msg_type",cJSON_CreateString("test"));
  cJSON_AddItemToObject(root,"parameters",fmt = cJSON_CreateObject());
  cJSON_AddStringToObject(fmt,"pam1","a");
  cJSON_AddNumberToObject(fmt,"pam2",100);
  data = cJSON_Print(root);
  printf("\n%s\n",data);

  /*
   * json解析
   * */
  cJSON * json = cJSON_Parse(data);
  printf("%d\n",json->child->type);
  printf("%s\n",json->child->string);
  printf("%s\n",json->child->valuestring);

  cJSON * fmtParse = json->child->next;
  
  printf("%d\n",fmtParse->child->type);
  printf("%s\n",fmtParse->child->string);
  printf("%s\n",fmtParse->child->valuestring);
  printf("%d\n",fmtParse->child->next->type);
  printf("%s\n",fmtParse->child->next->string);
  printf("%d\n",fmtParse->child->next->valueint);
//解析结束
  curl_global_init(CURL_GLOBAL_ALL);
  curl_handle = curl_easy_init();
 
  curl_easy_setopt(curl_handle, CURLOPT_URL, "http://127.0.0.1:8000");
  curl_easy_setopt(curl_handle,CURLOPT_POST,1);
  curl_easy_setopt(curl_handle,CURLOPT_POSTFIELDS,data);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  res = curl_easy_perform(curl_handle);
 
  if(res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
  }
  else {
    printf("\ndata:\n%s\n",chunk.memory);
    printf("%lu bytes retrieved\n", (long)chunk.size);
  }

  curl_easy_cleanup(curl_handle);
  free(chunk.memory);
  curl_global_cleanup();
 
  return 0;
}
