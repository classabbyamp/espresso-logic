#include <v8.h>
#include <node.h>
#include <nan.h>
#include <stdlib.h>

extern "C" char ** run_espresso_from_data(char ** data, unsigned int length);
extern "C" char ** run_espresso_from_path(char * path);

NAN_METHOD(minimize_from_data) {
  v8::Local<v8::Array>
    array = info[0].As<v8::Array>(),
    returnValue = Nan::New<v8::Array>();
  unsigned int length = array->Length();
  char
    **truthTable,
    **result;

  // returns an empty array if no input is provided
  if (length == 0) {
    info.GetReturnValue().Set(returnValue);
    return;
  }

  truthTable = new char*[length];

  for(unsigned int i = 0; i < length; ++i) {
    v8::Local<v8::String> src = array->Get(i)->ToString();
    Nan::Utf8String val(src);
    truthTable[i] = new char[strlen(*val)+1];
    strcpy(truthTable[i], *val);
  }

  result = run_espresso_from_data(truthTable, length);

  if (result != NULL) {
    for(unsigned int i = 0; result[i] != NULL; ++i) {
      Nan::Set(returnValue, i, Nan::New(result[i]).ToLocalChecked());

      // since the result comes from C code, the memory was
      // allocated using malloc and must be freed with free
      free(result[i]);
    }

    free(result);
  }

  // memory clean up
  for(unsigned int i = 0; i < length; delete truthTable[i++]);
  delete truthTable;

  info.GetReturnValue().Set(returnValue);
}

NAN_METHOD(minimize_from_path) {
  v8::Local<v8::String> path = info[0].As<v8::String>();
  v8::Local<v8::Array> returnValue = Nan::New<v8::Array>();
  char **result;

  // returns an empty array if no path
  if (path->Length() == 0) {
    info.GetReturnValue().Set(returnValue);
    return;
  }

  Nan::Utf8String c_pathstr(path);
  result = run_espresso_from_path(*c_pathstr);

  if (result != NULL) {
    for(unsigned int i = 0; result[i] != NULL; ++i) {
      Nan::Set(returnValue, i, Nan::New(result[i]).ToLocalChecked());

      // since the result comes from C code, the memory was
      // allocated using malloc and must be freed with free
      free(result[i]);
    }

    free(result);
  }

  info.GetReturnValue().Set(returnValue);
}

NAN_MODULE_INIT(init) {
  Nan::Set(target, Nan::New("minimize_from_data").ToLocalChecked(), Nan::GetFunction(Nan::New<v8::FunctionTemplate>(minimize_from_data)).ToLocalChecked());
  Nan::Set(target, Nan::New("minimize_from_path").ToLocalChecked(), Nan::GetFunction(Nan::New<v8::FunctionTemplate>(minimize_from_path)).ToLocalChecked());
}

NODE_MODULE(EspressoLogicMinimizer, init)
