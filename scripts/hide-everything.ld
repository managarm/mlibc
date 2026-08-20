{
  /* Hide all C++ symbols.  */
  local: _Z*;
  /* Hide the compiler's atomic helpers. GCC declares them with default visibility
     and it doesn't allow us to change the visibility by an attribute or pragma. */
  __atomic_*;
};
