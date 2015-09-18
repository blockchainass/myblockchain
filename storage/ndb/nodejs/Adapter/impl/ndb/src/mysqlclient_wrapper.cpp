/*
 Copyright (c) 2012, Oracle and/or its affiliates. All rights
 reserved.
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; version 2 of
 the License.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 02110-1301  USA
 */

#include <myblockchain.h>

#include "adapter_global.h"
#include "v8_binder.h"
#include "js_wrapper_macros.h"
#include "NativeCFunctionCall.h"

using namespace v8;

Handle<Value> myblockchain_init_wrapper(const Arguments &args) {
  HandleScope scope;
  
  REQUIRE_ARGS_LENGTH(0);  // there is one arg but we supply it here
  
  NativeCFunctionCall_1_<MYBLOCKCHAIN *, MYBLOCKCHAIN *> ncall(& myblockchain_init, args);
  ncall.arg0 = 0; 
  ncall.run();
  
  return scope.Close(ncall.jsReturnVal());
}


Handle<Value> myblockchain_close_wrapper(const Arguments &args) {
  HandleScope scope;
  
  REQUIRE_ARGS_LENGTH(0);
  
  NativeCVoidFunctionCall_1_<MYBLOCKCHAIN *> ncall(* myblockchain_close, args);
  ncall.run();
  
  return scope.Close(ncall.jsReturnVal());
}


Handle<Value> myblockchain_real_connect_wrapper(const Arguments &args) {
  HandleScope scope;
  
  REQUIRE_ARGS_LENGTH(8);
  
  NativeCFunctionCall_8_<MYBLOCKCHAIN *, MYBLOCKCHAIN *, const char *, const char *, const char *,
                         const char *, unsigned int, const char *, unsigned long> 
                         ncall(& myblockchain_real_connect, args);
  ncall.run();
  
  return scope.Close(ncall.jsReturnVal());
}


Handle<Value> myblockchain_error_wrapper(const Arguments &args) {
  HandleScope scope;
  
  REQUIRE_ARGS_LENGTH(1);
  
  NativeCFunctionCall_1_<const char *, MYBLOCKCHAIN *> ncall(& myblockchain_error, args);
  ncall.run();
  
  return scope.Close(ncall.jsReturnVal());
}


Handle<Value> myblockchain_query_wrapper(const Arguments &args) {
  HandleScope scope;
  
  REQUIRE_ARGS_LENGTH(2);
  
  NativeCFunctionCall_2_<int, MYBLOCKCHAIN *, const char *> ncall(& myblockchain_query, args);
  ncall.run();
  
  return scope.Close(ncall.jsReturnVal());
}


void myblockchainclient_initOnLoad(Handle<Object> target) {
  DEFINE_JS_FUNCTION(target, "myblockchain_init", myblockchain_init_wrapper);
  DEFINE_JS_FUNCTION(target, "myblockchain_close", myblockchain_close_wrapper);
  DEFINE_JS_FUNCTION(target, "myblockchain_real_connect", myblockchain_real_connect_wrapper);
  DEFINE_JS_FUNCTION(target, "myblockchain_error", myblockchain_error_wrapper);
  DEFINE_JS_FUNCTION(target, "myqsl_query", myblockchain_query_wrapper);  
}

