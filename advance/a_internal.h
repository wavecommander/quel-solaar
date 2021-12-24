
#define a_section_begin(n) a_internal_section_begin(n, __LINE__);
#define a_section_end(n) a_internal_section_begin(n, __LINE__);
#define a_sub_section_begin(n) a_internal_section_begin(n, __LINE__);
#define a_sub_section_end(n) a_internal_section_begin(n, __LINE__);
#define a_event(n, m) a_internal_section_begin(n, __LINE__);
