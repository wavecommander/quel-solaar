Read me Test version of Memmory debugger OPA.

Lets all start off by  saying that this application is very experimental. It works fine on my code, but who knows about anyone elses.

OPA is build on some libraries i have writen:

+ Forge /* Math and other basic definitions, and my other memory debiugging tools */
+ Imagine /* Wrappier for platform specific stuff that doesnt require UI */
+ Betray /* Wrapper for input, openGL and so on, think GLUT, SDL  */
+ Testify /* Binary serialization and networking */
+ Relinquish /* OpenGL wrapper */
+ Seduce /* UI tool kit */

The way i build my projects is, that i take the files, put them in a an empty project and press compile. I dont pre build any lib files. It should works in both Xcode and Visual studio. I have no dependencies you need to manage.

To build build OPA add all libraries, and all files beginning with opa_ except opa_client.c .

To debug your application, add Forge, Imagine, Testify and then add opa_client.c

Then you need to add the following function to debug a structure:

opa_watch(your_pointer, "PointerTypeName", "./Path_to_file_where_type_is_defined.h");

This function needs to be called repetedly so put in your main loop.

I have added a simple test application that lets you play with OPAs functionality. 


OPA contains a C parser that parses your source code. This is a bit of a hack, and im working on re impleenting it. In fact im writing an entire compiler. I am handling compilers with different padding, and it should work with any C compiler.
Im sure there are some things I dont support correctly, and if you find anything in your code that OPA cant handle let me know. 

Here is a list of known limitations:

-Cant handle recompiled headers (obviusly)
-I dont support bit fields.
-Enums are always sequential. Opa ignores = in any enum definitions.
-I dont support undefine
-I dont support Trigraph
-I dont support Macros


Let me know what you think about it by emailing me at "eskil at obsession dot se". Im not sure how much time i will spend on this in the future. It mostly depends on the responce. I'm an indipendent developer who is always looking for new fun things to work on. 

My projects can be found at: http://www.quelsolaar.com
My source and documentation can be found at: http://www.gamepipeline.org
Im currently working on Unravel: http://unravel.org
On twitter I am: @quelsolaar (work) and @eskilSteenberg (personal)
I stream development at http://twitch.tv/quelsolaar