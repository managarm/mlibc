__thread int tls_var = 42;

int *get_tls(void) {
    return &tls_var;
}
