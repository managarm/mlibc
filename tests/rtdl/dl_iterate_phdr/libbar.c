// Bar needs to have a relocation against foo in order to set DT_NEEDED.
int foo(void);
int bar() { return foo(); }
