int encrypt(char *passw, int idx, unsigned char *nonce, unsigned char *lm_resp,
    unsigned char *nt_resp);

int getNTLM1(char *udata, char *result);
int getNTLM2(char *udata, char *req, char *result);
