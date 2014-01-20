	,-.----.                                                   ___                                   
	\    /  \                    ,---,  ,--,                 ,--.'|_    ,--,                         
	;   :    \                 ,---.'|,--.'|                 |  | :,' ,--.'|     ,---.        ,---,  
	|   | .\ :                 |   | :|  |,                  :  : ' : |  |,     '   ,'\   ,-+-. /  | 
	.   : |: |   ,--.--.       |   | |`--'_       ,--.--.  .;__,'  /  `--'_    /   /   | ,--.'|'   | 
	|   |  \ :  /       \    ,--.__| |,' ,'|     /       \ |  |   |   ,' ,'|  .   ; ,. :|   |  ,"' | 
	|   : .  / .--.  .-. |  /   ,'   |'  | |    .--.  .-. |:__,'| :   '  | |  '   | |: :|   | /  | | 
	;   | |  \  \__\/: . . .   '  /  ||  | :     \__\/: . .  '  : |__ |  | :  '   | .; :|   | |  | | 
	|   | ;\  \ ," .--.; | '   ; |:  |'  : |__   ," .--.; |  |  | '.'|'  : |__|   :    ||   | |  |/  
	:   ' | \.'/  /  ,.  | |   | '/  '|  | '.'| /  /  ,.  |  ;  :    ;|  | '.'|\   \  / |   | |--'   
	:   : :-' ;  :   .'   \|   :    :|;  :    ;;  :   .'   \ |  ,   / ;  :    ; `----'  |   |/       
	|   |.'   |  ,     .-./ \   \  /  |  ,   / |  ,     .-./  ---`-'  |  ,   /          '---'        
	`---'      `--`---'      `----'    ---`-'   `--`---'               ---`-'                        
                                                                                                 
                                                                                                            
                                                                                                            
                                            
Radiation is a new Vim plugin in the (very) early stages of development which
will add extra syntactic sugar to your syntax file. Currently, the only
supported file type is C, but there are plans for Java and C++ in the *very*
near future.

## Screenshot

Below is a screenshot of Radiation in action.

![Screenshot 1](https://raw.github.com/jrahm/Radiation/master/snapshot.jpg)

Notice that not only does radiation detect and highlight the types found in the
file, but also radiation can find the types from included files as well. This is
because the C file is run through the C compiler preprocessor to exactly match
the state of the code at compilation.

## Why use Radiation?

1. Radiation is fast.
	* Personally, I have tried other plugins for this same problem, but all were
	too slow for me to use in a moderately sized code base, mostly because
	unlike, Radiation, they are written in VimL or some other scripting language
	* Radiation consists of a shared library written in native code as well with
	a python thin client attached to Vim. This means all the heavy lifting is
	done by native code which as of now is written *entirely* in C, although
	that may change.
2. Radiation isn't restricted
	* Since Radiation is written in C real threading becomes an option. Right
	now the threadding use is minimal; however, it is possible to do parallel
	parsing to increase the speed phenominally. (You'll probably see a branch
	for this soon)
## Why not use Radiation?
1. Radiation is still in alpha.
	* Since Radiation is a shared library in its alpha versions, C-ish type
	problems now pose a threat to Vim, such as segmentation faults and memory
	leaks. This is why it is recommended that you set up Radiation to save your
	code before it runs (it has to anyway). I have found this version to be
	*reasonably* stable, but I am one person running on a very specific
	environment.
2. If you run a restricted version of Vim.
	* Radiation needs a fully unlocked version of Vim to run.
3. If you run Windows.
	* If you are a *very* rare Vim user that uses Windows, I will likely **not**
	make a port to Windows; However, that does not mean some other kind
	soul won't, *I* just won't.
4. If you run anything other than Linux
	* Right now, the only official support is for Linux. If you run BSD or Mac,
	good luck, official support will come soon enough, but right now I have only
	*tested* on Linux. Theoretically it should work with those operating
	systems, but I have not tested.

## Installation
1. Requirements
	* You need to have Vim with +python enabled
	* You must have installed libbsd on your system, or have the "tree.h" header
	downloaded on your system
2. Process
	* Radiation will automatically detect if you have a pathogen `bundle`
	directory and install it there. If you
	do not then I recommend you back up your .vim directory if you care because
	the installer will attempt to install directly into your .vim folder. Or you
	could just install Pathogen, it's easy, trust me!
	* Simply download the git package and run `make install` inside the project
3. Caveats
	* When installing, the installer will ask for sudo privileges, this is to
	install the shared library in `/usr/lib`. If you do not have root access via
	sudo, then you may run `make install` with the environment variable
	`LIB_PREFIX` set to the folder you wish to install the shared library
	(presumably something like `$HOME/.lib/`) to.
		* Remember that this is a shared library, so make sure the LIB_PREFIX
		variable is on your `LD_LIBRARY_PATH` list when Vim starts or else it
		will fail to load the shared library.
	* For the plugin to work, you'll probably want gcc, although this may be
	modified.
	* Do not have libbsd, but have downloaded the tree.h header file, you will
	need to add a `-DTREE_H_PATH=\"<path to tree.h>\"` (with the escaped quotes)
	to the build flags. You do this by exporting the environment variable `OPTFLAGS`
	before `make install` This variable is additional flags to pass to the
	compiler during a build. While you're at it, it may be advantageous to add
	`-O2` as there are no optimization flags on by default.

## Usage
Radiation will already have allowed itself to initialize on startup, so there is
no need to do anything there.

With C highlighting, there are 2 Vim variables you may set to fine tune the
compilation:
	* `g:radiation_c_compiler` the compiler to use. By default, the compiler
	used is gcc, but this may be used to change it. 
	* `g:radiation_c_cflags` additional flags to be sent to the compiler

To radiate a file, use the command `:Radiate`. This will highlight the file for
you.

There are 5 different highlighting groups that Radiation uses and must be
defined by the user.
	* `RadiationCTypedef` is the highlighting group for typedef'd types
	* `RadiationCStruct` is the highlighting group for structures
	* `RadiationCEnum` is the highlighting group for enumerations
	* `RadiationCUnion` is the highlighting group for unions
	* `RadiationCFunction` is the highlighting group for functions
To add a highlighting group add to your vimrc or colorscheme file

	hi <group> ctermfg=<fg color for terminal> guifg=<fg for gvim>

for example (for the colors in the screenshot)

	hi RadiationCTypedef   ctermfg=3
	hi RadiationCStruct    ctermfg=142
	hi RadiationCUnion     ctermfg=73
	hi RadiationCEnum      ctermfg=59
	hi RadiationCFunction  ctermfg=110

There will be many more of these once there is support for more languages.

## FAQ

1. How do I get Radiation to run automatically on write?
	* I personally do not recommend doing this as the parser is still a little
	slow and may cause noticible lag to update the highlighting table on every
	write for projects that include lots of header files. So it is best to keep
	Radiate as a command or mapping; however, if you are dead set on updating
	with every write, it is possible to establish an autocmd. Add this to your
	vimrc `autocmd BufWrite *.c,*.h call Radiate`
2. Radiation exploded in my face!
	* These might be compiler errors that are spat back by the compiler.
	Unfortunately, there is no real easy way to deal with stderr from the
	compiler yet, but it is on the todo list. Simply clear the screen by
	switching tabs, but first figure out what the compiler wants.
3. `:Radiate` does nothing!
	* Did you remember to do the highlighting steps under `Usage`?
4. `bsd/sys/tree.h` not found.
	* Either install libbsd or download BSD's tree.h and use the compiler flag
	`-DTREE_H_PATH=\"<path/to/tree.h>\"`
