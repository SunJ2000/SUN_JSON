# SUN_JSON
一个简单的json库，支持utf_8.

jsondata 为一个json对象。
jsondata.stringfy() 负责将一个json对象转换为一个字符串。
json_parse（2）负责将文本转换为一个jsondata。

jsondata对象重载了下标运算符[]用于查找和修改jsondata;
[string]可以用于获得一个json对象键值对的值，[index] 用于获得一个数组json的元素；下标运算返回值为jsondata &;
