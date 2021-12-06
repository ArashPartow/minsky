/*
  @copyright Steve Keen 2021
  @author Russell Standish
  This file is part of Minsky.

  Minsky is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Minsky is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Minsky.  If not, see <http://www.gnu.org/licenses/>.
*/

/* A DelayLoad implementation for Windows. Because we're using Mingw,
   not MSVC, we cannot use the declspec(dllimport) and /DELAYLOAD
   mechanism, so instead we provide stubs here that delegate to the
   exported functions in node.exe */

#include "node_api.h"
#include <windows.h>
#include <iostream>

static HINSTANCE nodeExe=GetModuleHandle(nullptr);

extern "C"
{

#define NAPIFN(name,arg_decls,args)                                     \
  napi_status name arg_decls                                            \
  {                                                                     \
    static auto symbol=(decltype(name)*)GetProcAddress(nodeExe, #name); \
    return symbol? symbol args: napi_invalid_arg;                       \
  }                                                                     \

#define VOID_NAPIFN(name,arg_decls,args)                                \
  void name arg_decls                                                   \
  {                                                                     \
    static auto symbol=(decltype(name)*)GetProcAddress(nodeExe, #name); \
    if (symbol) symbol args;                                            \
  }                                                                     \


  
  NAPIFN(napi_create_array, (napi_env env, napi_value* result), (env,result));

  NAPIFN(napi_create_function, (napi_env env, const char* utf8name, size_t length,
                                napi_callback cb, void* data, napi_value* result),
         (env,utf8name,length,cb,data,result));

  NAPIFN(napi_add_finalizer, (napi_env env, napi_value js_object, void* native_object,
                              napi_finalize finalize_cb, void* finalize_hint, napi_ref* result),
         (env,js_object,native_object,finalize_cb,finalize_hint,result));

  NAPIFN(napi_create_string_utf8,
         (napi_env env, const char* str, size_t length, napi_value* result),
         (env,str,length,result));

  NAPIFN(napi_set_property,
         (napi_env env, napi_value object, napi_value key,napi_value value),
         (env,object,key,value));

  NAPIFN(napi_create_type_error,
         (napi_env env, napi_value code, napi_value msg, napi_value* result),
         (env,code,msg,result));
  
  NAPIFN(napi_create_reference,
         (napi_env env,napi_value value, uint32_t initial_refcount,napi_ref* result),
         (env,value,initial_refcount,result));

  NAPIFN(napi_delete_reference, (napi_env env, napi_ref ref), (env,ref));

  NAPIFN(napi_get_reference_value, (napi_env env, napi_ref ref, napi_value* result),
         (env,ref,result));

  NAPIFN(napi_open_handle_scope, (napi_env env, napi_handle_scope* result),(env,result));
  NAPIFN(napi_close_handle_scope, (napi_env env, napi_handle_scope scope), (env,scope));

  NAPIFN(napi_throw, (napi_env env, napi_value error),(env,error));

  NAPIFN(napi_get_undefined,(napi_env env, napi_value* result), (env,result));
  NAPIFN(napi_get_null, (napi_env env, napi_value* result), (env,result));

  NAPIFN(napi_typeof,(napi_env env,napi_value value,napi_valuetype* result),
         (env,value,result));
  
  NAPIFN(napi_get_boolean, (napi_env env,bool value, napi_value* result),(env,value,result));

  NAPIFN(napi_get_value_uint32,(napi_env env,napi_value value,uint32_t* result),
         (env,value,result));
  NAPIFN(napi_get_value_int32,(napi_env env,napi_value value,int32_t* result),
         (env,value,result));

  NAPIFN(napi_async_destroy, (napi_env env, napi_async_context async_context), (env,async_context));
  
  VOID_NAPIFN(napi_fatal_error, (const char* location, size_t location_len,
                                 const char* message, size_t message_len),
         (location,location_len,message,message_len));

  NAPIFN(napi_close_callback_scope, (napi_env env, napi_callback_scope scope),(env,scope));
  
  NAPIFN(napi_get_last_error_info, (napi_env env,const napi_extended_error_info** result),(env,result));

  NAPIFN(napi_is_exception_pending,(napi_env env, bool* result),(env,result));

  NAPIFN(napi_get_and_clear_last_exception,(napi_env env, napi_value* result),(env,result));
  
  NAPIFN(napi_create_error, (napi_env env, napi_value code, napi_value msg, napi_value* result),
         (env,code,msg,result));

  NAPIFN(napi_get_cb_info, (napi_env env, napi_callback_info cbinfo, size_t* argc, 
         napi_value* argv, napi_value* this_arg, void** data),
         (env,cbinfo,argc,argv,this_arg,data));

  NAPIFN(napi_open_escapable_handle_scope, (napi_env env, napi_escapable_handle_scope* result),
         (env,result));
  NAPIFN(napi_close_escapable_handle_scope, (napi_env env, napi_escapable_handle_scope scope), (env,scope));

  NAPIFN(napi_escape_handle,(napi_env env, napi_escapable_handle_scope scope, napi_value escapee,
                             napi_value* result),(env,scope,escapee,result));
  
  NAPIFN(napi_get_named_property,(napi_env env, napi_value object, const char* utf8name,
                                  napi_value* result),(env,object,utf8name,result));

  NAPIFN(napi_get_value_string_utf8, (napi_env env, napi_value value, char* buf,size_t bufsize,
                                      size_t* result), (env,value,buf,bufsize,result));
  NAPIFN(napi_call_function, (napi_env env, napi_value recv, napi_value func,size_t argc,
                              const napi_value* argv,napi_value* result),
         (env,recv,func,argc,argv,result));
  
  VOID_NAPIFN(napi_module_register, (napi_module* mod), (mod));

  NAPIFN(napi_coerce_to_string,(napi_env env,napi_value value,napi_value* result),(env,value,result));
  
  NAPIFN(napi_set_element, (napi_env env, napi_value object,uint32_t index, napi_value value),
         (env,object,index,value));

}


