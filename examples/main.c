#include "gccgc.h"

#include <stdlib.h>
#include <stdio.h>

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
	gccgc_struct_db_t* db;

	if (!(db = malloc(sizeof(gccgc_struct_db_t)))) {
		panic("failed to malloc db");
	}

	gccgc_db_init_primitives(db);

	// register structure maps
	static gccgc_member_t graph_map[] = {
		FMT_MEMBER_INFO(graph_t, name, CHAR, 0),
		FMT_MEMBER_INFO(graph_t, id, UINT32, 0),
		FMT_MEMBER_INFO(graph_t, age, UINT32, 0),
		FMT_MEMBER_INFO(graph_t, nested, OBJ_PTR, graph_t),
		FMT_MEMBER_INFO(graph_t, pt, FLOAT, 0),
		FMT_MEMBER_INFO(graph_t, r, OBJ_PTR, 0)
  };

  REGISTER_STRUCT(db, graph_t, graph_map);

	static gccgc_member_t node_map[] = {
		FMT_MEMBER_INFO(node_t, name, CHAR, 0),
		FMT_MEMBER_INFO(node_t, id, UINT32, 0),
		FMT_MEMBER_INFO(node_t, age, UINT32, 0),
		FMT_MEMBER_INFO(node_t, aggregate, FLOAT, 0),
		FMT_MEMBER_INFO(node_t, next, OBJ_PTR, node_t)
	};

	REGISTER_STRUCT(db, node_t, node_map);

	gccgc_print_db(db);
}
