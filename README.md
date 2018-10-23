# bake

### file handeling
* if -f flag present read following argument and open file at path
* else attempt to open file Bakefile in the cwd
* ignore lines beginning with '#'
* if the char before the newline char is a '\' the following line is a continuation.

### Invocation and Commandline Options

Options are:
* -C dirName : cd in to directory

* -f filename : read bakefile at path

* -i : ignore unsuccessful terminations of actions; contiuinue target actions if any fail

* -n : print (to stdout) each shell-command-sequence before it is to be executed, but do not

* actually execute the commands.

* -p : after reading in the specification file, print its information (from bake's internal

* 

* representation) to stdout with all variable expansions performed. Then simply exit with

  success.