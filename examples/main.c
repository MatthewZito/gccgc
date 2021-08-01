#include "gccgc.h"

#include <stdlib.h>
#include <string.h>

typedef struct graph {
	char name[30];
	unsigned int id;
	unsigned int age;
	struct graph* nested;
	float pt;
	int* r;
} graph_t;

typedef struct node {
	char name[32];
	unsigned int id;
	unsigned int age;
	float aggregate;
	struct node* next;
} node_t;

int panic(char* msg) {
	fprintf(stderr, "%s\n", msg);
	exit(EXIT_FAILURE);
}
int main(int argc, char* argv[]) {
	gccgc_struct_db_t* struct_db;

	if (!(struct_db = malloc(sizeof(gccgc_struct_db_t)))) {
		panic("failed to malloc db");
	}

	gccgc_db_init_primitives(struct_db);

	// register structure maps
	static gccgc_member_t graph_map[] = {
		FMT_MEMBER_INFO(graph_t, name, CHAR, 0),
		FMT_MEMBER_INFO(graph_t, id, UINT32, 0),
		FMT_MEMBER_INFO(graph_t, age, UINT32, 0),
		FMT_MEMBER_INFO(graph_t, nested, OBJ_PTR, graph_t),
		FMT_MEMBER_INFO(graph_t, pt, FLOAT, 0),
		FMT_MEMBER_INFO(graph_t, r, OBJ_PTR, 0)
  };

  REGISTER_STRUCT(struct_db, graph_t, graph_map);

	static gccgc_member_t node_map[] = {
		FMT_MEMBER_INFO(node_t, name, CHAR, 0),
		FMT_MEMBER_INFO(node_t, id, UINT32, 0),
		FMT_MEMBER_INFO(node_t, age, UINT32, 0),
		FMT_MEMBER_INFO(node_t, aggregate, FLOAT, 0),
		FMT_MEMBER_INFO(node_t, next, OBJ_PTR, node_t)
	};

	REGISTER_STRUCT(struct_db, node_t, node_map);

	gccgc_struct_db_print(struct_db);

	// init object database
	gccgc_obj_db_t* obj_db;

	if (!(obj_db = malloc(sizeof(gccgc_obj_db_t)))) {
		panic("failed to malloc object db");
	}

	obj_db->struct_db = struct_db;

	// memory allocate some objects
	node_t* node = gccgc_calloc(obj_db, "node_t", 1);
	gccgc_set_dynamic_root_obj(obj_db, node);

	node_t* node2 = gccgc_calloc(obj_db, "node_t", 1);
	if (!snprintf(node2->name, strlen("node2") + 1, "%s", "node2")) {
		panic("snprintf");
	}

	graph_t *graph = gccgc_calloc(obj_db, "graph_t", 2);
	gccgc_set_dynamic_root_obj(obj_db, graph);
	graph->r = gccgc_calloc(obj_db, "int", 1);

	gccgc_print_obj_db(obj_db);

	gccgc_algo_exec(obj_db);

	printf("Leaked objects: \n");
	gccgc_memleak_report(obj_db);
}
