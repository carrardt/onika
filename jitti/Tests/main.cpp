#include "jitti.h"

int main(int argc, const char **argv)
{
  jitti::Module* jitmodule = jitti::Compiler::createModuleFromFile(argv[1],"-DMYDEFINE=33");
  auto function = jitmodule->getFunction(argv[2]);

  function( "Hello!" , 666 );

  return function.getReturnValueAsULong();
}

