int ntlm_encrypt(char *passw, int idx, unsigned char *nonce, unsigned char *lm_resp,
    unsigned char *nt_resp);

int getNTLM1(char *udata, char *result, size_t res_size);
int getNTLM2(char *udata, char *req, char *result, size_t res_size);
