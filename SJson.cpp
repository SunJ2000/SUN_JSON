#include "SJson.h"
#include<stdlib.h>
#include<iostream>

#define ISDIGIT(ch)         ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch)     ((ch) >= '1' && (ch) <= '9')


void parse_whitespace(JsonContext* c) {
	const char* p = c->json;
	while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
		p++;
	}
	c->json = p;
}

ParseResult parse_null(JsonContext* c, JsonData* v) {
	if (c->json[0] != 'n' || c->json[1] != 'u' || c->json[2] != 'l'|| c->json[3] != 'l')
		return ParseResult::PARSE_INVALID_VALUE;
	c->json += 4;
	v->set_type(JsonType::EMPTY);
	return ParseResult::PARSE_OK;
}

ParseResult parse_true(JsonContext* c, JsonData* v) {
	if (c->json[0] != 't' || c->json[1] != 'r' || c->json[2] != 'u' || c->json[3] != 'e')
		return ParseResult::PARSE_INVALID_VALUE;
	c->json += 4;
	v->set_type(JsonType::BOOLEAN);
	v->set_bool(true);
	return ParseResult::PARSE_OK;
}

ParseResult parse_false(JsonContext* c, JsonData* v) {
	if (c->json[0] != 'f' || c->json[1] != 'a' || c->json[2] != 'l' || c->json[3] != 's'||c->json[4]!='e')
		return ParseResult::PARSE_INVALID_VALUE;
	c->json += 5;
	v->set_type(JsonType::BOOLEAN);
	v->set_bool(false);
	return ParseResult::PARSE_OK;
}

ParseResult parse_bool(JsonContext* c, JsonData* v) {
	if (c->json[0] == 't') {
		return parse_true(c, v);
	}
	if (c->json[0] == 'f') {
		return parse_false(c, v);
	}
	return ParseResult::PARSE_INVALID_VALUE;
}

ParseResult parese_double(JsonContext* c, JsonData* v) {
	const char* p = c->json;
	if (*p == '-')p++;

	if (*p == '0') p++;
	else {
		if (!ISDIGIT1TO9(*p)) return  ParseResult::PARSE_INVALID_VALUE;
		for (p++; ISDIGIT(*p); p++);
	}

	if (*p == '.') {
		p++;
		if (!ISDIGIT(*p)) return   ParseResult::PARSE_INVALID_VALUE;
		for (p++; ISDIGIT(*p); p++);
	}
	if (*p == 'e' || *p == 'E') {
		p++;
		if (*p == '+' || *p == '-') p++;
		if (!ISDIGIT(*p)) return   ParseResult::PARSE_INVALID_VALUE;
		for (p++; ISDIGIT(*p); p++);
	}
	v->set_number(strtod(c->json, nullptr));
	c->json = p;
	v->set_type(JsonType::DOUBLE);
	return   ParseResult::PARSE_OK;
}

ParseResult parse_string_raw(JsonContext* c, std::string* str) {
	const char* p = c->json;
	if (*p != '\"') {
		return ParseResult::PARSE_INVALID_VALUE;
	}
	p++;
	for (;;) {
		char ch = *p++;
		switch (ch)
		{
		case '\"':
			*str = c->str;
			c->str.clear();
			c->json = p;
			return ParseResult::PARSE_OK;
		case '\\':
			switch (*p++) {
			case '\"': c->str.push_back('\"'); break;
			case '\\': c->str.push_back('\\'); break;
			case '/':  c->str.push_back('/'); break;
			case 'b':  c->str.push_back('\b'); break;
			case 'f':  c->str.push_back('\f'); break;
			case 'n':  c->str.push_back('\n'); break;
			case 'r':  c->str.push_back('\r'); break;
			case 't':  c->str.push_back('\t'); break;
			default: return ParseResult::PARSE_INVALID_VALUE;
			}
			break;
		default:
			c->str.push_back(ch);
		}
	}
}

ParseResult parse_string(JsonContext* c, JsonData* v) {

	std::string str;
	ParseResult ret;
	if ((ret=parse_string_raw(c, &str) )== ParseResult::PARSE_OK) {
		v->set_string(str);
	}
	return ret;
}

ParseResult parse_array(JsonContext* c, JsonData* v) {
	//const char* p = c->json;
	size_t size = 0;
	ParseResult ret;
	if (*(c->json)++ != '[') {
		return ParseResult::PARSE_INVALID_VALUE;
	}
	parse_whitespace(c);
	if (*c->json == ']') {
		c->json++;
		v->set_type(JsonType::ARRAY);
		v->set_array_size(0);
		return ParseResult::PARSE_OK;
	}

	for (;;) {
		JsonData e;
		if ((ret = parse_value(c, &e))!= ParseResult::PARSE_OK) {
			std::cout << "parse error" << std::endl;
			return ret;
		}
		size++;
		c->JsonArray.push_back(e);
		parse_whitespace(c);
		if (*c->json == ',') {
			c->json++;
			parse_whitespace(c);
		}
		else if (*c->json == ']') {
			c->json++;
			size_t len = c->JsonArray.size();
			v->set_array_size(size);
			JsonData* arr = v->get_array();
			if (v->get_type() == JsonType::ARRAY) {
				//std::cout << "delete arr" << std::endl;
				delete[] arr;
			}
			arr = new JsonData[size];
			for (int i = 0; i < size; i++) {
				arr[i] = c->JsonArray[len-size+i];
			}
			for (int i = 0; i < size; i++) {
				c->JsonArray.pop_back();
			}
			v->set_array(arr);
			v->set_type(JsonType::ARRAY);
			return ParseResult::PARSE_OK;
		}
		else {
			return ParseResult::PARSE_INVALID_VALUE;
		}
	}
}

ParseResult  parse_object(JsonContext* c, JsonData* v) {
	const char* p = c->json;
	size_t size = 0;
	ParseResult ret;
	if (*(c->json)++ != '{') {
		return ParseResult::PARSE_INVALID_VALUE;
	}
	parse_whitespace(c);
	if (*c->json == '}') {
		c->json++;
		v->set_type(JsonType::OBJECT);
		v->set_object_size(0);
		v->set_object(nullptr);
		return ParseResult::PARSE_OK;
	}
	for (;;) {
		JsonMember e;
		std::string  key;
		if (*c->json != '"') {
			return ret = ParseResult::PARSE_INVALID_VALUE;
		}
		if ((ret = parse_string_raw(c, &key)) != ParseResult::PARSE_OK) {
			return ret;
		}
		e.k=key;
		parse_whitespace(c);

		if (*c->json != ':') {
			return ParseResult::PARSE_INVALID_VALUE;
		}
		c->json++;
		parse_whitespace(c);
		e.v = new JsonData();
		if ((ret = parse_value(c, e.v)) != ParseResult::PARSE_OK) {
			std::cout << "parse error" << std::endl;
			return ret;
		}
		size++;
		c->JsonMembers.push_back(e);
		parse_whitespace(c);
		if (*c->json == ',') {
			c->json++;
			parse_whitespace(c);
		}
		else if(*c->json=='}') {
			c->json++;
			size_t len = c->JsonMembers.size();
			v->set_object_size(size);
			JsonMember* members = v->get_object();
			if (v->get_type() == JsonType::OBJECT) {
				delete[] members;
			}
			members = new JsonMember[size];

			for (int i = 0; i < size; i++) {
				members[i] = c->JsonMembers[len - size + i];
			}
			for (int i = 0; i < size; i++) {
				c->JsonMembers.pop_back();
			}
			v->set_object(members);
			v->set_type(JsonType::OBJECT);
			return ParseResult::PARSE_OK;
		}
		else {
			return ParseResult::PARSE_INVALID_VALUE;
		}

	}
}

ParseResult parse_value(JsonContext* c, JsonData* v) {
	switch (*c->json) {
	case 't':  return parse_bool(c, v);
	case 'f':  return parse_bool(c, v);
	case 'n':  return parse_null(c, v);
	case '\"': return parse_string(c, v);
	case '\0': return ParseResult::PARSE_EXPECT_VALUE;
	case '[':  return parse_array(c, v);
	case '{': return parse_object(c, v);
	default:   return parese_double(c,v);
	}
}

ParseResult json_parse( const char * json,JsonData* value) {
	JsonContext c;
	ParseResult ret;
	c.json = json;
	value->set_type(JsonType::EMPTY);
	parse_whitespace(&c);
	if ((ret = parse_value(&c,value)) == ParseResult::PARSE_OK) {
		parse_whitespace(&c);
		if (*c.json != '\0')
			ret =ParseResult::PARSE_ROOT_NOT_SIGNULAR;
	}
	return ret;

}

void stringfy_string(const char* str, std::string& json,int len) {
	json += '\"';
	for (int i = 0; i < len; i++) {
		char ch = str[i];
		switch (ch) {
		case'\"':json += "\\\""; break;
		case'\\':json += "\\\\"; break;
		case '\b': json += "\\b"; break;
		case '\f': json += "\\f"; break;
		case '\n': json += "\\n"; break;
		case '\r':json += "\\r"; break;
		case '\t': json += "\\t"; break;
		default:
			json.push_back(ch);
		}
	}
	json += '\"';
}

void JsonData::stringfy(std::string & json) {
	switch (type) 
	{
	case JsonType::EMPTY:
		json += "null";
		break;
	case JsonType::BOOLEAN:
	{
		if (b) {
			json += "true";
		}
		else {
			json += "false";
		}
		break;
	}
	case JsonType::DOUBLE: 
	{
		json+= std::to_string(n);
		break;
	}
	case JsonType::STRING:
	{
		stringfy_string(str, json, len);
		break;
	}
	case JsonType::ARRAY:
	{
		json += '[';
		for (int i = 0; i < array_size; i++) {
			if (i > 0) {
				json += ',';
			}
			 arr[i].stringfy(json);
		}
		json += ']';
		break;
	}
	case JsonType::OBJECT:
	{
		json += '{';
		for (int i = 0; i < member_size; i++) {
			if (i > 0) {
				json += ',';
			}
			stringfy_string(m[i].k.c_str(), json, m[i].k.length());
			json += ':';
			m[i].v->stringfy(json);
		}
		json += '}';
		break;
	}
	default:
		break;
	}
}

JsonData& JsonData::operator[](int i) {	
	if (type == JsonType::ARRAY) {
		if (i >= array_size) {
			std::cout << "array size error" << std::endl;
			exit(0);
		}
		return arr[i];
	}
}

JsonData& JsonData::operator[](std::string str) {
	if (type != JsonType::OBJECT) {
		std::cout << "you can use [string] for a jsondata which is not a object" << std::endl;
		exit(0);
	}
	for (int i = 0; i < member_size; i++) {
		if (m[i].k == str) {
			return *m[i].v;
		}
	}
	
	std::cout << "the key:" << str << " is not exitence" << std::endl;
	exit(0);
}

JsonData& JsonData::operator[](const char * str) {
	if (type != JsonType::OBJECT) {
		std::cout << "you can use [string] for a jsondata which is not a object" << std::endl;
		exit(0);
	}
	std::string tmpstr(str);
	for (int i = 0; i < member_size; i++) {
		if (m[i].k == tmpstr) {
			return *m[i].v;
		}
	}
	std::cout << "the key:" << str << " is not exitence" << std::endl;
	exit(0);
}