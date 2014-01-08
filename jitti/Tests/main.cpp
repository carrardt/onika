#include "jitti.h"

int main(int argc, const char **argv)
{
  jitti::Module jitmodule = jitti::Compiler::createModuleFromFile(argv[1],"-DMYDEFINE=33");
  auto justdoit = jitmodule.getFunction(argv[2]);

  justdoit( "Hello!" , 666 );

  return justdoit.getReturnValueAsULong();
}

