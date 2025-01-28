// Bar needs to have a relocation against foo in order to set DT_NEEDED.
void foo(void);
void bar() { foo(); }
