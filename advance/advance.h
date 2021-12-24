


void a_init(char *file_name);
uint a_section_allocate(char *section);
void a_section_begin(uint id);
void a_section_end(uint id);
uint a_sub_section_allocate(uint id, char *section);
void a_sub_section_begin(uint id);
void a_sub_section_end(uint id);
uint a_counter_allocate(uint section, char *name);
void a_counter_add(uint id, float add);
void a_event(uint id, char *event);
void a_files_save_uint32(FILE *f, const uint32 data);
void a_files_save_real32(FILE *f, float data);
void a_files_save_uint16(FILE *f, uint16 value);
void a_files_save_string(FILE *f, char *text);
uint a_files_load_uint32(FILE *f);
float a_files_load_real32(FILE *f);
uint16 a_files_load_uint16(FILE *f);
void a_files_load_string(FILE *f, char *text);
void a_frame_end();
void a_record_end();
