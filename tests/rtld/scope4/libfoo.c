// Foo needs to have a relocation against baz in order to set DT_NEEDED.
void baz(void);
void foo() { baz(); }
