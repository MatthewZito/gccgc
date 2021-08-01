#ifndef GCCGC_H
#define GCCGC_H

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
		gccgc_db_add(db, record);                                                     \
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

void gccgc_print_db(gccgc_struct_db_t* db);

void gccgc_print_record(gccgc_struct_db_record_t* record);

void gccgc_db_add(gccgc_struct_db_t* db, gccgc_struct_db_record_t* record);

void gccgc_db_register(
	gccgc_struct_db_t* db,
	void* objptr,
	char* struct_name,
	unsigned int units
);

void* gccgc_calloc(gccgc_struct_db_t* db, char*name, int units);

void gccgc_db_init_primitives(gccgc_struct_db_t* db);

#endif /* GCCGC_H */
