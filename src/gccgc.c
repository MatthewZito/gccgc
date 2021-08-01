#include "gccgc.h"

#include <stdlib.h>
#include <string.h>

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

/**
 * @brief Print a struct db to stdout
 *
 * @param db
 */
void gccgc_struct_db_print(gccgc_struct_db_t* db) {
  if(!db) return;

	gccgc_struct_db_record_t* record = NULL;
	record = db->head;

	printf("n register structs = %d\n", db->size);

	int i = 0;
	while (record) {
		printf("struct n %d = (%p)\n", i++, record);
		gccgc_print_struct_record(record);
		record = record->next;
	}
}

/**
 * @brief Print a single record to stdout
 *
 * @param record
 */
void gccgc_print_struct_record(gccgc_struct_db_record_t* record) {
	if (!record) return;

	gccgc_member_t* member = NULL;
	printf(
		"%-20s | size = %-8d | n members = %-3d |\n",
		record->name,
		record->size,
		record->n_members
	);

	for (int j = 0; j < record->n_members; j++) {
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
 * @brief Perform a record lookup in the given struct db
 *
 * @param db
 * @param name
 * @return gccgc_struct_db_record_t*
 */
gccgc_struct_db_record_t* gccgc_struct_db_lookup(
	gccgc_struct_db_t *db,
	char* name
) {
	gccgc_struct_db_record_t* head = db->head;
	if (!head) return NULL;

	for (; head; head = head->next) {
		if (strncmp(head->name, name, MAX_NAME_SIZE) == 0) {
			return head;
		}
	}

	return NULL;
}

/**
 * @brief Add a record to the struct db
 *
 * @param db
 * @param record
 */
void gccgc_struct_db_add(gccgc_struct_db_t* db, gccgc_struct_db_record_t* record) {
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
 * @brief Print an object db to stdout
 *
 * @param db
 */
void gccgc_print_obj_db(gccgc_obj_db_t* db) {
	gccgc_obj_db_record_t* head = db->head;

	for (unsigned int i = 0; head; head = head->next) {
		gccgc_obj_record_print(head, i++);
	}
}

/**
 * @brief Print an object db record to stdout
 *
 * @param record
 * @param i
 */
void gccgc_obj_record_print(gccgc_obj_db_record_t* record, int i) {
	if (!record) return;

  printf(
		"%-3d pointer = %-10p | next = %-10p | units = %-4d | name = %-10s | root? = %s |\n",
    i,
		record->ptr,
		record->next,
		record->units,
		record->record->name,
		record->root ? "true" : "false"
	);
}

/**
 * @brief Print an object db record's details to stdout
 *
 * @param record
 */
void gccgc_obj_record_detail_print(gccgc_obj_db_record_t *record) {
	int n_members = record->record->n_members;
	gccgc_member_t* member = NULL;

	int units = record->units,
						member_idx = 0,
								obj_idx = 0;

	for (; obj_idx < units; obj_idx++) {
		char* curr = (char*)(record->ptr) + (obj_idx * record->record->size);

		for (member_idx = 0; member_idx < n_members; member_idx++) {
			member = &record->record->members[member_idx];

			switch (member->type) {
				case UINT8:
				case INT32:
				case UINT32:
					printf(
						"%s[%d]->%s = %d\n",
						record->record->name,
						obj_idx, member->name,
						*(int*)(curr + member->offset)
					);
					break;

				case CHAR:
					printf(
						"%s[%d]->%s = %s\n",
						record->record->name,
						obj_idx, member->name,
						(char*)(curr + member->offset)
					);
					break;

				case FLOAT:
					printf(
						"%s[%d]->%s = %f\n",
						record->record->name,
						obj_idx, member->name,
						*(float*)(curr + member->offset)
					);
					break;

				case DOUBLE:
					printf(
						"%s[%d]->%s = %f\n",
						record->record->name,
						obj_idx, member->name,
						*(double*)(curr + member->offset)
					);
					break;

				case OBJ_PTR:
					printf(
						"%s[%d]->%s = %p\n",
						record->record->name,
						obj_idx, member->name,
						(void*)(int*)(curr + member->offset)
					);
					break;

				case OBJ_STRUCT:
					// TODO
					break;

				default:
					break;
			}
		}
	}
}

/**
 * @brief Perform a record lookup in the given object db
 *
 * @param object_db
 * @param ptr
 * @return gccgc_obj_db_record_t*
 */
gccgc_obj_db_record_t* gccgc_obj_db_lookup(gccgc_obj_db_t *object_db, void *ptr) {
	gccgc_obj_db_record_t* head = object_db->head;

	if (!head) return NULL;

	for (; head; head = head->next) {
		if (head->ptr == ptr) return head;
	}

	return NULL;
}

/**
 * @brief Add a object record to the given object db
 *
 * @param db
 * @param ptr
 * @param units
 * @param record
 * @param is_root
 */
void gccgc_obj_db_add(
	gccgc_obj_db_t* db,
	void* ptr,
	int units, // n records to create
	gccgc_struct_db_record_t* struct_record,
	gccgc_bool_t is_root
) {
	gccgc_obj_db_record_t* record = gccgc_obj_db_lookup(db, ptr);

	record = malloc(sizeof(gccgc_obj_db_record_t)); // TODO check

	record->next = NULL;
	record->ptr = ptr;
	record->units = units;
	record->record = struct_record;
	record->visited = GCCGC_FALSE;
	record->root = is_root;

	gccgc_obj_db_record_t* head = db->head;

	if (!head) {
		db->head = record;
		record->next = NULL;
		db->size++;
		return;
	} else {
		record->next = head;
		db->head = record;
		db->size++;
	}
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

/**
 * @brief Allocate `units` bytes of heap memory
 *
 * @param db
 * @param name
 * @param units
 * @return void*
 */
void* gccgc_calloc(gccgc_obj_db_t* db, char* name, int units) {
	gccgc_struct_db_record_t* record = gccgc_struct_db_lookup(db->struct_db, name);
	// TODO
	void* ptr = calloc(units, record->size);
	gccgc_obj_db_add(db, ptr, units, record, GCCGC_FALSE);
	return ptr;
}

/**
 * @brief Set an existing object record to root
 *
 * @param db
 * @param ptr
 */
void gccgc_set_dynamic_root_obj(gccgc_obj_db_t* db, void* ptr) {
	gccgc_obj_db_record_t* record = gccgc_obj_db_lookup(db, ptr);
	if (!record) return;

  record->root = GCCGC_TRUE;
}

/**
 * @brief Recurse the root object, traversing all children
 *
 * @param db
 * @param parent_obj_rec
 */
void gccgc_root_obj_recurse (gccgc_obj_db_t* db, gccgc_obj_db_record_t* parent_obj_rec) {
	unsigned int n_members;

	char* parent_obj_ptr = NULL,
		* child_obj_offset = NULL;

	void* child_obj_addr = NULL;
	gccgc_member_t* member_metadata = NULL;

	gccgc_obj_db_record_t* child_obj_record = NULL;
	gccgc_struct_db_record_t* parent_struct_record = parent_obj_rec->record;

	if (parent_struct_record->n_members == 0) {
		return;
	}

  for (unsigned int i = 0; i < parent_obj_rec->units; i++) {
		parent_obj_ptr = (char*)(parent_obj_rec->ptr) + (i* parent_struct_record->size);

		for (n_members = 0; n_members < parent_struct_record->n_members; n_members++) {
			member_metadata = &parent_struct_record->members[n_members];

			switch (member_metadata->type) {
				case UINT8:
				case UINT32:
				case INT32:
				case CHAR:
				case FLOAT:
				case DOUBLE:
				case OBJ_STRUCT:
					break;

				case VOID_PTR:
				case OBJ_PTR:
				default:
					; // fall through

				// where child_obj_offset is the memory location inside parent object
				// at which the addr of the next object resides
				child_obj_offset = parent_obj_ptr + member_metadata->offset;

				memcpy(&child_obj_addr, child_obj_offset, sizeof(void*));

				// where child_obj_addr now stores the address of the next object in the
				// graph; it could be NULL, so we handle accordingly
				if (!child_obj_addr) continue;

				child_obj_record = gccgc_obj_db_lookup(db, child_obj_addr);

				if (!child_obj_record->visited) {
					child_obj_record->visited = GCCGC_TRUE;

					// we traverse the next object provided it is not a void pointer
					if (member_metadata->type != VOID_PTR) {
						gccgc_root_obj_recurse(db, child_obj_record);
					}
				} else continue; // explore next child object
			}
		}
  }
}

/**
 * @brief Get the next object record
 *
 * @param db
 * @param init_record
 * @return gccgc_obj_db_record_t*
 */
gccgc_obj_db_record_t* gccgc_obj_get_next(
	gccgc_obj_db_t* db,
	gccgc_obj_db_record_t* init_record
) {
	gccgc_obj_db_record_t* first = init_record ? init_record->next : db->head;
	while (first) {
		if (first->root) return first;
		first = first->next;
	}

	return NULL;
}

/**
 * @brief Initialize traversal defaults
 *
 * @param db
 */
void gccgc_algo_init(gccgc_obj_db_t* db) {
	gccgc_obj_db_record_t* record = db->head;

	while (record) {
		record->visited = GCCGC_FALSE;
		record = record->next;
	}
}

/**
 * @brief Execute the core gccgc algorithm
 *
 * @param db
 */
void gccgc_algo_exec(gccgc_obj_db_t* db) {
	// mark all objects as unvisited
	gccgc_algo_init(db);

	// get root object, or first if many are extant
	gccgc_obj_db_record_t* root_obj = gccgc_obj_get_next(db, NULL);

	while (root_obj) {
		// all accessible objects in the directed cyclic graph have been visited,
		// proceed here lest we enter an infinite loop
		if (root_obj->visited) {
			root_obj = gccgc_obj_get_next(db, root_obj);
			continue;
		}

		root_obj->visited = GCCGC_TRUE;

		// recurse all accessible objects on the given root object
		gccgc_root_obj_recurse(db, root_obj);

		root_obj = gccgc_obj_get_next(db, root_obj);
	}
}

/**
 * @brief Print a memory leak report to stdout
 *
 * @param db
 */
void gccgc_memleak_report(gccgc_obj_db_t* db) {
  gccgc_obj_db_record_t* head;
	int i = 0;

	for (head = db->head; head; head = head->next) {
		if (!head->visited) {
			gccgc_obj_record_print(head, i++);
			gccgc_obj_record_detail_print(head);
		}
	}
}
