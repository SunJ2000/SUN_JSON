#pragma once
#include<string>
#include<vector>
#include<iostream>

class JsonData;

enum class JsonType
{
	EMPTY,BOOLEAN, INT, DOUBLE, STRING, ARRAY,OBJECT,
};

enum class ParseResult
{
	PARSE_OK,PARSE_EXPECT_VALUE,PARSE_INVALID_VALUE,PARSE_ROOT_NOT_SIGNULAR
};


// 标识JSONobject的数据格式，注意v并没有申请空间，只是指针。
struct JsonMember {
	std::string k;
	JsonData* v;
};

class JsonData
{
public:
	JsonData() {};
	JsonData(double a) :type(JsonType::DOUBLE), n(a) {};
public:
	JsonType get_type() {
		return type;
	}
	void set_type(JsonType t) {
		type = t;
	}

	bool get_bool() {
		return b;
	}
	void set_bool(bool v) {
		b = v;
	}
	double get_number() {
		return n;
	}
	void set_number(double d) {
		n = d;
	}

	void set_string(const char* s, size_t l) {
		if (type == JsonType::STRING) {
			delete[] str;
			type = JsonType::EMPTY;
		}
		len = l;
		str = new char[len + 1];
		for (int i = 0; i < len; i++) {
			str[i] = s[i];
		}
		str[len] = '\0';
		type = JsonType::STRING;
	}
	void set_string(const std::string &s) {
		if (type == JsonType::STRING) {
			delete[] str;
			type = JsonType::EMPTY;
		}
		len = s.size();
		str = new char[len + 1];
		for (int i = 0; i < len; i++) {
			str[i] = s[i];
		}
		str[len] = '\0';
		type = JsonType::STRING;
	}
	char* get_string() {
		return str;
	}
	size_t get_string_length() {
		return len;
	}

	size_t get_array_size() {
		return array_size;
	}
	JsonData* get_array_element(int index) {
		return &arr[index];
	}
	JsonData* get_array() {
		return arr;
	}
	void set_array_size(size_t s) {
		array_size = s;
	}
	void set_array(JsonData* a) {
		arr = a;
	}

	size_t get_object_size() {
		return  member_size;
	}
	JsonMember* get_object() {
		return m;
	}
	void set_object(JsonMember *members) {
		m = members;
	}
	void set_object_size(int i) {
		member_size = i;
	}

	void stringfy(std::string& json);

	JsonData& operator[](int i);
	JsonData& operator[](std::string);
	JsonData& operator[](const char *);

private:
	JsonType type;
	union 
	{
		//对象
		struct{
			JsonMember* m;
			size_t member_size;
		};
		//字符串
		struct {
			char * str;
			size_t len;
		};
		//数组
		struct {
			JsonData* arr;
			size_t array_size;
		};
		double n;
		bool b;
	};
};

struct JsonContext {
	const char* json;
	std::string str;
	std::vector<JsonData> JsonArray;
	std::vector<JsonMember> JsonMembers;
};

ParseResult  json_parse( const char* json, JsonData* value);

void parse_whitespace(JsonContext* c);

ParseResult  parse_null(JsonContext* c, JsonData* v);

ParseResult parse_true(JsonContext* c, JsonData* v);

ParseResult parse_false(JsonContext* c, JsonData* v);

ParseResult parse_bool(JsonContext* c, JsonData* v);

ParseResult parese_double(JsonContext* c, JsonData* v);

ParseResult parse_value( JsonContext* c, JsonData* v);

ParseResult parse_string_raw(JsonContext* c,std::string * str);

ParseResult parse_string(JsonContext* c, JsonData* v);

ParseResult parse_array(JsonContext* c, JsonData* v);

ParseResult  parse_object(JsonContext* c, JsonData* v);