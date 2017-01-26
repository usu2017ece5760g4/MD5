#pragma once

typedef unsigned int uint;
typedef unsigned char byte;

//---------------------------------------------------------------------------------------------------------------------+
// Generates a flattened list of lowercase alpha passwords of length n                                                 |
//---------------------------------------------------------------------------------------------------------------------+
unsigned int loop_alpha(unsigned int n, byte*** out);
