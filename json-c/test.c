#include <json/json.h>
#include <stdio.h>

void offical_example()
{
	int i;
	struct json_object *my_array;
	my_array = json_object_new_array();
	json_object_array_add(my_array, json_object_new_int(1));
	json_object_array_add(my_array, json_object_new_int(2));
	json_object_array_add(my_array, json_object_new_int(3));
	json_object_array_put_idx(my_array, 4, json_object_new_int(5));
	printf("my_array=\n");
	for(i=0; i < json_object_array_length(my_array); i++) {
		struct json_object *obj = json_object_array_get_idx(my_array, i);
		printf("\t[%d]=%s\n", i, json_object_to_json_string(obj));
	}
	printf("my_array.to_string()=%s\n", json_object_to_json_string(my_array));
}

void json_example1()
{
	struct json_object *a;

	a = json_object_new_object();
	json_object_object_add(a, "result", json_object_new_int(10));
	json_object_object_add(a, "msg", json_object_new_string("test result!"));
	
	printf("%s\n", json_object_to_json_string(a));
}

int main()
{
	//offical_example();
	json_example1();
}
