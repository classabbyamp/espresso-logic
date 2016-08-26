#include <v8.h>
#include <node.h>
#include <nan.h>
#include <stdlib.h>
#include "bridge.h"

NAN_METHOD(minimize) {
  v8::Local<v8::Array> array = info[0].As<v8::Array>();
  unsigned int length = array->Length();
  char ** truthTable = new char*[length];

  for(unsigned int i = 0; i < length; ++i) {
    v8::Local<v8::String> src = array->Get(i)->ToString();
    Nan::Utf8String val(src);
    truthTable[i] = new char[strlen(*val)+1];
    strcpy(truthTable[i], *val);
  }

  char ** result = run_espresso(truthTable, length);

  if (result != NULL) {
    unsigned int resultLength;

    for (resultLength = 0; result[resultLength] != NULL; ++resultLength);

    v8::Local<v8::Array> returnValue = Nan::New<v8::Array>(resultLength);

    for(unsigned int i = 0; i < length; ++i) delete truthTable[i];
    delete truthTable;

    for(unsigned int i = 0; i < resultLength; ++i) {
      Nan::Set(returnValue, i, Nan::New(result[i]).ToLocalChecked());

      // since the result comes from C code, the memory was
      // allocated using malloc and must be freed with free
      free(result[i]);
    }

    free(result);
    info.GetReturnValue().Set(returnValue);
  }

  info.GetReturnValue().Set(Nan::New<v8::Array>(0));
}

NAN_MODULE_INIT(init) {
  Nan::Set(target, Nan::New("minimize").ToLocalChecked(), Nan::GetFunction(Nan::New<v8::FunctionTemplate>(minimize)).ToLocalChecked());
}

NODE_MODULE(EspressoLogicMinimizer, init)
