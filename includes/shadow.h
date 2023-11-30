#ifndef SHADOW_H
#define SHADOW_H

const unsigned char USER_LOGIN_MAX = 15;
const unsigned char USER_PASSWORD_HASH = 65;

#include "shared.h"
typedef struct shadow {
  u16 uid;
  char login[USER_LOGIN_MAX];
  char password[USER_PASSWORD_HASH];
} shadow;

#endif // !SHADOW_H
