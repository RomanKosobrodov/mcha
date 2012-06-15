#include <octave/oct.h>

DEFUN_DLD (helloOctave, args, nargout, "Hello World Help String")
{

  int nargin = args.length();

  octave_stdout << "Hello World has " << nargin << " input arguments and " << nargout << " output arguments.\n";

  return octave_value_list();

}
