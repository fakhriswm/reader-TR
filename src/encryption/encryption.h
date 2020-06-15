#include <Arduino.h>
#include <mbedtls/aes.h>

class aes{
  public : 
    void encrypt(unsigned char *plainText, char *key, unsigned char *outputBuffer);
    void decrypt(unsigned char * chipherText, char * key, unsigned char * outputBuffer);
    void string_insert (String& string, const char* what, size_t where);
    void uuid_strip_insert(String& raw_uuid);
    void aes_encrypt(String text_input, String& text_output);
};

