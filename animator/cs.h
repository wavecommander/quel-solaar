
extern CCharacter ca_character;
extern CCharDesc ca_description;

extern uint ca_animator_selected_clip;
extern uint ca_animator_selected_point;
extern uint ca_animator_selected_key;
extern uint ca_animator_selected_tool;
extern float ca_animator_selected_time;
extern boolean ca_animator_play;

extern boolean ca_character_stroke_rec;
extern float *ca_character_stroke;
extern uint ca_character_stroke_length;

extern void cs_save(CCharDesc *desc);

double sui_draw_slider(BInputState *input, double x_pos, double y_pos, double width, double value, float *start_color, float *end_color, float *color);
boolean ca_draw_settings_menu(BInputState *input, CCharDesc *c);
boolean ca_draw_menu_movement(BInputState *input, float *speed);
boolean ca_draw_anim_menu(BInputState *input, CCharDesc *c);
void ca_input_animator(BInputState *input, CCharDesc *d, CCharacter *c);
extern void ca_input_animator_edit_point_get(CCharDesc *desc, CCharacter *c, CAnimPoint *point, uint value_id);
extern uint ca_input_animator_edit_point_set(CCharDesc *desc, CCharacter *c, CAnimPoint *point, uint value_id);
extern boolean ca_draw_menu_param(BInputState *input, CCharDesc *desc, CCharacter *c, uint selected);
extern void ca_draw_timeline(BInputState *input, CCharDesc *desc, CCharacter *c, float delta);
extern void ca_ground_draw();
extern void ca_character_draw(float *tool_matrix, uint tool_count);
extern void ca_draw_popup(CCharDesc *desc,  CCharacter *c, BInputState *input);