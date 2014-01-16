#ifdef linux
#include <bsd/sys/tree.h>
#else

#ifdef BSD || __APPLE__
#include <sys/tree.h>
#else

#ifdef TREE_H_PATH
#include TREE_H_PATH
#else
/* can't compile for dozer yet :-( */
#error Cannot find the tree.h file. If you know where it is, add a #define called TREE_H_PATH with the path of the file surrounded by quotes.
#error You may need to install libbsd if you are on a linux system

#endif
#endif
#endif
