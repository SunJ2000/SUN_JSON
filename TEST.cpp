#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<iostream>
#include "SJson.h"

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality) \
    do {\
        test_count++;\
        if (equality)\
            test_pass++;\
        else {\
            std::cout<<__FILE__<<" "<<__LINE__<<std::endl;\
            main_ret = 1;\
        }\
    } while(0)

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual))

#define TEST_NUMBER(expect,actual) \
		do{\
			JsonData v;\
			EXPECT_EQ_INT(ParseResult::PARSE_OK, json_parse(actual,&v));\
			EXPECT_EQ_INT(JsonType::DOUBLE, v.get_type());\
			EXPECT_EQ_INT(expect, v.get_number());\
		} while (0);

void TEST_STRING(const char* expect, const char* actual) {
    JsonData v;
    v.set_type(JsonType::EMPTY);
    EXPECT_EQ_INT(ParseResult::PARSE_OK, json_parse(actual, &v));
    EXPECT_EQ_INT(JsonType::STRING, v.get_type());
}

void TEST_ARRAY(const char* json,int n) {
    JsonData v;
    EXPECT_EQ_INT(ParseResult::PARSE_OK, json_parse(json, &v));
    EXPECT_EQ_INT(JsonType::ARRAY, v.get_type());
    EXPECT_EQ_INT(n, v.get_array_size());
    EXPECT_EQ_INT(JsonType::EMPTY, v.get_array_element(0)->get_type());
    EXPECT_EQ_INT(JsonType::BOOLEAN, v.get_array_element(1)->get_type());
    EXPECT_EQ_INT(JsonType::BOOLEAN, v.get_array_element(2)->get_type());
    EXPECT_EQ_INT(JsonType::DOUBLE, v.get_array_element(3)->get_type());
    EXPECT_EQ_INT(JsonType::STRING, v.get_array_element(4)->get_type());
    EXPECT_EQ_INT(JsonType::ARRAY, v.get_array_element(5)->get_type());
    EXPECT_EQ_INT(JsonType::EMPTY, v.get_array_element(5)->get_array_element(0)->get_type());
    EXPECT_EQ_INT(JsonType::BOOLEAN, v.get_array_element(5)->get_array_element(1)->get_type());
    EXPECT_EQ_INT(JsonType::BOOLEAN, v.get_array_element(5)->get_array_element(2)->get_type());
    EXPECT_EQ_INT(JsonType::DOUBLE, v.get_array_element(5)->get_array_element(3)->get_type());
    EXPECT_EQ_INT(JsonType::STRING, v.get_array_element(5)->get_array_element(4)->get_type());
    EXPECT_EQ_INT(123, v.get_array_element(5)->get_array_element(3)->get_number());
    char* ch = v.get_array_element(4)->get_string();
    std::cout<<"actual: "<<v.get_array_element(4)->get_string() << "   expect :abc"<<std::endl;
    std::cout << "actual: " << v.get_array_element(5)->get_array_element(4)->get_string() << "   expect :abc" << std::endl;
}

void TEST_OBJECT(const char* json, int n) {
    JsonData o;
    EXPECT_EQ_INT(ParseResult::PARSE_OK, json_parse(json, &o));
    EXPECT_EQ_INT(JsonType::OBJECT, o.get_type());
    EXPECT_EQ_INT(n, o.get_object_size());
    JsonMember* members = o.get_object();
    std::cout << "key:" << members[0].k << std::endl;
    JsonData * v = members[0].v;
    EXPECT_EQ_INT(JsonType::ARRAY, v->get_type());
    EXPECT_EQ_INT(JsonType::OBJECT, v->get_array_element(0)->get_type());
    EXPECT_EQ_INT(JsonType::EMPTY, v->get_array_element(1)->get_type());
    EXPECT_EQ_INT(JsonType::BOOLEAN, v->get_array_element(2)->get_type());
    EXPECT_EQ_INT(JsonType::BOOLEAN, v->get_array_element(3)->get_type());
    EXPECT_EQ_INT(JsonType::DOUBLE, v->get_array_element(4)->get_type());
    EXPECT_EQ_INT(JsonType::STRING, v->get_array_element(5)->get_type());
    EXPECT_EQ_INT(123, v->get_array_element(4)->get_number());
    std::cout << "actual: " << v->get_array_element(5)->get_string() << "   expect :sunc" << std::endl;
    
    JsonData * first_ch= v->get_array_element(0);
    JsonMember* sun_members = v->get_array_element(0)->get_object();
    EXPECT_EQ_INT(2, first_ch->get_object_size());

    for (int i = 0; i < 2; i++) {
        std::cout << sun_members[i].k << ":" << sun_members[i].v->get_string() << std::endl;
    }
}

 void test_parse_null() {
    JsonData v;
    v.set_type(JsonType::BOOLEAN);
    EXPECT_EQ_INT(ParseResult::PARSE_OK, json_parse("null",&v));
    EXPECT_EQ_INT(JsonType::EMPTY, v.get_type());
}

 void test_parse_bool() {
     JsonData v;
     v.set_type(JsonType::EMPTY);
     EXPECT_EQ_INT(ParseResult::PARSE_OK, json_parse("true", &v));
     EXPECT_EQ_INT(JsonType::BOOLEAN, v.get_type());

     EXPECT_EQ_INT(ParseResult::PARSE_OK, json_parse("false", &v));
     EXPECT_EQ_INT(JsonType::BOOLEAN, v.get_type());
 }

 void test_parse_double() {
     TEST_NUMBER(0.0, "0.0");
     TEST_NUMBER(0.0, "-0.0");
     TEST_NUMBER(1.0, "1");
     TEST_NUMBER(-1.0, "-1");
     TEST_NUMBER(1.5, "1.5");
     TEST_NUMBER(-1.5, "-1.5");
     TEST_NUMBER(3.1416, "3.1416");
     TEST_NUMBER(1E10, "1E10");
     TEST_NUMBER(1e10, "1e10");
     TEST_NUMBER(1E+10, "1E+10");
     TEST_NUMBER(1E-10, "1E-10");
     TEST_NUMBER(-1E10, "-1E10");
     TEST_NUMBER(-1e10, "-1e10");
     TEST_NUMBER(-1E+10, "-1E+10");
     TEST_NUMBER(-1E-10, "-1E-10");
     TEST_NUMBER(1.234E+10, "1.234E+10");
     TEST_NUMBER(1.234E-10, "1.234E-10");
 }

 void test_parse_string() {
     TEST_STRING("sun", "\"sun\"");
     TEST_STRING("jian", "\"jian\"");
     TEST_STRING("zhe", "\"zhe\"");     
 }
/* ... */

 void test_parse_array() {
     TEST_ARRAY("[ null , false , true , 123 , \"abc\" ,[null , false , true,123 , \"abc\"]  ]",6);  
 }

 void test_parse_object() {
     TEST_OBJECT("{\"sites\": [{ \"name\":\"runoob\", \"url\" : \"www.runoob.com\" },null , false , true , 123 , \"sunc\"] }", 1);
 }
 
 void test_stringfy() {
     JsonData v;
     json_parse("{\"sites\": [{ \"name\":\"runoob\", \"url\" : \"www.runoob.com\" },null , false , true , 123 , \"sunc\"] }", &v);
     std::string json;
     v.stringfy(json);
     std::cout << std::endl;
     std::cout << json << std::endl;
 }

 void test_subindex() {
     JsonData v;
     json_parse("{\"sites\": [{ \"name\":\"runoob\", \"url\" : \"www.runoob.com\" },null , false , true , 123 , \"sunc\"] }", &v);
     std::string json;
     v["sites"].stringfy(json);
     std::cout << std::endl << json << std::endl;
     json.clear();
     v["sites"][0]["name"].stringfy(json);
     std::cout << std::endl << json << std::endl;
     JsonData v2;
     json_parse("135", &v2);
     v["sites"][4] =134;
     json.clear();
     v["sites"].stringfy(json);
     std::cout << std::endl << json << std::endl;
 }

static void test_parse() {
    test_parse_null();
    test_parse_bool();
    test_parse_double();
    test_parse_string();
    test_parse_array();
    test_parse_object();
    test_stringfy();
    test_subindex();
    /* ... */
}

int main() {
    test_parse();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}
