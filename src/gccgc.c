#include "gccgc.h"

#include <stdlib.h>

char* DATA_TYPE[] = {
	"UINT8",
	"UINT32",
	"INT32",
	"CHAR",
	"OBJ_PTR",
	"VOID_PTR",
	"FLOAT",
	"DOUBLE",
	"OBJ_STRUCT"
};

void gccgc_print_db(gccgc_struct_db_t* db) {
  if(!db) return;

	gccgc_struct_db_record_t* record = NULL;
	record = db->head;

	printf("n register structs = %d\n", db->size);

	int i = 0;
	while (record) {
		printf("struct n %d = (%p)\n", i++, record);
		print_structure_rec(record);
		record = record->next;
	}
}

void gccgc_print_record(gccgc_struct_db_record_t* record) {
	if (!record) return;

	gccgc_member_t* member = NULL;
	printf(
		"%-20s | size = %-8d | n members = $-3d |\n",
		record->name,
		record->size,
		record->n_members
	);

	for (int j = 0; j < record->n_members; j++){
		member = &record->members[j];
		printf("  %-20s |", "");
		printf(
			"%-3d %-20s | type = %-15s | size = %-5d | offset = %-6d|  nested struct name = %-20s |\n",
			j,
			member->name,
			DATA_TYPE[member->type],
			member->size,
			member->offset,
			member->nested_name
		);
	}
}

/**
 * @brief Add a record to the struct db
 *
 * @param db
 * @param record
 */
void gccgc_db_add(gccgc_struct_db_t* db, gccgc_struct_db_record_t* record) {
	gccgc_struct_db_record_t* head = db->head;

	if (!head) {
		record->next = NULL;
	} else {
		record->next = head;
	}

	db->head = record;
	db->size++;
}

/**
 * @brief Initialize db primitives support
 *
 * @param db
 */
void gccgc_db_init_primitives(gccgc_struct_db_t* db) {
	REGISTER_STRUCT(db, int, 0);
	REGISTER_STRUCT(db, float, 0);
	REGISTER_STRUCT(db, double, 0);
}
