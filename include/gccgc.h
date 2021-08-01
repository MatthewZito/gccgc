#ifndef GCCGC_H
#define GCCGC_H

#include <assert.h>
#include <stdio.h>

#define MAX_NAME_SIZE 128

#define OFFSET_OF(struct, member) (unsigned long)&(((struct*)0)->member)

#define MEMBER_SIZE(struct, member) sizeof(((struct*)0)->member)

#define FMT_MEMBER_INFO(struct, member, ctype, nested_name) {                     \
	#member, ctype, MEMBER_SIZE(struct, member),                                    \
	OFFSET_OF(struct, member), #nested_name                                         \
}

#define REGISTER_STRUCT(db, member_name, members_arr) {                           \
	do {                                                                            \
		gccgc_struct_db_record_t* record = malloc(sizeof(gccgc_struct_db_record_t));  \
		if (!record) assert(0);                                                       \
		if (!snprintf(record->name, MAX_NAME_SIZE, "%s", #member_name)) assert(0);    \
		record->size = sizeof(member_name);                                           \
		record->n_members = sizeof(members_arr) / sizeof(gccgc_member_t);             \
		record->members = members_arr;                                                \
		gccgc_struct_db_add(db, record);                                                     \
	} while(0);                                                                     \
}

typedef enum {
	UINT8,
	UINT32,
	INT32,
	CHAR,
	OBJ_PTR,
	VOID_PTR,
	FLOAT,
	DOUBLE,
	OBJ_STRUCT
} gccgc_ctype_t;

typedef enum {
	GCCGC_FALSE,
	GCCGC_TRUE
} gccgc_bool_t;

typedef struct struct_db_record gccgc_struct_db_record_t;

/**
 * @brief Represents metadata for a struct member
 */
typedef struct member_metadata {
	/* name of the represented member */
	char name [MAX_NAME_SIZE];

	/* C data type of the represented member */
	gccgc_ctype_t type;

	/* size of the represented member */
	unsigned int size;

	/* offset of the represented member */
	unsigned int offset;

	/* used if the member is a struct */
	char nested_name[MAX_NAME_SIZE];
} gccgc_member_t;

/**
 * @brief Represents metadata for a struct,
 * and a record in the structure db
 */
struct struct_db_record {
	/* pointer to the next structure in the linked list in which this struct is a node */
	gccgc_struct_db_record_t* next;

	/* name of the represented struct */
	char name [MAX_NAME_SIZE];

	/* size of the represented struct */
	unsigned int size;

	/* n members in the represented struct */
	unsigned int n_members;

	/* pointer to the members of the represented struct */
	gccgc_member_t* members;
};

/**
 * @brief Structure database (linked list)
 */
typedef struct struct_db {
	// head node
	gccgc_struct_db_record_t* head;
	// linked list size
	unsigned int size;
} gccgc_struct_db_t;

typedef struct obj_db_record gccgc_obj_db_record_t;

struct obj_db_record {
	gccgc_obj_db_record_t* next;
	void* ptr;
	unsigned int units;
	gccgc_struct_db_record_t* record;
	/* have we visited this object? (qua graph traversal) */
	gccgc_bool_t visited;

	/* is this the root object? */
	gccgc_bool_t root;
};

/**
 * @brief Object database (linked list)
 */
typedef struct object_db {
	gccgc_struct_db_t* struct_db;
	// head node
	gccgc_obj_db_record_t* head;
	// linked list size
	unsigned int size;
} gccgc_obj_db_t;

void gccgc_struct_db_print(gccgc_struct_db_t* db);

void gccgc_print_struct_record(gccgc_struct_db_record_t* record);

gccgc_struct_db_record_t* gccgc_struct_db_lookup(
	gccgc_struct_db_t *db,
	char* name
);

void gccgc_struct_db_add(gccgc_struct_db_t* db, gccgc_struct_db_record_t* record);

void gccgc_print_obj_db(gccgc_obj_db_t* db);

void gccgc_obj_record_print(gccgc_obj_db_record_t* record, int i);

void gccgc_obj_record_detail_print(gccgc_obj_db_record_t *record);

gccgc_obj_db_record_t* gccgc_obj_db_lookup(gccgc_obj_db_t *object_db, void *ptr);

void gccgc_obj_db_add(
	gccgc_obj_db_t* db,
	void* ptr,
	int units,
	gccgc_struct_db_record_t* struct_record,
	gccgc_bool_t is_root
);

void gccgc_db_init_primitives(gccgc_struct_db_t* db);

void* gccgc_calloc(gccgc_obj_db_t* db, char* name, int units);

void gccgc_set_dynamic_root_obj(gccgc_obj_db_t* db, void* ptr);

void gccgc_root_obj_recurse (gccgc_obj_db_t* db, gccgc_obj_db_record_t* parent_obj_rec);

gccgc_obj_db_record_t* gccgc_obj_get_next(
	gccgc_obj_db_t* db,
	gccgc_obj_db_record_t* init_record
);

void gccgc_algo_init(gccgc_obj_db_t* db);

void gccgc_algo_exec(gccgc_obj_db_t* db);

void gccgc_memleak_report(gccgc_obj_db_t* db);

#endif /* GCCGC_H */
