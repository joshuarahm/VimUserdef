/* All modules should have a statement like the
 * following in this file for this tells the program
 * which modules should be included
 *
 * To disable certain modules, you can comment out any
 * of the following lines of code. To enable a module 
 * add a line in the form INCLUDE_MODULE( <module_name> ) 
 * (without the <>)
 *
 * The module name should be the same as the folder that the
 * module is in.
 */

/* This line includes the test module in the
 * build. The test module is mostly pointless,
 * so to disable it, you may comment out this
 * file
 * */
INCLUDE_MODULE( test )

/*
 * Include the module for highlighting
 * .c files.
 */
INCLUDE_MODULE( c )
