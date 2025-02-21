/* Include the generated files first to test that all required header
   files are included. */
#include "choices_use_enum_choices.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "test_framework/unity.h"


void test_enum_codec(void){
    struct choices_use_enum_choices_foo_t msg;
    msg.foo = CHOICES_USE_ENUM_CHOICES_FOO_FOO_A_NEGATIVE_VALUE;
    uint8_t data[8];
    choices_use_enum_choices_foo_pack(data,&msg,sizeof(data));
    choices_use_enum_choices_foo_unpack(&msg,data,sizeof(data));
    TEST_ASSERT_EQUAL(msg.foo, CHOICES_USE_ENUM_CHOICES_FOO_FOO_A_NEGATIVE_VALUE);
}

